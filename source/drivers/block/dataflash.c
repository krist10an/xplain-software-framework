/**
 * \file
 *
 * \brief DataFlash&reg; block device driver
 *
 * Copyright (C) 2009 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
#include <assert.h>
#include <bitops.h>
#include <debug.h>
#include <interrupt.h>
#include <malloc.h>
#include <status_codes.h>
#include <string.h>
#include <util.h>
#include <string.h>
#include <spi.h>
#include <block/device.h>
#include <block/dataflash.h>
#include <workqueue.h>
#include <mempool.h>
#include <flash/at45_device.h>

/**
 * \ingroup block_device_dataflash_group
 * \defgroup block_device_dataflash_internals_group Internal Documentation
 *
 * \section dataflash_state_machine DataFlash state machine
 *
 * When refering to buffer, that means the DataFlash built-in page buffer.
 *
 * \dot
	digraph dataflash_read_write {
		size = "16, 16";

		node [shape = circle];
		IDLE;
		START;
		SETUP_READ [label="SETUP\nREAD"];
		SETUP_WRITE [label="SETUP\nWRITE"];
		WRITE_BUFFERED [label="WRITE\nBUFFERED"];
		WRITE_BUFFERED_WAIT [label="WRITE\nBUFFERED\nWAIT"];
		WRITE_READY [label="WRITE\nREADY"];
		TRANSFER;
		WRITE_DONE [label="WRITE\nDONE"];
		WRITE_WAIT [label="WRITE\nWAIT"];
		PAGE_DONE [label="PAGE\nDONE"];

		IDLE -> START [label="block_submit_req"];
		START -> SETUP_READ [label="read operation"];
		START -> SETUP_WRITE [label="write operation"];
		SETUP_WRITE -> WRITE_READY
			[label="is aligned to whole page"];
		SETUP_WRITE -> WRITE_BUFFERED [label="partial page write\n \
read-back flash into buffer"];
		WRITE_BUFFERED -> WRITE_BUFFERED_WAIT [label="wait for ready"];
		WRITE_BUFFERED_WAIT -> WRITE_BUFFERED_WAIT [label="busy"];
		WRITE_BUFFERED_WAIT -> WRITE_READY [label="ready"];
		WRITE_READY -> TRANSFER [label="Initiate write to buffer"];
		SETUP_READ -> TRANSFER [label="Initiate read page"];
		TRANSFER -> PAGE_DONE [label="read page done"];
		TRANSFER -> WRITE_DONE [label="write page buffer done"];
		TRANSFER -> TRANSFER [label="more data..."];
		WRITE_DONE -> WRITE_WAIT [label="program buffer into flash"];
		WRITE_WAIT -> WRITE_WAIT [label="busy"];
		WRITE_WAIT -> PAGE_DONE [label="ready"];
		PAGE_DONE -> SETUP_WRITE [label="more pages to write"];
		PAGE_DONE -> SETUP_READ [label="more pages to read"];
		PAGE_DONE -> IDLE [label="last page"];
	}
 * \enddot
 *
 * @{
 */

//! The DataFlash block device block size
#define DATAFLASH_BLOCK_SIZE 512

//! DataFlash specific block device request
struct dataflash_breq {
	//! Base block request
	struct block_request  breq;
	//! Task associated with this request
	struct workqueue_task task;
	//! Logical block address of the block to process
	block_addr_t          lba;
	//! Number of remaining blocks to process
	block_len_t           remaining_blocks;
	//! Operation to process (read, write)
	enum block_operation  operation;
	//! Indicates if operation is waiting for free buffers
	bool                  sleeping;
};

//! DataFlash specific block device
struct dataflash_bdev {
	//! Base block device
	struct block_device   bdev;
	//! AT45 device
	struct at45_device    at45d;
	//! Workqueue task to run on events
	struct workqueue_task *event_task;
	//! Memory pool used to allocate memory for \ref dataflash_breq
	struct mem_pool       req_pool;
	//! Workqueue task for underlying driver use
	struct workqueue_task task;
	/**
	 * Indicates the shfit between page size and block size.
	 *   = 0 means equal
	 *   < 0 means page size is smaller than block size
	 *   > 0 means page size if larger than block size
	 */
	int8_t                page_block_shift;
	//! Current buffer list beeing transfered
	struct slist          current_buf_list;
	//! Current transfer byte position
	int                   transfer_pos;
};

static inline struct dataflash_breq *dataflash_breq_of(
		struct block_request *req)
{
	return container_of(req, struct dataflash_breq, breq);
}

static inline struct dataflash_bdev *dataflash_bdev_of(
		struct block_device *dev)
{
	return container_of(dev, struct dataflash_bdev, bdev);
}

static inline struct dataflash_breq *dataflash_breq_of_task(
		struct workqueue_task *task)
{
	return container_of(task, struct dataflash_breq, task);
}

/**
 * \brief Get DataFlash page size vs. block size shift value
 *
 * The values for block size and page size are given in power-of-2 values. So
 * this means that the difference between them can be represented as a bit
 * shift operation.
 *
 * \param df_breq DataFlash block request struct
 * \retval <0 Page size is smaller than block size
 * \retval 0  Identical page size and block size
 * \retval >0 Page size is bigger than block size
 */
static int8_t dataflash_get_page_block_shift(struct dataflash_breq *df_breq)
{
	struct dataflash_bdev *df_bdev = dataflash_bdev_of(df_breq->breq.bdev);

	return df_bdev->page_block_shift;
}

/**
 * \brief Test if request currently is aligned into whole page
 *
 * Tests the alignment between page size and block size. This is tested for the
 * current block request position.
 *
 * \param df_breq DataFlash block request struct
 * \retval true  Remaining data transfer is aligned toa whole page
 * \retval false Remaining data transfer is for a partial page
 */
static bool dataflash_is_page_aligned(struct dataflash_breq *df_breq)
{
	int8_t shift;

	shift = dataflash_get_page_block_shift(df_breq);
	if (shift == 0)
		return true; // Always true for same size page and block
	else if (shift == 1) {
		/* For a page size the double of block size; the block address
		 * (lba) must be on a even address and ramining block must be
		 * larger than 2 for the remaining data transfer to cover a
		 * whole page.
		 */
		if (!(df_breq->lba & 1) && (df_breq->remaining_blocks >= 2))
			return true;
		else
			return false;
	} else
		return true; // Always true for page size smaller than block
}

/**
 * \brief Store DataFlash page size into shift value
 *
 * \param df_bdev   DataFlash block device struct
 * \param page_size Page size in bytes
 * \retval true  Valid page size to store
 * \retval false Invalid page size
 */
static bool dataflash_store_page_size(struct dataflash_bdev *df_bdev,
		uint16_t page_size)
{
	switch (page_size) {
	case 256:
		df_bdev->page_block_shift = -1;
		return true;
	case 512:
		df_bdev->page_block_shift = 0;
		return true;
	case 1024:
		df_bdev->page_block_shift = 1;
		return true;
	default:
		return false;
	}
}

/**
 * \brief Get DataFlash remaining partial page size
 *
 * For current transfer posisiton get the ramining size of (parital) page.
 *
 * \param df_breq DataFlash block request struct
 */
static uint16_t dataflash_get_remaining_page_size(
		struct dataflash_breq *df_breq)
{
	int8_t shift;

	shift = dataflash_get_page_block_shift(df_breq);

	// We only support 256, 512 and 1024 byte page size
	assert((shift >= -1) && (shift <= 1));

	if (shift == 0) // Equal page size and block size
		return 512;
	else if (shift == 1) {
		/* Page size if bigger than block size.
		 * Check if address is even and ramining transfer is large
		 * enough to cover a full page or not.
		 */
		if (!(df_breq->lba & 1) && (df_breq->remaining_blocks >= 2))
			return 1024;
		else
			return 512;
	} else {
		// Page size smaller than block size
		return 256;
	}
}

static void dataflash_read_setup(struct workqueue_task *task);
static void dataflash_write_setup(struct workqueue_task *task);

static void dataflash_page_done(struct workqueue_task *task)
{
	struct dataflash_breq *df_breq = dataflash_breq_of_task(task);
	struct dataflash_bdev *df_bdev = dataflash_bdev_of(df_breq->breq.bdev);

	df_breq->breq.buf_list_done(&df_bdev->bdev, &df_breq->breq,
			&df_bdev->current_buf_list);
	df_breq->breq.bytes_xfered += df_bdev->transfer_pos;
	df_breq->remaining_blocks -=
			df_bdev->transfer_pos / DATAFLASH_BLOCK_SIZE;
	df_breq->lba += df_bdev->transfer_pos / DATAFLASH_BLOCK_SIZE;
	if (df_breq->remaining_blocks) {
		if (df_breq->operation == BLK_OP_READ)
			dataflash_read_setup(task);
		else
			dataflash_write_setup(task);
	} else {
		dbg_verbose("DataFlash: req done\n");
		at45_release(&df_bdev->at45d);
		df_breq->breq.status = STATUS_OK;
		df_breq->breq.req_done(df_breq->breq.bdev, &df_breq->breq);
	}
}

static void dataflash_read_done(struct workqueue_task *task);
static void dataflash_write_done(struct workqueue_task *task);

static void dataflash_transfer(struct workqueue_task *task)
{
	struct dataflash_breq *df_breq = dataflash_breq_of_task(task);
	struct dataflash_bdev *df_bdev = dataflash_bdev_of(df_breq->breq.bdev);
	struct buffer         *buf;
	int                   next_pos;
	int                   end_pos;
	irqflags_t            flags;

	if (!slist_is_empty(&df_bdev->current_buf_list)) {
		df_breq->breq.buf_list_done(&df_bdev->bdev, &df_breq->breq,
				&df_bdev->current_buf_list);
		slist_init(&df_bdev->current_buf_list);
	}

	flags = cpu_irq_save();
	if (slist_is_empty(&df_breq->breq.buf_list)) {
		dbg_verbose("DataFlash: sleep\n");
		df_breq->sleeping = true;
		cpu_irq_restore(flags);
		return;
	}
	cpu_irq_restore(flags);

	next_pos = df_bdev->transfer_pos;
	end_pos = dataflash_get_remaining_page_size(df_breq);
	while (next_pos < end_pos) {
		flags = cpu_irq_save();
		if (slist_is_empty(&df_breq->breq.buf_list)) {
			cpu_irq_restore(flags);
			break;
		} else
			buf = buf_list_pop_head(&df_breq->breq.buf_list);
		cpu_irq_restore(flags);
		assert((next_pos + buf->len) <= end_pos);
		slist_insert_tail(&df_bdev->current_buf_list, &buf->node);
		next_pos += buf->len;
	}

	df_bdev->transfer_pos = next_pos;

	if (df_breq->operation == BLK_OP_READ) {
		if (next_pos == end_pos) {
			workqueue_task_set_work_func(&df_breq->task,
					dataflash_read_done);
		}
		at45_read_buf_list(&df_bdev->at45d, &df_bdev->current_buf_list);
	} else {
		if (next_pos == end_pos) {
			workqueue_task_set_work_func(&df_breq->task,
					dataflash_write_done);
		}
		at45_write_buf_list(&df_bdev->at45d, &df_bdev->current_buf_list);
	}
}

static void dataflash_read_done(struct workqueue_task *task)
{
	struct dataflash_breq *df_breq = dataflash_breq_of_task(task);
	struct dataflash_bdev *df_bdev = dataflash_bdev_of(df_breq->breq.bdev);

	at45_deselect(&df_bdev->at45d);
	dataflash_page_done(task);
}

static void dataflash_read_setup(struct workqueue_task *task)
{
	struct dataflash_breq *df_breq = dataflash_breq_of_task(task);
	struct dataflash_bdev *df_bdev = dataflash_bdev_of(df_breq->breq.bdev);

	workqueue_task_set_work_func(&df_breq->task, dataflash_transfer);

	df_bdev->transfer_pos = 0;

	at45_select(&df_bdev->at45d);
	at45_cmd_cont_array_read(&df_bdev->at45d, df_breq->lba >> 1,
			(df_breq->lba & 1) << 9);
}

static void dataflash_write_wait(struct workqueue_task *task)
{
	struct dataflash_breq *df_breq = dataflash_breq_of_task(task);
	struct dataflash_bdev *df_bdev = dataflash_bdev_of(df_breq->breq.bdev);
	bool                  done;

	done = at45_wait_ready(&df_bdev->at45d);
	if (!done) {
		// task will be re-sceduled on new wait event so just return
		return;
	}
	dataflash_page_done(task);
}

static void dataflash_write_wait_begin(struct workqueue_task *task)
{
	struct dataflash_breq *df_breq = dataflash_breq_of_task(task);
	struct dataflash_bdev *df_bdev = dataflash_bdev_of(df_breq->breq.bdev);

	at45_deselect(&df_bdev->at45d);
	workqueue_task_set_work_func(&df_breq->task,
			dataflash_write_wait);
	dataflash_write_wait(task);
}

static void dataflash_write_done(struct workqueue_task *task)
{
	struct dataflash_breq *df_breq = dataflash_breq_of_task(task);
	struct dataflash_bdev *df_bdev = dataflash_bdev_of(df_breq->breq.bdev);

	workqueue_task_set_work_func(&df_breq->task,
			dataflash_write_wait_begin);

	at45_deselect(&df_bdev->at45d);
	at45_select(&df_bdev->at45d);
	at45_cmd_buffer_1_main_memory_program_with_erase(&df_bdev->at45d,
			df_breq->lba >> 1);
}

static void dataflash_write_ready(struct workqueue_task *task)
{
	struct dataflash_breq *df_breq = dataflash_breq_of_task(task);
	struct dataflash_bdev *df_bdev = dataflash_bdev_of(df_breq->breq.bdev);

	workqueue_task_set_work_func(&df_breq->task, dataflash_transfer);

	at45_select(&df_bdev->at45d);
	at45_cmd_buffer_1_write(&df_bdev->at45d, (df_breq->lba & 1) << 9);
}

static void dataflash_write_buffered_wait(struct workqueue_task *task)
{
	struct dataflash_breq *df_breq = dataflash_breq_of_task(task);
	struct dataflash_bdev *df_bdev = dataflash_bdev_of(df_breq->breq.bdev);
	bool                  done;

	done = at45_wait_ready(&df_bdev->at45d);
	if (!done) {
		// task will be re-sceduled on new wait event so just return
		return;
	}
	dataflash_write_ready(task);
}

static void dataflash_write_buffered(struct workqueue_task *task)
{
	struct dataflash_breq *df_breq = dataflash_breq_of_task(task);
	struct dataflash_bdev *df_bdev = dataflash_bdev_of(df_breq->breq.bdev);

	at45_deselect(&df_bdev->at45d);
	workqueue_task_set_work_func(&df_breq->task,
			dataflash_write_buffered_wait);
	dataflash_write_buffered_wait(task);
}

static void dataflash_write_setup(struct workqueue_task *task)
{
	struct dataflash_breq *df_breq = dataflash_breq_of_task(task);
	struct dataflash_bdev *df_bdev = dataflash_bdev_of(df_breq->breq.bdev);

	workqueue_task_set_work_func(&df_breq->task, dataflash_write_buffered);

	df_bdev->transfer_pos = 0;

	if (dataflash_is_page_aligned(df_breq))
		dataflash_write_ready(task);
	else {
		/* For writes not covering a whole page it needs to read into
		 * buffer, modify buffer and write back page.
		 */
		at45_select(&df_bdev->at45d);
		at45_cmd_main_memory_to_buffer_1_transfer(&df_bdev->at45d,
				df_breq->lba >> 1);
	}
}

static void dataflash_start(struct workqueue_task *task)
{
	struct dataflash_breq *df_breq = dataflash_breq_of_task(task);

	switch (df_breq->operation) {
	case BLK_OP_READ:
		dbg_verbose("DataFlash: reading %ld blocks @ 0x%04lx ...\n",
				df_breq->remaining_blocks, df_breq->lba);
		dataflash_read_setup(task);
		break;
	case BLK_OP_WRITE:
		dbg_verbose("DataFlash: writing %ld blocks @ 0x%04lx ...\n",
				df_breq->remaining_blocks, df_breq->lba);
		dataflash_write_setup(task);
		break;
	default:
		unhandled_case(df_breq->operation);
	}
}

//! \see block_submit_req
static void dataflash_submit(struct block_device *bdev,
		struct block_request *breq)
{
	struct dataflash_bdev *df_bdev = dataflash_bdev_of(bdev);
	struct dataflash_breq *df_breq = dataflash_breq_of(breq);

	at45_request(&df_bdev->at45d, &df_breq->task);
}

//! \see block_submit_buf_list
static int dataflash_submit_buf_list(struct block_device *bdev,
		struct block_request *breq, struct slist *buf_list)
{
	struct dataflash_breq *df_breq = dataflash_breq_of(breq);

	dbg_verbose("DataFlash: submit_buf_list\n");
	slist_move_to_tail(&breq->buf_list, buf_list);
	if (df_breq->sleeping) {
		dbg_verbose("DataFlash: wakeup\n");
		df_breq->sleeping = false;
		workqueue_add_task(&main_workqueue, &df_breq->task);
	}

	return STATUS_OK;
}

//! \see block_prepare_req
static void dataflash_prepare_req(struct block_device *bdev,
		struct block_request *breq,
		block_addr_t lba, block_len_t nr_blocks,
		enum block_operation operation)
{
	struct dataflash_breq *df_breq = dataflash_breq_of(breq);

	breq->status = OPERATION_IN_PROGRESS;
	breq->bytes_xfered = 0;
	workqueue_task_init(&df_breq->task, dataflash_start);
	df_breq->lba = lba;
	df_breq->remaining_blocks = nr_blocks;
	df_breq->operation = operation;
	df_breq->sleeping = false;
}

//! \see block_alloc_request
static struct block_request *dataflash_alloc_req(struct block_device *bdev)
{
	struct dataflash_bdev *df_bdev = dataflash_bdev_of(bdev);
	struct dataflash_breq *df_breq;

	df_breq = mem_pool_alloc(&df_bdev->req_pool);
	if (!df_breq)
		return NULL;

	slist_init(&df_breq->breq.buf_list);
	df_breq->breq.bdev = bdev;
	df_breq->breq.req_submit = dataflash_submit;
	df_breq->breq.req_submit_buf_list = dataflash_submit_buf_list;

	return &df_breq->breq;
}

//! \see block_free_request
static void dataflash_free_req(struct block_device *bdev,
		struct block_request *breq)
{
	struct dataflash_bdev *df_bdev = dataflash_bdev_of(bdev);
	struct dataflash_breq *df_breq = dataflash_breq_of(breq);

	mem_pool_free(&df_bdev->req_pool, df_breq);
}

static inline struct dataflash_bdev *dataflash_bdev_of_task(
		struct workqueue_task *task)
{
	return container_of(task, struct dataflash_bdev, task);
}

/**
 * \brief Scan for AT45 DataFlash device
 *
 * \param task Workqueue task
 */
static void dataflash_detect(struct workqueue_task *task)
{
	struct dataflash_bdev *df_bdev = dataflash_bdev_of_task(task);
	bool                  done;

	done = at45_identify(&df_bdev->at45d);
	if (!done) {
		/* task will be re-sceduled on new identify event so just
		 * return.
		 */
		return;
	}

	if (test_bit(AT45_FLAG_VALID, &df_bdev->at45d.flags)) {
		if (!test_bit(AT45_FLAG_PROTECTED, &df_bdev->at45d.flags))
			set_bit(BDEV_WRITEABLE, &df_bdev->bdev.flags);

		blkdev_set_block_size(&df_bdev->bdev, DATAFLASH_BLOCK_SIZE);
		df_bdev->bdev.nr_blocks =
				df_bdev->at45d.size / DATAFLASH_BLOCK_SIZE;
		if (!dataflash_store_page_size(df_bdev,
				df_bdev->at45d.page_size))
			dbg_warning("DataFlash: Unsupported page size!\n");
		else
			set_bit(BDEV_PRESENT, &df_bdev->bdev.flags);
	}

	workqueue_add_task(&main_workqueue, df_bdev->event_task);
}

//! @}

/**
 * \ingroup block_device_dataflash_group
 * \brief Initialize a DataFlash block device
 *
 * Will allocated memory and initialize internals, and will then start a
 * DataFlash detection sequence. Once this sequence is completed it will run
 * the workqueue task \a event_task.
 *
 * \param spi_id     \ref spi_module_id
 * \param master     SPI master struct
 * \param device     SPI device struct
 * \param event_task Workqueue task to run on an event
 *
 * \return A new DataFlash block device, or NULL if insufficient memory is
 * available.
 */
struct block_device *dataflash_blkdev_init(spi_id_t spi_id,
		struct spi_master *master, struct spi_device *device,
		struct workqueue_task *event_task)
{
	struct dataflash_bdev *df_bdev;

	df_bdev = zalloc(sizeof(struct dataflash_bdev));
	if (!df_bdev)
		return NULL;

	at45_device_init(&df_bdev->at45d, spi_id, master, device);
	df_bdev->bdev.prepare_req = dataflash_prepare_req;
	df_bdev->bdev.alloc_req = dataflash_alloc_req;
	df_bdev->bdev.free_req = dataflash_free_req;
	df_bdev->event_task = event_task;
	slist_init(&df_bdev->current_buf_list);

	mem_pool_init_physmem(&df_bdev->req_pool, &cpu_sram_pool,
			4, sizeof(struct dataflash_breq), 2);

	// Start up detection task
	workqueue_task_init(&df_bdev->task, dataflash_detect);
	at45_request(&df_bdev->at45d, &df_bdev->task);

	return &df_bdev->bdev;
}


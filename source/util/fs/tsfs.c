/**
 * \file
 *
 * \brief Tiny Simple File System (TSFS)
 *
 * Copyright (C) 2010 Atmel Corporation. All rights reserved.
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
#include <dma.h>
#include <util.h>
#include <assert.h>
#include <malloc.h>
#include <stdint.h>
#include <string.h>
#include <byteorder.h>
#include <status_codes.h>

#include <board/physmem.h>

#include <fs/tsfs.h>

#ifdef CONFIG_FS_TSFS_USE_HUGEMEM
# include <hugemem.h>
#endif

/**
 * \ingroup tsfs_group
 *
 * @{
 */
#ifdef CONFIG_FS_TSFS_USE_HUGEMEM
static void tsfs_read_filetable_page_done(struct tsfs *tsfs);
#endif

/**
 * \brief Block device list complete callback.
 */
static void tsfs_buf_list_done(struct block_device *bdev,
		struct block_request *breq, struct slist *buf_list)
{
	/* Intentionally empty. */
}

/**
 * \brief Block device request complete callback.
 */
static void tsfs_read_page_done(struct block_device *bdev,
		struct block_request *breq)
{
	struct tsfs *tsfs = breq->context;

	block_free_request(bdev, breq);
	tsfs->current_breq = NULL;

	tsfs->page_read_callback(tsfs);
}


static void tsfs_read_page(struct tsfs *tsfs, block_addr_t lba,
		void (*callback_func)(struct tsfs *tsfs))
{
	struct block_device *bdev = tsfs->bdev;

	// Busy reading another file?
	assert(!tsfs->current_breq);

	tsfs->page_read_callback    = callback_func;
	tsfs->current_breq          = block_alloc_request(bdev);
	tsfs->lba_in_buf            = lba;

	// Prepare to read one block of data from the block device.
	block_prepare_req(bdev, tsfs->current_breq, lba, 1, BLK_OP_READ);

	tsfs->current_breq->req_done = tsfs_read_page_done;
	tsfs->current_breq->buf_list_done = tsfs_buf_list_done;
	tsfs->current_breq->context = tsfs;

	buffer_init_rx(&tsfs->buffer, tsfs->buffer_data, TSFS_BLOCKSIZE);
	blk_req_add_buffer(tsfs->current_breq, &tsfs->buffer);

	block_submit_req(bdev, tsfs->current_breq);
}

static void tsfs_parse_filetable_from_buffer(struct tsfs *tsfs,
		uint_fast8_t offset_filetable, uint_fast8_t offset_block,
		uint_fast8_t nr_entries)
{
	struct tsfs_filetable_entry     ft_entry;
	uint_fast8_t                    size;
	uint_fast8_t                    i;

	size = sizeof(struct tsfs_filetable_entry);

	for (i = 0; i < nr_entries; i++) {
#ifdef CONFIG_FS_TSFS_USE_HUGEMEM
		uint16_t        mem_table_offset;
#endif
		uint_fast16_t   mem_block_offset = (offset_block + i) * size;

		memcpy(&ft_entry, tsfs->buffer_data + mem_block_offset, size);

		ft_entry.file_offset = be32_to_cpu(ft_entry.file_offset);
		ft_entry.file_size = be32_to_cpu(ft_entry.file_size);

#ifdef CONFIG_FS_TSFS_USE_HUGEMEM
		mem_table_offset = (offset_filetable + i) * size;
		hugemem_write_block((hugemem_ptr_t)((phys_addr_t)
					tsfs->filetable_address
					+ mem_table_offset), &ft_entry, size);
#else
		memcpy(&tsfs->filetable[offset_filetable + i], &ft_entry, size);
#endif
	}

#ifdef CONFIG_FS_TSFS_USE_HUGEMEM
	tsfs->filetable_entries_read += nr_entries;

	if (tsfs->filetable_entries_read < tsfs->header.nr_files) {
		// More entries to read, queue reading of another page.
		block_addr_t next_lba = tsfs->lba_in_buf + 1;
		tsfs_read_page(tsfs, next_lba, tsfs_read_filetable_page_done);
	} else {
#endif
		/* Done reading filetable. flag system as ready and schedule
		 * task if required.
		 */
		tsfs->status = STATUS_OK;

		if (tsfs->current_read_request.task) {
			workqueue_add_task(&main_workqueue,
					tsfs->current_read_request.task);
		}
#ifdef CONFIG_FS_TSFS_USE_HUGEMEM
	}
#endif
}

#ifdef CONFIG_FS_TSFS_USE_HUGEMEM
static void tsfs_read_filetable_page_done(struct tsfs *tsfs)
{
	uint_fast8_t    offset_filetable = tsfs->filetable_entries_read;

	tsfs_parse_filetable_from_buffer(tsfs, offset_filetable, 0,
			TSFS_FILETABLE_ENTRIES_PER_BLOCK);
}
#endif

static void tsfs_read_header_done(struct tsfs *tsfs)
{
	memcpy(&tsfs->header, tsfs->buffer_data, sizeof(tsfs->header));

	tsfs->header.id             = be16_to_cpu(tsfs->header.id);
	tsfs->header.volume_size    = be32_to_cpu(tsfs->header.volume_size);
	tsfs->header.nr_files       = be32_to_cpu(tsfs->header.nr_files);

	if (tsfs->header.nr_files > TSFS_MAX_FILES)
		tsfs->header.nr_files = TSFS_MAX_FILES;

	if (tsfs->header.id == TSFS_ID) {
		uint8_t entries_to_read = TSFS_FILETABLE_ENTRIES_PER_BLOCK;

		// substract one because of TSFS header structure
		entries_to_read -= 1;

		uint8_t nr_to_read = min_u(tsfs->header.nr_files, entries_to_read );

		tsfs_parse_filetable_from_buffer(tsfs, 0, 1, nr_to_read);
	} else {
		tsfs->status = ERR_BAD_FORMAT;

		if (tsfs->current_read_request.task) {
			workqueue_add_task(&main_workqueue,
					tsfs->current_read_request.task);
		}
	}
}

static void read_header(struct tsfs *tsfs)
{
	tsfs_read_page(tsfs, 0, tsfs_read_header_done);
}

/**
 * \brief Initiates a Tiny Simple File System
 *
 * The block device specified by \a system_blockdevice has to have a block size
 * of \ref TSFS_BLOCKSIZE bytes.
 *
 * Immediately after returning from this function, the file system will have
 * status \ref ERR_BUSY. The system is ready for use when the status changes to
 * \ref STATUS_OK. This can be polled for, or the parameter \a init_done_task
 * can be used to specify a task to be scheduled when the system is ready.
 *
 * In case the data loaded from the block device is not TSFS compatible the
 * file system will not schedule the callback task, and set its own status to
 * \ref ERR_BAD_FORMAT. At this point the file system has to be re-initialized
 * with a valid block device if it is to be used.
 *
 * \param tsfs TSFS structure which holds file system information
 * \param bdev Block device to read data from
 * \param init_done_task Callback task for when system is ready for use
 */
status_t tsfs_init(struct tsfs *tsfs, struct block_device *bdev,
		struct workqueue_task *init_done_task)
{
	tsfs->buffer_data = malloc(TSFS_BLOCKSIZE);
	if (!tsfs->buffer_data)
		return ERR_NO_MEMORY;

	tsfs->bdev      = bdev;
	tsfs->status    = ERR_BUSY;

	tsfs->current_read_request.task = init_done_task;
#ifdef CONFIG_FS_TSFS_USE_HUGEMEM
	tsfs->filetable_address =
		(hugemem_ptr_t)physmem_alloc(&board_extram_pool,
				sizeof(struct tsfs_filetable_entry) *
				TSFS_MAX_FILES, CPU_DMA_ALIGN);

	assert(tsfs->filetable_address != PHYSMEM_ALLOC_ERR);
#endif
	read_header(tsfs);

	return STATUS_OK;
}

static void tsfs_read_buffer_ready(struct tsfs *tsfs)
{
	uint16_t        copy_start;
	uint16_t        copy_len;
	void            *buffer;

	// Figure out where in block buffer to read from and to.
	copy_start  = tsfs->current_read_request.cursor % TSFS_BLOCKSIZE;
	copy_len    = min_u(TSFS_BLOCKSIZE - copy_start,
			tsfs->current_read_request.remaining_bytes);

	buffer = tsfs->buffer_data + copy_start;

	memcpy(tsfs->current_read_request.buffer, buffer, copy_len);

	//update metadata with remaining bytes to be read, and buffer postition
	tsfs->current_read_request.buffer=
		(uint8_t *)tsfs->current_read_request.buffer+ copy_len;
	tsfs->current_read_request.cursor += copy_len;
	tsfs->current_read_request.remaining_bytes -= copy_len;

	if (tsfs->current_read_request.remaining_bytes > 0) {
		// More entries to read, queue reading of another page.
		block_addr_t next_lba =
			tsfs->current_read_request.cursor
			>> ilog2(TSFS_BLOCKSIZE);
		tsfs_read_page(tsfs, next_lba, tsfs_read_buffer_ready);
	} else {
		/* Done reading, flag that the file system is ready and
		 * schedule user task.
		 */
		tsfs->status = STATUS_OK;
		workqueue_add_task(&main_workqueue,
				tsfs->current_read_request.task);
	}
}

/**
 * \brief Reads a chunk of data from a file to a buffer
 *
 * Any file system can only issue one read request at a time, and hence if the
 * system is busy reading another file \a tsfs_read will return \ref ERR_BUSY.
 * The user application should pick up on this and reschedule its read attempt.
 *
 * The \a task is scheduled to execute once the copy operation is complete, but
 * only if \a tsfs_read returns \ref STATUS_OK. If it returns any error
 * message, the task is \a not \a scheduled.
 *
 * The \a length parameter is trimmed to never exceed the length of the file.
 *
 * \param tsfs TSFS structure which holds file system information
 * \param file Handle of the file to be read from
 * \param buffer Pointer to buffer where data is read to
 * \param length Number of bytes to read
 * \param task Task to be scheduled upon successful completion
 *
 * \retval \ref STATUS_OK if successful
 * \retval \ref ERR_INVALID_ARG if trying to read at the end of a file, or
 *         trying to read 0 bytes.
 * \retval \ref ERR_BUSY if file system is busy reading from another file.
 */
status_t tsfs_read(struct tsfs *tsfs, struct tsfs_file *file,
		void *buffer, uint32_t length, struct workqueue_task *task)
{
	block_addr_t block_nr;

	if (tsfs->status != STATUS_OK)
		return ERR_BUSY;

	// check and limit range of length of data to be read.
	if (file->cursor + length > file->end)
		length = file->end - file->cursor;
	if (!length)
		return ERR_INVALID_ARG;

	tsfs->status = ERR_BUSY;

	tsfs->current_read_request.task = task;
	tsfs->current_read_request.buffer = buffer;
	tsfs->current_read_request.cursor = file->cursor;
	tsfs->current_read_request.remaining_bytes = length;

	// Check which block we are reading from.
	block_nr = file->cursor >> ilog2(TSFS_BLOCKSIZE);

	if (tsfs->lba_in_buf == block_nr) {
		// Block already in buffer, perform first read directly.
		tsfs_read_buffer_ready(tsfs);
	} else {
		// Schedule a block read to buffer.
		tsfs_read_page(tsfs, block_nr, tsfs_read_buffer_ready);
	}

	// Move file cursor.
	file->cursor += length;

	return STATUS_OK;
}

static void tsfs_get_filetable_entry(struct tsfs *tsfs, uint8_t file_index,
		struct tsfs_filetable_entry *ft_entry)
{
	uint8_t size = sizeof(struct tsfs_filetable_entry);

#ifdef CONFIG_FS_TSFS_USE_HUGEMEM
	hugemem_read_block(ft_entry,
			(hugemem_ptr_t)((phys_addr_t)tsfs->filetable_address
				+ (size * file_index)), size);
#else
	memcpy(ft_entry, &tsfs->filetable[file_index], size);
#endif
}

static uint32_t tsfs_locate_file_in_table(struct tsfs *tsfs,
		const char *filename)
{
	struct tsfs_filetable_entry     ft_entry;
	uint32_t                        file_index = 0;
	char                            name_buffer[TSFS_FILENAME_LEN];

	while (file_index < tsfs->header.nr_files) {
		tsfs_get_filetable_entry(tsfs, file_index, &ft_entry);

		memcpy(name_buffer, ft_entry.filename, TSFS_FILENAME_LEN);

		if (strncmp(name_buffer, filename, TSFS_FILENAME_LEN) == 0)
			return file_index;

		file_index++;
	}

	return file_index;
}

/**
 * \brief Opens a file by populating a file handle
 *
 * Searches through the filetable of \a tsfs and opens the first file that
 * matches the \a filename string. Note that this can be a pointer to any
 * normal character array, even though TSFS filenames do not have a termchar
 *
 * \param tsfs TSFS structure which holds file system information
 * \param filename Name of file to be opened
 * \param filehandle File structure to store results
 *
 * \retval \ref STATUS_OK if successful.
 * \retval \ref ERR_INVALID_ARG if file is not found.
 */
status_t tsfs_open(struct tsfs *tsfs, const char *filename,
		struct tsfs_file *filehandle)
{
	struct tsfs_filetable_entry     entry;
	uint32_t                        file_index;

	file_index = tsfs_locate_file_in_table(tsfs, filename);

	/* If returned file_index (starts at index 0) is greater or equal to
	 * number of files, then the file was not found.
	 */
	if (file_index >= tsfs->header.nr_files)
		return ERR_INVALID_ARG;

	tsfs_get_filetable_entry(tsfs, file_index, &entry);

	filehandle->start   = entry.file_offset;
	filehandle->cursor  = entry.file_offset;
	filehandle->end     = entry.file_offset + entry.file_size;

	return STATUS_OK;
}

/**
 * \brief Manualy move the file cursor
 *
 * Available offsets are found in \ref tsfs_seek_origin. Note that to offset
 * inwards from the end of the file, negative values should be used for \a
 * offset. It is possible to seek the file cursor to any value, even invalid
 * ones, so care should be taken when using this function. If an invalid value
 * is passed for \a origin, the application will execute \ref unhandled_case()
 *
 * \param file File to seek in
 * \param offset Number of bytes to move the cursor
 * \param origin Position from which offset is counted
 *
 * \return Returns \ref STATUS_OK if successful
 */
status_t tsfs_seek(struct tsfs_file* file, int32_t offset,
		enum tsfs_seek_origin origin)
{
	switch(origin) {
	case SEEK_SET:
		file->cursor = file->start + offset;
		break;

	case SEEK_CUR:
		file->cursor += offset;
		break;

	case SEEK_END:
		file->cursor = file->end + offset;
		break;

	default:
		unhandled_case(origin);
		break;
	}

	return STATUS_OK;
}

/**
 * \brief Reads the filename of an indexed file into a buffer.
 *
 * \note buffer must be able to hold at least \ref TSFS_FILENAME_LEN bytes.
 *
 * \param tsfs TSFS structure which holds file system information
 * \param file_index Index of file which name is to be fetched
 * \param buffer Buffer where filename will be written
 */
void tsfs_get_filename(struct tsfs *tsfs, uint_fast8_t file_index,
		uint8_t *buffer)
{
	struct tsfs_filetable_entry     entry;
	tsfs_get_filetable_entry(tsfs, file_index, &entry);
	memcpy(buffer, entry.filename, TSFS_FILENAME_LEN);
}

//! @}

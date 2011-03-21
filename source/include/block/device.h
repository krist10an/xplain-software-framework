/**
 * \file
 *
 * \brief Block Device interface
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
#ifndef BLOCK_DEVICE_H_INCLUDED
#define BLOCK_DEVICE_H_INCLUDED

#include <buffer.h>
#include <slist.h>
#include <types.h>
#include <status_codes.h>

/**
 * \defgroup block_device_group Block Device
 *
 * @{
 */

/**
 * \brief Type for holding a logical block address (LBA)
 */
typedef uint32_t block_addr_t;

/**
 * \brief Type for holding a block length (i.e. number of blocks)
 */
typedef uint32_t block_len_t;

struct block_device;

/**
 * \brief A block device request
 *
 * This structure represents an asynchronous request to a block
 * device. It is allocated by calling block_device::prepare_req().
 * After initializing all the necessary fields, call
 * block_request::req_submit() to submit it for I/O. When it is done,
 * block_request::req_done() will be called by the driver.
 */
struct block_request {
	/** List of buffers associated with this request */
	struct slist	buf_list;
	void (*req_started)(struct block_device *bdev,
			struct block_request *req);
	/**
	 * Function provided by the client to be called when this
	 * request is completed.
	 *
	 * \param bdev The Block Device that handled the request
	 * \param req The request that was completed.
	 */
	void (*req_done)(struct block_device *bdev, struct block_request *req);
	void (*buf_list_done)(struct block_device *bdev,
			struct block_request *req, struct slist *buf_list);
	void (*req_submit)(struct block_device *bdev,
			struct block_request *req);
	int (*req_submit_buf_list)(struct block_device *bdev,
			struct block_request *req, struct slist *buf_list);
	void (*req_abort)(struct block_device *bdev,
			struct block_request *req);
	/**
	 * Arbitrary data pointer associated with this request, for
	 * use by the submitter.
	 */
	void			*context;
	/**
	 * Status code set by the block device driver upon completion
	 * of this request.
	 *
	 * This field is always updated before req_done() is called, and
	 * before buf_list_done() if an error occurred during transfer
	 * of that particular buffer list. While the transfer is still
	 * in progress, including during the last buf_list_done()
	 * callback, it contains the value -STATUS_IN_PROGRESS.
	 */
	enum status_code	status;
	block_len_t		bytes_xfered;
	/** The block request to which this request belongs */
	struct block_device	*bdev;
};

/**
 * \brief Flags representing the state of a block device
 */
enum block_device_flag {
	BDEV_UNIT_ATTENTION,	//!< Information about the device changed
	BDEV_PRESENT,		//!< Device is present
	BDEV_WRITEABLE,		//!< Device can be written to
};

/**
 * \brief Block device operation codes
 */
enum block_operation {
	BLK_OP_READ,		//!< Read data from the device
	BLK_OP_WRITE,		//!< Write data to the device
};

/**
 * \brief A block device
 *
 * A block device can handle block-oriented requests like read and
 * write. Each request operates on one or more blocks.
 */
struct block_device {
	/** The total number of blocks the device can hold */
	uint32_t	nr_blocks;
#ifndef CONFIG_BLOCK_FIXED_BLOCK_SIZE
	//! \internal \see blkdev_get_block_size()
	uint16_t	block_size;
#endif
	/** Flags indicating the state of the device */
	uint8_t 	flags;
	/** \internal */
	void (*prepare_req)(struct block_device *bdev,
			struct block_request *req,
			block_addr_t lba, block_len_t nr_blocks,
			enum block_operation operation);
	/** \internal */
	struct block_request *(*alloc_req)(struct block_device *bdev);
	/** \internal */
	void (*free_req)(struct block_device *bdev, struct block_request *req);
	/** \internal */
	uint32_t (*get_dev_id)(struct block_device *bdev);
};

/**
 * \brief Return the block size of \a bdev.
 *
 * This returns the size in bytes of the smallest addressable unit that
 * make up \a bdev. It is highly recommended to use this function rather
 * than accessing block_device::block_size directly, as the latter may
 * be replaced by a compile-time constant in some applications. A
 * constant block size may eliminate some expensive calculations at run
 * time, thus making the code smaller and faster.
 *
 * \param bdev The block device to query
 * \return The block size of \a bdev in bytes
 */
static inline uint16_t blkdev_get_block_size(struct block_device *bdev)
{
#ifdef CONFIG_BLOCK_FIXED_BLOCK_SIZE
	return CONFIG_BLOCK_FIXED_BLOCK_SIZE;
#else
	return bdev->block_size;
#endif
}

/**
 * \brief Set the block size of \a bdev.
 *
 * This changes the block size of \a bdev to be \a block_size bytes.
 * If CONFIG_BLOCK_FIXED_BLOCK_SIZE is set, this is the only legal value
 * for \a block_size.
 *
 * \param bdev The block device to query
 * \param block_size The new block size of \a bdev in bytes
 */
static inline void blkdev_set_block_size(struct block_device *bdev,
		uint16_t block_size)
{
#ifdef CONFIG_BLOCK_FIXED_BLOCK_SIZE
	assert(block_size == CONFIG_BLOCK_FIXED_BLOCK_SIZE);
#else
	bdev->block_size = block_size;
#endif
}

extern struct block_request *block_alloc_request(struct block_device *bdev);
extern void block_free_request(struct block_device *bdev,
		struct block_request *req);

/**
 * \brief Prepare a block request
 *
 * This function will initialize a block_request structure with default
 * values (no buffers, no bytes transferred, etc.) The req_submit()
 * field will point to the function that will start the operation
 * indicated by \a operation.
 *
 * \param bdev The block device which should handle the request
 * \param req The block request to be initialized
 * \param lba The Logical Block Address of the first block
 * \param nr_blocks The number of blocks to operate on
 * \param operation One of the operations defined by block_operation.
 *
 * \pre \a bdev is the block device for which \a req was allocated
 */
static inline void block_prepare_req(struct block_device *bdev,
		struct block_request *req, block_addr_t lba,
		block_len_t nr_blocks, enum block_operation operation)
{
	assert(bdev == req->bdev);

	bdev->prepare_req(bdev, req, lba, nr_blocks, operation);
}

/**
 * \brief Submit a request for I/O.
 *
 * \param bdev The Block Device to handle the request.
 * \param req This request
 *
 * \pre \a bdev is the block device for which \a req was allocated and
 * prepared.
 */
static inline void block_submit_req(struct block_device *bdev,
		struct block_request *req)
{
	assert(bdev == req->bdev);

	req->req_submit(bdev, req);
}

static inline void block_queue_req(struct block_device *bdev,
		struct block_request *req, block_addr_t lba,
		block_len_t nr_blocks, enum block_operation operation)
{
	block_prepare_req(bdev, req, lba, nr_blocks, operation);
	block_submit_req(bdev, req);
}

static inline void block_abort_req(struct block_device *bdev,
		struct block_request *req)
{
	/* FIXME */
}

static inline uint32_t block_get_dev_id(struct block_device *bdev)
{
	assert(bdev);
	assert(bdev->get_dev_id);

	return bdev->get_dev_id(bdev);
}

/**
 * \brief Submit a list of buffers for an already-queued request
 *
 * This function will atomically submit a list of buffers for
 * transfering data associated with a block request. If the request has
 * already failed before the buffers are queued, this function will
 * leave the buffers alone and return -STATUS_FLUSHED. The caller is
 * responsible for cleaning up the buffer list when this happens.
 *
 * \param bdev The block device handling the request
 * \param breq The block request associated with the buffers
 * \param buf_list List of buffers to be submitted
 *
 * \retval 0 The buffers were successfully submitted
 * \retval -STATUS_FLUSHED \a breq is not queued (i.e. it might have
 * encountered an error.)
 */
static inline int block_submit_buf_list(struct block_device *bdev,
		struct block_request *breq, struct slist *buf_list)
{
	assert(breq->bdev == bdev);
	assert(breq->req_submit_buf_list);

	return breq->req_submit_buf_list(bdev, breq, buf_list);
}

/**
 * \brief Get the number of blocks actually transfered for a request.
 *
 * This function returns the number of blocks actually transfered. May
 * be less than requested if an error occurred.
 *
 * This number is updated before each time buf_list_done() and
 * req_done() is called. Note that when buf_list_done() is called for
 * write requests, this indicates the number of blocks transfered to the
 * controller, which may be more than the number of blocks written to
 * the underlying storage.
 */
static inline block_len_t blk_req_get_blocks_xfered(struct block_device *bdev,
		struct block_request *breq)
{
	assert(bdev == breq->bdev);

	return breq->bytes_xfered / blkdev_get_block_size(bdev);
}

/**
 * \brief Get the number of bytes actually transfered for a request.
 *
 * This function returns the number of bytes actually transfered. May be
 * less than requested if an error occurred.
 *
 * This number is updated before each time buf_list_done() and
 * req_done() is called. Note that when buf_list_done() is called for
 * write requests, this indicates the number of bytes transfered to the
 * controller, which may be more than the number of bytes written to the
 * underlying storage.
 */
static inline block_len_t blk_req_get_bytes_xfered(struct block_device *bdev,
		struct block_request *breq)
{
	assert(bdev == breq->bdev);

	return breq->bytes_xfered;
}

/**
 * \brief Associate the buffer \a buf with the block request \a req
 */
static inline void blk_req_add_buffer(struct block_request *req,
		struct buffer *buf)
{
	slist_insert_tail(&req->buf_list, &buf->node);
}

/**
 * \brief Associated the list of buffers \a list with the block
 * request \a req.
 *
 * \a list will be empty when this function returns
 */
static inline void blk_req_add_buffer_list(struct block_request *req,
		struct slist *list)
{
	slist_move_to_tail(&req->buf_list, list);
}

//! @}

#endif /* BLOCK_DEVICE_H_INCLUDED */

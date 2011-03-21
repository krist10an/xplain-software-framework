/**
 * \file
 *
 * \brief Generic Buffer structure and associated helper functions.
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
#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include <dma.h>
#include <slist.h>
#include <types.h>

/**
 * \defgroup buffer_group Generic Buffers
 *
 * This is a generic buffer data structure used by several subsystems.
 * Chains of buffers may be passed around between different drivers,
 * minimizing the cost of copying and data conversion.
 *
 * @{
 */

/**
 * \brief A generic data buffer
 *
 * This buffer represents a physically and virtually contiguous memory
 * area used for data storage.
 */
struct buffer {
	/** DMA descriptor associated with this buffer, if any. */
	void			*dma_desc;
	/** Address of the data area */
	dma_addr_t		addr;
	/** Length of the data area in bytes. */
	size_t			len;
	/** List node for use by the owner of the buffer */
	struct slist_node	node;
};

/**
 * \brief Initialize a buffer for transmitting data
 * \param buf The buffer
 * \param data Data area to associate with the buffer
 * \param len Length of the data area in bytes
 */
static inline void buffer_init_tx(struct buffer *buf,
		const void *data, size_t len)
{
	buf->addr = dma_map_single(data, len, DMA_TO_DEVICE);
	buf->len = len;
}

/**
 * \brief Initialize an already-DMA-mapped buffer for transmitting
 * data
 *
 * \param buf The buffer
 * \param addr Physical/virtual address of the data DMA mapping
 * \param len Length of the data area in bytes
 */
static inline void buffer_init_tx_mapped(struct buffer *buf,
		dma_addr_t addr, size_t len)
{
	buf->addr = addr;
	buf->len = len;
}

/**
 * \brief Initialize a buffer for receiving data
 * \param buf The buffer
 * \param data Data area to associate with the buffer
 * \param len Length of the data area in bytes
 */
static inline void buffer_init_rx(struct buffer *buf, void *data, size_t len)
{
	buf->addr = dma_map_single(data, len, DMA_FROM_DEVICE);
	buf->len = len;
}

/**
 * \brief Initialize an already-DMA-mapped buffer for receiving data
 *
 * \param buf The buffer
 * \param addr Physical/virtual address of the data DMA mapping
 * \param len Length of the data area in bytes
 */
static inline void buffer_init_rx_mapped(struct buffer *buf,
		dma_addr_t addr, size_t len)
{
	buf->addr = addr;
	buf->len = len;
}

extern struct buffer *buffer_alloc(void);
extern void buffer_free(struct buffer *buf);
extern void buffer_resize(struct buffer *buf, size_t new_size);

extern struct buffer *buffer_dma_alloc(size_t size);
extern void buffer_dma_free(struct buffer *buf, size_t size);

extern void buffer_pool_init(void);

/**
 * \name Buffer List Manipulation
 * The following macros are simple wrappers around the generic
 * singly-linked-list functionality designed to make it easier to
 * operate on singly linked lists of buffers (which is a very common
 * thing to put on singly linked lists).
 */
//@{

//! Convert a slist node to a buffer struct
#define buf_list_entry(slnode)					\
	slist_entry(slnode, struct buffer, node)
//! Return the first buffer in \a list
#define buf_list_peek_head(list)				\
	slist_peek_head(list, struct buffer, node)
//! Return the last buffer in \a list
#define buf_list_peek_tail(list)				\
	slist_peek_tail(list, struct buffer, node)
//! Return the buffer following \a buf in the list
#define buf_list_peek_next(buf)					\
	slist_peek_next(&(buf)->node, struct buffer, node)
//! Return the first buffer in \a list and remove it
#define buf_list_pop_head(list)					\
	slist_pop_head(list, struct buffer, node)

//@}

//! @}

#endif /* BUFFER_H_INCLUDED */

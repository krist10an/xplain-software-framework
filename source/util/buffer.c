/**
 * \file
 *
 * \brief Generic Buffer management
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
#include <buffer.h>
#include <mempool.h>
#include <physmem.h>

static struct mem_pool	buffer_pool;

/**
 * \brief Allocate a buffer structure
 * \return A new buffer structure, or NULL if the buffer pool is
 * exhausted.
 */
struct buffer *buffer_alloc(void)
{
	struct buffer	*buf;

	buf = mem_pool_alloc(&buffer_pool);
	buf->dma_desc = NULL;

	return buf;
}

/**
 * \brief Free the buffer structure \a buf
 */
void buffer_free(struct buffer *buf)
{
	mem_pool_free(&buffer_pool, buf);
}

/**
 * \brief Resize the buffer pointed to by \a buf. The caller must make sure
 * the new size fit into the allocated memory space.
 */
void buffer_resize(struct buffer *buf, size_t new_size)
{
	assert(buf);

	buf->len = new_size;
}

#ifdef CONFIG_DMAPOOL_GENERIC_POOLS

#include <dmapool.h>

/**
 * \brief Allocate buffer structure with dma memory
 *
 * Allocates a buffer structure and dma memory of size \a size.
 * Initilalizes the buffer structure with dma memory parameters.
 *
 * \param size Size of the dma memory region to allocate
 * \return A new buffer structure, or NULL if the buffer pool or dma
 * pool is exhausted.
 */
struct buffer *buffer_dma_alloc(size_t size)
{
	struct buffer	*buf;
	dma_addr_t	addr;

	buf = buffer_alloc();
	if (buf == NULL)
		return NULL;

	addr = dma_alloc(size);
	if (dma_addr_is_failed(addr)) {
		buffer_free(buf);
		return NULL;
	}

	buf->addr = addr;
	buf->len = size;

	return buf;
}

/**
 * \brief Free buffer structure \a buf with dma memory
 */
void buffer_dma_free(struct buffer *buf, size_t size)
{
	dma_free(buf->addr, size);
	buffer_free(buf);
}
#endif /* CONFIG_DMAPOOL_GENERIC_POOLS */

/**
 * \brief Initialize the buffer pool
 *
 * This must be called by the application before any buffers may be
 * allocated from the pool.
 */
void buffer_pool_init(void)
{
	mem_pool_init_physmem(&buffer_pool, &cpu_sram_pool,
			CONFIG_NR_BUFFERS, sizeof(struct buffer), 2);
}

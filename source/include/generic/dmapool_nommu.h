/**
 * \file
 *
 * \brief DMA pool definitions for processors without an MMU
 *
 * \note Strictly speaking, MMU-less does not mean cache-less, yet these
 * primitives do not support processors with a cache. In practice,
 * however, this framework does not support any MMU-less devices with a
 * cache anyway.
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
#ifndef GENERIC_DMAPOOL_NOMMU_H_INCLUDED
#define GENERIC_DMAPOOL_NOMMU_H_INCLUDED

#include <dma.h>
#include <mempool.h>

/**
 * \weakgroup dmapool_group
 *
 * @{
 */

/**
 * \brief DMA memory pool
 *
 * This is just a wrapper around a memory pool, since we don't need to
 * do any additional bookkeeping on mmu-less devices.
 */
struct dma_pool {
	/**
	 * \internal
	 * \brief The underlying memory pool
	 */
	struct mem_pool	mempool;
};

static inline void dma_pool_init_coherent(struct dma_pool *pool,
		phys_addr_t phys_start, size_t size, size_t objsize,
		unsigned int align_order)
{
	mem_pool_init(&pool->mempool, (void *)(uintptr_t)phys_start, size,
			objsize, align_order);
}

static inline dma_addr_t dma_pool_alloc(struct dma_pool *pool)
{
	dma_addr_t	addr;

	/* This will also initialize phys since dma_addr_t is a union */
	addr.ptr = mem_pool_alloc(&pool->mempool);

	return addr;
}

static inline void dma_pool_free(struct dma_pool *pool, dma_addr_t addr)
{
	mem_pool_free(&pool->mempool, addr.ptr);
}

//! @}

#endif /* GENERIC_DMAPOOL_NOMMU_H_INCLUDED */

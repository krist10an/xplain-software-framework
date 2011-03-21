/**
 * \file
 *
 * \brief DMA memory pool allocator
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
#ifndef DMAPOOL_H_INCLUDED
#define DMAPOOL_H_INCLUDED

#include <cpu/dmapool.h>

/**
 * \ingroup mem_alloc_group
 * \defgroup dmapool_group DMA pool allocator
 *
 * This is a memory pool allocator for DMA memory. DMA memory differs
 * from "regular" memory in that both the physical and virtual addresses
 * are kept track of, and cache coherency issues are easier to deal
 * with.
 *
 * On cache- and MMU-less processors, DMA memory pools are essentially
 * identical to regular memory pools. Nevertheless, it is highly
 * recommended to utilize DMA memory for any memory which is to be used
 * in data transfers involving hardware, as it will make the code more
 * easily portable to other chips.
 *
 * @{
 */

void dma_pool_init_coherent_physmem(struct dma_pool *dmapool,
		struct physmem_pool *phys_pool, unsigned int nr_objects,
		size_t objsize, unsigned int align_order);

/**
 * \fn void dma_pool_init_coherent(struct dma_pool *pool,
 *		phys_addr_t phys_start, size_t size, size_t objsize,
 *		unsigned int align_order)
 * \brief Initialize a DMA-coherent memory pool
 *
 * This function will initialize a pool of fixed-size objects located in
 * DMA-coherent memory. Since the memory is coherent, no cache
 * synchronization is needed before and after DMA data transfers.
 *
 * \param pool The DMA pool to be initialized
 * \param phys_start The physical address of the underlying memory area
 * \param size The size of the underlying memory area in bytes
 * \param objsize The size of each object in bytes
 * \param align_order log2 of the minimum object alignment in bytes
 */
/**
 * \fn dma_addr_t dma_pool_alloc(struct dma_pool *pool)
 * \brief Allocate an object from a DMA pool
 *
 * \param pool The pool from which the object will be allocated
 */
/**
 * \fn void dma_pool_free(struct dma_pool *pool, dma_addr_t addr)
 * \brief Free an object previously allocated from \a pool
 *
 * \warning If an object is freed into a different pool than the one
 * from which it was allocated, or if the same object is freed multiple
 * times, the DMA pool will get corrupted and subsequent allocations may
 * return invalid objects or fail in other, unexpected ways.
 *
 * \param pool The pool from which the object was allocated
 * \param addr The physical and virtual address of the object
 */

// #ifdef CONFIG_DMAPOOL_GENERIC_POOLS
#include <assert.h>
#include <app/dmapool.h>

/**
 * \defgroup dmapool_generic_group Generic DMA pool allocator
 *
 * The <em>Generic DMA pool allocator</em> allows DMA objects to be
 * allocated without setting up a dedicated pool for them. When enabled,
 * the generic DMA pool allocator will set up two "generic" pools: One
 * for large objects and one for small objects. The application may
 * specify the number and size of these objects by providing the
 * following definitions in the configuration makefile:
 *   - #CONFIG_DMAPOOL_SMALL_OBJ_SIZE: The size of each "small" object
 *   - #CONFIG_DMAPOOL_NR_SMALL_OBJS: The number of "small" objects available
 *   - #CONFIG_DMAPOOL_LARGE_OBJ_SIZE: The size of each "large" object
 *   - #CONFIG_DMAPOOL_NR_LARGE_OBJS: The number of "large" objects available
 *
 * Based on the requested size, the smallest pool which is large enough
 * to satisfy the allocation is selected automatically. If the
 * application doesn't provide any of the above definitions for one of
 * the pools, that pool will be unavailable and all allocations will be
 * made from the other pool.
 *
 * The memory returned by the generic DMA pool allocator is coherent, so
 * there's no need to perform any cache synchronization.
 *
 * @{
 */

#ifdef CONFIG_DMAPOOL_SMALL_OBJ_SIZE
extern struct dma_pool dmapool_size_small;
#endif
#ifdef CONFIG_DMAPOOL_LARGE_OBJ_SIZE
extern struct dma_pool dmapool_size_large;
#endif

static inline struct dma_pool *dmapool_find_pool(size_t alloc_size)
{
#ifdef CONFIG_DMAPOOL_SMALL_OBJ_SIZE
	if (alloc_size <= CONFIG_DMAPOOL_SMALL_OBJ_SIZE)
		return &dmapool_size_small;
#endif
#ifdef CONFIG_DMAPOOL_LARGE_OBJ_SIZE
	if (alloc_size <= CONFIG_DMAPOOL_LARGE_OBJ_SIZE)
		return &dmapool_size_large;
#endif

	return NULL;
}

static inline dma_addr_t dma_alloc_inline(size_t size)
{
	struct dma_pool	*pool;

	pool = dmapool_find_pool(size);
	if (likely(pool))
		return dma_pool_alloc(pool);
	else
		return dma_addr_failure();
}

extern dma_addr_t dma_alloc_noninline(size_t size);

/**
 * \brief Allocate an object from the generic DMA pools
 *
 * This function picks the smallest generic DMA pool which can hold
 * objects of size greater than or equal to \a size and allocates an
 * object from it.
 *
 * \param size Minimum size of the object to allocate
 *
 * \return A physical/virtual address pair of an object of at least \a
 * size bytes, or NULL if the selected DMA pool is exhausted.
 */
static inline dma_addr_t dma_alloc(size_t size)
{
	if (is_constant(size))
		return dma_alloc_inline(size);
	else
		return dma_alloc_noninline(size);
}

static inline void dma_free_inline(dma_addr_t obj, size_t size)
{
	struct dma_pool	*pool;

	pool = dmapool_find_pool(size);
	assert(pool);
	dma_pool_free(pool, obj);
}

extern void dma_free_noninline(dma_addr_t obj, size_t size);

/**
 * \brief Free an object to the generic DMA pools
 *
 * This function picks the same generic DMA pool picked based on \a
 * size, and frees the object to it.
 *
 * \param obj The object to be freed.
 * \param size The minimum size of the object. Must be the same value
 * as was passed to dma_alloc() when this object was returned.
 */
static inline void dma_free(dma_addr_t obj, size_t size)
{
	if (is_constant(size))
		dma_free_inline(obj, size);
	else
		dma_free_noninline(obj, size);
}

extern void dma_pool_init(void);

//! @}

// #endif /* CONFIG_DMAPOOL_GENERIC_POOLS */

//! @}

#endif /* DMAPOOL_H_INCLUDED */

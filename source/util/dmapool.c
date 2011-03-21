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
#include <assert.h>
#include <dmapool.h>
#include <physmem.h>

#ifndef dma_pool_small_physmem_pool
# define dma_pool_small_physmem_pool	dma_sram_pool
#endif
#ifndef dma_pool_large_physmem_pool
# define dma_pool_large_physmem_pool	dma_sram_pool
#endif

/**
 * \brief Initialize a DMA pool using the physmem allocator
 *
 * This function will grab enough physical memory for \a nr_objects
 * from \a phys_pool and use this to initialize \a dmapool.
 *
 * \param dmapool The DMA pool to be initialized.
 * \param phys_pool The physical memory pool from which to grab memory
 * \param nr_objects The number of objects in the pool
 * \param objsize The size of each object in bytes.
 * \param align_order log2 of the minimum object alignment in bytes.
 *
 * \pre \a nr_objects must be at least one.
 * \pre There must be enough available memory in \a phys_pool.
 * \pre \a objsize must be larger than the size of a pointer.
 *
 * \sa dma_pool_init()
 */
void dma_pool_init_coherent_physmem(struct dma_pool *dmapool,
		struct physmem_pool *phys_pool, unsigned int nr_objects,
		size_t objsize, unsigned int align_order)
{
	phys_addr_t	pool_addr;
	size_t		pool_size;
	size_t		block_size;

	assert(dmapool);
	assert(phys_pool);
	assert(nr_objects > 0);

	block_size = round_up(objsize, align_order);
	pool_size = nr_objects * block_size;

	pool_addr = physmem_alloc(phys_pool, pool_size, align_order);
	assert(pool_addr != PHYSMEM_ALLOC_ERR);

	dma_pool_init_coherent(dmapool, pool_addr, pool_size,
			objsize, align_order);
}

#ifdef CONFIG_DMAPOOL_GENERIC_POOLS

#include <app/dmapool.h>

#ifdef CONFIG_DMAPOOL_SMALL_OBJ_SIZE
struct dma_pool dmapool_size_small;
#endif
#ifdef CONFIG_DMAPOOL_LARGE_OBJ_SIZE
struct dma_pool dmapool_size_large;
#endif

dma_addr_t dma_alloc_noninline(size_t size)
{
	return dma_alloc_inline(size);
}

void dma_free_noninline(dma_addr_t obj, size_t size)
{
	dma_free_inline(obj, size);
}

/**
 * \brief Initialize generic DMA object pools
 *
 * This function initializes the "generic" DMA objects pools used by
 * dma_alloc() and dma_free().
 */
void dma_pool_init(void)
{
#ifdef CONFIG_DMAPOOL_LARGE_OBJ_SIZE
	dma_pool_init_coherent_physmem(&dmapool_size_large,
			&dma_pool_large_physmem_pool,
			CONFIG_DMAPOOL_NR_LARGE_OBJS,
			CONFIG_DMAPOOL_LARGE_OBJ_SIZE, CPU_DMA_ALIGN);
#endif
#ifdef CONFIG_DMAPOOL_SMALL_OBJ_SIZE
	dma_pool_init_coherent_physmem(&dmapool_size_small,
			&dma_pool_small_physmem_pool,
			CONFIG_DMAPOOL_NR_SMALL_OBJS,
			CONFIG_DMAPOOL_SMALL_OBJ_SIZE, CPU_DMA_ALIGN);
#endif
}
#endif /* CONFIG_DMAPOOL_GENERIC_POOLS */

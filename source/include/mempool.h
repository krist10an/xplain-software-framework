/**
 * \file
 *
 * \brief Memory pool allocator
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
#ifndef MEMPOOL_H_INCLUDED
#define MEMPOOL_H_INCLUDED

#include <types.h>

/**
 * \ingroup mem_alloc_group
 * \defgroup mempool_group Memory Pool Allocator
 *
 * A memory pool contains a number of objects of the same size that
 * can be allocated in constant time. This is useful for allocating
 * request structures, etc.
 *
 * Memory pool objects are managed in Last-In-First-Out (LIFO) order
 * to maximize the chances of allocating a cache-hot object.
 * @{
 */

struct physmem_pool;

/**
 * \brief Opaque structure representing a free memory pool object.
 */
struct mem_pool_object;

/**
 * \brief Memory pool
 *
 * This structure represents a pool of fixed-size objects which can be
 * allocated using mem_pool_alloc().
 */
struct mem_pool {
	/**
	 * \internal
	 * \brief Pointer to the first free object in the pool.
	 */
	struct mem_pool_object	*freelist;
};

extern void mem_pool_init(struct mem_pool *pool, void *start,
		size_t size, size_t objsize, unsigned int align_order);

extern void mem_pool_init_physmem(struct mem_pool *mempool,
		struct physmem_pool *phys_pool, unsigned int nr_objects,
		size_t objsize, unsigned int align_order);

void *mem_pool_alloc(struct mem_pool *pool);
void mem_pool_free(struct mem_pool *pool, const void *obj);

//! @}

#endif /* MEMPOOL_H_INCLUDED */

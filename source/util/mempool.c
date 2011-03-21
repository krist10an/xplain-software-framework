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
#include <assert.h>
#include <debug.h>
#include <interrupt.h>
#include <mempool.h>
#include <physmem.h>
#include <util.h>
#include <stdint.h>

/**
 * \weakgroup mempool_group
 * @{
 */

struct mem_pool_object {
	struct mem_pool_object	*next;
};

/**
 * \brief Initialize a memory pool
 *
 * This function will initialize a memory pool, populating its
 * freelist with objects tightly packed into the specified memory
 * range.
 *
 * All objects in the pool are guaranteed to be aligned to a multiple
 * of \f$2^{align\_order}\f$ bytes. For example, if \a align_order is
 * 4, each object will start on a 16-byte boundary.
 *
 * \param pool The memory pool to be initialized.
 * \param start The memory area to be used for object storage.
 * \param size The size of the memory area in bytes.
 * \param objsize The size of each object in bytes.
 * \param align_order log2 of the minimum object alignment in bytes.
 *
 * \pre \a objsize must be larger than the size of a pointer.
 * \pre \a size must be large enough to hold at least one object.
 */
void mem_pool_init(struct mem_pool *pool, void *start, size_t size,
		size_t objsize, unsigned int align_order)
{
	struct mem_pool_object	*obj;
	struct mem_pool_object	*prev_obj;
	uintptr_t		aligned_start;
	unsigned int		offset;

	assert(pool);
	assert(start);
	assert((uintptr_t)start + size > (uintptr_t)start);

	aligned_start = round_up((uintptr_t)start, align_order);
	size -= (aligned_start - (uintptr_t)start);
	objsize = round_up(objsize, align_order);

	assert(objsize >= sizeof(struct mem_pool_object));
	assert(size >= objsize);

	obj = prev_obj = (void *)aligned_start;
	pool->freelist = obj;

	for (offset = objsize; (offset + objsize) <= size; offset += objsize) {
		obj = (void *)(aligned_start + offset);
		prev_obj->next = obj;
		prev_obj = obj;
	}
	obj->next = NULL;

	dbg_info("mempool @ %p initialized with %zu objects of size %zu\n",
			start, size / objsize, objsize);
}

/**
 * \brief Initialize a memory pool using the physmem allocator
 *
 * This function will grab enough physical memory for \a nr_objects
 * and use this to initialize \a mempool.
 *
 * \param mempool The memory pool to be initialized.
 * \param phys_pool The physical memory pool from which to grab memory
 * \param nr_objects The number of objects in the pool
 * \param objsize The size of each object in bytes.
 * \param align_order log2 of the minimum object alignment in bytes.
 *
 * \pre \a nr_objects must be at least one.
 * \pre There must be enough available memory in \a phys_pool.
 * \pre \a objsize must be larger than the size of a pointer.
 *
 * \sa mem_pool_init()
 */
void mem_pool_init_physmem(struct mem_pool *mempool,
		struct physmem_pool *phys_pool, unsigned int nr_objects,
		size_t objsize, unsigned int align_order)
{
	phys_addr_t	pool_addr;
	size_t		pool_size;
	size_t		block_size;
	void		*pool_vaddr;

	assert(mempool);
	assert(phys_pool);
	assert(nr_objects > 0);

	block_size = round_up(objsize, align_order);
	pool_size = nr_objects * block_size;

	pool_addr = physmem_alloc(phys_pool, pool_size, align_order);
	assert(pool_addr != PHYSMEM_ALLOC_ERR);

	pool_vaddr = physmem_map(pool_addr, pool_size,
			PHYS_MAP_WRBUF | PHYS_MAP_WRBACK);
	mem_pool_init(mempool, pool_vaddr, pool_size, objsize, align_order);
}

/**
 * \brief Allocate an object from a memory pool.
 *
 * \param pool The memory pool from which the object is allocated.
 * \return A pointer to the newly allocated object, or NULL if the
 *	pool is exhausted.
 */
void *mem_pool_alloc(struct mem_pool *pool)
{
	struct mem_pool_object	*obj;
	unsigned long		iflags;

	assert(pool);

	iflags = cpu_irq_save();
	obj = pool->freelist;
	if (obj)
		pool->freelist = obj->next;
	cpu_irq_restore(iflags);

	return obj;
}

/**
 * \brief Free an object previously allocated from a memory pool.
 *
 * The caller is responsible for making sure \a obj was originally
 * allocated from \a pool. If an object is freed into a different pool,
 * the integrity of the pool can no longer be guaranteed.
 *
 * \note The object ceases to exist the moment this function is called,
 * so even though this function casts away the const qualifier
 * internally, it doesn't technically modify the object. Having the
 * const qualifier there allows callers to keep const pointers to
 * objects which are never modified after initialization. Objects stored
 * in flash should obviously never be freed.
 *
 * \param pool The memory pool which the object belongs to.
 * \param obj The object to be freed.
 */
void mem_pool_free(struct mem_pool *pool, const void *obj)
{
	struct mem_pool_object	*free_obj;
	unsigned long		iflags;

	assert(pool);

	if (!obj)
		return;

	free_obj = (struct mem_pool_object *)obj;

	iflags = cpu_irq_save();
	free_obj->next = pool->freelist;
	pool->freelist = free_obj;
	cpu_irq_restore(iflags);
}

//! @}

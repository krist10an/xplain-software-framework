/**
 * \file
 *
 * \brief Memory bag allocator
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
#include <util.h>
#include <physmem.h>
#include <string.h>
#include <mempool.h>
#include <membag.h>
#include <app/membag.h>

//! Internal structure used by membag to keep track of memory
struct membag {
	size_t block_size; //!< Number of bytes per block in this bag.
	size_t num_blocks; //!< Total number of blocks.
	struct physmem_pool *phys_pool; //!< Pointer to physical pool to allocate mempool from
	struct mem_pool pool; //!< Memory pool used for allocation.

	void *start; //!< Pointer to start of this bag
	void *end; //!< Pointer to end of this bag
#ifdef CONFIG_MEMBAG_USE_TUNING
	struct membag_bagstats stats; //!< Keeps track of statistics for this bag
#endif
};

/**
 * \brief Macro for configuring memory bag allocator
 *
 * MEMBAG is used in user application to define the size and number of blocks
 * for each bag. Please refer to description in \ref membags.
 *
 * \param objsize Size of each block in this bag
 * \param nr_objs Number of blocks in this bag
 * \param pool Pointer to physmem_pool to allocate this memory from.
 * \sa physmem_pool
 */
#define MEMBAG(objsize, nr_objs, pool)			\
	{ .block_size = objsize, .num_blocks = nr_objs, .phys_pool = pool }

/**
 * \brief Memory bag
 *
 * This structure represents one or more different size bags which memory
 * can be allocated from. The fixed size memory bags avoid internal
 * fragmentation when memory is allocated and freed.
 *
 * The APP_MEMBAG_INITIALIZER macro needs to be defined in app/membags.h in the
 * user application using the MEMBAG macro. Here is an example:
 *
 * \code
 * define APP_MEMBAG_INITIALIZER	\
 *	MEMBAG(16, 2, &cpu_sram_pool),		\
 *	MEMBAG(32, 4, &cpu_sram_pool),
 * \endcode
 *
 * The definition above is used to configure membag to use one bag with 2
 * blocks of 16 bytes each and another bag with 4 blocks of 32 bytes each.
 * Both bags are here allocated from the cpu_sram_pool.
 *
 * \sa MEMBAG
 */
static struct membag membags[] = {
	APP_MEMBAG_INITIALIZER
};


/**
 * \internal Internal function for initializing each membag
 *
 * Initialize a membag by getting memory from physmem.
 *
 * This function essentially does the same as the \ref mem_pool_init_physmem
 * function, but keeps track of start and end of the allocated memory.
 *
 * \param mb Pointer to membag structure to initialize
 * \param align_order log2 of the minimum object alignment in bytes.
 * \pre Not in interrupt context.
 */
static void membag_pool_init_physmem(struct membag *mb, unsigned int align_order)
{
	phys_addr_t	pool_addr;
	size_t		pool_size;
	size_t		block_size;
	void		*pool_vaddr;

	assert(mb);
	assert(mb->phys_pool);
	assert(mb->block_size > 0);
	assert(mb->num_blocks > 0);

	block_size = round_up(mb->block_size, align_order);
	pool_size = mb->num_blocks * block_size;

	pool_addr = physmem_alloc(mb->phys_pool, pool_size, align_order);

	assert(pool_addr != PHYSMEM_ALLOC_ERR);

	pool_vaddr = physmem_map(pool_addr, pool_size,
			PHYS_MAP_WRBUF | PHYS_MAP_WRBACK);
	mem_pool_init(&mb->pool, pool_vaddr, pool_size, mb->block_size, align_order);
	mb->start = pool_vaddr;
	mb->end = (uint8_t *)pool_vaddr + pool_size;

#ifdef CONFIG_MEMBAG_USE_TUNING
	mb->stats.num_free_blocks = mb->num_blocks;
	mb->stats.max_blocks_used = 0;
	mb->stats.min_block_size = block_size;
	mb->stats.max_block_size = 0;
	mb->stats.num_allocations = 0;
#endif
}


/**
 * \brief Initialize memory manager before use
 *
 * This function will allocate all the memory bags using \ref physmem_pool and
 * \ref mempool_group "Memory Pool". Memory sizes and number of blocks are
 * defined with APP_MEMBAG_INITIALIZER defined in app/membag.h for each
 * individual application.
 *
 * \param align_order log2 of the minimum object alignment in bytes.
 * \sa membags
 * \sa mem_pool_init_physmem
 * \pre Not in interrupt context.
 */
void membag_init(const unsigned int align_order)
{
	uint8_t i;

	// Iterate over all bags and allocate them from physical memory
	for (i = 0; i < ARRAY_LEN(membags); i++) {
		membag_pool_init_physmem(&membags[i], align_order);
	}
}

/**
 * \brief Total amount of memory in bytes
 *
 * This function returns the total memory ie. the total number of blocks
 * in each bag multiplied by the block size for each bag.
 *
 * \return Sum of total bytes in all bags
 */
size_t membag_get_total(void)
{
	size_t total = 0;
	uint16_t blocks;
	uint16_t size;
	uint8_t i;

	// Iterate over all bags and sum the total memory available
	for (i = 0; i < ARRAY_LEN(membags); i++) {
		size = membags[i].block_size;
		blocks = membags[i].num_blocks;
		total += size * blocks;
	}
	return total;
}

#ifdef CONFIG_MEMBAG_USE_TUNING
/**
 * \brief Total amount of free memory in bytes
 *
 * This function sums the free memory available in all bags
 *
 * \return Sum of free bytes in all bags
 */
size_t membag_get_free(void)
{
	size_t free = 0;
	uint8_t i;

	// Iterate over all bags and sum the free memory available
	for (i = 0; i < ARRAY_LEN(membags); i++) {
		free += membags[i].block_size * membags[i].stats.num_free_blocks;
	}

	return free;
}
#endif

/**
 * \brief Smallest free block in bytes
 *
 * This searches through all bags to find the smallest available block
 *
 * \return Number of bytes for smallest available block
 */
size_t membag_get_smallest_free_block_size(void)
{
	uint8_t i;

	// Start at first bag and iterate up to find first available block
	for (i = 0; i < ARRAY_LEN(membags); i++) {
		if (membags[i].pool.freelist) {
			return membags[i].block_size;
		}
	}
	return 0;
}

/**
 * \brief Largest free block size in bytes
 *
 * This searches through all bags to find the largest available block
 *
 * \return Number of bytes for largest available block
 */
size_t membag_get_largest_free_block_size(void)
{
	uint8_t i = ARRAY_LEN(membags);

	// Start at last bag and iterate down to find first available block
	while (i > 0) {
		i--;
		if (membags[i].pool.freelist) {
			return membags[i].block_size;
		}
	}
	return 0;
}

/**
 * \brief Allocate memory
 *
 * This function finds the smallest bag with available memory that is
 * equal or larger than the number of bytes requested. Memory is then
 * allocated from within that bag.
 *
 * \param size Size of buffer to allocate
 * \return A pointer to the memory block. NULL if no memory was available
 */
void *membag_alloc(size_t size)
{
	uint8_t i;
	void *ptr = NULL;

	// Iterate through bags to find smallest available bag
	for (i = 0; i < ARRAY_LEN(membags); i++) {
		// If bag is too small then skip to next bag
		if (membags[i].block_size < size) {
			continue;
		}

		// Try to allocate memory
		ptr = mem_pool_alloc(&membags[i].pool);

		// If allocation failed we continue to the next bag
		if (ptr == NULL) {
			continue;
		}

#ifdef CONFIG_MEMBAG_USE_TUNING
		membags[i].stats.num_free_blocks--;

		// Update allocations
		membags[i].stats.num_allocations++;

		// Update high/low watermarks
		if (membags[i].stats.num_allocations >
				membags[i].stats.max_blocks_used) {
			membags[i].stats.max_blocks_used =
				membags[i].stats.num_allocations;
		}

		if (size < membags[i].stats.min_block_size) {
			membags[i].stats.min_block_size = size;
		}

		if (size > membags[i].stats.max_block_size) {
			membags[i].stats.max_block_size = size;
		}
#endif

		// Memory has been allocated, skip any further looping
		break;
	}
	return ptr;
}

/**
 * \brief Free previously allocated memory
 *
 * This function returns a block of memory to the correct memory bag
 *
 * \note Invalid addresses will be ignored.
 * \note Do not free previously freed memory
 *
 */
void membag_free(void *ptr)
{
	uint8_t i;

	// Iterate through bags to free memory in correct bag
	for (i = 0; i < ARRAY_LEN(membags); i++) {
		if ((ptr >= membags[i].start) && (ptr <= membags[i].end)) {
			// Found correct bag. Free memory and return
			mem_pool_free(&membags[i].pool, ptr);
#ifdef CONFIG_MEMBAG_USE_TUNING
			membags[i].stats.num_free_blocks++;
#endif
			break;
		}
	}
}

#ifdef CONFIG_MEMBAG_USE_TUNING
/**
 * \brief Read memory bag statistics
 *
 * This function will give usage statistics for a specific bag. This data is
 * useful for tuining the block sizes and number of blocks for a specific
 * application
 *
 * \param bag_no The bag number to return statistics for. Must be smaller than
 *               the available number of bags
 * \param stats The struct to store the statistics in.
 */
void membag_get_bag_stats(size_t bag_no, struct membag_bagstats *stats)
{
	assert(bag_no < ARRAY_LEN(membags));

	memcpy(stats, &membags[bag_no].stats, sizeof(struct membag_bagstats));
}
#endif

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
#ifndef MEMBAG_H_INCLUDED
#define MEMBAG_H_INCLUDED

#include <types.h>

/**
 * \ingroup mem_alloc_group
 * \defgroup membag_group Memory Bag Allocator
 *
 * The memory bag allocator uses several bags of different size to
 * allocate memory from. The Memory bag allocator will always allocate
 * from the smallest available bag which is equal to or larger than the
 * number of bytes requested. The size of each bag and number of blocks
 * in each bag is user configurable. See \ref membags and \ref
 * membag_init.
 *
 * The allocator also has statistics functionality for tuning the size
 * of bags and number of blocks within each bag to get the best memory
 * usage for the application.  See \ref CONFIG_MEMBAG_USE_TUNING and
 * \ref membag_get_bag_stats
 *
 * The memory bag allocator always allocates memory from a fixed size
 * bag/pool in the same way as the \ref mempool_group. This helps reduce
 * fragmentation compared to an generic allocator that gives exactly the
 * bytes requested.  (Reduces external fragmentation)
 *
 * Compared to the \ref mempool_group, the membag allocator has the
 * advantage of providing several bags of different sizes to allocate
 * from which can help reduce the memory usage in applications where
 * different size objects is allocated.  (Reduces internal
 * fragmentation)
 *
 * Allocation of memory with the membag allocator has a max run time which
 * is dependent on the number of bags that is configured.
 *
 * @{
 */

#ifdef CONFIG_MEMBAG_USE_TUNING
//! Statistics for a bag, used for debugging and tuning.
struct membag_bagstats {
	size_t num_free_blocks; //!< Number of free blocks in this bag.
	size_t max_blocks_used; //!< High watermark for blocks used in this bag.
	size_t min_block_size; //!< Low watermark for size requests for this bag.
	size_t  max_block_size; //!< High watermark for size requests for this bag.
	uint32_t num_allocations; //!< Totalt number of allocations in this bag.
};
#endif


void membag_init(unsigned int align_order);

size_t membag_get_total(void);

#ifdef CONFIG_MEMBAG_USE_TUNING
size_t membag_get_free(void);
#endif

size_t membag_get_smallest_free_block_size(void);
size_t membag_get_largest_free_block_size(void);
void *membag_alloc(size_t size);
void membag_free(void *ptr);

#ifdef CONFIG_MEMBAG_USE_TUNING
void membag_get_bag_stats(size_t bag_no, struct membag_bagstats *stats);
#endif

//! @}

#endif /* MEMBAG_H_INCLUDED */

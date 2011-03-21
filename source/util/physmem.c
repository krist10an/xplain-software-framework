/**
 * \file
 *
 * \brief Physical memory allocator implementation.
 *
 * This physical memory allocator implemention only supports
 * allocating physical memory, not freeing it. This makes the
 * allocator very lightweight, while still useful for initializing
 * other allocators, and for expanding the malloc() pool.
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
#include <physmem.h>
#include <util.h>

/**
 * \brief Allocate a region of physical memory.
 *
 * This function tries to allocate a block of physical memory from the
 * given pool, at the highest possible address.
 *
 * The following pools must be available on any CPU with internal SRAM
 * (though they may refer to the same pool):
 * - #cpu_sram_pool - Memory suitable for fast CPU access.
 * - #dma_sram_pool - Memory suitable for fast DMA access.
 *
 * \param pool The physical memory pool to allocate from.
 * \param size The number of bytes to allocate.
 * \param align_order log2 of required alignment.
 *
 * \return A valid physical address, or #PHYSMEM_ALLOC_ERR if there
 * isn't sufficient physical memory available.
 *
 * \pre Not in interrupt context.
 */
phys_addr_t physmem_alloc(struct physmem_pool *pool, phys_size_t size,
		unsigned int align_order)
{
	phys_addr_t	addr;

	addr = round_down(pool->end.addr - size, align_order);
	if (addr >= pool->start.addr)
		pool->end.addr = addr;
	else
		addr = PHYSMEM_ALLOC_ERR;

	return addr;
}

/**
 * \brief Allocate a low region of physical memory.
 *
 * This function tries to allocate a block of physical memory from the
 * given pool, at the highest possible address.
 *
 * \note This function is only intended for use by malloc(). All other
 * users should call physmem_alloc() instead.
 *
 * \param pool The physical memory pool to allocate from.
 * \param size The number of bytes to allocate.
 * \param align_order log2 of required alignment.
 *
 * \return A valid physical address, or #PHYSMEM_ALLOC_ERR if there
 * isn't sufficient physical memory available.
 *
 * \pre Not in interrupt context.
 */
phys_addr_t physmem_alloc_low(struct physmem_pool *pool, phys_size_t size,
		unsigned int align_order)
{
	phys_addr_t	addr;

	addr = round_up(pool->start.addr, align_order);
	if (addr + size <= pool->end.addr)
		pool->start.addr = addr + size;
	else
		addr = PHYSMEM_ALLOC_ERR;

	return addr;
}

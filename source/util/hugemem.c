/**
 * \file
 *
 * \brief Huge memory allocator implementation.
 *
 * Copyright (C) 2010 Atmel Corporation. All rights reserved.
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
#include <hugemem.h>

/**
 * \brief Allocate a region of huge memory.
 *
 * This function tries to allocate a block of huge memory from the given pool,
 * at the highest possible address.
 *
 * \param pool The huge memory pool to allocate from.
 * \param size The number of bytes to allocate.
 * \param align_order log2 of required alignment.
 *
 * \return A valid hugemem address, or #HUGEMEM_NULL if there isn't sufficient
 * memory available.
 *
 * \pre Not in interrupt context.
 */
hugemem_ptr_t hugemem_alloc(struct physmem_pool *pool, phys_size_t size,
		unsigned int align_order)
{
	phys_addr_t     address;

	address = physmem_alloc(pool, size, align_order);
	if (address == PHYSMEM_ALLOC_ERR)
		return HUGEMEM_NULL;

	return (hugemem_ptr_t)address;
}

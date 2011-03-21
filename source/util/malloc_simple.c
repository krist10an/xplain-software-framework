/**
 * \file
 *
 * \brief Extremely simple malloc() implementation
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
#include <malloc.h>
#include <physmem.h>
#include <string.h>
#include <types.h>

/* Sanity check */
#ifndef MALLOC_SIMPLE_H_INCLUDED
# error malloc() header does not match implementation
#endif

/**
 * \brief Allocate \a size bytes of dynamic memory.
 */
void *malloc(size_t size)
{
	phys_addr_t	addr;

	assert(size > 0);

	addr = physmem_alloc_low(&cpu_sram_pool, size, 2);
	if (addr == PHYSMEM_ALLOC_ERR)
		return NULL;

	return physmem_map(addr, size, PHYS_MAP_WRBUF | PHYS_MAP_WRBACK);
}

/**
 * \brief Allocate \a size bytes of zero-initialized dynamic memory.
 */
void *zalloc(size_t size)
{
	void	*p;

	p = malloc(size);
	if (p)
		memset(p, 0, size);

	return p;
}

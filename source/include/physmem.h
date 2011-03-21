/**
 * \file
 *
 * \brief Physical memory allocation and mapping.
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
#ifndef PHYSMEM_H_INCLUDED
#define PHYSMEM_H_INCLUDED

#include <cpu/physmem.h>
#include <chip/memory-map.h>

/**
 * \brief A region of available physical memory.
 */
struct physmem_pool {
	union {
		/** \brief The first available address in this region. */
		phys_addr_t     addr;
		/**
		 * \brief Work around for 8-bit arch support, since these
		 * devices have up to 24-bit memory pointers.
		 */
		uintptr_t       vaddr;
	} start;

	union {
		/** \brief The first not-available address after this region. */
		phys_addr_t     addr;
		/**
		 * \brief Work around for 8-bit arch support, since these
		 * devices have up to 24-bit memory pointers.
		 */
		uintptr_t       vaddr;
	} end;
};

phys_addr_t physmem_alloc(struct physmem_pool *pool, phys_size_t size,
		unsigned int align_order);
phys_addr_t physmem_alloc_low(struct physmem_pool *pool, phys_size_t size,
		unsigned int align_order);

#endif /* PHYSMEM_H_INCLUDED */

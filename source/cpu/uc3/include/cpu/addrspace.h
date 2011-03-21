/**
 * \file
 *
 * \brief Address space definitions for AVR32 UC3 devices.
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
#ifndef CPU_ADDRSPACE_H_INCLUDED
#define CPU_ADDRSPACE_H_INCLUDED

#define CACHEABLE_VMA(phys)	(phys)
#define UNCACHEABLE_VMA(phys)	(phys)
#define PHYSICAL_ADDR(virt)	(virt)

#ifndef __ASSEMBLY__

#include <types.h>

/**
 * Convert a physical address to a virtual pointer to cacheable memory.
 * @param paddr A physical address.
 * @return A virtual address in P1 corresponding to @a paddr.
 */
static inline void *phys_to_cached(phys_addr_t paddr)
{
	return (void *)CACHEABLE_VMA(paddr);
}

/**
 * Convert a physical address to a virtual pointer to non-cacheable
 * memory.
 * @param paddr A physical address.
 * @return A virtual address in P2 corresponding to @a paddr.
 */
static inline void *phys_to_uncached(phys_addr_t paddr)
{
	return (void *)UNCACHEABLE_VMA(paddr);
}

/**
 * Convert a virtual pointer to a physical address. @a vaddr must
 * contain an address in either P1 or P2, for example one returned by
 * phys_to_cached() or phys_to_uncached().
 * @param vaddr A virtual address in P1 or P2.
 * @return The physical address corresponding to @a vaddr.
 */
static inline phys_addr_t virt_to_phys(void *vaddr)
{
	return PHYSICAL_ADDR((unsigned long)vaddr);
}

#endif /* __ASSEMBLY__ */

#endif /* CPU_ADDRSPACE_H_INCLUDED */

/**
 * \file
 *
 * \brief Physical memory definitions for processors without MMU
 *
 * This file is intended to be included from cpu/physmem.h on processors
 * without a Memory Management Unit (MMU). Do not include this file
 * directly!
 *
 * \sa physmem.h
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
#ifndef GENERIC_PHYSMEM_NOMMU_H
#define GENERIC_PHYSMEM_NOMMU_H

#include <stdint.h>

/** \brief Create a coherent (uncached) mapping */
#define PHYS_MAP_COHERENT	(0)
/** \brief Write-buffering is allowed */
#define PHYS_MAP_WRBUF		(0)
/** \brief Write-through caching is allowed */
#define PHYS_MAP_WRTHROUGH	(0)
/** \brief Write-back caching is allowed */
#define PHYS_MAP_WRBACK		(0)

/**
 * \brief Map a physical address to a virtual address.
 *
 * This function returns a valid virtual address which can be used to
 * access a given physical memory area with given access and caching
 * properties.
 *
 * \param phys A valid physical address.
 * \param size The size of the physical region to be mapped.
 * \param flags Flags specifying cache behaviour, etc.
 *
 * \return A virtual address which can be used to access \a phys.
 */
static inline void *physmem_map(phys_addr_t phys, phys_size_t size,
		unsigned long flags)
{
	return (void *)(uintptr_t)phys;
}

/**
 * \brief Unmap a virtual-to-physical mapping.
 *
 * This function cleans up any mappings set up by physmem_map(). It
 * must be called when the virtual mapping isn't needed anymore.
 *
 * \param vaddr A valid virtual address returned by physmem_map().
 * \param size The size of the mapped region.
 */
static inline void physmem_unmap(void *vaddr, phys_size_t size)
{

}

#endif /* GENERIC_PHYSMEM_NOMMU_H */

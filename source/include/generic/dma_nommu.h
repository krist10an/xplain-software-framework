/**
 * \file
 *
 * \brief DMA mapping primitives for processors without an MMU
 *
 * \note Strictly speaking, MMU-less does not mean cache-less, yet these
 * primitives do not support processors with a cache. In practice,
 * however, this framework does not support any MMU-less devices with a
 * cache anyway.
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
#ifndef GENERIC_DMA_NOMMU_H_INCLUDED
#define GENERIC_DMA_NOMMU_H_INCLUDED

#include <physmem.h>
#include <types.h>

/**
 * \weakgroup dma_group
 * @{
 */

/**
 * \brief A physical/virtual address pair used for DMA
 *
 * Since physical and virtual addresses are always identical on devices
 * without an MMU, they can share the same storage as a union.
 */
typedef union {
	phys_addr_t	phys;	//!< Physical address
	void		*ptr;	//!< Virtual address
} dma_addr_t;

/**
 * \brief Return an address indicating failure
 *
 * This is used primarily by the DMA allocator to indicate allocation
 * failure.
 */
static inline dma_addr_t dma_addr_failure(void)
{
	dma_addr_t	addr;

	addr.ptr = NULL;

	return addr;
}

/**
 * \brief Test if a DMA address \a addr indicates failure
 *
 * \retval true \a addr indicates failure
 * \retval false \a addr is valid
 */
static inline bool dma_addr_is_failed(dma_addr_t addr)
{
	return addr.ptr == NULL;
}

/**
 * \brief Synchronize a DMA buffer before transfer
 *
 * After this function has been called, the device can access the
 * buffer, but the CPU must not.
 *
 * \param addr The physical/virtual addresses of the buffer
 * \param size The length of the buffer in bytes
 * \param direction The direction of the transfer
 */
static inline void dma_sync_for_device(dma_addr_t addr, size_t size,
		enum dma_direction direction)
{

}

/**
 * \brief Synchronize a DMA buffer after transfer
 *
 * After this function has been called, the CPU can access the buffer,
 * but the device must not.
 *
 * \param addr The physical/virtual addresses of the buffer
 * \param size The length of the buffer in bytes
 * \param direction The direction of the transfer
 */
static inline void dma_sync_for_cpu(dma_addr_t addr, size_t size,
		enum dma_direction direction)
{

}

/**
 * \brief Map a single contiguous buffer for DMA transfer
 *
 * After this function has been called, the device can access the
 * buffer, but the CPU must not.
 *
 * \param vaddr The virtual address of the buffer
 * \param size The length of the buffer in bytes
 * \param direction The direction of the transfer
 *
 * \return A physical/virtual address pair for the mapping
 */
static inline dma_addr_t dma_map_single(const void *vaddr, size_t size,
		enum dma_direction direction)
{
	dma_addr_t	addr = { .ptr = (void *)vaddr };

	return addr;
}

/**
 * \brief Unmap a single contiguous buffer after a DMA transfer
 *
 * After this function has been called, the CPU can access the buffer,
 * but the device must not.
 *
 * \param addr The physical/virtual addresses of the buffer
 * \param size The length of the buffer in bytes
 * \param direction The direction of the transfer
 */
static inline void dma_unmap_single(dma_addr_t addr, size_t size,
		enum dma_direction direction)
{

}

//! @}

#endif /* GENERIC_DMA_NOMMU_H_INCLUDED */

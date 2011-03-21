/**
 * \file
 *
 * \brief Direct Memory Access (DMA) support.
 *
 * This file defines helper functions for preparing memory buffers for
 * DMA transfers.
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
#ifndef DMA_H_INCLUDED
#define DMA_H_INCLUDED

#include <types.h>
#include <util.h>

/**
 * \defgroup dma_group Direct Memory Access (DMA) helpers
 *
 * Many chips supported by this framework allow certain on-chip (and in
 * some cases, external) peripherals to transfer data to and from RAM
 * directly without CPU intervention. This is known as Direct Memory
 * Access, or DMA.
 *
 * Depending on the chip in question, there may be various limitations
 * imposed on the memory used in a DMA transfer:
 *   - Processors with MMU hardware may use different addresses
 *     internally (virtual addresses) than peripherals use (physical
 *     addresses)
 *   - Processors with a cache may require various cache synchronization
 *     operations to be performed in order to make sure that the
 *     peripheral and the CPU see the same memory contents
 *   - Such processors may also allow the memory to be made "coherent",
 *     which will ensure that the CPU and peripheral views of the memory
 *     are always consistent, but at the cost of increased CPU overhead
 *     when accessing it.
 *
 * The DMA helper functions in this module aim to make it easier to
 * write generic code which will work on chips with any or none of these
 * limitations without imposing any unnecessary overhead.
 *
 * Note that some chips may not support DMA at all. It is still
 * recommended, however, to follow the rules laid out in this module
 * when writing code dealing with data transfer, as it will make it
 * easier to port it to a different chip in the future. On such devices,
 * utilizing the generic DMA helpers will not introduce any additional
 * overhead.
 *
 * \section dma_phys_virt Physical and Virtual Addresses
 *
 * Most of the chips supported by this framework do not distinguish
 * between physical addresses (i.e. addresses that appear on the bus)
 * and virtual addresses (i.e. addresses used in CPU instructions). On
 * these chips, the virtual address accessed by software will always
 * appear unchanged as a physical address on the bus.
 *
 * Other chips, e.g. all chips in the AVR32 AP7 family, have a Memory
 * Management Unit which translates each address accessed by the CPU
 * into a (possibly different) physical address before it appears on the
 * bus. When setting up DMA transfers on these chips, care must be taken
 * to give the \em physical address to the peripheral that is to perform
 * the data transfer, as the virtual address used to access the memory
 * may not even be a valid address on the bus.
 *
 * In order to obtain a physical address corresponding to an arbitrary
 * virtual address, the dma_map_single() function may be used. The value
 * returned represents both the physical and virtual address of the
 * mapped object, so it's usually a good idea to store this value as an
 * reference to the object instead of just the virtual pointer. On
 * platforms without MMU hardware, the value is stored as a union, so it
 * won't take any additional space.
 *
 * When the DMA transfer is finished, the object must be unmapped by
 * calling dma_unmap_single().
 *
 * \section dma_cache_sync Cache synchronization
 *
 * The dma_map_single() and dma_unmap_single() functions described in
 * the previous section will also ensure that any cached data is
 * properly synchronized before returning. Sometimes, however, the same
 * object may be used for multiple data transfers, and it is somewhat
 * wasteful to do the address translation for each and every transfer.
 *
 * The dma_sync_for_cpu() function may be used to synchronize the caches
 * after a data transfer without unmapping the object. After the CPU is
 * done processing the data and wants to re-use the object for another
 * transfer, the dma_sync_for_device() may be called to resynchronize
 * the caches before the next transfer.
 *
 * In order to ensure correct operation on all supported chips, the
 * following rules must be observed:
 *   - After calling dma_map_single() or dma_sync_for_device(), the
 *     peripheral may access the data but the CPU must not.
 *   - After calling dma_unmap_single() or dma_sync_for_cpu(), the CPU
 *     may access the data, but the peripheral must not.
 *
 * @{
 */

/**
 * \brief The direction of a DMA transfer.
 */
enum dma_direction {
	DMA_FROM_DEVICE		= 0,	//!< From memory to peripheral
	DMA_TO_DEVICE		= 1,	//!< From peripheral to memory
	DMA_BIDIRECTIONAL	= 2,	//!< Both of the above
};

/*
 * The actual mapping functions dealing with cache flushing, etc. are
 * CPU-specific.
 */
#include <cpu/dma.h>

//! @}

#endif /* DMA_H_INCLUDED */

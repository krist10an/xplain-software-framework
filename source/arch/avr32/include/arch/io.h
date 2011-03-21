/**
 * \file
 *
 * \brief I/O accessor functions: AVR32 implementation
 *
 * Copyright (C) 2009 Atmel Corporation. All rights reserved.
 *
 * \sa include/io.h
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
#ifndef ARCH_IO_H_INCLUDED
#define ARCH_IO_H_INCLUDED

#include <compiler.h>
#include <stdint.h>

/**
 * \weakgroup mmio_group
 * @{
 */

/**
 * \brief Read a byte from a memory-mapped register.
 *
 * \param p A virtual address.
 * \return The value at the virtual address specified by \a p.
 */
static inline uint8_t mmio_read8(const void *p)
{
	return *(const volatile uint8_t *)p;
}

/**
 * \brief Read a halfword from a memory-mapped register.
 *
 * \param p A virtual address.
 * \return The value at the virtual address specified by \a p.
 */
static inline uint16_t mmio_read16(const void *p)
{
	return *(const volatile uint16_t *)p;
}

/**
 * \brief Read a word from a memory-mapped register.
 *
 * \param p A virtual address.
 * \return The value at the virtual address specified by \a p.
 */
static inline uint32_t mmio_read32(const void *p)
{
	return *(const volatile uint32_t *)p;
}

/**
 * \brief Write a byte to a memory-mapped register.
 *
 * \param val The value to write to the virtual address specified by
 * \a p.
 * \param p A virtual address.
 */
static inline void mmio_write8(void *p, uint8_t val)
{
	*(volatile uint8_t *)p = val;
}

/**
 * \brief Write a halfword to a memory-mapped register.
 *
 * \param val The value to write to the virtual address specified by
 * \a p.
 * \param p A virtual address.
 */
static inline void mmio_write16(void *p, uint16_t val)
{
	*(volatile uint16_t *)p = val;
}

/**
 * \brief Write a word to a memory-mapped register.
 *
 * \param val The value to write to the virtual address specified by
 * \a p.
 * \param p A virtual address.
 */
static inline void mmio_write32(void *p, uint32_t val)
{
	*(volatile uint32_t *)p = val;
}

//! @}

#endif /* ARCH_IO_H_INCLUDED */

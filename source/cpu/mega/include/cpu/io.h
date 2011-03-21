/**
 * \file
 *
 * \brief ATmega I/O read/write functions.
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
#ifndef CPU_IO_H_INCLUDED
#define CPU_IO_H_INCLUDED

/**
 * \weakgroup mmio_group
 * @{
 */

/**
 * \brief Memory mapped register address offset.
 *
 * Offset that needs to be added to an I/O space address to get the
 * corresponding memory mapped address.
 */
#define AVR_MMIO_OFFSET	0x20

#ifndef __ASSEMBLY__

#include <compiler.h>
#include <stdint.h>

/**
 * \brief Read a byte from a memory-mapped register.
 *
 * The compiler will optimise this to an I/O space access when applicable.
 *
 * \param p A virtual address.
 * \return The value at the virtual address specified by \a p.
 */
static inline uint8_t mmio_read8(const void *p)
{
	return *(const volatile uint8_t *) p;
}

static inline void mmio_write8(void *p, uint8_t val)
{
	*(volatile uint8_t *) p = val;
}

#endif /* __ASSEMBLY__ */

//! @}

#endif /* CPU_IO_H_INCLUDED */

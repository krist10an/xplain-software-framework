/**
 * \file
 *
 * \brief AVR XMEGA I/O read/write functions.
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

#ifndef __ASSEMBLY__

#include <stdint.h>

static inline uint8_t mmio_read8(const void *p)
{
	return *((const volatile uint8_t *)p);
}

static inline void mmio_write8(void *p, uint8_t val)
{
	*(volatile uint8_t *)p = val;
}

/**
 * \brief 16-bit read from I/O register
 *
 * Reads LSB from address \a p and MSB from address \a p+1, in this order.
 *
 * \note Interrupts are not disabled in this function. This must be done in the
 * calling code if interrupts can corrupt reads, i.e., they access 16-bit
 * registers in the same peripheral module and at the same time as this
 * function.
 *
 * \param p Address of LSB I/O register to read from
 */
static inline uint16_t mmio_read16(const void *p)
{
	uint8_t *addr = (uint8_t *)p;
	uint16_t val;

	val = mmio_read8(addr);
	val |= mmio_read8(++addr) << 8;

	return val;
}

/** \brief 16-bit write to I/O register
 *
 * Writes LSB to address \a p and MSB to address \a p+1, in this order.
 *
 * \note Interrupts are not disabled in this function. This must be done in the
 * calling code if interrupts can corrupt writes, i.e., they access 16-bit
 * registers in the same peripheral module and at the same time as this
 * function.
 *
 * \param p   Address of LSB I/O register to write to
 * \param val 16-bit value to write to registers
 */
static inline void mmio_write16(void *p, uint16_t val)
{
	uint8_t *addr = (uint8_t *)p;

	mmio_write8(addr, val & 0xff);
	mmio_write8(++addr, val >> 8);
}

/**
 * \brief Write to a CCP-protected 8-bit register
 *
 * \param addr Address of the I/O register
 * \param value Value to be written
 */
extern void mmio_ccp_write8(void *addr, uint8_t value);

#endif /* __ASSEMBLY__ */

//! @}

#endif /* CPU_IO_H_INCLUDED */

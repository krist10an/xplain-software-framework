/**
 * \file
 *
 * \brief AVR32/GCC-specific compiler abstraction
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
#ifndef ARCH_COMPILER_GCC_H_INCLUDED
#define ARCH_COMPILER_GCC_H_INCLUDED

#include <stdint.h>
#include <arch/sysreg.h>

/**
 * \weakgroup compiler_group
 * @{
 */

__always_inline static uint32_t compiler_priv_brev32(uint32_t word)
{
	asm("brev	%0" : "=r"(word) : "0"(word));
	return word;
}

#define compiler_priv_brev16(word)      (compiler_priv_brev32(word) >> 16)
#define compiler_priv_brev8(word)       (compiler_priv_brev32(word) >> 24)

#define compiler_brev(x) \
	compiler_priv_demux_size(sizeof(x), compiler_priv_brev, (x))

#define compiler_clz(x)         __builtin_clz(x)
#define compiler_ctz(x)         __builtin_ctz(x)

//! \name AVR32-specific macros
//@{

/**
 * \brief Write the value \a x to the Status Register
 *
 * Write the value \a x to the status register and make sure that the
 * compiler doesn't assume that the condition register is valid
 * afterwards.
 */
#define avr32_write_sr(x) asm volatile("mtsr 0, %0" :: "r"(x) : "cc")

/**
 * \brief Set bit \a x in the Status Register
 */
#define avr32_set_sr_bit(x)     __builtin_ssrf(x)

/**
 * \brief Clear bit \a x in the Status Register
 */
#define avr32_clear_sr_bit(x)   __builtin_csrf(x)

//@}

#define cpu_irq_enable()                                \
	do {                                            \
		barrier();                              \
		avr32_clear_sr_bit(SYSREG_SR_GM_BIT);   \
	} while (0)

#define cpu_irq_disable()                               \
	do {                                            \
		avr32_set_sr_bit(SYSREG_SR_GM_BIT);     \
		barrier();                              \
	} while (0)

//! @}

#endif /* ARCH_COMPILER_GCC_H_INCLUDED */

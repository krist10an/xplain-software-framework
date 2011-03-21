/**
 * \file
 *
 * \brief AVR XMEGA CPU registers.
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
#ifndef CPU_REGS_H_INCLUDED
#define CPU_REGS_H_INCLUDED

#define XMEGA_CPU_BASE	0x30	//!< Base address of CPU registers

#define CPUR_CCP	0x04	//!< Configuration Change Protection
#define CPUR_RAMPD	0x08	//!< Extended Direct Address
#define CPUR_RAMPX	0x09	//!< Extended X Pointer
#define CPUR_RAMPY	0x0a	//!< Extended Y Pointer
#define CPUR_RAMPZ	0x0b	//!< Extended Z Pointer
#define CPUR_EIND	0x0c	//!< Extended Indirect
#define CPUR_SPL	0x0d	//!< Stack Pointer Low
#define CPUR_SPH	0x0e	//!< Stack Pointer High
#define CPUR_SREG	0x0f	//!< Status Register
# define CPUR_SREG_I		(1 << 7)	//!< Interrupt Enable
# define CPUR_SREG_T		(1 << 6)	//!< Bit Copy Storage
# define CPUR_SREG_H		(1 << 5)	//!< Half Carry
# define CPUR_SREG_S		(1 << 4)	//!< Sign
# define CPUR_SREG_V		(1 << 3)	//!< Overflow
# define CPUR_SREG_N		(1 << 2)	//!< Negative
# define CPUR_SREG_Z		(1 << 1)	//!< Zero
# define CPUR_SREG_C		(1 << 0)	//!< Carry

//! Get address of register \a name.
#define CPU_REG(name)  (XMEGA_CPU_BASE + CPUR_##name)

/* CCP commands */
#define CCP_SPM		0x9D
#define CCP_IOREG	0xD8

#ifndef __ASSEMBLY__

#include <io.h>

/**
 * \brief Read the 8-bit CPU register \a reg
 */
#define cpu_read_reg8(reg)						\
	mmio_read8((void *)(XMEGA_CPU_BASE + CPUR_##reg))

/**
 * \brief Write \a value to the 8-bit CPU register \a reg
 */
#define cpu_write_reg8(reg, value)					\
	mmio_write8((void *)(XMEGA_CPU_BASE + CPUR_##reg), value)

#else /* __ASSEMBLY__ */

/* Register manipulation macros */
#define LD_CPUREG(val, name)	in	val, XMEGA_CPU_BASE + CPUR_##name
#define ST_CPUREG(val, name)	out	XMEGA_CPU_BASE + CPUR_##name, val

#endif /* __ASSEMBLY__ */

#endif /* CPU_REGS_H_INCLUDED */

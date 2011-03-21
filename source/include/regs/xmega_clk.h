/**
 * \file
 *
 * \brief Xmega Clock register definitions
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
#ifndef REGS_XMEGA_CLK_H_INCLUDED
#define REGS_XMEGA_CLK_H_INCLUDED

#include <chip/memory-map.h>
#include <io.h>

/**
 * \ingroup regs_group
 * \defgroup xmega_clk_regs_group Xmega Clock Register Definitions
 *
 * These are the Xmega Clock registers taken from the Xmega A manual.
 *
 * @{
 */

//! \name Register Offsets
//@{
#define XMEGA_CLK_CTRL          0x00    //!< System Clock Control
#define XMEGA_CLK_PSCTRL        0x01    //!< System Clock Prescaler
#define XMEGA_CLK_LOCK          0x02    //!< Clock System Lock
#define XMEGA_CLK_RTCCTRL       0x03    //!< RTC Control
//@}

//! \name Bitfields in CTRL
//@{
#define XMEGA_CLK_SCLKSEL_START 0       //< System Clock Selection
#define XMEGA_CLK_SCLKSEL_SIZE  3       //< System Clock Selection
//@}

//! \name Bitfields in PSCTRL
//@{
#define XMEGA_CLK_PSBCDIV_START 0       //!< Prescaler B/C Division Factor
#define XMEGA_CLK_PSBCDIV_SIZE  2       //!< Prescaler B/C Division Factor
#define XMEGA_CLK_PSADIV_START  0       //!< Prescaler A Division Factor
#define XMEGA_CLK_PSADIV_SIZE   0       //!< Prescaler A Division Factor
//@}

//! \name Bitfields in LOCK
//@{
#define XMEGA_CLK_LOCK_BIT      0       //!< Clock System Lock
//@}

//! \name Bitfields in LOCK
//@{
#define XMEGA_CLK_RTCEN_BIT     0       //!< RTC Clock Source Enable
#define XMEGA_CLK_RTCSRC_START  1       //!< RTC Clock Source
#define XMEGA_CLK_RTCSRC_SIZE   3       //!< RTC Clock Source
//@}

//! \name Prescaler A Division Factor
//@{
#define XMEGA_CLK_PSADIV_1      0       //!< No division
#define XMEGA_CLK_PSADIV_2      1       //!< Divide by 2
#define XMEGA_CLK_PSADIV_4      3       //!< Divide by 2
#define XMEGA_CLK_PSADIV_8      5       //!< Divide by 2
#define XMEGA_CLK_PSADIV_16     7       //!< Divide by 2
#define XMEGA_CLK_PSADIV_32     9       //!< Divide by 2
#define XMEGA_CLK_PSADIV_64     11      //!< Divide by 2
#define XMEGA_CLK_PSADIV_128    13      //!< Divide by 2
#define XMEGA_CLK_PSADIV_256    15      //!< Divide by 2
#define XMEGA_CLK_PSADIV_512    17      //!< Divide by 2
//@}

//! \name Prescaler B/C Division Factor
//@{
#define XMEGA_CLK_PSBCDIV_1_1   0       //!< No division / No division
#define XMEGA_CLK_PSBCDIV_1_2   1       //!< No division / Divide by 2
#define XMEGA_CLK_PSBCDIV_4_1   2       //!< Divide by 4 / No division
#define XMEGA_CLK_PSBCDIV_2_2   3       //!< Divide by 2 / Divide by 2
//@}

//! \name Bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set.
#define CLK_BIT(name)           (1U << XMEGA_CLK_##name##_BIT)
//! \brief Create a mask with bitfield \a name set to \a value.
#define CLK_BF(name, value)                                             \
	((value) << XMEGA_CLK_##name##_START)
//! \brief Create a mask of the bitfield \a name.
#define CLK_BFMASK(name)                                                \
	(((1U << XMEGA_CLK_##name##_SIZE) - 1)                          \
		<< XMEGA_CLK_##name##_START)
//! \brief Extract the value of bitfield \a name from \a regval.
#define CLK_BFEXT(name, regval)                                         \
	(((regval) >> XMEGA_CLK_##name##_START)                         \
		& ((1U << XMEGA_CLK_##name##_SIZE) - 1))
//! \brief Return \a regval with bitfield \a name set to \a value.
#define CLK_BFINS(name, value, regval)                                  \
	(((regval) & ~(((1U << XMEGA_CLK_##name##_SIZE) - 1)            \
			<< XMEGA_CLK_##name##_START))                   \
		| XMEGA_CLK_BF(name, value))
//@}

//! \name Register access macros
//@{
//! \brief Read the value of CLK register \a reg.
#define clk_read_reg(reg)                                       \
	_clk_read_reg(reg)
#define _clk_read_reg(reg)                                      \
	mmio_read8((void *)(CLK_BASE + XMEGA_CLK_##reg))
//! \brief Write \a value to CLK register \a reg.
#define clk_write_reg(reg, value)                               \
	_clk_write_reg(reg, value)
#define _clk_write_reg(reg, value)                              \
	mmio_write8((void *)(CLK_BASE + XMEGA_CLK_##reg), (value))
//! \brief Write \a value to CCP-protected CLK register \a reg.
#define clk_write_ccp_reg(reg, value)                           \
	_clk_write_ccp_reg(reg, value)
#define _clk_write_ccp_reg(reg, value)                          \
	mmio_ccp_write8((void *)(CLK_BASE + XMEGA_CLK_##reg), (value))
//@}

//! @}

#endif /* REGS_XMEGA_CLK_H_INCLUDED */

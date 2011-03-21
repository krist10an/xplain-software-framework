/**
 * \file
 *
 * \brief Xmega Oscillator register definitions
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
#ifndef REGS_XMEGA_OSC_H_INCLUDED
#define REGS_XMEGA_OSC_H_INCLUDED

#include <chip/memory-map.h>
#include <io.h>

/**
 * \ingroup regs_group
 * \defgroup xmega_osc_regs_group Xmega Oscillator Register Definitions
 *
 * These are the Xmega Oscillator registers taken from the Xmega A
 * manual.
 *
 * @{
 */

//! \name Register Offsets
//@{
#define XMEGA_OSC_CTRL          0x00    //!< Oscillator Control
#define XMEGA_OSC_STATUS        0x01	//!< Oscillator Status
#define XMEGA_OSC_XOSCCTRL      0x02    //!< XOSC Control
#define XMEGA_OSC_XOSCFAIL      0x03    //!< XOSC Failure Detection
#define XMEGA_OSC_RC32KCAL      0x04    //!< 32 kHz Oscillator Calibration
#define XMEGA_OSC_PLLCTRL       0x05    //!< PLL Control
#define XMEGA_OSC_DFLLCTRL      0x06    //!< DFLL Control
//@}

//! \name Bitfields in CTRL
//@{
#define XMEGA_OSC_RC2MEN_BIT      0     //!< 2 MHz RCOSC Enable
#define XMEGA_OSC_RC32MEN_BIT     1     //!< 32 MHz RCOSC Enable
#define XMEGA_OSC_RC32KEN_BIT     2     //!< 32 kHz RCOSC Enable
#define XMEGA_OSC_XOSCEN_BIT      3     //!< External Oscillator Enable
#define XMEGA_OSC_PLLEN_BIT       4     //!< PLL Enable
//@}

//! \name Bitfields in STATUS
//@{
#define XMEGA_OSC_RC2MRDY_BI      0     //!< 2 MHz RCOSC Enable
#define XMEGA_OSC_RC32MRDY_BIT    1     //!< 32 MHz RCOSC Enable
#define XMEGA_OSC_RC32KRDY_BIT    2     //!< 32 kHz RCOSC Enable
#define XMEGA_OSC_XOSCRDY_BIT     3     //!< External Oscillator Enable
#define XMEGA_OSC_PLLRDY_BIT      4     //!< PLL Enable
//@}

//! \name Bitfields in XOSCCTRL
//@{
#define XMEGA_OSC_XOSCSEL_START   0     //!< Crystal Oscillator Type
#define XMEGA_OSC_XOSCSEL_SIZE    4     //!< Crystal Oscillator Type
#define XMEGA_OSC_X32KLPM_BIT     5     //!< XTAL 32 kHz Low Power Mode
#define XMEGA_OSC_FRQRANGE_START  6     //!< XTAL Frequency Range
#define XMEGA_OSC_FRQRANGE_SIZE   2     //!< XTAL Frequency Range
//@}

//! \name Bitfields in XOSCFAIL
//@{
#define XMEGA_OSC_XOSCFDEN_BIT    0     //!< Failure Detection Enable
#define XMEGA_OSC_XOSCFDIF_BIT    1     //!< Failure Detection Interrupt Flag
//@}

//! \name Bitfields in PLLCTRL
//@{
#define XMEGA_OSC_PLLFAC_START    0     //!< Multiplication Factor
#define XMEGA_OSC_PLLFAC_SIZE     5     //!< Multiplication Factor
#define XMEGA_OSC_PLLSRC_START    6     //!< Clock Source
#define XMEGA_OSC_PLLSRC_SIZE     2     //!< Clock Source
//@}

//! \name Bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set.
#define OSC_BIT(name)           (1U << XMEGA_OSC_##name##_BIT)
//! \brief Create a mask with bitfield \a name set to \a value.
#define OSC_BF(name, value)                                             \
	((value) << XMEGA_OSC_##name##_START)
//! \brief Create a mask of the bitfield \a name.
#define OSC_BFMASK(name)                                                \
	(((1U << XMEGA_OSC_##name##_SIZE) - 1)                          \
		<< XMEGA_OSC_##name##_START)
//! \brief Extract the value of bitfield \a name from \a regval.
#define OSC_BFEXT(name, regval)                                         \
	(((regval) >> XMEGA_OSC_##name##_START)                         \
		& ((1U << XMEGA_OSC_##name##_SIZE) - 1))
//! \brief Return \a regval with bitfield \a name set to \a value.
#define OSC_BFINS(name, value, regval)                                  \
	(((regval) & ~(((1U << XMEGA_OSC_##name##_SIZE) - 1)            \
			<< XMEGA_OSC_##name##_START))                   \
		| XMEGA_OSC_BF(name, value))
//@}

//! \name Register access macros
//@{
//! \brief Read the value of OSC register \a reg.
#define osc_read_reg(reg)                                       \
	_osc_read_reg(reg)
#define _osc_read_reg(reg)                                      \
	mmio_read8((void *)(OSC_BASE + XMEGA_OSC_##reg))
//! \brief Write \a value to OSC register \a reg.
#define osc_write_reg(reg, value)                               \
	_osc_write_reg(reg, value)
#define _osc_write_reg(reg, value)                              \
	mmio_write8((void *)(OSC_BASE + XMEGA_OSC_##reg), (value))
//! \brief Write \a value to CCP-protected OSC register \a reg.
#define osc_write_ccp_reg(reg, value)                           \
	_osc_write_ccp_reg(reg, value)
#define _osc_write_ccp_reg(reg, value)                          \
	mmio_ccp_write8((void *)(OSC_BASE + XMEGA_OSC_##reg), (value))
//@}

//! @}

#endif /* REGS_XMEGA_OSC_H_INCLUDED */

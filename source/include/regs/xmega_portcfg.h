/**
 * \file
 *
 * \brief XMEGA Port Configuration (PORTCFG) Register Definitions
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

#ifndef REGS_XMEGA_PORTCFG_H_INCLUDED
#define REGS_XMEGA_PORTCFG_H_INCLUDED

#include <chip/memory-map.h>
#include <io.h>

/**
 * \ingroup regs_group
 * \defgroup portcfg_regs_group Port Configuration
 * @{
 */

//! \name Register Offsets
//@{
#define PORTCFG_MPCMASK        0x0 //!< Multi-Pin Configuration Mask Register
#define PORTCFG_VPCTRLA        0x2 //!< Virtual Port-Map Control A Register
#define PORTCFG_VPCTRLB        0x3 //!< Virtual Port-Map Control B Register
#define PORTCFG_CLKEVOUT       0x4 //!< Clock and Event Out Register
//@}

//! \name VPCTRLA Bitfields
//@{
#define PORTCFG_VPCTRLA_VP0MAP_START   0 //!< Virtual Port 0 Mapping
#define PORTCFG_VPCTRLA_VP0MAP_SIZE    4 //!< Virtual Port 0 Mapping
#define PORTCFG_VPCTRLA_VP1MAP_START   4 //!< Virtual Port 1 Mapping
#define PORTCFG_VPCTRLA_VP1MAP_SIZE    4 //!< Virtual Port 1 Mapping
//@}

//! \name VPCTRLB Bitfields
//@{
#define PORTCFG_VPCTRLB_VP2MAP_START   0 //!< Virtual Port 2 Mapping
#define PORTCFG_VPCTRLB_VP2MAP_SIZE    4 //!< Virtual Port 2 Mapping
#define PORTCFG_VPCTRLB_VP3MAP_START   4 //!< Virtual Port 3 Mapping
#define PORTCFG_VPCTRLB_VP3MAP_SIZE    4 //!< Virtual Port 3 Mapping
//@}

//! \name CLKEVOUT Bitfields
//@{
#define PORTCFG_CLKEVOUT_CLKOUT_START  0 //!< Clock Output Port
#define PORTCFG_CLKEVOUT_CLKOUT_SIZE   2 //!< Clock Output Port
#define PORTCFG_CLKEVOUT_EVOUT_START   4 //!< Event Output Port
#define PORTCFG_CLKEVOUT_EVOUT_SIZE    2 //!< Event Output Port
//@}

//! \name VPCTRLA and VPCTRLB Bitfield Values
//@{
#define PORTCFG_PORTA    0x0 //!< Map Port A to Virtual Port
#define PORTCFG_PORTB    0x1 //!< Map Port B to Virtual Port
#define PORTCFG_PORTC    0x2 //!< Map Port C to Virtual Port
#define PORTCFG_PORTD    0x3 //!< Map Port D to Virtual Port
#define PORTCFG_PORTE    0x4 //!< Map Port E to Virtual Port
#define PORTCFG_PORTF    0x5 //!< Map Port F to Virtual Port
#define PORTCFG_PORTG    0x6 //!< Map Port G to Virtual Port
#define PORTCFG_PORTH    0x7 //!< Map Port H to Virtual Port
#define PORTCFG_PORTJ    0x8 //!< Map Port J to Virtual Port
#define PORTCFG_PORTK    0x9 //!< Map Port K to Virtual Port
#define PORTCFG_PORTL    0xa //!< Map Port L to Virtual Port
#define PORTCFG_PORTM    0xb //!< Map Port M to Virtual Port
#define PORTCFG_PORTN    0xc //!< Map Port N to Virtual Port
#define PORTCFG_PORTP    0xd //!< Map Port P to Virtual Port
#define PORTCFG_PORTQ    0xe //!< Map Port Q to Virtual Port
#define PORTCFG_PORTR    0xf //!< Map Port R to Virtual Port
//@}

//! \name CLKEVOUT_CLKOUT Bitfield Values
//@{
#define PORTCFG_CLKOUT_OFF     0x0 //!< Clock Out Disabled
#define PORTCFG_CLKOUT_PC7     0x1 //!< Clock Output on Port C Pin 7
#define PORTCFG_CLKOUT_PD7     0x2 //!< Clock Output on Port D Pin 7
#define PORTCFG_CLKOUT_PE7     0x3 //!< Clock Output on Port E Pin 7
//@}

//! \name CLKEVOUT_EVOUT Bitfield Values
//@{
#define PORTCFG_EVOUT_OFF      0x0 //!< Event Out Disabled
#define PORTCFG_EVOUT_PC7      0x1 //!< Event Channel 0 on Port C Pin 7
#define PORTCFG_EVOUT_PD7      0x2 //!< Event Channel 0 on Port D Pin 7
#define PORTCFG_EVOUT_PE7      0x3 //!< Event Channel 0 on Port E Pin 7
//@}

//! \name Bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set.
#define PORTCFG_BIT(name)						\
	(1U << PORTCFG_##name##_BIT)
//! \brief Create a mask with bitfield \a name set to \a value.
#define PORTCFG_BF(name, value)						\
	((value) << PORTCFG_##name##_START)
//! \brief Create a mask of the bitfield \a name.
#define PORTCFG_BFMASK(name)						\
	(((1U << %s_##name##_SIZE) - 1) << %s_##name##_START)
//! \brief Extract the value of bitfield \a name from \a regval.
#define PORTCFG_BFEXT(name, regval)					\
	(((regval) >> PORTCFG_##name##_START)				\
		& ((1U << PORTCFG_##name##_SIZE) - 1))
//! \brief Return \a regval with bitfield \a name set to \a value.
#define PORTCFG_BFINS(name, value, regval)				\
	(((regval) & ~(((1U << PORTCFG_##name##_SIZE) - 1)		\
			<< PORTCFG_##name##_START))			\
		| PORTCFG_BF(name, value))
//@}

//! \name Register access macros
//@{
//! \brief Read the value of PORT register \a reg.
#define portcfg_read_reg(base, reg)					\
	mmio_read8((void *)((uintptr_t)(base) + PORTCFG_##reg))
//! \brief Write \a value to PORT register \a reg.
#define portcfg_write_reg(base, reg, value)				\
	mmio_write8((void *)((uintptr_t)(base) + PORTCFG_##reg), (value))
//@}

//! @}
#endif /* REGS_XMEGA_PORTCFG_H_INCLUDED */

/**
 * \file
 *
 * \brief XMEGA I/O Ports Register Interface.
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

#ifndef REGS_XMEGA_PORT_H_INCLUDED
#define REGS_XMEGA_PORT_H_INCLUDED

#include <chip/memory-map.h>
#include <io.h>

/**
 * \ingroup regs_group
 * \defgroup port_regs_group I/O Ports Register Definitions
 * @{
 */

//! \name Register Offsets
//@{
#define PORT_DIR                0x00  //!< Data Direction
#define PORT_DIRSET             0x01  //!< Data Direction Set
#define PORT_DIRCLR             0x02  //!< Data Direction Clear
#define PORT_DIRTGL             0x03  //!< Data Direction Toggle
#define PORT_OUT                0x04  //!< Data Output Value
#define PORT_OUTSET             0x05  //!< Data Output Value Set
#define PORT_OUTCLR             0x06  //!< Data Output Value Clear
#define PORT_OUTTGL             0x07  //!< Data Output Value Toggle
#define PORT_IN                 0x08  //!< Data Input Value
#define PORT_INTCTRL            0x09  //!< Interrupt Control
#define PORT_INT0MASK           0x0A  //!< Interrupt 0 Mask
#define PORT_INT1MASK           0x0B  //!< Interrupt 1 Mask
#define PORT_INTFLAGS           0x0C  //!< Interrupt Flags
#define PORT_PIN0CTRL           0x10  //!< Pin 0 Configuration
#define PORT_PIN1CTRL           0x11  //!< Pin 1 Configuration
#define PORT_PIN2CTRL           0x12  //!< Pin 2 Configuration
#define PORT_PIN3CTRL           0x13  //!< Pin 3 Configuration
#define PORT_PIN4CTRL           0x14  //!< Pin 4 Configuration
#define PORT_PIN5CTRL           0x15  //!< Pin 5 Configuration
#define PORT_PIN6CTRL           0x16  //!< Pin 6 Configuration
#define PORT_PIN7CTRL           0x17  //!< Pin 7 Configuration
//@}

//! \name Bitfields in INTCTRL
//@{
#define PORT_INTCTRL_INT0LVL_START  0 //!< Interrupt 0 Level
#define PORT_INTCTRL_INT0LVL_SIZE   2 //!< Interrupt 0 Level
#define PORT_INTCTRL_INT1LVL_START  2 //!< Interrupt 1 Level
#define PORT_INTCTRL_INT1LVL_SIZE   2 //!< Interrupt 1 Level
//@}

//! \name Bitfields in INTFLAGS
//@{
#define PORT_INTFLAGS_INT0IF_BIT    0 //!< Interrupt 0 Flag
#define PORT_INTFLAGS_INT1IF_BIT    1 //!< Interrupt 1 Flag
//@}

//! \name Bitfields in PINCTRL
//@{
#define PORT_PINCTRL_ISC_START      0 //!< Input/Sense Configuration
#define PORT_PINCTRL_ISC_SIZE       3 //!< Input/Sense Configuration
#define PORT_PINCTRL_OPC_START      3 //!< Output and Pull Configuration
#define PORT_PINCTRL_OPC_SIZE       3 //!< Output and Pull Configuration
#define PORT_PINCTRL_INVEN_BIT      6 //!< Inverted I/O Enable
#define PORT_PINCTRL_SRLEN_BIT      7 //!< Slew Rate Limit Enable
//@}

//! \name PINCTRL_OPC Bitfield Values
//@{
#define PORT_PINCTRL_TOTEM          0x0 //!< Totempole/(N/A)
#define PORT_PINCTRL_BUSKEEPER      0x1 //!< Totempole/Bus keeper
#define PORT_PINCTRL_PULLDOWN       0x2 //!< Totempole/Pull-down (on input)
#define PORT_PINCTRL_PULLUP         0x3 //!< Totempole/Pull-up (on input)
#define PORT_PINCTRL_WIREDOR        0x4 //!< Wired OR/(N/A)
#define PORT_PINCTRL_WIREDAND       0x5 //!< Wired AND/(N/A)
#define PORT_PINCTRL_WIREDORPULL    0x6 //!< Wired OR/Pull-down
#define PORT_PINCTRL_WIREDANDPULL   0x7 //!< Wired AND/Pull-up
//@}

//! \name PINCTRL_ISC Bitfield Values
//@{
#define PORT_PINCTRL_ISC_BOTHEDGES      0x0 //!< Sense Both Edges
#define PORT_PINCTRL_ISC_RISING         0x1 //!< Sense Rising Edge
#define PORT_PINCTRL_ISC_FALLING        0x2 //!< Sense Falling Edge
#define PORT_PINCTRL_ISC_LEVEL          0x3 //!< Sense Low Level
#define PORT_PINCTRL_ISC_INPUT_DISABLE  0x7 //!< Input Buffer Disabled
//@}

//! \name Bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set.
#define PORT_BIT(name)							\
	(1U << PORT_##name##_BIT)
//! \brief Create a mask with bitfield \a name set to \a value.
#define PORT_BF(name, value)						\
	((value) << PORT_##name##_START)
//! \brief Create a mask of the bitfield \a name.
#define PORT_BFMASK(name)						\
	(((1U << %s_##name##_SIZE) - 1) << %s_##name##_START)
//! \brief Extract the value of bitfield \a name from \a regval.
#define PORT_BFEXT(name, regval)					\
	(((regval) >> PORT_##name##_START)				\
		& ((1U << PORT_##name##_SIZE) - 1))
//! \brief Return \a regval with bitfield \a name set to \a value.
#define PORT_BFINS(name, value, regval)				\
	(((regval) & ~(((1U << PORT_##name##_SIZE) - 1)		\
			<< PORT_##name##_START))			\
		| PORT_BF(name, value))
//@}

//! \name Register access macros
//@{
//! \brief Read the value of PORT register \a reg.
#define port_read_reg(base, reg)					\
	mmio_read8((void *)((uintptr_t)(base) + PORT_##reg))
//! \brief Write \a value to PORT register \a reg.
#define port_write_reg(base, reg, value)				\
	mmio_write8((void *)((uintptr_t)(base) + PORT_##reg), (value))
//@}

//! @}
#endif /* REGS_XMEGA_PORT_H_INCLUDED */

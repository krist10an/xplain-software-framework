/**
 * \file
 *
 * \brief XMEGA USART register definitions.
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

#ifndef REGS_XMEGA_USART_H_INCLUDED
#define REGS_XMEGA_USART_H_INCLUDED

#include <io.h>

/**
 * \ingroup regs_group
 * \defgroup xmega_usart_regs_group XMEGA USART Register Definitions
 *
 * This is the register interface to the XMEGA USART. The registers are
 * defined as offsets relative to the base address of the module, so
 * they may be easily reused across several modules on the same chip, or
 * across different types of chips.
 *
 * @{
 */

//! \name Register Offsets
//@{
#define USART_DATA		0x00	//!< TX/RX Data Register
#define USART_STATUS		0x01	//!< Status Register
#define USART_CTRLA		0x03	//!< Control Register A
#define USART_CTRLB		0x04	//!< Control Register B
#define USART_CTRLC		0x05	//!< Control Register C
#define USART_BAUDCTRLA		0x06	//!< Baud Rate Register A
#define USART_BAUDCTRLB		0x07	//!< Baud Rate Register B
//@}

//! \name STATUS register bits
//@{
#define USART_RXB8_BIT		0	//!< Receive Bit 8
#define USART_PERR_BIT		2	//!< Parity Error
#define USART_BUFOVF_BIT	3	//!< Buffer Overflow
#define USART_FERR_BIT		4	//!< Frame Error
#define USART_DREIF_BIT		5	//!< Data Register Empty
#define USART_TXCIF_BIT		6	//!< Transmit Complete
#define USART_RXCIF_BIT		7	//!< Receive Complete
//@}

//! \name CTRLA register bits
//@{
#define USART_DREINTLVL_START	0	//!< Data Reg Empty Interrupt Level
#define USART_DREINTLVL_SIZE	2	//!< Data Reg Empty Interrupt Level
#define USART_TXCINTLVL_START	2	//!< TX Complete Interrupt Level
#define USART_TXCINTLVL_SIZE	2	//!< TX Complete Interrupt Level
#define USART_RXCINTLVL_START	4	//!< RX Complete Interrupt Level
#define USART_RXCINTLVL_SIZE	2	//!< RX Complete Interrupt Level
//@}

//! \name CTRLB register bits
//@{
#define USART_TXB8_BIT		0	//!< Transmit Bit 8
#define USART_MPCM_BIT		1	//!< Multi-processor Communication Mode
#define USART_CLK2X_BIT		2	//!< Double Transmission Speed
#define USART_TXEN_BIT		3	//!< Transmitter Enable
#define USART_RXEN_BIT		4	//!< Receiver Enable
//@}

//! \name CTRLC register bits
//@{
#define USART_CHSIZE_START	0	//!< Character Size
#define USART_CHSIZE_SIZE	3	//!< Character Size
#define USART_SBMODE_START	3	//!< Stop Bit Mode
#define USART_SBMODE_SIZE	1	//!< Stop Bit Mode
#define USART_PMODE_START	4	//!< Parity Mode
#define USART_PMODE_SIZE	2	//!< Parity Mode
#define USART_CMODE_START	6	//!< Communication Mode
#define USART_CMODE_SIZE	2	//!< Communication Mode
//@}

//! \name BAUDCTRLB register bits
//@{
#define USART_BSELB_START	0	//!< Baud Rate bits 8..11
#define USART_BSELB_SIZE	4	//!< Baud Rate bits 8..11
#define USART_BSCALE_START	4	//!< Baud Rate Scale factor
#define USART_BSCALE_SIZE	4	//!< Baud Rate Scale factor
//@}

//! \name Bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set
#define USART_BIT(name)						\
	(1U << USART_##name##_BIT)
//! \brief Create a mask with bitfield \a name set to \a value
#define USART_BF(name, value)					\
	((value) << USART_##name##_START)
//! \brief Extract the value of bitfield \a name from \a regval
#define USART_BFEXT(name, regval)				\
	(((regval) >> USART_##name##_START)			\
		& ((1U << USART_##name##_SIZE) - 1))
//! \brief Return \a regval with bitfield \a name set to \a value
#define USART_BFINS(name, value, regval)			\
	(((regval) & ~(((1U << USART_##name##_SIZE) - 1)	\
			<< USART_##name##_START))		\
		| USART_BF(name, value))

//@}

//! \name Register access macros
//@{
//! \brief Read the value of \a reg on \a usart
#define usart_read_reg(usart, reg) \
	mmio_read8((void *)((uintptr_t)(usart) + USART_##reg))
//! \brief Write \a value to \a reg on \a usart
#define usart_write_reg(usart, reg, value) \
	mmio_write8((void *)((uintptr_t)(usart) + USART_##reg), value)
//@}

//! @}

#endif /* REGS_XMEGA_USART_H_INCLUDED */

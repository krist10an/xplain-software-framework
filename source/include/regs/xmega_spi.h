/**
 * \file
 *
 * \brief ATxmega SPI registers.
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

#ifndef REGS_XMEGA_SPI_H_INCLUDED
#define REGS_XMEGA_SPI_H_INCLUDED

#include <io.h>

/**
 * \ingroup regs_group
 * \defgroup spi_regs_group SPI Register Defintions
 * @{
 */

//! \name Register Offset
//@{
#define SPI_CTRL                        0       //!< Control Register
#define SPI_INTCTRL                     1       //!< Interrupt Control Register
#define SPI_STATUS                      2       //!< Status Register
#define SPI_DATA                        3       //!< Data Register
//@}

// \name Bitfields in CTRL
//@{
#define SPI_CTRL_PRESCALER_START        0       //!< Clock Prescaler
#define SPI_CTRL_PRESCALER_SIZE         2       //!< Clock Prescaler
#define SPI_CTRL_MODE_START             2       //!< Mode
#define SPI_CTRL_MODE_SIZE              2       //!< Mode
#define SPI_CTRL_MASTER_BIT             4       //!< Master/Slave Select
#define SPI_CTRL_DORD_BIT               5       //!< Data Order
#define SPI_CTRL_ENABLE_BIT             6       //!< Enable
#define SPI_CTRL_CLK2X_BIT              7       //!< Clock Double
//@}

// \name Bitfields in INTCTRL
//@{
# define SPI_INTCTRL_INTLVL_START       0       //!< Interrupt Level
# define SPI_INTCTRL_INTLVL_SIZE        2       //!< Interrupt Level
//@}

// \name Bitfields in INTCTRL
//@{
# define SPI_STATUS_WRCOL_BIT           6       //!< Write Collision Flag
# define SPI_STATUS_IF_BIT              7       //!< Interrupt Flag
//@}

//! \name Bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set
#define SPI_BIT(name)                                   \
	(1 << SPI_##name##_BIT)
//! \brief Create a mask with bitfield \a name set to \a value
#define SPI_BF(name,value)                              \
	(((value) & ((1 << SPI_##name##_SIZE) - 1))     \
	 << SPI_##name##_START)
//! \brief Extract the value of bitfield \a name from \a regval
#define SPI_BFEXT(name,value)                           \
	(((value) >> USART_##name##_START)              \
	 & ((1 << USART_##name##_SIZE) - 1))
//! \brief Return \a regval with bitfield \a name set to \a value
#define SPI_BFINS(name,value,old)                       \
	(((old) & ~(((1 << SPI_##name##_SIZE) - 1)      \
		    << SPI_##name##_START))             \
	 | SPI_BF(name,value))
//@}

//! \name Register access macros
//@{
//! \brief Write \a value to SPI register \a reg
#define spi_write_reg(spi, reg, value) \
	mmio_write8((void *)((uintptr_t)(spi) + SPI_##reg), value)
//! \brief Read the value of SPI register \a reg
#define spi_read_reg(spi, reg) \
	mmio_read8((void *)((uintptr_t)(spi) + SPI_##reg))
//@}

//! @}
#endif /* REGS_XMEGA_SPI_H_INCLUDED */

/**
 * \file
 *
 * \brief Chip-specific system clock management functions
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
#ifndef CHIP_SYSCLK_H_INCLUDED
#define CHIP_SYSCLK_H_INCLUDED

#include <chip/regs.h>

/**
 * \weakgroup sysclk_group
 * @{
 */

//! \name Maskable module clock IDs
//@{
#define SYSCLK_ADC              0x0001  //!< A/D Converter
#define SYSCLK_SPI              0x0004  //!< SPI controller
#define SYSCLK_TC1              0x0008  //!< Timer/Counter 1
#define SYSCLK_TC0              0x0020  //!< Timer/Counter 0
#define SYSCLK_TC2              0x0040  //!< Timer/Counter 2
#define SYSCLK_TWI              0x0080  //!< TWI controller
#define SYSCLK_USART1           0x0101  //!< USART 1
//!< USART 1 replicated due to chip naming convention */
#define SYSCLK_USART0           SYSCLK_USART1
#define SYSCLK_TC3              0x0108  //!< Timer/counter 3
#define SYSCLK_USB              0x0180  //!< USB controller
//@}

#ifndef __ASSEMBLY__

#include <assert.h>
#include <interrupt.h>
#include <stdint.h>

//! \name Enabling and disabling synchronous clocks
//@{

/**
 * \brief Enable a module clock derived from the system clock
 * \param id The ID of the module clock to be enabled (e.g.
 * #SYSCLK_USART1 to enable the second USART.)
 */
static inline void sysclk_enable_module(uint16_t id)
{
	irqflags_t iflags;
	uint8_t reg_offset;
	uint8_t mask;

	assert(id <= SYSCLK_USB);

	reg_offset = id >> 8;

	iflags = cpu_irq_save();
	mask = avr_read_reg8(PRR0 + reg_offset);
	mask &= ~(id & 0xff);
	avr_write_reg8(PRR0 + reg_offset, mask);
	cpu_irq_restore(iflags);
}

/**
 * \brief Disable a module clock derived from the system clock
 * \param id The ID of the module clock to be disabled (e.g.
 * #SYSCLK_USART1 to disable the second USART.)
 */
static inline void sysclk_disable_module(uint16_t id)
{
	irqflags_t iflags;
	uint8_t reg_offset;
	uint8_t mask;

	assert(id <= SYSCLK_USB);

	reg_offset = id >> 8;

	iflags = cpu_irq_save();
	mask = avr_read_reg8(PRR0 + reg_offset);
	mask |= (id & 0xff);
	avr_write_reg8(PRR0 + reg_offset, mask);
	cpu_irq_restore(iflags);
}

//@}

static inline void sysclk_init(void)
{
	avr_write_reg8(PRR0, 0xed);
	avr_write_reg8(PRR1, 0x89);
}

#endif /* !__ASSEMBLY__ */

//! @}

#endif /* CHIP_SYSCLK_H_INCLUDED */

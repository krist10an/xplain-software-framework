/**
 * \file
 *
 * \brief AT32UC3A3 portmux configuration
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
#ifndef CHIP_PORTMUX_H_INCLUDED
#define CHIP_PORTMUX_H_INCLUDED

#include <assert.h>
#include <gpio/portmux_gpio.h>

#define PORTMUX_PORT_A		((void *)GPIO_BASE)
#define PORTMUX_PORT_B		((void *)(GPIO_BASE + 0x100))
#define PORTMUX_PORT_C		((void *)(GPIO_BASE + 0x200))
#define PORTMUX_PORT_X0		((void *)(GPIO_BASE + 0x300))
#define PORTMUX_PORT_X1		((void *)(GPIO_BASE + 0x400))

/**
 * \brief Return the version of the GPIO module on this chip.
 */
static inline unsigned int portmux_gpio_version(void)
{
	return PORTMUX_GPIO_VER(1, 1);
}

/*
 * The below functions are usually called only once, so they are
 * defined inline to help gcc eliminate dead code and evaluate
 * constant expressions.
 */

#define PORTMUX_USART_RX	(1 << 0)	//!< Enable USART RX pin
#define PORTMUX_USART_TX	(1 << 1)	//!< Enable USART TX pin

static inline void portmux_enable_usart_inline(unsigned int id,
		unsigned long flags, unsigned long drive_strength)
{
	unsigned long		pin_mask = 0;

	switch (id) {
	case 0:
		if (flags & PORTMUX_USART_RX)
			pin_mask |= 1U << 3;
		if (flags & PORTMUX_USART_TX)
			pin_mask |= 1U << 4;
		portmux_select_peripheral(PORTMUX_PORT_A, pin_mask,
				PORTMUX_FUNC_A, PORTMUX_PULL_UP);
		break;
	case 1:
		if (flags & PORTMUX_USART_RX)
			pin_mask |= 1U << 5;
		if (flags & PORTMUX_USART_TX)
			pin_mask |= 1U << 6;
		portmux_select_peripheral(PORTMUX_PORT_A, pin_mask,
				PORTMUX_FUNC_A, PORTMUX_PULL_UP);
		break;
	case 2:
		if (flags & PORTMUX_USART_RX)
			portmux_select_peripheral(PORTMUX_PORT_A, 1U << 31,
					PORTMUX_FUNC_B, PORTMUX_PULL_UP);
		if (flags & PORTMUX_USART_TX)
			portmux_select_peripheral(PORTMUX_PORT_B, 1U << 0,
					PORTMUX_FUNC_B, PORTMUX_PULL_UP);
		break;
	case 3:
		if (flags & PORTMUX_USART_RX)
			portmux_select_peripheral(PORTMUX_PORT_B, 1U << 4,
					PORTMUX_FUNC_B, PORTMUX_PULL_UP);
		if (flags & PORTMUX_USART_TX)
			portmux_select_peripheral(PORTMUX_PORT_A, 1U << 29,
					PORTMUX_FUNC_B, PORTMUX_PULL_UP);
		break;
	default:
		unhandled_case(id);
		break;
	}
}

extern void portmux_enable_usart_noninline(unsigned int id,
		unsigned long flags, unsigned long drive_strength);

/**
 * \brief Configure the portmux to enable a given USART.
 *
 * \param id The USART ID to configure.
 * \param flags Flags indicating which pins to set up:
 *	- \c PORTMUX_USART_RX: \copybrief PORTMUX_USART_RX
 *	- \c PORTMUX_USART_TX: \copybrief PORTMUX_USART_TX
 * \param drive_strength The required drive strength of the USART pins.
 */
static inline void portmux_enable_usart(unsigned int id,
		unsigned long flags, unsigned long drive_strength)
{
	if (is_constant(id))
		portmux_enable_usart_inline(id, flags, drive_strength);
	else
		portmux_enable_usart_noninline(id, flags, drive_strength);
}

#define PORTMUX_TWI_ALERT	(1 << 0)	//!< Enable SMBus Alert

static inline void portmux_enable_twi(unsigned int id, unsigned long flags)
{
	switch (id) {
	case 0:
		portmux_select_peripheral(PORTMUX_PORT_A, 1 << 25 | 1 << 26,
				PORTMUX_FUNC_A, 0);
		if (flags & PORTMUX_TWI_ALERT)
			portmux_select_peripheral(PORTMUX_PORT_A, 1 << 14,
					PORTMUX_FUNC_B, 0);
		break;

	case 1:
		portmux_select_peripheral(PORTMUX_PORT_A, 1 << 14 | 1 << 15,
				PORTMUX_FUNC_C, 0);
		if (flags & PORTMUX_TWI_ALERT)
			portmux_select_peripheral(PORTMUX_PORT_A, 1 << 25,
					PORTMUX_FUNC_B, 0);
		break;

	default:
		unhandled_case(id);
		break;
	}
}

#define PORTMUX_USBB_ID		(1 << 0)	//!< Enable USBB ID pin
#define PORTMUX_USBB_VBOF	(1 << 1)	//!< Enable USBB VBOF pin

/**
 * \brief Configure the portmux to enable the USBB controller.
 *
 * \param flags Flags indicating which USBB pins to set up.
 *
 * \todo We actually have five different alternatives for each pin...
 */
static inline void portmux_enable_usbb(unsigned long flags)
{
	if (flags & PORTMUX_USBB_ID)
		portmux_select_peripheral(PORTMUX_PORT_B, 1 << 5,
				PORTMUX_FUNC_A, PORTMUX_PULL_UP);

	if (flags & PORTMUX_USBB_VBOF)
		portmux_select_peripheral(PORTMUX_PORT_B, 1 << 6,
				PORTMUX_FUNC_A, 0);
}

#define PORTMUX_MMCI_4BIT	(1 << 0)
#define PORTMUX_MMCI_8BIT	(PORTMUX_MMCI_4BIT | (1 << 1))
#define PORTMUX_MMCI_EXT_PULLUP	(1 << 2)

static inline void portmux_enable_mmci(unsigned int id, unsigned int slot,
		unsigned long flags, unsigned long drive_strength)
{
	unsigned long mask_a, mask_b;
	unsigned long portmux_flags = drive_strength | PORTMUX_PULL_UP;

	assert(id == 0);

	if (flags & PORTMUX_MMCI_EXT_PULLUP)
		portmux_flags = drive_strength;

	switch (slot) {
	case 0:
		/* First, the CLK signal. It doesn't need a pull-up */
		portmux_select_peripheral(PORTMUX_PORT_A, 1 << 27,
				PORTMUX_FUNC_A, drive_strength);

		mask_a = (1 << 28) | (1 << 29);	/* CMD and DATA0 */
		mask_b = 0;
		if (flags & PORTMUX_MMCI_4BIT) {
			/* DATA1..DATA2 */
			mask_a |= (1 << 30) | (1U << 31);
			/* DATA3 */
			mask_b |= 1 << 0;
		}
		if (flags & PORTMUX_MMCI_8BIT)
			/* DATA4..DATA7 */
			mask_b |= (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);

		portmux_select_peripheral(PORTMUX_PORT_A, mask_a,
				PORTMUX_FUNC_A, portmux_flags);

		portmux_select_peripheral(PORTMUX_PORT_B, mask_b,
				PORTMUX_FUNC_A, portmux_flags);
		break;

	case 1:
		/* First, the CLK signal. It doesn't need a pull-up */
		portmux_select_peripheral(PORTMUX_PORT_A, 1 << 27,
				PORTMUX_FUNC_A, drive_strength);

		mask_a = (1 << 15) | (1 << 19);	/* CMD and DATA0 */
		mask_b = 0;
		if (flags & PORTMUX_MMCI_4BIT)
			/* DATA1..DATA3 */
			mask_a |= (1 << 18) | (1 << 17) | (1 << 16);

		portmux_select_peripheral(PORTMUX_PORT_A, mask_a,
				PORTMUX_FUNC_A, portmux_flags);
		break;

	default:
		unhandled_case(slot);
		break;
	}
}

static inline void portmux_enable_spi_inline(unsigned int id,
		unsigned long drive_strength)
{
	switch (id) {
	case 0:
		/* MOSI and SCK */
		portmux_select_peripheral(PORTMUX_PORT_A,
				(1 << 8) | (1 << 10),
				PORTMUX_FUNC_A, drive_strength);

		/* MISO may float */
		portmux_select_peripheral(PORTMUX_PORT_A, 1 << 11,
				PORTMUX_FUNC_A,
				drive_strength | PORTMUX_BUSKEEPER);
		break;
	case 1:
		/* MOSI and SCK */
		portmux_select_peripheral(PORTMUX_PORT_A,
				(1 << 15) | (1 << 16),
				PORTMUX_FUNC_B, drive_strength);

		/* MISO may float */
		portmux_select_peripheral(PORTMUX_PORT_A, 1 << 19,
				PORTMUX_FUNC_B,
				drive_strength | PORTMUX_BUSKEEPER);
		break;
	default:
		unhandled_case(id);
		break;
	}
}

extern void portmux_enable_spi_noninline(unsigned int id,
		unsigned long drive_strength);

/**
 * \brief Configure the portmux to enable a given SPI controller.
 *
 * \param id The ID of the SPI controller to be configured.
 * \param drive_strength The required drive strength of the SPI pins.
 */
static inline void portmux_enable_spi(unsigned int id,
		unsigned long drive_strength)
{
	if (is_constant(id))
		portmux_enable_spi_inline(id, drive_strength);
	else
		portmux_enable_spi_noninline(id, drive_strength);
}

#endif /* CHIP_PORTMUX_H_INCLUDED */

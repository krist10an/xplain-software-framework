/**
 * \file
 *
 * \brief Xplain AT90USB1287 board-specific declarations
 *
 * Copyright (C) 2009 - 2010 Atmel Corporation. All rights reserved.
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
#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include <gpio.h>

/** \brief Board I/O lines definitions. */
enum board_gpio_id {
	/** \brief SPI clock line. */
	BOARD_SPI_SCK_ID        = CREATE_GPIO_PIN(PORTB, 1),
	/** \brief SPI MOSI line. */
	BOARD_SPI_MOSI_ID       = CREATE_GPIO_PIN(PORTB, 2),
	/** \brief SPI MISO line. */
	BOARD_SPI_MISO_ID       = CREATE_GPIO_PIN(PORTB, 3),
	/** \brief Board output line wired to XMEGA chip reset pin. */
	BOARD_XMEGA_RESET_ID    = CREATE_GPIO_PIN(PORTD, 5),
	/**
	 * \brief Board input line wired to TPI header pin used as user input
	 *        to select between different USB modes.
	 */
	BOARD_USB_MODE_ID       = CREATE_GPIO_PIN(PORTF, 7),
};

void board_init(void);
void board_gpio_mcu_reset(bool set);
bool board_gpio_is_usb_msc_mode(void);
void board_gpio_set_spi_master_mode(bool master_mode);

#endif /* BOARD_H_INCLUDED */

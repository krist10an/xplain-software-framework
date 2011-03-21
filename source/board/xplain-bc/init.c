/**
 * \file
 *
 * \brief Initialization code for the AT90USB chip on the Xplain board
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
#include <led.h>
#include <gpio.h>
#include <board.h>
#include <interrupt.h>

#include <chip/regs.h>

void board_init(void)
{
	uint8_t         mcucr;
	irqflags_t      iflags;

	/*
	 * Make sure no interrupts can come along and interfere with the cycle
	 * critical operation to write the MCUCR register.
	 */
	iflags = cpu_irq_save();

	/* Disable JTAG interface to make I/O lines available. */
	mcucr = avr_read_reg8(MCUCR) | AVR_BIT(MCUCR_JTD);
	avr_write_reg8(MCUCR, mcucr);
	avr_write_reg8(MCUCR, mcucr);

	cpu_irq_restore(iflags);

	/* Start up by holding the XMEGA reset line. */
	port_select_gpio_pin(BOARD_XMEGA_RESET_ID,
			GPIO_DIR_OUTPUT | GPIO_INIT_LOW);

	/*
	 * Initialize the RED LED deactivated and configure the USB mode switch
	 * pin as an input line with pull-up enabled.
	 */
	port_select_gpio_pin(BOARD_LED_RED, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	port_select_gpio_pin(BOARD_USB_MODE_ID, GPIO_DIR_INPUT | GPIO_PULL_UP);

	/* Finally set the SPI I/O lines for SPI master functionality. */
	board_gpio_set_spi_master_mode(true);
}

/**
 * \brief Control the reset line to the XMEGA chip.
 *
 * This function will set or release the reset line to the XMEGA chip depending
 * on the \a set variable. To hold the XMEGA chip in reset \a set must be true,
 * to release from reset \a set must be false.
 */
void board_gpio_mcu_reset(bool set)
{
	gpio_set_value(BOARD_XMEGA_RESET_ID, !set);
}

/**
 * \brief Read input line to get if the USB MSC interface should be enabled.
 *
 * This function will read the \ref BOARD_USB_MODE_ID input line and return
 * true if MSC should be active and false otherwize.
 */
bool board_gpio_is_usb_msc_mode(void)
{
	return !gpio_get_value(BOARD_USB_MODE_ID);
}

/**
 * \brief Set SPI I/O lines given the SPI mode.
 *
 * This function will set the SPI I/O lines in master mode when \a master_mode
 * is true, the I/O lines will be configured for slave mode otherwize.
 */
void board_gpio_set_spi_master_mode(bool master_mode)
{
	if (master_mode) {
		/* Set the SPI interface in master mode. \todo revisit. */
		port_select_gpio_pin(BOARD_SPI_SCK_ID, GPIO_DIR_OUTPUT);
		port_select_gpio_pin(BOARD_SPI_MOSI_ID, GPIO_DIR_OUTPUT);
		port_select_gpio_pin(BOARD_SPI_MISO_ID, GPIO_DIR_INPUT);
	} else {
		/* Set the SPI interface in slave mode. \todo revisit. */
		port_select_gpio_pin(BOARD_SPI_SCK_ID, GPIO_DIR_INPUT);
		port_select_gpio_pin(BOARD_SPI_MOSI_ID, GPIO_DIR_INPUT);
		port_select_gpio_pin(BOARD_SPI_MISO_ID, GPIO_DIR_OUTPUT);
	}
}

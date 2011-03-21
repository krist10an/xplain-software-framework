/**
 * \file
 *
 * \brief Initialization code for the XMEGA chip on the Xplain board
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
#include <board.h>
#include <gpio.h>
#ifdef __ICCAVR__
#include <pmic.h>
#endif

#ifdef CONFIG_TOUCH_RESISTIVE
# include <board/touch/resistive/touch.h>
#endif
#ifdef CONFIG_EXTRAM_SDRAM
# include <board/sdram.h>
#endif

void board_init(void)
{
	/* Set LED pins to be outputs, and all off. */
	port_select_gpio((void *)PORTE_BASE, 0xff, PORT_DIR_OUTPUT
			| PORT_INIT_HIGH);
	/* If USARTD0 is used for UART communication set PORTD tx pin as output */
#ifdef CONFIG_XPLAIN_UARTD0
	port_select_gpio_pin(27, PORT_DIR_OUTPUT | PORT_INIT_HIGH);
#endif
	/* If USARTD1 is used for UART communication set PORTD tx pin as output */
#ifdef CONFIG_XPLAIN_UARTD1
	port_select_gpio_pin(31, PORT_DIR_OUTPUT | PORT_INIT_HIGH);
#endif
	/* Set UART0 TX pin (PC3) as output */
	port_select_gpio_pin(19, PORT_DIR_OUTPUT | PORT_INIT_HIGH);

	/* Init SPI master IO on port C
	 * Outputs: SCK on pin 7 and MOSI on pin 5
	 * Inputs: MISO on pin 6 (Leave unchanged)
	 */
	port_select_gpio_pin(21, PORT_DIR_OUTPUT);
	port_select_gpio_pin(23, PORT_DIR_OUTPUT);

	/* Set button pins on port F to be inputs with pull-up and interrupt
	 * sensing on both edges.
	 */
	port_select_gpio((void *)PORTF_BASE, 0xFF,
			PORT_DIR_INPUT | PORT_BOTHEDGES | PORT_PULL_UP);

	/* If resistive touch is configured, unset the PR bit for ADC A */
#ifdef CONFIG_TOUCH_RESISTIVE
	board_enable_touch_adc();
#endif

	/*
	 * IAR does not enable low, medium and high interrupts on startup in
	 * PMIC, so they must be enabled "manually".
	 *
	 * \todo rewrite this into a constructor for main.
	 */
#ifdef __ICCAVR__
	pmic_enable_low_lvl_int();
	pmic_enable_medium_lvl_int();
	pmic_enable_high_lvl_int();
#endif

	/* If huge memory is enabled, enable the onboard SDRAM. */
#ifdef CONFIG_EXTRAM_SDRAM
	// Configure I/O ports used by the EBI, except data lines.
	port_write_reg(PORTH_BASE, OUT, 0x0f);
	port_write_reg(PORTH_BASE, DIR, 0xff);
	port_write_reg(PORTK_BASE, DIR, 0xff);
	port_write_reg(PORTJ_BASE, DIR, 0xf0);

	board_enable_sdram();
#endif
}

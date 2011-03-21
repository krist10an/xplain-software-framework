/**
 * \file
 *
 * \brief UART example application that loops back data it receives.
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
#include <clk/sys.h>
#include <led.h>
#include <uart.h>
#include <uart/ctrl.h>

#define UART_ID CONFIG_UART_ID

#ifndef CONFIG_BOARD_LED_READY
# define CONFIG_BOARD_LED_READY	BOARD_LED0_ID
#endif
#ifndef CONFIG_BOARD_LED_XFER
# define CONFIG_BOARD_LED_XFER	BOARD_LED1_ID
#endif

int main(void)
{
	uint8_t data;
	bool led_on = true;

	sysclk_init();
	board_init();

	uart_enable_clock(UART_ID);
	uart_ctrl_init_defaults(UART_ID);
	uart_enable(UART_ID, UART_FLAG_RX | UART_FLAG_TX);

	// Light up a LED to show that we are ready
	led_activate(CONFIG_BOARD_LED_READY);

	for (;;) {

		while (!uart_get_byte(UART_ID, &data))
			;
		// toggle a LED each time we get data
		if (led_on == true) {
			led_activate(CONFIG_BOARD_LED_XFER);
			led_on = false;
		}
		else {
			led_deactivate(CONFIG_BOARD_LED_XFER);
			led_on = true;
		}
		while (!uart_put_byte(UART_ID, data))
			;
	}
}

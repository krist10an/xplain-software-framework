/**
 * \file
 *
 * \brief Board-specific LED control
 *
 * Please don't include this file directly. Use <led.h> instead.
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
#ifndef BOARD_LED_H_INCLUDED
#define BOARD_LED_H_INCLUDED

#include <chip/gpio.h>

/** \brief Connection of on-board LEDs */
enum board_led_id {
	BOARD_LED0_ID	= 67,
	BOARD_LED1_ID	= 101,
	BOARD_LED2_ID	= 105,
	BOARD_LED3_ID	= 102,
};

/**
 * \brief Turn the LED identified by \a id on.
 */
__always_inline static void led_activate(enum board_led_id id)
{
	gpio_set_value(id, false);
}

/**
 * \brief Turn the LED identified by \a id off.
 */
__always_inline static void led_deactivate(enum board_led_id id)
{
	gpio_set_value(id, true);
}

/**
 * \brief Toggle the LED identified by \a id.
 */
__always_inline static void led_toggle(enum board_led_id id)
{
	gpio_toggle_value(id);
}
#endif /* BOARD_LED_H_INCLUDED */

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

#include <gpio.h>
#include <compiler.h>

enum board_led_id {
	BOARD_LED0_ID = CREATE_GPIO_PIN(PORTE, 0),
	BOARD_LED1_ID = CREATE_GPIO_PIN(PORTE, 1),
	BOARD_LED2_ID = CREATE_GPIO_PIN(PORTE, 2),
	BOARD_LED3_ID = CREATE_GPIO_PIN(PORTE, 3),
	BOARD_LED4_ID = CREATE_GPIO_PIN(PORTE, 4),
	BOARD_LED5_ID = CREATE_GPIO_PIN(PORTE, 5),
	BOARD_LED6_ID = CREATE_GPIO_PIN(PORTE, 6),
	BOARD_LED7_ID = CREATE_GPIO_PIN(PORTE, 7),
};

__always_inline static void led_activate(enum board_led_id id)
{
	gpio_set_value(id, false);
}

__always_inline static void led_deactivate(enum board_led_id id)
{
	gpio_set_value(id, true);
}

__always_inline static void led_toggle(enum board_led_id id)
{
	gpio_toggle_value(id);
}
#endif /* BOARD_LED_H_INCLUDED */

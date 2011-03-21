/**
 * \file
 *
 * \brief AVR XMEGA timer/counter Timer.
 *
 * This file contains function prototypes and inline definitions for the
 * AVR XMEGA timer/counter implementation of the Timer driver.
 *
 * Do not include this file directly. Include <timer.h> instead.
 *
 * Copyright (c) 2010 Atmel Corporation. All rights reserved.
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

#ifndef TIMER_TC_TIMER_XMEGA_H_INCLUDED
#define TIMER_TC_TIMER_XMEGA_H_INCLUDED

#include <timer.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * \addtogroup timer_xmega_group
 * \section tc_timer_xmega_section AVR XMEGA TC timer
 *
 * Each AVR XMEGA TC-module corresponds to a single 16-bit timer, and this
 * driver makes use of their compare channel A as the alarm trigger.\n
 *
 * To enable a timer, config symbols \a CONFIG_TIMER_n must be defined, where
 * \a n corresponds to the ID of the timer. \a CONFIG_TIMER_RESOLUTION must
 * also be set to a value (in Hz) for the default TC clock rate to be set.\n
 *
 * This driver will disable the TCs whenever they are not needed to conserve
 * power.
 *
 * \todo Revisit, add 32-bit TCs. This requires use of the event system.
 * @{
 */

void tc_timer_init(uint8_t tc_id, void *regs, struct timer *timer,
		timer_callback_t timer_callback);
void tc_timer_start(uint8_t tc_id, struct timer *timer);
void tc_timer_stop(uint8_t tc_id, struct timer *timer);
void tc_timer_set_alarm(uint8_t tc_id, struct timer *timer, uint16_t delay);
uint16_t tc_timer_get_time(uint8_t tc_id, struct timer *timer);
timer_res_t tc_timer_set_resolution(uint8_t tc_id, uint32_t resolution);
void tc_timer_write_resolution(uint8_t tc_id, struct timer *timer,
		timer_res_t resolution);
uint32_t tc_timer_get_resolution(uint8_t tc_id, timer_res_t timer_res);

/**
 * \brief Returns the maximum delta for timers.
 * \internal
 *
 * \return Maximum number of ticks that can be used when setting timer alarms.
 */
static inline uint16_t tc_timer_maximum_delta(void)
{
	return (uint16_t)~0;
}

/**
 * \brief Read the currect clock selection of a timer.
 * \internal
 *
 * \param timer Pointer to timer struct.
 *
 * \return Current clock selection for \a timer.
 */
static inline timer_res_t tc_timer_read_resolution(struct timer *timer)
{
	return timer->clksel;
}

//!@}

#endif /* TIMER_TC_TIMER_XMEGA_H_INCLUDED */

/**
 * \file
 *
 * \brief AVR32 timer definitions.
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
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

#ifndef TIMER_TIMER_AVR32_H_INCLUDED
#define TIMER_TIMER_AVR32_H_INCLUDED

#include <types.h>

/**
 * \defgroup timer_avr32_group AVR32 Timer
 * \ingroup timer_group
 *
 * The AVR32 devices have several hardware sources that can be used as a
 * timer. Each timer can be selected by using an unique ID. Valid IDs and
 * their corresponding implementations are listed in the sub-sections which
 * are generated from <chip/timer.h>.
 *
 *@{
 */

/**
 * \brief Driver internal representation of the resolution.
 */
typedef uint32_t timer_res_t;

/**
 * \brief Timer type.
 *
 * An object of this type must be passed to the timer driver.
 */
typedef struct timer timer_t;

/**
 * \brief Timer callback function type.
 */
typedef void (*timer_callback_t) (timer_t *timer);
/** @} */

/**
 * \defgroup timer_avr32_internal_group AVR32 Timer internals
 * \ingroup timer_avr32_group
 * @{
 */

/**
 * \brief Timer data.
 * \internal
 *
 * This type of  structure is passed to the driver and is initialized during
 * the timer initialization. It stores all needed driver internal data.
 *
 */
struct timer {
	/** Base address of timer registers. */
	void *regs;
	/** Function pointer to application timer callback function*/
	void (*callback)(struct timer *timer);
};

/**
 * \brief Select a timer function.
 * \internal
 *
 * This macro is needed to expand macros properly that are passed in as IDs.
 * In addition it reduces these kind of macros to one set instead of several
 * ones for each function.
 *
 * \param func Function name.
 * \param timer_id Timer ID.
 * \param ... Variadic arguments.
 */
#define TIMER_SELECT(func, timer_id, ...) \
	timer##timer_id##_##func##_priv(__VA_ARGS__)

#define timer_init(timer_id, timer, callback) \
	TIMER_SELECT(init, timer_id, timer, callback)

#define timer_start(timer_id, timer) \
	TIMER_SELECT(start, timer_id, timer)

#define timer_stop(timer_id, timer) \
	TIMER_SELECT(stop, timer_id, timer)

#define timer_set_alarm(timer_id, timer, delay) \
	TIMER_SELECT(set_alarm, timer_id, timer, delay)

#define timer_get_time(timer_id, timer) \
	TIMER_SELECT(get_time, timer_id, timer)

#define timer_default_resolution(timer_id) \
	TIMER_SELECT(default_resolution, timer_id)

#define timer_write_resolution(timer_id, timer, timer_res) \
	TIMER_SELECT(write_resolution, timer_id, timer, timer_res)

#define timer_read_resolution(timer_id, timer) \
	TIMER_SELECT(read_resolution, timer_id, timer)

#define timer_set_resolution(timer_id, timer, timer_res) \
	TIMER_SELECT(set_resolution, timer_id, timer, timer_res)

#define timer_get_resolution(timer_id, timer, resolution) \
	TIMER_SELECT(get_resolution, timer_id, timer, resolution)

#define timer_maximum_delta(timer_id, timer) \
	TIMER_SELECT(maximum_delta, timer_id, timer)
/** @} */

#endif /* TIMER_TIMER_AVR32_H_INCLUDED */

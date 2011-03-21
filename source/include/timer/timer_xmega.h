/**
 * \file
 *
 * \brief AVR XMEGA Timer.
 *
 * This file contains definitions of the three data types needed for use of the
 * Timer driver:
 * - \ref timer struct - common to all timer functions
 * - \ref timer_callback_t - pointer to timer callback functions
 * - \ref timer_res_t - used when changing resolution of timers
 *
 * In addition, this file contains macros that expand the function names defined
 * in the API to macro names that contain the TC ID. This is necessary for
 * different implementations for the different timer IDs to be possible.\n
 * Each chip has a <chip/timer.h> that in turn define these expanded macros
 * names as the function names of whichever implementations are available for
 * that specific chip.
 *
 * Do not include this file directly. Include <timer.h> instead.
 *
 * Copyright (C) 2010 Atmel Corporation. All rights reserved.
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
#ifndef TIMER_TIMER_XMEGA_H_INCLUDED
#define TIMER_TIMER_XMEGA_H_INCLUDED

#include <chip/timer.h>
#include <types.h>

/**
 * \defgroup timer_xmega_group AVR XMEGA Timer
 * \ingroup timer_group
 *
 * The AVR XMEGA family features different modules that can be used as timers.
 * A timer can be selected by using an ID. The valid IDs and the corresponding
 * implementation are listed in the chip-specific section of this documentation
 * (<chip/timer.h>) since the different devices feature different
 * configurations.
 * @{
 */

/**
 * \brief Driver internal timer resolution type
 */
typedef uint8_t timer_res_t;

struct timer;

/**
 * \brief Timer callback function type.
 *
 * Pointer to the function to be called once a timer expires.
 *
 * \param timer Pointer to the timer_t object that was associated with the
 * current alarm/timer interrupt.
 */
typedef void (*timer_callback_t) (struct timer *timer);

//!@}

/**
 * \defgroup timer_xmega_internal_group AVR XMEGA Timer internals
 * \ingroup timer_xmega_group
 * @{
 */

/**
 * \brief Timer data.
 * \internal
 *
 * This type of structure is passed to the driver and is initialized during
 * the timer initialization. It stores all needed driver internal data.
 *
 */
struct timer {
	//! Base address of timer registers.
	void *regs;
	//! Clock selection.
	uint8_t clksel;
	//! Interrupt level.
	uint8_t intlvl;
	//! Function pointer to application timer callback function.
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
 * \param func     Function name.
 * \param timer_id ID of timer to use.
 * \param ...      Variadic arguments.
 */
#define TIMER_SELECT(func, timer_id, ...)                                  \
	timer##timer_id##_##func##_priv(__VA_ARGS__)

#define timer_init(timer_id, timer, timer_callback)                        \
	TIMER_SELECT(init, timer_id, timer, timer_callback)

#define timer_start(timer_id, timer)                                       \
	TIMER_SELECT(start, timer_id, timer)

#define timer_stop(timer_id, timer)                                        \
	TIMER_SELECT(stop, timer_id, timer)

#define timer_set_alarm(timer_id, timer, delay)                            \
	TIMER_SELECT(set_alarm, timer_id, timer, delay)

#define timer_get_time(timer_id, timer)                                    \
	TIMER_SELECT(get_time, timer_id, timer)

#define timer_default_resolution(timer_id)                                 \
	TIMER_SELECT(default_resolution, timer_id)

#define timer_write_resolution(timer_id, timer, timer_resolution)          \
	TIMER_SELECT(write_resolution, timer_id, timer, timer_resolution)

#define timer_read_resolution(timer_id, timer)                             \
	TIMER_SELECT(read_resolution, timer_id, timer)

#define timer_set_resolution(timer_id, timer, resolution)                  \
	TIMER_SELECT(set_resolution, timer_id, timer, resolution)

#define timer_get_resolution(timer_id, timer, timer_resolution)            \
	TIMER_SELECT(get_resolution, timer_id, timer, timer_resolution)

#define timer_maximum_delta(timer_id, timer)                               \
	TIMER_SELECT(maximum_delta, timer_id, timer)

//!@}

#endif /* TIMER_TIMER_XMEGA_H_INCLUDED */

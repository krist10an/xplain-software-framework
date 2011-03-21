/**
 * \file
 *
 * \brief Timer API.
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

#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <chip/timer.h>

/**
 * \defgroup timer_group Timer
 * @{
 *
 * A timer can be used to time events on the system. For example instead of
 * active waiting, the CPU can go to sleep until the timer wakes the CPU
 * up again. This is done by setting alarms on the timers. It is also possible
 * to configure callback functions to be called once the alarms trigger.
 *
 * Timers are based on various different hardware implementations and these
 * are described in the sub-sections in this documentation. Depending on the
 * chips hardware features, some of them may be available or not. A description
 * which timer IDs correspond to which implementation is available in the
 * sub-sections (added from <chip/timer.h>).
 *
 * To enable the timer framework, an application must define the configuration
 * symbol CONFIG_TIMER. Further configuration options are described in the
 * specific implementations documentation.
 *
 * The timers have a default resolution that can be changed by defining a
 * CONFIG_TIMER_RESOLUTION symbol. The resolution must be expressed in ticks
 * per second.
 *
 * \note Depending on the implementation, the callback functions may be called
 * inside an interrupt handler. The callback functions should therefore be kept
 * as short and quick as possible, to avoid keeping other interrupts waiting.
 */

/*
 * Because the timer has various implementations due to the different modules,
 * architectures, chips... we keep the common API description here to avoid
 * redunancy.
 * The implementation specific API is documented in the in the specific
 * drivers (e.g. XMEGA specific API is available in the XMEGA timer driver).
 */


/**
 * \def timer_init(timer_id, timer, timer_callback)
 *
 * \brief Timer initialization.
 *
 * An application can pass in a function pointer to a callback function which
 * will be called each time the timer triggers.
 *
 * \param timer_id Timer ID.
 * \param timer Pointer to timer struct.
 * \param timer_callback Function pointer to callback function.
 */

/**
 * \def timer_start(timer_id, timer)
 *
 * \brief Start the timer.
 *
 * Calling this function will start the timer. The timer is reset before it
 * starts thus it will start counting from 0.
 *
 * \param timer_id Timer ID.
 * \param timer Pointer to timer struct.
 */

/**
 * \def timer_stop(timer_id, timer)
 *
 * \brief Stop the timer.
 *
 * \param timer_id Timer ID.
 * \param timer Pointer to timer struct.
 */

/**
 * \def timer_set_alarm(timer_id, timer, delay)
 *
 * \brief Set timer alarm.
 *
 * The caller must ensure that the delay is within the allowed range of the
 * timer. The maximum allowed value can be obtained by calling the function
 * \a timer_maximum_delta.
 * It is only possible to configure one alarm at once. If this function is
 * called again while the current alarm has not triggered yet it will replace
 * the current alarm settings.
 *
 * \param timer_id Timer ID.
 * \param timer Pointer to timer struct.
 * \param delay Delay in timer ticks after which the alarm should trigger.
 */

/**
 * \def timer_get_time(timer_id, timer)
 *
 * \brief Get current timer time stamp.
 *
 * When the timer is stopped this function will always return 0 since the
 * timer will start from 0 when started again.
 *
 * \param timer_id Timer ID.
 * \param timer Pointer to timer struct.
 * \return Current time stamp.
 */

/**
 * \def timer_write_resolution(timer_id, timer, timer_resolution)
 *
 * \brief Set a timer resolution.
 *
 * \param timer_id Timer ID.
 * \param timer Pointer to timer struct.
 * \param timer_resolution This is a driver internal representation of the
 * timer resolution. It can be generated by calling the \a timer_set_resolution
 * function.
 */

/**
 * \def timer_set_resolution(timer_id, timer, resolution)
 *
 * \brief Generate a timer resolution configuration.
 *
 * \param timer_id Timer ID.
 * \param timer Pointer to timer struct.
 * \param resolution Desired timer resolution in ticks per second.
 */

/**
 * \def timer_read_resolution(timer_id, timer)
 *
 * \brief Read the current timer resolution configuration.
 *
 * \param timer_id Timer ID.
 * \param timer Pointer to timer struct.
 * \return Implementation specific representation of the timer resolution.
 * This value can be converted with \a timer_get_resolution to a representation
 * in ticks per second.
 */

/**
 * \def timer_get_resolution(timer_id, timer, timer_resolution)
 *
 * \brief Convert the timer resolution configuration to ticks per second.
 *
 * \param timer_id Timer ID.
 * \param timer Pointer to timer struct.
 * \param timer_resolution Driver specific timer resolution representation.
 * \return Timer resolution in ticks per second.
 */

/**
 * \def timer_maximum_delta(timer_id, timer)
 *
 * \brief Get the maximum allowed timer delta.
 *
 * \param timer_id Timer ID.
 * \param timer Pointer to timer struct.
 * \return Maximum allowed delta in timer ticks that can be set with the \a
 * timer_set_alarm function.
 */

/** @} */
#endif /* TIMER_H_INCLUDED */

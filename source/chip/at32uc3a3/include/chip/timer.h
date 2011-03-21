/**
 * \file
 *
 * \brief AT32UC3A3 timer definitions.
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
#ifndef CHIP_TIMER_H_INCLUDED
#define CHIP_TIMER_H_INCLUDED

#include <timer/timer_avr32.h>
#include <timer/tc_timer_avr32.h>
/**
 * \addtogroup timer_avr32_group
 *
 * \section at32uc3a3_timers AT32UC3A3 timers.
 *
 * The list below show the relation between timer IDs and used hardware:
 * - ID 0: Channel 0 of TC module 0
 * - ID 1: Channel 1 of TC module 0
 * - ID 2: Channel 2 of TC module 0
 * - ID 3: Channel 0 of TC module 1
 * - ID 4: Channel 1 of TC module 1
 * - ID 5: Channel 2 of TC module 1
 */


#define timer0_init_priv(timer, callback) \
	tc_timer0_init(timer, callback)
#define timer1_init_priv(timer, callback) \
	tc_timer1_init(timer, callback)
#define timer2_init_priv(timer, callback) \
	tc_timer2_init(timer, callback)
#define timer3_init_priv(timer, callback) \
	tc_timer3_init(timer, callback)
#define timer4_init_priv(timer, callback) \
	tc_timer4_init(timer, callback)
#define timer5_init_priv(timer, callback) \
	tc_timer5_init(timer, callback)

#define timer0_start_priv(timer) \
	tc_timer_start(timer)
#define timer1_start_priv(timer) \
	tc_timer_start(timer)
#define timer2_start_priv(timer) \
	tc_timer_start(timer)
#define timer3_start_priv(timer) \
	tc_timer3_start(timer)
#define timer4_start_priv(timer) \
	tc_timer_start(timer)
#define timer5_start_priv(timer) \
	tc_timer_start(timer)

#define timer0_stop_priv(timer) \
	tc_timer_stop(timer)
#define timer1_stop_priv(timer) \
	tc_timer_stop(timer)
#define timer2_stop_priv(timer) \
	tc_timer_stop(timer)
#define timer3_stop_priv(timer) \
	tc_timer_stop(timer)
#define timer4_stop_priv(timer) \
	tc_timer_stop(timer)
#define timer5_stop_priv(timer) \
	tc_timer_stop(timer)

#define timer0_set_alarm_priv(timer, delay) \
	tc_timer_set_alarm(timer, delay)
#define timer1_set_alarm_priv(timer, delay) \
	tc_timer_set_alarm(timer, delay)
#define timer2_set_alarm_priv(timer, delay) \
	tc_timer_set_alarm(timer, delay)
#define timer3_set_alarm_priv(timer, delay) \
	tc_timer_set_alarm(timer, delay)
#define timer4_set_alarm_priv(timer, delay) \
	tc_timer_set_alarm(timer, delay)
#define timer5_set_alarm_priv(timer, delay) \
	tc_timer_set_alarm(timer, delay)

#define timer0_get_time_priv(timer) \
	tc_timer_get_time(timer)
#define timer1_get_time_priv(timer) \
	tc_timer_get_time(timer)
#define timer2_get_time_priv(timer) \
	tc_timer_get_time(timer)
#define timer3_get_time_priv(timer) \
	tc_timer_get_time(timer)
#define timer4_get_time_priv(timer) \
	tc_timer_get_time(timer)
#define timer5_get_time_priv(timer) \
	tc_timer_get_time(timer)

#define timer0_default_resolution_priv(timer) \
	tc_timer_default_resolution(timer)
#define timer1_default_resolution_priv(timer) \
	tc_timer_default_resolution(timer)
#define timer2_default_resolution_priv(timer) \
	tc_timer_default_resolution(timer)
#define timer3_default_resolution_priv(timer) \
	tc_timer_default_resolution(timer)
#define timer4_default_resolution_priv(timer) \
	tc_timer_default_resolution(timer)
#define timer5_default_resolution_priv(timer) \
	tc_timer_default_resolution(timer)

#define timer0_write_resolution_priv(timer, resolution) \
	tc_timer_write_resolution(timer, resolution)
#define timer1_write_resolution_priv(timer, resolution) \
	tc_timer_write_resolution(timer, resolution)
#define timer2_write_resolution_priv(timer, resolution) \
	tc_timer_write_resolution(timer, resolution)
#define timer3_write_resolution_priv(timer, resolution) \
	tc_timer_write_resolution(timer, resolution)
#define timer4_write_resolution_priv(timer, resolution) \
	tc_timer_write_resolution(timer, resolution)
#define timer5_write_resolution_priv(timer, resolution) \
	tc_timer_write_resolution(timer, resolution)

#define timer0_read_resolution_priv(timer) \
	tc_timer_read_resolution(timer)
#define timer1_read_resolution_priv(timer) \
	tc_timer_read_resolution(timer)
#define timer2_read_resolution_priv(timer) \
	tc_timer_read_resolution(timer)
#define timer3_read_resolution_priv(timer) \
	tc_timer_read_resolution(timer)
#define timer4_read_resolution_priv(timer) \
	tc_timer_read_resolution(timer)
#define timer5_read_resolution_priv(timer) \
	tc_timer_read_resolution(timer)

#define timer0_set_resolution_priv(timer, resolution) \
	tc_timer_set_resolution(0, resolution)
#define timer1_set_resolution_priv(timer, resolution) \
	tc_timer_set_resolution(0, resolution)
#define timer2_set_resolution_priv(timer, resolution) \
	tc_timer_set_resolution(0, resolution)
#define timer3_set_resolution_priv(timer, resolution) \
	tc_timer_set_resolution(1, resolution)
#define timer4_set_resolution_priv(timer, resolution) \
	tc_timer_set_resolution(1, resolution)
#define timer5_set_resolution_priv(timer, resolution) \
	tc_timer_set_resolution(1, resolution)

#define timer0_get_resolution_priv(timer, resolution) \
	tc_timer_get_resolution(0, resolution)
#define timer1_get_resolution_priv(timer, resolution) \
	tc_timer_get_resolution(0, resolution)
#define timer2_get_resolution_priv(timer, resolution) \
	tc_timer_get_resolution(0, resolution)
#define timer3_get_resolution_priv(timer, resolution) \
	tc_timer_get_resolution(1, resolution)
#define timer4_get_resolution_priv(timer, resolution) \
	tc_timer_get_resolution(1, resolution)
#define timer5_get_resolution_priv(timer, resolution) \
	tc_timer_get_resolution(1, resolution)

#define timer0_maximum_delta_priv(timer) \
	tc_timer_maximum_delta(timer)
#define timer1_maximum_delta_priv(timer) \
	tc_timer_maximum_delta(timer)
#define timer2_maximum_delta_priv(timer) \
	tc_timer_maximum_delta(timer)
#define timer3_maximum_delta_priv(timer) \
	tc_timer_maximum_delta(timer)
#define timer4_maximum_delta_priv(timer) \
	tc_timer_maximum_delta(timer)
#define timer5_maximum_delta_priv(timer) \
	tc_timer_maximum_delta(timer)

#endif /* CHIP_TIMER_H_INCLUDED */

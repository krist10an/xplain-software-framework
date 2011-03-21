/**
 * \file
 *
 * \brief ATxmega128A1 chip-specific Timer definitions.
 *
 * This file contains macros that define the expanded macros from
 * <timer/timer_xmega.h> as calls to the functions of the Timer implementations
 * available for this chip.
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
#ifndef CHIP_TIMER_H_INCLUDED
#define CHIP_TIMER_H_INCLUDED

#include <tc/tc_xmega.h>
#include <timer/timer_xmega.h>
#include <timer/tc_timer_xmega.h>

/**
 * \addtogroup timer_xmega_group
 * \section atxmega128a1_timer_section ATxmega128A1 timers
 *
 * The list below shows the relation between timer ID and its corresponding
 * TC module:
 * - ID 0: TC 0 on port C
 * - ID 1: TC 1 on port C
 * - ID 2: TC 0 on port D
 * - ID 3: TC 1 on port D
 * - ID 4: TC 0 on port E
 * - ID 5: TC 1 on port E
 * - ID 6: TC 0 on port F
 * - ID 7: TC 1 on port F
 */

#define timer0_init_priv(timer, callback) \
	tc_timer_init(0, tc_get_regs(0), timer, callback)
#define timer1_init_priv(timer, callback) \
	tc_timer_init(1, tc_get_regs(1), timer, callback)
#define timer2_init_priv(timer, callback) \
	tc_timer_init(2, tc_get_regs(2), timer, callback)
#define timer3_init_priv(timer, callback) \
	tc_timer_init(3, tc_get_regs(3), timer, callback)
#define timer4_init_priv(timer, callback) \
	tc_timer_init(4, tc_get_regs(4), timer, callback)
#define timer5_init_priv(timer, callback) \
	tc_timer_init(5, tc_get_regs(5), timer, callback)
#define timer6_init_priv(timer, callback) \
	tc_timer_init(6, tc_get_regs(6), timer, callback)
#define timer7_init_priv(timer, callback) \
	tc_timer_init(7, tc_get_regs(7), timer, callback)

#define timer0_start_priv(timer) \
	tc_timer_start(0, timer)
#define timer1_start_priv(timer) \
	tc_timer_start(1, timer)
#define timer2_start_priv(timer) \
	tc_timer_start(2, timer)
#define timer3_start_priv(timer) \
	tc_timer_start(3, timer)
#define timer4_start_priv(timer) \
	tc_timer_start(4, timer)
#define timer5_start_priv(timer) \
	tc_timer_start(5, timer)
#define timer6_start_priv(timer) \
	tc_timer_start(6, timer)
#define timer7_start_priv(timer) \
	tc_timer_start(7, timer)

#define timer0_stop_priv(timer) \
	tc_timer_stop(0, timer)
#define timer1_stop_priv(timer) \
	tc_timer_stop(1, timer)
#define timer2_stop_priv(timer) \
	tc_timer_stop(2, timer)
#define timer3_stop_priv(timer) \
	tc_timer_stop(3, timer)
#define timer4_stop_priv(timer) \
	tc_timer_stop(4, timer)
#define timer5_stop_priv(timer) \
	tc_timer_stop(5, timer)
#define timer6_stop_priv(timer) \
	tc_timer_stop(6, timer)
#define timer7_stop_priv(timer) \
	tc_timer_stop(7, timer)

#define timer0_set_alarm_priv(timer, delay) \
	tc_timer_set_alarm(0, timer, delay)
#define timer1_set_alarm_priv(timer, delay) \
	tc_timer_set_alarm(1, timer, delay)
#define timer2_set_alarm_priv(timer, delay) \
	tc_timer_set_alarm(2, timer, delay)
#define timer3_set_alarm_priv(timer, delay) \
	tc_timer_set_alarm(3, timer, delay)
#define timer4_set_alarm_priv(timer, delay) \
	tc_timer_set_alarm(4, timer, delay)
#define timer5_set_alarm_priv(timer, delay) \
	tc_timer_set_alarm(5, timer, delay)
#define timer6_set_alarm_priv(timer, delay) \
	tc_timer_set_alarm(6, timer, delay)
#define timer7_set_alarm_priv(timer, delay) \
	tc_timer_set_alarm(7, timer, delay)

#define timer0_get_time_priv(timer) \
	tc_timer_get_time(0, timer)
#define timer1_get_time_priv(timer) \
	tc_timer_get_time(1, timer)
#define timer2_get_time_priv(timer) \
	tc_timer_get_time(2, timer)
#define timer3_get_time_priv(timer) \
	tc_timer_get_time(3, timer)
#define timer4_get_time_priv(timer) \
	tc_timer_get_time(4, timer)
#define timer5_get_time_priv(timer) \
	tc_timer_get_time(5, timer)
#define timer6_get_time_priv(timer) \
	tc_timer_get_time(6, timer)
#define timer7_get_time_priv(timer) \
	tc_timer_get_time(7, timer)

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
#define timer6_default_resolution_priv(timer) \
	tc_timer_default_resolution(timer)
#define timer7_default_resolution_priv(timer) \
	tc_timer_default_resolution(timer)

#define timer0_write_resolution_priv(timer, resolution) \
	tc_timer_write_resolution(0, timer, resolution)
#define timer1_write_resolution_priv(timer, resolution) \
	tc_timer_write_resolution(1, timer, resolution)
#define timer2_write_resolution_priv(timer, resolution) \
	tc_timer_write_resolution(2, timer, resolution)
#define timer3_write_resolution_priv(timer, resolution) \
	tc_timer_write_resolution(3, timer, resolution)
#define timer4_write_resolution_priv(timer, resolution) \
	tc_timer_write_resolution(4, timer, resolution)
#define timer5_write_resolution_priv(timer, resolution) \
	tc_timer_write_resolution(5, timer, resolution)
#define timer6_write_resolution_priv(timer, resolution) \
	tc_timer_write_resolution(6, timer, resolution)
#define timer7_write_resolution_priv(timer, resolution) \
	tc_timer_write_resolution(7, timer, resolution)

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
#define timer6_read_resolution_priv(timer) \
	tc_timer_read_resolution(timer)
#define timer7_read_resolution_priv(timer) \
	tc_timer_read_resolution(timer)

#define timer0_set_resolution_priv(timer, resolution) \
	tc_timer_set_resolution(0, resolution)
#define timer1_set_resolution_priv(timer, resolution) \
	tc_timer_set_resolution(1, resolution)
#define timer2_set_resolution_priv(timer, resolution) \
	tc_timer_set_resolution(2, resolution)
#define timer3_set_resolution_priv(timer, resolution) \
	tc_timer_set_resolution(3, resolution)
#define timer4_set_resolution_priv(timer, resolution) \
	tc_timer_set_resolution(4, resolution)
#define timer5_set_resolution_priv(timer, resolution) \
	tc_timer_set_resolution(5, resolution)
#define timer6_set_resolution_priv(timer, resolution) \
	tc_timer_set_resolution(6, resolution)
#define timer7_set_resolution_priv(timer, resolution) \
	tc_timer_set_resolution(7, resolution)

#define timer0_get_resolution_priv(timer, resolution) \
	tc_timer_get_resolution(0, resolution)
#define timer1_get_resolution_priv(timer, resolution) \
	tc_timer_get_resolution(1, resolution)
#define timer2_get_resolution_priv(timer, resolution) \
	tc_timer_get_resolution(2, resolution)
#define timer3_get_resolution_priv(timer, resolution) \
	tc_timer_get_resolution(3, resolution)
#define timer4_get_resolution_priv(timer, resolution) \
	tc_timer_get_resolution(4, resolution)
#define timer5_get_resolution_priv(timer, resolution) \
	tc_timer_get_resolution(5, resolution)
#define timer6_get_resolution_priv(timer, resolution) \
	tc_timer_get_resolution(6, resolution)
#define timer7_get_resolution_priv(timer, resolution) \
	tc_timer_get_resolution(7, resolution)

#define timer0_get_maximum_delta_priv(timer) \
	tc_timer_get_maximum_delta()
#define timer1_get_maximum_delta_priv(timer) \
	tc_timer_get_maximum_delta()
#define timer2_get_maximum_delta_priv(timer) \
	tc_timer_get_maximum_delta()
#define timer3_get_maximum_delta_priv(timer) \
	tc_timer_get_maximum_delta()
#define timer4_get_maximum_delta_priv(timer) \
	tc_timer_get_maximum_delta()
#define timer5_get_maximum_delta_priv(timer) \
	tc_timer_get_maximum_delta()
#define timer6_get_maximum_delta_priv(timer) \
	tc_timer_get_maximum_delta()
#define timer7_get_maximum_delta_priv(timer) \
	tc_timer_get_maximum_delta()

#endif /* CHIP_TIMER_H_INCLUDED */

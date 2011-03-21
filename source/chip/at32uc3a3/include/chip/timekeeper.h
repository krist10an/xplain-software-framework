/**
 * \file
 *
 * \brief AT32UC3A3 chip-specific timekeeper definitions.
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
#ifndef CHIP_TIMEKEEPER_H_INCLUDED
#define CHIP_TIMEKEEPER_H_INCLUDED

#include <timekeeper/timekeeper_avr32.h>
#include <timekeeper/tc_timekeeper_avr32.h>
#include <timekeeper/cpu_timekeeper_avr32.h>
#include <chip/sysclk.h>

/**
 * \addtogroup timekeeper_avr32_group
 * \section at32uc3a3_timekeeper_section AT32UC3A3 timekeepers
 *
 * The list below show the relation between timer IDs and used hardware:
 * - ID 0: CPU internal counter
 * - ID 1: Channel 0 of TC module 0
 * - ID 2: Channel 1 of TC module 0
 * - ID 3: Channel 2 of TC module 0
 * - ID 4: Channel 0 of TC module 1
 * - ID 5: Channel 1 of TC module 1
 * - ID 6: Channel 2 of TC module 1
 * - ID 7: Channel 0 and 1 of TC module 0 combined to form 32 bit
 * - ID 8: Channel 0 and 1 of TC module 1 combined to form 32 bit
 */

static void inline cpu_timekeeper_start(timekeeper_t *timekeeper)
{
	sysclk_enable_cpu_module(SYSCLK_SYSTIMER);
}

static void inline cpu_timer_stop(timekeeper_t *timekeeper)
{
	sysclk_disable_cpu_module(SYSCLK_SYSTIMER);
}

#define timekeeper0_init_priv(timekeeper) \
	cpu_timekeeper_init(timekeeper)
#define timekeeper1_init_priv(timekeeper) \
	tc_timekeeper0_init(timekeeper)
#define timekeeper2_init_priv(timekeeper) \
	tc_timekeeper1_init(timekeeper)
#define timekeeper3_init_priv(timekeeper) \
	tc_timekeeper2_init(timekeeper)
#define timekeeper4_init_priv(timekeeper) \
	tc_timekeeper3_init(timekeeper)
#define timekeeper5_init_priv(timekeeper) \
	tc_timekeeper4_init(timekeeper)
#define timekeeper6_init_priv(timekeeper) \
	tc_timekeeper5_init(timekeeper)
#define timekeeper7_init_priv(timekeeper) \
	tc_timekeeper0_32bit_init(timekeeper)
#define timekeeper8_init_priv(timekeeper) \
	tc_timekeeper1_32bit_init(timekeeper)

#define timekeeper0_start_priv(tk) \
	cpu_timekeeper_start(tk)
#define timekeeper1_start_priv(tk) \
	tc_timekeeper_start(tk)
#define timekeeper2_start_priv(tk) \
	tc_timekeeper_start(tk)
#define timekeeper3_start_priv(tk) \
	tc_timekeeper_start(tk)
#define timekeeper4_start_priv(tk) \
	tc_timekeeper_start(tk)
#define timekeeper5_start_priv(tk) \
	tc_timekeeper_start(tk)
#define timekeeper6_start_priv(tk) \
	tc_timekeeper_start(tk)
#define timekeeper7_start_priv(tk) \
	tc_timekeeper_32bit_start(tk)
#define timekeeper8_start_priv(tk) \
	tc_timekeeper_32bit_start(tk)

#define timekeeper0_stop_priv(tk) \
	cpu_timekeeper_stop(tk)
#define timekeeper1_stop_priv(tk) \
	tc_timekeeper_stop(tk)
#define timekeeper2_stop_priv(tk) \
	tc_timekeeper_stop(tk)
#define timekeeper3_stop_priv(tk) \
	tc_timekeeper_stop(tk)
#define timekeeper4_stop_priv(tk) \
	tc_timekeeper_stop(tk)
#define timekeeper5_stop_priv(tk) \
	tc_timekeeper_stop(tk)
#define timekeeper6_stop_priv(tk) \
	tc_timekeeper_stop(tk)
#define timekeeper7_stop_priv(tk) \
	tc_timekeeper_32bit_stop(tk)
#define timekeeper8_stop_priv(tk) \
	tc_timekeeper_32bit_stop(tk)

#define timekeeper0_timeout_init_priv(tk, timeout, delay) \
	cpu_timekeeper_timeout_init(tk, timeout, delay)
#define timekeeper1_timeout_init_priv(tk, timeout, delay) \
	tc_timekeeper_timeout_init(tk, timeout, delay)
#define timekeeper2_timeout_init_priv(tk, timeout, delay) \
	tc_timekeeper_timeout_init(tk, timeout, delay)
#define timekeeper3_timeout_init_priv(tk, timeout, delay) \
	tc_timekeeper_timeout_init(tk, timeout, delay)
#define timekeeper4_timeout_init_priv(tk, timeout, delay) \
	tc_timekeeper_timeout_init(tk, timeout, delay)
#define timekeeper5_timeout_init_priv(tk, timeout, delay) \
	tc_timekeeper_timeout_init(tk, timeout, delay)
#define timekeeper6_timeout_init_priv(tk, timeout, delay) \
	tc_timekeeper_timeout_init(tk, timeout, delay)
#define timekeeper7_timeout_init_priv(tk, timeout, delay) \
	tc_timekeeper_32bit_timeout_init(tk, timeout, delay)
#define timekeeper8_timeout_init_priv(tk, timeout, delay) \
	tc_timekeeper_32bit_timeout_init(tk, timeout, delay)

#define timekeeper0_timeout_has_expired_priv(tk, timeout) \
	cpu_timekeeper_timeout_has_expired(tk, timeout)
#define timekeeper1_timeout_has_expired_priv(tk, timeout) \
	tc_timekeeper_timeout_has_expired(tk, timeout)
#define timekeeper2_timeout_has_expired_priv(tk, timeout) \
	tc_timekeeper_timeout_has_expired(tk, timeout)
#define timekeeper3_timeout_has_expired_priv(tk, timeout) \
	tc_timekeeper_timeout_has_expired(tk, timeout)
#define timekeeper4_timeout_has_expired_priv(tk, timeout) \
	tc_timekeeper_timeout_has_expired(tk, timeout)
#define timekeeper5_timeout_has_expired_priv(tk, timeout) \
	tc_timekeeper_timeout_has_expired(tk, timeout)
#define timekeeper6_timeout_has_expired_priv(tk, timeout) \
	tc_timekeeper_timeout_has_expired(tk, timeout)
#define timekeeper7_timeout_has_expired_priv(tk, timeout) \
	tc_timekeeper_32bit_timeout_has_expired(tk, timeout)
#define timekeeper8_timeout_has_expired_priv(tk, timeout) \
	tc_timekeeper_32bit_timeout_has_expired(tk, timeout)

#define timekeeper0_get_time_priv(tk) \
	cpu_timekeeper_get_time(tk)
#define timekeeper1_get_time_priv(tk) \
	tc_timekeeper_get_time(tk)
#define timekeeper2_get_time_priv(tk) \
	tc_timekeeper_get_time(tk)
#define timekeeper3_get_time_priv(tk) \
	tc_timekeeper_get_time(tk)
#define timekeeper4_get_time_priv(tk) \
	tc_timekeeper_get_time(tk)
#define timekeeper5_get_time_priv(tk) \
	tc_timekeeper_get_time(tk)
#define timekeeper6_get_time_priv(tk) \
	tc_timekeeper_get_time(tk)
#define timekeeper7_get_time_priv(tk) \
	tc_timekeeper_32bit_get_time(tk)
#define timekeeper8_get_time_priv(tk) \
	tc_timekeeper_32bit_get_time(tk)

#define timekeeper0_write_resolution_priv(tk, tk_res) \
	cpu_timekeeper_write_resolution(tk, tk_res)
#define timekeeper1_write_resolution_priv(tk, tk_res) \
	tc_timekeeper_write_resolution(tk, tk_res)
#define timekeeper2_write_resolution_priv(tk, tk_res) \
	tc_timekeeper_write_resolution(tk, tk_res)
#define timekeeper3_write_resolution_priv(tk, tk_res) \
	tc_timekeeper_write_resolution(tk, tk_res)
#define timekeeper4_write_resolution_priv(tk, tk_res) \
	tc_timekeeper_write_resolution(tk, tk_res)
#define timekeeper5_write_resolution_priv(tk, tk_res) \
	tc_timekeeper_write_resolution(tk, tk_res)
#define timekeeper6_write_resolution_priv(tk, tk_res) \
	tc_timekeeper_write_resolution(tk, tk_res)
#define timekeeper7_write_resolution_priv(tk, tk_res) \
	tc_timekeeper_write_resolution(tk, tk_res)
#define timekeeper8_write_resolution_priv(tk, tk_res) \
	tc_timekeeper_write_resolution(tk, tk_res)

#define timekeeper0_read_resolution_priv(tk) \
	cpu_timekeeper_read_resolution(tk)
#define timekeeper1_read_resolution_priv(tk) \
	tc_timekeeper_read_resolution(tk)
#define timekeeper2_read_resolution_priv(tk) \
	tc_timekeeper_read_resolution(tk)
#define timekeeper3_read_resolution_priv(tk) \
	tc_timekeeper_read_resolution(tk)
#define timekeeper4_read_resolution_priv(tk) \
	tc_timekeeper_read_resolution(tk)
#define timekeeper5_read_resolution_priv(tk) \
	tc_timekeeper_read_resolution(tk)
#define timekeeper6_read_resolution_priv(tk) \
	tc_timekeeper_read_resolution(tk)
#define timekeeper7_read_resolution_priv(tk) \
	tc_timekeeper_read_resolution(tk)
#define timekeeper8_read_resolution_priv(tk) \
	tc_timekeeper_read_resolution(tk)

#define timekeeper0_set_resolution_priv(tk, tk_res) \
	cpu_timekeeper_set_resolution(tk, tk_res)
#define timekeeper1_set_resolution_priv(tk, tk_res) \
	tc_timekeeper_set_resolution(0, tk_res)
#define timekeeper2_set_resolution_priv(tk, tk_res) \
	tc_timekeeper_set_resolution(0, tk_res)
#define timekeeper3_set_resolution_priv(tk, tk_res) \
	tc_timekeeper_set_resolution(0, tk_res)
#define timekeeper4_set_resolution_priv(tk, tk_res) \
	tc_timekeeper_set_resolution(1, tk_res)
#define timekeeper5_set_resolution_priv(tk, tk_res) \
	tc_timekeeper_set_resolution(1, tk_res)
#define timekeeper6_set_resolution_priv(tk, tk_res) \
	tc_timekeeper_set_resolution(1, tk_res)
#define timekeeper7_set_resolution_priv(tk, tk_res) \
	tc_timekeeper_set_resolution(0, tk_res)
#define timekeeper8_set_resolution_priv(tk, tk_res) \
	tc_timekeeper_set_resolution(1, tk_res)


#define timekeeper0_get_resolution_priv(tk, resolution) \
	cpu_timekeeper_get_resolution(resolution)
#define timekeeper1_get_resolution_priv(tk, resolution) \
	tc_timekeeper_get_resolution(0, resolution)
#define timekeeper2_get_resolution_priv(tk, resolution) \
	tc_timekeeper_get_resolution(0, resolution)
#define timekeeper3_get_resolution_priv(tk, resolution) \
	tc_timekeeper_get_resolution(0, resolution)
#define timekeeper4_get_resolution_priv(tk, resolution) \
	tc_timekeeper_get_resolution(1, resolution)
#define timekeeper5_get_resolution_priv(tk, resolution) \
	tc_timekeeper_get_resolution(1, resolution)
#define timekeeper6_get_resolution_priv(tk, resolution) \
	tc_timekeeper_get_resolution(1, resolution)
#define timekeeper7_get_resolution_priv(tk, resolution) \
	tc_timekeeper_get_resolution(0, resolution)
#define timekeeper8_get_resolution_priv(tk, resolution) \
	tc_timekeeper_get_resolution(1, resolution)

#define timekeeper0_get_maximum_delta(tk) \
	cpu_timekeeper_get_maximum_delta(tk)
#define timekeeper1_get_maximum_delta(tk) \
	tc_timekeeper_get_maximum_delta(tk)
#define timekeeper2_get_maximum_delta(tk) \
	tc_timekeeper_get_maximum_delta(tk)
#define timekeeper3_get_maximum_delta(tk) \
	tc_timekeeper_get_maximum_delta(tk)
#define timekeeper4_get_maximum_delta(tk) \
	tc_timekeeper_get_maximum_delta(tk)
#define timekeeper5_get_maximum_delta(tk) \
	tc_timekeeper_get_maximum_delta(tk)
#define timekeeper6_get_maximum_delta(tk) \
	tc_timekeeper_get_maximum_delta(tk)
#define timekeeper7_get_maximum_delta(tk) \
	tc_timekeeper_32bit_get_maximum_delta(tk)
#define timekeeper8_get_maximum_delta(tk) \
	tc_timekeeper_32bit_get_maximum_delta(tk)

#endif /* CHIP_TIMEKEEPER_H_INCLUDED */

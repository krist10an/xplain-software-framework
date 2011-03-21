/**
 * \file
 *
 * \brief AVR32 TC timer definitions.
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

#ifndef TIMER_TC_TIMER_AVR32_H_INCLUDED
#define TIMER_TC_TIMER_AVR32_H_INCLUDED

#include <timer.h>
#include <regs/avr32_tc.h>
#include <chip/tc.h>

/**
 * \addtogroup timer_avr32_group
 * \section tc_timer_avr32_section AVR32 TC timer
 *
 * This timer implementation is based on the TC modules that are available on
 * the AVR32 devices. In order to use it the configuration symbol CONFIG_TC
 * must be defined.
 *
 * The default interrupt level is 0 but this can be changed by following
 * configuration symbols:
 * - CONFIG_TC0_INTLVL: Sets the group interrupt level for TC module 0
 * - CONFIG_TC1_INTLVL: Sets the group interrupt level for TC module 1
 * Valid level values are described in the datasheet and these are 0 (INT0),
 * 1 (INT1), 2 (INT2) and 3 (INT3).
 *
 *@{
 */


extern void tc_timer0_init(timer_t *timer, timer_callback_t timer_callback);
extern void tc_timer1_init(timer_t *timer, timer_callback_t timer_callback);
extern void tc_timer2_init(timer_t *timer, timer_callback_t timer_callback);
extern void tc_timer3_init(timer_t *timer, timer_callback_t timer_callback);
extern void tc_timer4_init(timer_t *timer, timer_callback_t timer_callback);
extern void tc_timer5_init(timer_t *timer, timer_callback_t timer_callback);
extern void tc_timer6_init(timer_t *timer, timer_callback_t timer_callback);

extern void tc_timer_start(timer_t *timer);
extern void tc_timer_stop(timer_t *timer);
extern void tc_timer_set_alarm(timer_t *timer, uint32_t delay);
extern uint32_t tc_timer_get_time(timer_t *timer);


extern timer_res_t tc_timer_set_resolution(unsigned int tc_id,
	unsigned long resolution);

static inline unsigned long tc_timer_maximum_delta(timer_t *timer)
{
	return 0xFFFF;
}

static inline timer_res_t tc_timer_read_resolution(timer_t *timer)
{
	return TC_BFEXT(CMR_TCCLKS, tc_read_reg(timer->regs, CMR));
}

static inline void tc_timer_write_resolution(timer_t *timer,
	timer_res_t resolution)
{
	tc_write_reg(timer->regs, CMR, TC_BIT(CMR_WAVE)
		| TC_BF(CMR_WAVSEL, TC_WAVSEL_UP)
		| TC_BF(CMR_TCCLKS, resolution));
}

static inline unsigned long tc_timer_get_resolution(unsigned int tc_id,
	timer_res_t timer_res)
{
	return tc_get_resolution(tc_id, timer_res);
}

static inline unsigned int tc_timer_default_resolution(unsigned int tc_id)
{
	timer_res_t res;
	res = tc_select_clock(tc_id, CONFIG_TIMER_RESOLUTION);
	return tc_get_resolution(tc_id, res);
}

/** @} */
#endif /* TIMER_TC_TIMER_AVR32_H_INCLUDED */

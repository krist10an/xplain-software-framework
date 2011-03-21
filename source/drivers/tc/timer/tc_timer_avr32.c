/**
 * \file
 *
 * \brief AVR32 TC timer driver.
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

#include <tc/tc_avr32.h>
#include <timer/tc_timer_avr32.h>
#include <regs/avr32_tc.h>
#include <tc/tc_ints_avr32.h>
#include <assert.h>
#include <interrupt.h>

/**
 * \weakgroup tc_timer_avr32_internal_group AVR32 TC timer internals
 * \ingroup timer_avr32_group
 * @{
 */


/**
 * \brief Timer interrupt handler.
 * \internal
 * This handler will be executed whenever the timer counter matches the compare
 * value for comparator A. The handler will disable the compare interrupt and
 * thus the application needs to set a new alarm time to enable the interrupt
 * again.
 *
 * If a timer callback function is installed it will be called from this
 * handler each time it triggers.
 *
 * \param data Pointer to timer struct.
 */
static void tc_timer_irq_handler(void *data)
{
	timer_t *timer = (timer_t *) data;
	unsigned long status = tc_read_reg(timer->regs, SR);

	if (status & TC_BIT(CPAS)) {
		/* Disable compare interrupt, the interrupt will be re-enabled
		 * by setting a new alarm time
		 */
		tc_write_reg(timer->regs, IDR, TC_BIT(CPAS));
		if (timer->callback)
			timer->callback(timer);
	}
}

static void tc_timer_init_common(timer_t *timer, unsigned int tc_id,
		uint32_t clksel)
{
	bool pclk_enabled;
	irqflags_t flags;

	flags = cpu_irq_save();
	pclk_enabled = tc_pclk_is_enabled(tc_id);

	if (!pclk_enabled)
		tc_enable_pclk(tc_id);
	tc_write_reg(timer->regs, CMR, TC_BIT(CMR_WAVE)
		| TC_BF(CMR_WAVSEL, TC_WAVSEL_UP)
		| TC_BF(CMR_TCCLKS, clksel));
	if (!pclk_enabled)
		tc_disable_pclk(tc_id);

	cpu_irq_restore(flags);
}

/*
 * The tc_get_channel_regs macro needs a constant as argument to expand
 * properly. Because of that this implementation provides init functions for
 * each timer. This will lead to more code but is considered better than making
 * this function a macro.
 */
#ifdef TC0_BASE
void tc_timer0_init(timer_t *timer, timer_callback_t timer_callback)
{
	uint32_t clksel = tc_select_clock(0, CONFIG_TIMER_RESOLUTION);

	timer->regs = tc_get_channel_regs(0, 0);
	timer->callback = timer_callback;
	tc_register_channel_int(0, 0, tc_timer_irq_handler, timer);
	tc_timer_init_common(timer, 0, clksel);
}

void tc_timer1_init(timer_t *timer, timer_callback_t timer_callback)
{
	uint32_t clksel = tc_select_clock(0, CONFIG_TIMER_RESOLUTION);

	timer->regs = tc_get_channel_regs(0, 1);
	timer->callback = timer_callback;
	tc_register_channel_int(0, 1, tc_timer_irq_handler, timer);
	tc_timer_init_common(timer, 0, clksel);
}

void tc_timer2_init(timer_t *timer, timer_callback_t timer_callback)
{
	uint32_t clksel = tc_select_clock(0, CONFIG_TIMER_RESOLUTION);

	timer->regs = tc_get_channel_regs(0, 2);
	timer->callback = timer_callback;
	tc_register_channel_int(0, 2, tc_timer_irq_handler, timer);
	tc_timer_init_common(timer, 0, clksel);
}
#endif
#ifdef TC1_BASE
void tc_timer3_init(timer_t *timer, timer_callback_t timer_callback)
{
	uint32_t clksel = tc_select_clock(1, CONFIG_TIMER_RESOLUTION);

	timer->regs = tc_get_channel_regs(1, 0);
	timer->callback = timer_callback;
	tc_register_channel_int(1, 0, tc_timer_irq_handler, timer);
	tc_timer_init_common(timer, 1, clksel);
}

void tc_timer4_init(timer_t *timer, timer_callback_t timer_callback)
{
	uint32_t clksel = tc_select_clock(1, CONFIG_TIMER_RESOLUTION);

	timer->regs = tc_get_channel_regs(1, 1);
	timer->callback = timer_callback;
	tc_register_channel_int(1, 1, tc_timer_irq_handler, timer);
	tc_timer_init_common(timer, 1, clksel);
}

void tc_timer5_init(timer_t *timer, timer_callback_t timer_callback)
{
	uint32_t clksel = tc_select_clock(1, CONFIG_TIMER_RESOLUTION);

	timer->regs = tc_get_channel_regs(1, 2);
	timer->callback = timer_callback;
	tc_register_channel_int(1, 2, tc_timer_irq_handler, timer);
	tc_timer_init_common(timer, 1, clksel);
}
#endif

timer_res_t tc_timer_set_resolution(unsigned int tc_id,
		unsigned long resolution)
{
	return tc_select_clock(tc_id, resolution);
}

void tc_timer_start(timer_t *timer)
{
	// Reset timer count value and enable input clock
	tc_write_reg(timer->regs, CCR, TC_BIT(CCR_CLKEN) | TC_BIT(CCR_SWTRG));
}

void tc_timer_set_alarm(timer_t *timer, uint32_t delay)
{

	uint32_t start, stop, now;
	irqflags_t flags;

	start = tc_timer_get_time(timer);
	stop = start + delay;

	tc_write_reg(timer->regs, RA, stop);
	tc_read_reg(timer->regs, SR); // clear any pending interrupts
	tc_write_reg(timer->regs, IER, TC_BIT(CPAS));

	now = tc_read_reg(timer->regs, CV);
	/* Lets see if we have already passed the configured alarm time. If
	 * not we can return.
	 */
	if (start > stop) {
		if (now >= start && now <= stop)
			return;
	} else {
		if (now >= start || now <= stop)
			return;
	}
	/* If the timer has not triggered the compare match interrupt is still
	 * enabled. If that is the case we disable it and call the callback
	 * function.
	 */
	flags = cpu_irq_save();
	if (tc_read_reg(timer->regs, IMR) & TC_BIT(CPAS)) {
		tc_write_reg(timer->regs, IDR, TC_BIT(CPAS));
		cpu_irq_restore(flags);
		timer->callback(timer);
	}
	else
		cpu_irq_restore(flags);
}

void tc_timer_stop(timer_t *timer)
{
	tc_write_reg(timer->regs, IDR, TC_BIT(CPAS));
	// Disable timer by disabling the input clock
	tc_write_reg(timer->regs, CCR, TC_BIT(CCR_CLKDIS));
}

uint32_t tc_timer_get_time(timer_t *timer)
{
	return (uint32_t) tc_read_reg(timer->regs, CV);
}
/** @} */


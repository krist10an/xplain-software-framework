/**
 * \file
 *
 * \brief AVR XMEGA timer/counter Timer.
 *
 * This file contains the function definitions for the AVR XMEGA timer/counter
 * implementation of the Timer driver.
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

#include <timer/tc_timer_xmega.h>
#include <assert.h>
#include <interrupt.h>
#include <intc.h>
#include <regs/xmega_tc.h>
#include <chip/tc.h>
#include <pmic.h>

/**
 * \weakgroup timer_xmega_internal_group
 * \ingroup timer_xmega_group
 * @{
 */

/**
 * \brief TC compare interrupt handler.
 * \internal
 *
 * This handler is executed when compare channel A of the TC is triggered and
 * an alarm has been set. It disables the compare interrupt, but leaves the
 * TC running. If a callback function has been set for the timer, it is called
 * directly by this handler.
 *
 * \param int_data Pointer to a timer struct.
 */
static void tc_timer_irq_handler(void *int_data)
{
	struct timer *timer = (struct timer *)int_data;

	assert(timer);

	tc_write_reg8(timer->regs, INTCTRLB, TC_BF(INTCTRLB_CCAINTLVL,
			PMIC_INTLVL_OFF));

	if (timer->callback)
		timer->callback(timer);
}

// Set interrupt handler for the available and configured compare interrupts.
#if (defined(TC0_BASE) && defined(CONFIG_TIMER_0)) || defined(__DOXYGEN__)
INTC_DEFINE_HANDLER(PMIC_TC0_CCA_IRQ, tc_timer_irq_handler,
		CONFIG_INTLVL_TC0_CCA);
#endif

#if defined(TC1_BASE) && defined(CONFIG_TIMER_1)
INTC_DEFINE_HANDLER(PMIC_TC1_CCA_IRQ, tc_timer_irq_handler,
		CONFIG_INTLVL_TC1_CCA);
#endif

#if defined(TC2_BASE) && defined(CONFIG_TIMER_2)
INTC_DEFINE_HANDLER(PMIC_TC2_CCA_IRQ, tc_timer_irq_handler,
		CONFIG_INTLVL_TC2_CCA);
#endif

#if defined(TC3_BASE) && defined(CONFIG_TIMER_3)
INTC_DEFINE_HANDLER(PMIC_TC3_CCA_IRQ, tc_timer_irq_handler,
		CONFIG_INTLVL_TC3_CCA);
#endif

#if defined(TC4_BASE) && defined(CONFIG_TIMER_4)
INTC_DEFINE_HANDLER(PMIC_TC4_CCA_IRQ, tc_timer_irq_handler,
		CONFIG_INTLVL_TC4_CCA);
#endif

#if defined(TC5_BASE) && defined(CONFIG_TIMER_5)
INTC_DEFINE_HANDLER(PMIC_TC5_CCA_IRQ, tc_timer_irq_handler,
		CONFIG_INTLVL_TC5_CCA);
#endif

#if defined(TC6_BASE) && defined(CONFIG_TIMER_6)
INTC_DEFINE_HANDLER(PMIC_TC6_CCA_IRQ, tc_timer_irq_handler,
		CONFIG_INTLVL_TC6_CCA);
#endif

#if defined(TC7_BASE) && defined(CONFIG_TIMER_7)
INTC_DEFINE_HANDLER(PMIC_TC7_CCA_IRQ, tc_timer_irq_handler,
		CONFIG_INTLVL_TC7_CCA);
#endif

/**
 * \brief Common initialization for timers.
 * \internal
 *
 * Stores data needed for the timer: TC base address, default clock selection
 * and callback function. The TC is reset, and compare channel A enabled before
 * the TC is left disabled.
 *
 * \param tc_id    ID of TC.
 * \param regs     Base address of TC
 * \param timer    Pointer to timer struct.
 * \param callback Pointer to callback function.
 */
void tc_timer_init(uint8_t tc_id, void *regs, struct timer *timer,
		timer_callback_t callback)
{
	assert(regs);
	assert(timer);

	timer->regs = regs;
	timer->callback = callback;
	timer->clksel = tc_select_clock(tc_id,
			(uint32_t)CONFIG_TIMER_RESOLUTION);

	tc_enable_pclk(tc_id);
	tc_write_reg8(regs, CTRLA, TC_CLKSEL_OFF);
	tc_write_reg8(regs, CTRLFSET, TC_BF(CMD, TC_CMD_RESET));
	tc_write_reg8(regs, CTRLB, TC_BIT(CTRLB_CCAEN));
	tc_disable_pclk(tc_id);

	switch (tc_id) {
#if (defined(TC0_BASE) && defined(CONFIG_TIMER_0)) || defined(__DOXYGEN__)
	case 0:
		intc_set_irq_data(PMIC_TC0_CCA_IRQ, timer);
		timer->intlvl = CONFIG_INTLVL_TC0_CCA;
		break;
#endif

#if defined(TC1_BASE) && defined(CONFIG_TIMER_1)
	case 1:
		intc_set_irq_data(PMIC_TC1_CCA_IRQ, timer);
		timer->intlvl = CONFIG_INTLVL_TC1_CCA;
		break;
#endif

#if defined(TC2_BASE) && defined(CONFIG_TIMER_2)
	case 2:
		intc_set_irq_data(PMIC_TC2_CCA_IRQ, timer);
		timer->intlvl = CONFIG_INTLVL_TC2_CCA;
		break;
#endif

#if defined(TC3_BASE) && defined(CONFIG_TIMER_3)
	case 3:
		intc_set_irq_data(PMIC_TC3_CCA_IRQ, timer);
		timer->intlvl = CONFIG_INTLVL_TC3_CCA;
		break;
#endif

#if defined(TC4_BASE) && defined(CONFIG_TIMER_4)
	case 4:
		intc_set_irq_data(PMIC_TC4_CCA_IRQ, timer);
		timer->intlvl = CONFIG_INTLVL_TC4_CCA;
		break;
#endif

#if defined(TC5_BASE) && defined(CONFIG_TIMER_5)
	case 5:
		intc_set_irq_data(PMIC_TC5_CCA_IRQ, timer);
		timer->intlvl = CONFIG_INTLVL_TC5_CCA;
		break;
#endif

#if defined(TC6_BASE) && defined(CONFIG_TIMER_6)
	case 6:
		intc_set_irq_data(PMIC_TC6_CCA_IRQ, timer);
		timer->intlvl = CONFIG_INTLVL_TC6_CCA;
		break;
#endif

#if defined(TC7_BASE) && defined(CONFIG_TIMER_7)
	case 7:
		intc_set_irq_data(PMIC_TC7_CCA_IRQ, timer);
		timer->intlvl = CONFIG_INTLVL_TC7_CCA;
		break;
#endif

	default:
		unhandled_case(tc_id);
	}
}

/**
 * \brief Get best TC clock selection for specified resolution.
 * \internal
 *
 * Returns the clock selection for the TC that gives a resolution equal to or
 * better than the one specified, assuming the system clock allows for it.
 * If the desired resolution is not attainable, the clock selection for maximum
 * rate is returned.
 *
 * \param tc_id      ID if TC.
 * \param resolution Desired TC resolution in Hz.
 *
 * \return Clock selection for TC.
 */
timer_res_t tc_timer_set_resolution(uint8_t tc_id, uint32_t resolution)
{
	return tc_select_clock(tc_id, resolution);
}

/**
 * \brief Get resolution in Hz for TC clock selection.
 * \internal
 *
 * \param tc_id     ID of TC.
 * \param timer_res Clock selection for TC.
 *
 * \return TC resolution in Hz.
 */
uint32_t tc_timer_get_resolution(uint8_t tc_id, timer_res_t timer_res)
{
	return tc_get_resolution(tc_id, timer_res);
}

/**
 * \brief Update clock selection for TC.
 * \internal
 *
 * Stores new clock selection, and updates the current clock selection of the
 * TC if it is enabled.
 *
 * \param tc_id     ID of the TC.
 * \param timer     Pointer to timer struct.
 * \param timer_res New clock selection for TC.
 */
void tc_timer_write_resolution(uint8_t tc_id, struct timer *timer,
		timer_res_t timer_res)
{
	assert(timer);
	assert(timer->regs);

	timer->clksel = timer_res;
	tc_write_reg8(timer->regs, CTRLA, timer_res);
}

/**
 * \brief Enable, reset and start TC.
 * \internal
 *
 * \param tc_id ID of the TC.
 * \param timer Pointer to timer struct.
 */
void tc_timer_start(uint8_t tc_id, struct timer *timer)
{
	assert(timer->regs);

	tc_enable_pclk(tc_id);

	// Reset TC value, then start counting.
	tc_write_reg8(timer->regs, CTRLFSET, TC_BF(CMD, TC_CMD_RESTART));
	tc_write_reg8(timer->regs, CTRLA, timer->clksel);
}

/**
 * \brief Configure new alarm on TC.
 * \internal
 *
 * Sets a new compare value for compare channel A and enables its interrupt.
 *
 * \param tc_id ID of the TC.
 * \param timer Pointer to timer struct.
 * \param delay Delay for timer alarm.
 */
void tc_timer_set_alarm(uint8_t tc_id, struct timer *timer, uint16_t delay)
{
	irqflags_t flags;
	bool enabled;
	uint16_t start;

	assert(timer);
	assert(timer->regs);

	// Disable any current alarms.
	tc_write_reg8(timer->regs, INTCTRLB, TC_BF(INTCTRLB_CCAINTLVL,
			PMIC_INTLVL_OFF));

	// TC must be stopped to be sure to avoid "overshoots" of delays.
	enabled = tc_pclk_is_enabled(tc_id);
	if (!enabled)
		tc_enable_pclk(tc_id);
	else
		tc_write_reg8(timer->regs, CTRLA, TC_CLKSEL_OFF);

	// Disable interrupts to prevent corruption of 16-bit read and writes.
	flags = cpu_irq_save();

	tc_write_reg8(timer->regs, INTFLAGS, TC_BIT(INTFLAGS_CCAIF));

	/* TC must be reset and started with \ref tc_timer_start() if disabled.
	 * Set compare value accordingly.
	 */
	if (!enabled)
		tc_write_reg16(timer->regs, CCA, delay);
	else {
		start = tc_read_reg16(timer->regs, CNT);
		tc_write_reg16(timer->regs, CCA, start + delay);
	}

	cpu_irq_restore(flags);

	tc_write_reg8(timer->regs, INTCTRLB, TC_BF(INTCTRLB_CCAINTLVL,
			timer->intlvl));

	// Leave the TC in the state it was upon entry of this function.
	if (!enabled)
		tc_disable_pclk(tc_id);
	else
		tc_write_reg8(timer->regs, CTRLA, timer->clksel);
}

/**
 * \brief Stop and disable TC.
 * \internal
 *
 * \param tc_id ID of TC.
 * \param timer Pointer to timer struct.
 */
void tc_timer_stop(uint8_t tc_id, struct timer *timer)
{
	assert(timer);
	assert(timer->regs);

	// Disable compare interrupt, then stop and disable the TC.
	tc_write_reg8(timer->regs, INTCTRLB, TC_BF(INTCTRLB_CCAINTLVL,
			PMIC_INTLVL_OFF));
	tc_write_reg8(timer->regs, CTRLA, TC_BF(CTRLA_CLKSEL, TC_CLKSEL_OFF));
	tc_disable_pclk(tc_id);
}

/**
 * \brief Get current time of TC.
 * \internal
 *
 * Returns the current time of the TC, if it is enabled.
 *
 * \param tc_id ID of the TC.
 * \param timer Pointer to timer struct.
 *
 * \return Time of TC, or 0 if it is disabled.
 */
uint16_t tc_timer_get_time(uint8_t tc_id, struct timer *timer)
{
	bool enabled;
	irqflags_t flags;
	uint16_t time;

	assert(timer);
	assert(timer->regs);

	enabled = tc_pclk_is_enabled(tc_id);

	if (enabled) {
		// Disable interrupts to prevent corruption of 16-bit read.
		flags = cpu_irq_save();
		time = tc_read_reg16(timer->regs, CNT);
		cpu_irq_restore(flags);
	} else {
		time = 0;
	}

	return time;
}

//!@}

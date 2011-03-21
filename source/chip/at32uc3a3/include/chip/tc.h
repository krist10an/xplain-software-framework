/**
 * \file
 *
 * \brief AT32UC3A3 chip specific TC definitions.
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

#ifndef CHIP_TC_H_INCLUDED
#define CHIP_TC_H_INCLUDED

#include <clk/sys.h>
#include <assert.h>

/* \brief Get the bus clock rate connected to a TC.
 *
 * \param id The ID of the TC of which to query the bus clock rate.
 * \return The TC bus clock rate in Hz.
 */
static inline unsigned long tc_get_pclk_hz(unsigned int id)
{
	return sysclk_get_pba_hz();
}

/**
 * \brief Enable the clock of a TC module.
 *
 * \param id TC module ID.
 */
static inline void tc_enable_pclk(unsigned int id)
{
	uint32_t pbamask = pm_read_reg(PBAMASK);
	assert(id < 2);

	if (id == 0)
		pbamask |= 1 << 16;
	else
		pbamask |= 1 << 17;

	pm_write_reg(PBAMASK, pbamask);
}

/**
 * \brief Disable the clock of a TC module.
 *
 * \param id TC module ID.
 */
static inline void tc_disable_pclk(unsigned int id)
{
	uint32_t pbamask = pm_read_reg(PBAMASK);
	assert(id < 2);

	if (id == 0)
		pbamask &= ~(1 << 16);
	else
		pbamask &= ~(1 << 17);

	pm_write_reg(PBAMASK, pbamask);
}

/**
 * \brief  Test if TC clock is enabled.
 *
 * \param id TC ID.
 */
static inline bool tc_pclk_is_enabled(unsigned int id)
{
	uint32_t pbamask = pm_read_reg(PBAMASK);
	assert(id < 2);

	if (id == 0) {
		if (pbamask & (1 << 16))
			return true;
		else
			return false;
	} else {
		if (pbamask & (1 << 17))
			return true;
		else
			return false;
	}
}
/**
 * \brief Select a input clock that fits best to the resolution.
 * \internal
 *
 * This function makes only use of the known clock sources that are derived
 * from the bus clock.
 *
 * \param tc ID of the Timer/Counter module
 * \param resolution Desired resolution in Hz.
 * \returns Hardware settings for specified \a resolution.
 */
__always_inline static unsigned int tc_select_clock(unsigned int tc,
	unsigned long resolution)
{
	unsigned long tc_clk_rate = tc_get_pclk_hz(tc);

	if (resolution <= (tc_clk_rate / 128))
		return 4;
	else if (resolution <= (tc_clk_rate / 32))
		return 3;
	else if (resolution <= (tc_clk_rate / 8))
		return 2;
	else
		return 1;
}

/**
 * \brief Get the resolution of the current selected input clock.
 *
 * This function will only return a valid resolution for clocks that are
 * derived from the bus clock.
 *
 * \param tc Timer/counter ID.
 * \param clksel Selected input clock.
 * \returns Resolution for selected clock or 0 if clock is not valid.
 */
static inline unsigned long tc_get_resolution(unsigned int tc,
	unsigned int clksel)
{
	unsigned long clock_rate = tc_get_pclk_hz(tc);

	switch(clksel) {
	case 1:
		clock_rate /= 2;
		break;
	case 2:
		clock_rate /= 8;
		break;
	case 3:
		clock_rate /= 32;
		break;
	case 4:
		clock_rate /= 128;
		break;
	default:
		unhandled_case(clksel);
		break;
	}
	return clock_rate;
}

#endif /* CHIP_TC_H_INCLUDED */


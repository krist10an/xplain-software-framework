/**
 * \file
 *
 * \brief Oscillator management using the AVR32 PM module
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
#ifndef CLK_AVR32_PM_OSC_H_INCLUDED
#define CLK_AVR32_PM_OSC_H_INCLUDED

#include <regs/avr32_pm_v2.h>

/**
 * \weakgroup osc_group
 * @{
 */

#ifndef __ASSEMBLY__

#include <assert.h>
#include <types.h>

__always_inline static void osc_enable(uint8_t id)
{
	uint32_t	oscctrl;
	uint32_t	mcctrl;

	switch (id) {
#ifdef BOARD_OSC0_HZ
	case 0:
		oscctrl = PM_BF(OSCCTRL_STARTUP, OSC0_STARTUP_VALUE);
		oscctrl |= PM_BF(OSCCTRL_MODE, OSC0_MODE_VALUE);
		pm_write_reg(OSC0CTRL, oscctrl);
		mcctrl = pm_read_reg(MCCTRL);
		pm_write_reg(MCCTRL, mcctrl | PM_BIT(MCCTRL_OSC0EN));
		break;
#endif
#ifdef BOARD_OSC1_HZ
	case 1:
		oscctrl = PM_BF(OSCCTRL_STARTUP, OSC1_STARTUP_VALUE);
		oscctrl |= PM_BF(OSCCTRL_MODE, OSC1_MODE_VALUE);
		pm_write_reg(OSC1CTRL, oscctrl);
		mcctrl = pm_read_reg(MCCTRL);
		pm_write_reg(MCCTRL, mcctrl | PM_BIT(MCCTRL_OSC1EN));
		break;
#endif
	default:
		unhandled_case(id);
		return;
	}
}

static inline void osc_disable(uint8_t id)
{
	uint32_t	mcctrl;

	mcctrl = pm_read_reg(MCCTRL);
	mcctrl &= ~(PM_BIT(MCCTRL_OSC0EN) << id);
	pm_write_reg(MCCTRL, mcctrl);
}

static inline bool osc_is_running(uint8_t id)
{
	return pm_read_reg(POSCSR) & (PM_BIT(OSC0RDY) << id);
}

__always_inline static uint32_t osc_get_rate(uint8_t id)
{
	switch (id) {
#ifdef BOARD_OSC0_HZ
	case 0:
		return BOARD_OSC0_HZ;
#endif
#ifdef BOARD_OSC1_HZ
	case 1:
		return BOARD_OSC1_HZ;
#endif
	default:
		unhandled_case(id);
		return 0;
	}
}

#endif /* !__ASSEMBLY__ */

//! @}

#endif /* CLK_AVR32_PM_OSC_H_INCLUDED */

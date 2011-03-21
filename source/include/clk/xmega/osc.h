/**
 * \file
 *
 * \brief Oscillator management using the Xmega OSC module
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
#ifndef CLK_XMEGA_OSC_H_INCLUDED
#define CLK_XMEGA_OSC_H_INCLUDED

#include <regs/xmega_osc.h>

/**
 * \weakgroup osc_group
 * @{
 */

#ifndef __ASSEMBLY__

#include <assert.h>
#include <types.h>

/**
 * \internal
 * \brief Enable internal oscillator \a id
 *
 * Do not call this function directly. Use osc_enable() instead.
 */
__always_inline static void osc_enable_internal(uint8_t id)
{
	uint8_t ctrl;

	ctrl = osc_read_reg(CTRL);
	ctrl |= 1 << id;
	osc_write_reg(CTRL, ctrl);
}

/**
 * \fn osc_enable_external(uint8_t id)
 * \internal
 * \brief Enable external oscillator \a id
 *
 * Do not call this function directly. Use osc_enable() instead. Also
 * note that this function is only available if the board actually has
 * an external oscillator crystal.
 */
#ifdef BOARD_XOSC_HZ
__always_inline static void osc_enable_external(uint8_t id)
{
	assert(id == OSC_ID_XOSC);

	osc_write_reg(XOSCCTRL, OSC_BF(FRQRANGE, BOARD_XOSC_RANGE)
			| OSC_BF(XOSCSEL, BOARD_XOSC_TYPE));
}
#else
ERROR_FUNC(osc_no_external, "No external oscillator on the selected board");

static inline void osc_enable_external(uint8_t id)
{
	osc_no_external();
}
#endif

__always_inline static void osc_disable(uint8_t id)
{
	uint8_t ctrl;

	ctrl = osc_read_reg(CTRL);
	ctrl &= ~(1 << id);
	osc_write_reg(CTRL, ctrl);
}

static inline bool osc_is_running(uint8_t id)
{
	return osc_read_reg(STATUS) & (1 << id);
}

#endif /* __ASSEMBLY__ */

//! @}

#endif /* CLK_XMEGA_OSC_H_INCLUDED */

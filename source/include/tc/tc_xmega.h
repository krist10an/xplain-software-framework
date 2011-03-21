/**
 * \file
 *
 * \brief Generic AVR XMEGA timer/counter definitions.
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

#ifndef TC_TC_XMEGA_H_INCLUDED
#define TC_TC_XMEGA_H_INCLUDED

#include <assert.h>
#include <stdint.h>
#include <chip/memory-map.h>
#include <chip/tc.h>

/**
 * \defgroup tc_xmega_group AVR XMEGA TC
 *
 * Definitions for getting a TC's register base address at compile time, and
 * clock selection and frequency at runtime.
 *
 * @{
 */

/**
 * \brief Get a pointer to the TC registers.
 *
 * \param id The ID of the TC module. This must be a constant value.
 * The first ID is 0 and it refers to the first TC in the peripheral module
 * address map. The other TCs that follow get increasing IDs  1, 2, 3 ...
 *
 * \return A pointer to a virtual address corresponding to the base
 * address of the selected TC peripheral.
 */
#define tc_get_regs(id)    ((void *) (TC##id##_BASE))

/**
 * \brief Select clock for a specified TC and resolution.
 *
 * This function returns the clock selection, as prescaled CLKper, for a
 * specified TC that gives a resolution at least as high as the one specified.
 * The resolution of a TC is synonymous with its clock frequency.
 *
 * \note It is also possible to clock TCs with event channels. This is not
 * handled by this implementation.
 *
 * \param tc_id      ID of TC to get clock selection for.
 * \param resolution Desired resolution for the TC.
 *
 * \return The clock selection (prescaled CLKper) that gives a resolution at
 * least as high as \a resolution for \a tc_id.
 */
static inline uint8_t tc_select_clock(uint8_t tc_id, uint32_t resolution)
{
	uint32_t tc_clk_rate = tc_get_pclk_hz(tc_id);

	if (resolution <= (tc_clk_rate / 1024))
		return TC_CLKSEL_DIV1024;
	else if (resolution <= (tc_clk_rate / 256))
		return TC_CLKSEL_DIV256;
	else if (resolution <= (tc_clk_rate / 64))
		return TC_CLKSEL_DIV64;
	else if (resolution <= (tc_clk_rate / 8))
		return TC_CLKSEL_DIV8;
	else if (resolution <= (tc_clk_rate / 4))
		return TC_CLKSEL_DIV4;
	else if (resolution <= (tc_clk_rate / 2))
		return TC_CLKSEL_DIV2;
	else
		return TC_CLKSEL_DIV1;
}

/**
 * \brief Get the resolution for a specified TC and clock selection.
 *
 * This function returns the resolution which the specified clock selection
 * and TC will result in. The resolution of a TC is synonymous with its clock
 * frequency.
 *
 * \note This function does not handle event channel clock selections.
 *
 * \param tc_id  ID of TC to get resolution for.
 * \param clksel Clock selection, as prescaled CLKper.
 *
 * \return The resolution of \a tc_id with the clock selection \a clksel.
 */
static inline uint32_t tc_get_resolution(uint8_t tc_id, uint8_t clksel)
{
	uint32_t clock_rate = tc_get_pclk_hz(tc_id);

	switch(clksel) {
	case TC_CLKSEL_OFF:
		clock_rate = 0;
		break;

	case TC_CLKSEL_DIV1024:
		clock_rate /= 1024;
		break;

	case TC_CLKSEL_DIV256:
		clock_rate /= 256;
		break;

	case TC_CLKSEL_DIV64:
		clock_rate /= 64;
		break;

	case TC_CLKSEL_DIV8:
		clock_rate /= 8;
		break;

	case TC_CLKSEL_DIV4:
		clock_rate /= 4;
		break;

	case TC_CLKSEL_DIV2:
		clock_rate /= 2;
		break;

	case TC_CLKSEL_DIV1:
		break;

	default:
		unhandled_case(clksel);
		break;
	}

	return clock_rate;
}

//@}
#endif /* TC_TC_XMEGA_H_INCLUDED */

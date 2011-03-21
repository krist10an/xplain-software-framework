/**
 * \file
 *
 * \brief ATxmega128A1 chip-specific timer/counter definitions.
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

#ifndef CHIP_TC_H_INCLUDED
#define CHIP_TC_H_INCLUDED

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <clk/sys.h>
#include <regs/xmega_tc.h>

/**
 * \ingroup tc_xmega_group
 * \section ATxmega128A1 chip-specific TC definitions
 * @{
 */

/**
 * \brief Get the SYSCLK port for a specified \a tc_id
 */
static inline uint8_t tc_get_sysclk_port(uint8_t tc_id)
{
	switch (tc_id) {
	case 0: /* Fall through. */
	case 1:
		return SYSCLK_PORT_C;

	case 2: /* Fall through. */
	case 3:
		return SYSCLK_PORT_D;

	case 4: /* Fall through. */
	case 5:
		return SYSCLK_PORT_E;

	case 6: /* Fall through. */
	case 7:
		return SYSCLK_PORT_F;

	default:
		unhandled_case(tc_id);
		return 0;
	}
}

/**
 * \brief Get the SYSCLK ID for a specified \a tc_id
 */
static inline uint8_t tc_get_sysclk_id(uint8_t tc_id)
{
	switch (tc_id) {
	case 0: /* Fall through */
	case 2:
	case 4:
	case 6:
		return SYSCLK_TC0;

	case 1: /* Fall through */
	case 3:
	case 5:
	case 7:
		return SYSCLK_TC1;

	default:
		unhandled_case(tc_id);
		return 0;
	}
}

/**
 * \brief Enable the peripheral clock for a specified \a tc_id
 */
static inline void tc_enable_pclk(uint8_t tc_id)
{
	sysclk_enable_module(tc_get_sysclk_port(tc_id),
			tc_get_sysclk_id(tc_id));
}

/**
 * \brief Disable the peripheral clock for a specified \a tc_id
 */
static inline void tc_disable_pclk(uint8_t tc_id)
{
	sysclk_disable_module(tc_get_sysclk_port(tc_id),
			tc_get_sysclk_id(tc_id));
}

/**
 * \brief Return whether peripheral clock is enabled for a specified \a tc_id
 */
static inline bool tc_pclk_is_enabled(uint8_t tc_id)
{
	return sysclk_module_is_enabled(tc_get_sysclk_port(tc_id),
			tc_get_sysclk_id(tc_id));
}

/**
 * \brief Get the clock frequency for a specified \a tc_id.
 *
 * All TCs in this chip are connected to CLKper, which is the same frequency
 * as CLKcpu. \a tc_id is therefore only defined as a parameter for code
 * consistency across devices.
 *
 * \todo Revisit, remove dependence on CONFIG_CPU_HZ.
 */
static inline uint32_t tc_get_pclk_hz(uint8_t tc_id)
{
	return (uint32_t)CONFIG_CPU_HZ;
}

//@}

#endif /* CHIP_TC_H_INCLUDED */

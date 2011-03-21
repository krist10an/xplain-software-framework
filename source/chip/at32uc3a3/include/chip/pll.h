/**
 * \file
 *
 * \brief Chip-specific PLL definitions
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
#ifndef CHIP_PLL_H_INCLUDED
#define CHIP_PLL_H_INCLUDED

/**
 * \weakgroup pll_group
 * @{
 */

#define PLL_MAX_STARTUP_CYCLES		63
#define NR_PLLS				2

/**
 * \note The PLL must run at twice this frequency internally, but the
 * output frequency may be divided by two by setting the PLLOPT[1] bit.
 */
#define PLL_MIN_HZ			40000000
#define PLL_MAX_HZ			240000000

//! \name Chip-specific PLL options
//@{
//! VCO frequency range is 80-180 MHz (160-240 MHz if unset).
#define PLL_OPT_VCO_RANGE_LOW		0
//! Divide output frequency by two
#define PLL_OPT_OUTPUT_DIV		1
//! Disable wide-bandwidth mode
#define PLL_OPT_WBM_DISABLE		2
#define PLL_NR_OPTIONS			3
//! The threshold under which to set the #PLL_OPT_VCO_RANGE_LOW option
#define PLL_VCO_LOW_THRESHOLD		170000000
//@}

enum pll_source {
	PLL_SRC_OSC0		= 0,	//!< Oscillator 0
	PLL_SRC_OSC1		= 1,	//!< Oscillator 1
	PLL_NR_SOURCES,			//!< Number of PLL sources
};

#include <clk/avr32_pm/pll.h>

//! @}

#endif /* CHIP_PLL_H_INCLUDED */

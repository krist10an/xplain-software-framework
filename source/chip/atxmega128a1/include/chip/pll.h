/**
 * \file
 *
 * \brief Chip-specific PLL definitions
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
#ifndef CHIP_PLL_H_INCLUDED
#define CHIP_PLL_H_INCLUDED

#include <assert.h>
#include <clk/osc.h>
#include <regs/xmega_osc.h>
#include <status_codes.h>
#include <stdint.h>

/**
 * \weakgroup pll_group
 * @{
 */

#define NR_PLLS				1

#define PLL_MIN_HZ			10000000UL
#define PLL_MAX_HZ			200000000UL

enum pll_source {
	PLL_SRC_RC2MHZ          = 0,    //!< 2 MHz Internal RC Oscillator
	PLL_SRC_RC32MHZ         = 2,    //!< 32 MHz Internal RC Oscillator
	PLL_SRC_XOSC            = 3,    //!< External Clock Source
	PLL_NR_SOURCES,                 //!< Number of PLL sources
};

#define pll_get_default_rate(pll_id)                            \
	((osc_get_rate(CONFIG_PLL##pll_id##_SOURCE)             \
			* CONFIG_PLL##pll_id##_MUL)             \
		/ CONFIG_PLL##pll_id##_DIV)

struct pll_config {
	uint8_t ctrl;
};

/**
 * \note The XMEGA PLL hardware uses hard-wired input dividers, so the
 * user must ensure that \a div is set as follows:
 *   - If \a src is PLL_SRC_32MHZ, \a div must be set to 4.
 *   - Otherwise, \a div must be set to 1.
 */
static inline void pll_config_init(struct pll_config *cfg,
		enum pll_source src, unsigned int div, unsigned int mul)
{
	/* Verify that all parameters are supported by hardware */
	assert(src < PLL_NR_SOURCES && src != 1);
	assert(mul >= 1 && mul <= 31);
	assert(osc_get_rate(src) >= 400000);
	assert(osc_get_rate(src) * mul <= PLL_MAX_HZ);

	if (src == PLL_SRC_RC32MHZ)
		assert(div == 4);
	else
		assert(div == 1);

	/* Initialize the configuration */
	cfg->ctrl = OSC_BF(PLLSRC, src) | OSC_BF(PLLFAC, mul);
}

#define pll_config_defaults(cfg, pll_id)                                \
	pll_config_init(cfg,                                            \
			CONFIG_PLL##pll_id##_SOURCE,                    \
			CONFIG_PLL##pll_id##_DIV,                       \
			CONFIG_PLL##pll_id##_MUL)

static inline void pll_config_read(struct pll_config *cfg, unsigned int pll_id)
{
	assert(pll_id < NR_PLLS);

	cfg->ctrl = osc_read_reg(PLLCTRL);
}

static inline void pll_config_write(const struct pll_config *cfg,
		unsigned int pll_id)
{
	assert(pll_id < NR_PLLS);

	osc_write_reg(PLLCTRL, cfg->ctrl);
}

static inline void pll_enable(const struct pll_config *cfg,
		unsigned int pll_id)
{
	uint8_t osc_ctrl;

	pll_config_write(cfg, pll_id);
	osc_ctrl = osc_read_reg(CTRL);
	osc_ctrl |= OSC_BIT(PLLEN);
	osc_write_reg(CTRL, osc_ctrl);
}

static inline void pll_disable(unsigned int pll_id)
{
	uint8_t osc_ctrl;

	assert(pll_id < NR_PLLS);

	osc_ctrl = osc_read_reg(CTRL);
	osc_ctrl &= ~OSC_BIT(PLLEN);
	osc_write_reg(CTRL, osc_ctrl);
}

static inline status_t pll_wait_for_lock(unsigned int pll_id)
{
	assert(pll_id < NR_PLLS);

	while (!(osc_read_reg(STATUS) & OSC_BIT(PLLRDY))) {
		/* Do nothing */
	}

	return STATUS_OK;
}

//! @}

#endif /* CHIP_PLL_H_INCLUDED */

/**
 * \file
 *
 * \brief PLL implementation using the AVR32 PM module
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
#ifndef CLK_AVR32_PM_PLL_H_INCLUDED
#define CLK_AVR32_PM_PLL_H_INCLUDED

#include <assert.h>
#include <status_codes.h>
#include <types.h>
#include <util.h>
#include <clk/osc.h>
#include <regs/avr32_pm_v2.h>

/**
 * \weakgroup pll_group
 * @{
 */

/**
 * \brief Number of milliseconds to wait for PLL lock
 */
#define PLL_TIMEOUT_MS						\
	div_ceil(1000 * (PLL_MAX_STARTUP_CYCLES * 2),		\
			OSC_SLOW_MIN_HZ)

#define pll_get_default_rate(pll_id)					\
	((osc_get_rate(CONFIG_PLL##pll_id##_SOURCE)                     \
			* CONFIG_PLL##pll_id##_MUL)	                \
		/ CONFIG_PLL##pll_id##_DIV)

struct pll_config {
	uint32_t ctrl;
};

static inline void pll_config_set_option(struct pll_config *cfg,
		unsigned int option)
{
	assert(option <= PLL_NR_OPTIONS);

	cfg->ctrl |= PM_BF(PLL_OPT, 1U << option);
}

static inline void pll_config_clear_option(struct pll_config *cfg,
		unsigned int option)
{
	assert(option <= PLL_NR_OPTIONS);

	cfg->ctrl &= ~PM_BF(PLL_OPT, 1U << option);
}

/**
 * The PLL options #PLL_OPT_VCO_RANGE_LOW and #PLL_OPT_OUTPUT_DIV will
 * be set automatically based on the calculated target frequency.
 */
static inline void pll_config_init(struct pll_config *cfg,
		enum pll_source src, unsigned int div, unsigned int mul)
{
	uint32_t	vco_hz;

	assert(src < PLL_NR_SOURCES);

	/* Calculate internal VCO frequency */
	vco_hz = (osc_get_rate(src) * mul) / div;
	assert(vco_hz >= PLL_MIN_HZ);
	assert(vco_hz <= PLL_MAX_HZ);

	cfg->ctrl = 0;

	/* Bring the internal VCO frequency up to the minimum value */
	if ((vco_hz < PLL_MIN_HZ * 2) && (mul <= 8)) {
		mul *= 2;
		vco_hz *= 2;
		pll_config_set_option(cfg, PLL_OPT_OUTPUT_DIV);
	}

	/* Set VCO frequency range according to calculated value */
	if (vco_hz < PLL_VCO_LOW_THRESHOLD)
		pll_config_set_option(cfg, PLL_OPT_VCO_RANGE_LOW);

	assert(mul > 2 && mul <= 16);
	assert(div > 0 && div <= 15);

	cfg->ctrl |= PM_BF(PLL_MUL, mul - 1) | PM_BF(PLL_DIV, div)
		| PM_BF(PLL_COUNT, PLL_MAX_STARTUP_CYCLES)
		| (src ? PM_BIT(PLL_OSC) : 0)
		| PM_BIT(PLL_EN);
}

#define pll_config_defaults(cfg, pll_id)                                \
	pll_config_init(cfg,                                            \
			CONFIG_PLL##pll_id##_SOURCE,                    \
			CONFIG_PLL##pll_id##_DIV,                       \
			CONFIG_PLL##pll_id##_MUL)

static inline void pll_config_read(struct pll_config *cfg, unsigned int pll_id)
{
	assert(pll_id < NR_PLLS);

	cfg->ctrl = pm_read_reg(PLL0 + pll_id);
}

static inline void pll_config_write(const struct pll_config *cfg,
		unsigned int pll_id)
{
	assert(pll_id < NR_PLLS);

	pm_write_reg(PLL0 + pll_id, cfg->ctrl);
}

static inline void pll_enable(const struct pll_config *cfg,
		unsigned int pll_id)
{
	assert(pll_id < NR_PLLS);

	pm_write_reg(PLL0 + pll_id, cfg->ctrl | PM_BIT(PLL_EN));
}

static inline void pll_disable(unsigned int pll_id)
{
	assert(pll_id < NR_PLLS);

	pm_write_reg(PLL0 + pll_id, 0);
}

static inline status_t pll_wait_for_lock(unsigned int pll_id)
{
	assert(pll_id < NR_PLLS);

	while (!(pm_read_reg(POSCSR) & (PM_BIT(LOCK0) << pll_id)));

	return STATUS_OK;
}

//! @}

#endif /* CLK_AVR32_PM_PLL_H_INCLUDED */

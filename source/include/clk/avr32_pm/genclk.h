/**
 * \file
 *
 * \brief Generic clock management using the AVR32 PM module
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
#ifndef CLK_AVR32_PM_GENCLK_H_INCLUDED
#define CLK_AVR32_PM_GENCLK_H_INCLUDED

#include <assert.h>
#include <util.h>
#include <regs/avr32_pm_v2.h>

/**
 * \weakgroup genclk_group
 * @{
 */

//! \brief Hardware representation of a set of generic clock parameters
typedef uint32_t genclk_config_t;

/**
 * \brief Return the default configuration of genclk \a id
 */
static inline genclk_config_t genclk_config_defaults(unsigned int id)
{
	return PM_BIT(GCCTRL_CEN);
}

/**
 * \brief Return the currently active configuration of genclk \a id
 */
static inline genclk_config_t genclk_config_read(unsigned int id)
{
	return pm_read_reg(GCCTRL0 + id * sizeof(genclk_config_t));
}

/**
 * \brief Activate the configuration \a config on genclk \a id
 */
static inline void genclk_config_write(unsigned int id, genclk_config_t config)
{
	pm_write_reg(GCCTRL0 + id * sizeof(genclk_config_t), config);
}

/**
 * \brief Enable generic clock \a old and return the new configuration
 */
static inline genclk_config_t genclk_config_enable(genclk_config_t old)
{
	return old | PM_BIT(GCCTRL_CEN);
}

/**
 * \brief Disable generic clock \a old and return the new configuration
 */
static inline genclk_config_t genclk_config_disable(genclk_config_t old)
{
	return old & ~PM_BIT(GCCTRL_CEN);
}

/**
 * \brief Select a new source clock \a src and return the new configuration
 */
static inline genclk_config_t genclk_config_set_source(genclk_config_t old,
		enum genclk_source src)
{
	uint32_t	mask;

	mask = PM_BIT(GCCTRL_OSCSEL) | PM_BIT(GCCTRL_PLLSEL);
	assert(!(src & ~mask));

	return (old & ~mask) | (src << PM_GCCTRL_OSCSEL_BIT);
}

/**
 * \brief Return a new configuration dividing the source clock by \a divider
 */
static inline genclk_config_t genclk_config_set_divider(genclk_config_t old,
		unsigned int divider)
{
	genclk_config_t	cfg;

	assert(divider > 0 && divider <= GENCLK_DIV_MAX);

	/* Clear all the bits we're about to modify */
	cfg = old & ~(PM_BIT(GCCTRL_DIVEN) | PM_BFMASK(GCCTRL_DIV));

	if (divider > 1) {
		cfg |= PM_BIT(GCCTRL_DIVEN);
		cfg |= PM_BF(GCCTRL_DIV, div_ceil(divider, 2) - 1);
	}

	return cfg;
}

//! @}

#endif /* CLK_AVR32_PM_GENCLK_H_INCLUDED */

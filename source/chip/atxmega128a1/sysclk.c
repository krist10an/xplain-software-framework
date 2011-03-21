/**
 * \file
 *
 * \brief Chip-specific system clock management functions
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
#include <assert.h>
#include <clk/sys.h>
#include <clk/osc.h>
#include <regs/xmega_clk.h>

#ifdef CONFIG_PLL0_SOURCE
static void sysclk_init_pll(void)
{
	struct pll_config pllcfg;

	switch (CONFIG_PLL0_SOURCE) {
	case PLL_SRC_RC2MHZ:
		break;
	case PLL_SRC_RC32MHZ:
		osc_enable(OSC_ID_RC32M);
		do { } while (!osc_is_running(OSC_ID_RC32M));
		break;
	case PLL_SRC_XOSC:
		osc_enable(OSC_ID_XOSC);
		do { } while (!osc_is_running(OSC_ID_XOSC));
		break;
	default:
		unhandled_case(CONFIG_PLL0_SOURCE);
		break;
	}

	pll_config_defaults(&pllcfg, 0);
	pll_enable(&pllcfg, 0);
	pll_wait_for_lock(0);
}
#endif

void sysclk_init(void)
{
	unsigned int i;

	build_assert(CONFIG_SYSCLK_SOURCE <= SYSCLK_SRC_PLL);
	build_assert(CONFIG_SYSCLK_PSADIV <= XMEGA_CLK_PSADIV_512);
	build_assert((CONFIG_SYSCLK_PSADIV & 0x01)
			|| (CONFIG_SYSCLK_PSADIV == 0));
	build_assert(CONFIG_SYSCLK_PSBCDIV <= XMEGA_CLK_PSBCDIV_2_2);

	/* Turn off all peripheral clocks that can be turned off */
	for (i = 0; i <= XMEGA_PR_PRPF; i++)
		mmio_write8((void *)(PR_BASE + i), 0xff);

	/* Set up system clock prescalers if different from defaults */
	if (CONFIG_SYSCLK_PSADIV != XMEGA_CLK_PSADIV_1
			|| CONFIG_SYSCLK_PSBCDIV != XMEGA_CLK_PSBCDIV_1_1) {
		clk_write_ccp_reg(PSCTRL, CLK_BF(PSADIV, CONFIG_SYSCLK_PSADIV)
				| CLK_BF(PSBCDIV, CONFIG_SYSCLK_PSBCDIV));
	}

	/*
	 * Switch to the selected initial system clock source, unless
	 * the default internal 2 MHz oscillator is selected.
	 */
	if (CONFIG_SYSCLK_SOURCE != SYSCLK_SRC_RC2MHZ) {
		bool need_rc2mhz = false;

		switch (CONFIG_SYSCLK_SOURCE) {
		case SYSCLK_SRC_RC32MHZ:
			osc_enable(OSC_ID_RC32M);
			do { } while (!osc_is_running(OSC_ID_RC32M));
			break;

		case SYSCLK_SRC_RC32KHZ:
			osc_enable(OSC_ID_RC32K);
			do { } while (!osc_is_running(OSC_ID_RC32K));
			break;

#ifdef CONFIG_PLL0_SOURCE
		case SYSCLK_SRC_PLL:
			if (CONFIG_PLL0_SOURCE == PLL_SRC_RC2MHZ)
				need_rc2mhz = true;
			sysclk_init_pll();
			break;
#endif

		default:
			unhandled_case(CONFIG_SYSCLK_SOURCE);
			return;
		}

		clk_write_ccp_reg(CTRL, CLK_BF(SCLKSEL, CONFIG_SYSCLK_SOURCE));
		assert(clk_read_reg(CTRL)
				== CLK_BF(SCLKSEL, CONFIG_SYSCLK_SOURCE));
		if (!need_rc2mhz)
			osc_disable(OSC_ID_RC2M);
	}
}

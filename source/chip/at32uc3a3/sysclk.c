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
#include <clk/sys.h>
#include <interrupt.h>
#include <regs/avr32_pm_v2.h>

/**
 * \weakgroup sysclk_group
 * @{
 */

/**
 * \internal
 * \defgroup sysclk_internals_group System Clock internals
 *
 * System clock management is fairly straightforward apart from one
 * thing: Enabling and disabling bus bridges. When all peripherals on a
 * given bus are disabled, the bridge to the bus may be disabled. Only
 * the PBA, PBB and PBC busses support this, and it is not practical to
 * disable the PBA bridge as it includes the Power Manager, so turning
 * it off would make it impossible to turn anything back on again. Also,
 * PBB and PBC share the same MASK register (with the AES module being
 * the only peripheral on the PBC.)
 *
 * The system clock implementation keeps track of a reference count for
 * PBB and PBC. When the reference count is zero, the bus bridge is
 * disabled, otherwise it is enabled.
 *
 * @{
 */

/**
 * \internal
 * \name Initial module clock masks
 *
 * These are the mask values written to the xxxMASK registers during
 * initialization. These values assume that
 *   - Debugging should be possible
 *   - The program may be running from flash
 *   - The PM should be available to unmask other clocks
 *   - All on-chip RAM should be available
 *   - INTC and GPIO are made permanently available for now, this may
 *     change in the future.
 */
//@{
//! \internal
//! \brief Initial value of CPUMASK
#define SYSCLK_INIT_CPUMASK                             \
	((1 << SYSCLK_OCD) | (1 << SYSCLK_SYSTIMER))
//! \internal
//! \brief Initial value of HSBMASK
#define SYSCLK_INIT_HSBMASK                             \
	((1 << SYSCLK_FLASHC_DATA)                      \
		| (1 << SYSCLK_PBA_BRIDGE)              \
		| (1 << SYSCLK_HRAMC0)                  \
		| (1 << SYSCLK_HRAMC1))
//! \internal
//! \brief Initial value of PBAMASK
#define SYSCLK_INIT_PBAMASK                             \
	((1 << SYSCLK_INTC)                             \
		| (1 << SYSCLK_GPIO)                    \
		| (1 << SYSCLK_PM))
//! \internal
//! \brief Initial value of PBBMASK
#define SYSCLK_INIT_PBBMASK     0
//@}

/**
 * \internal
 * \brief Number of enabled peripherals on the PBB and PBC busses.
 */
uint8_t sysclk_bus_refcount[2];

/**
 * \internal
 * \brief Enable a maskable module clock.
 * \param bus_id Bus index starting at 0 and following the same order as
 * the xxxMASK registers.
 * \param module_index Index of the module to be enabled. This is the
 * bit number in the corresponding xxxMASK register.
 */
void sysclk_priv_enable_module(unsigned int bus_id, unsigned int module_index)
{
	irqflags_t iflags;
	uint32_t mask;

	iflags = cpu_irq_save();

	/*
	 * Poll MSKRDY before changing mask rather than after, as it's
	 * highly unlikely to actually be cleared at this point.
	 */
	while (!(pm_read_reg(POSCSR) & PM_BIT(MSKRDY)))
		barrier();

	/* Enable the clock */
	mask = pm_read_reg(CPUMASK + 4 * bus_id);
	mask |= 1U << module_index;
	pm_write_reg(CPUMASK + 4 * bus_id, mask);

	cpu_irq_restore(iflags);
}

/**
 * \internal
 * \brief Disable a maskable module clock.
 * \param bus_id Bus index starting at 0 and following the same order as
 * the xxxMASK registers.
 * \param module_index Index of the module to be disabled. This is the
 * bit number in the corresponding xxxMASK register.
 */
void sysclk_priv_disable_module(unsigned int bus_id, unsigned int module_index)
{
	irqflags_t iflags;
	uint32_t mask;

	iflags = cpu_irq_save();

	/*
	 * Poll MSKRDY before changing mask rather than after, as it's
	 * highly unlikely to actually be cleared at this point.
	 */
	while (!(pm_read_reg(POSCSR) & PM_BIT(MSKRDY)))
		barrier();

	/* Disable the clock */
	mask = pm_read_reg(CPUMASK + 4 * bus_id);
	mask &= ~(1U << module_index);
	pm_write_reg(CPUMASK + 4 * bus_id, mask);

	cpu_irq_restore(iflags);
}

//! @}

/**
 * \brief Enable a module clock derived from the PBB clock
 * \param index Index of the module clock in the PBBMASK register
 */
void sysclk_enable_pbb_module(unsigned int index)
{
	unsigned int pbus_id = 0;
	irqflags_t iflags;

	/* The AES module is on PBC, others are on PBB */
	if (index == SYSCLK_AES)
		pbus_id = 1;

	/* Enable the bridge if necessary */
	iflags = cpu_irq_save();
	if (!sysclk_bus_refcount[pbus_id])
		sysclk_enable_hsb_module(2 + 4 * pbus_id);
	sysclk_bus_refcount[pbus_id]++;
	cpu_irq_restore(iflags);

	/* Enable the module */
	sysclk_priv_enable_module(3, index);
}

/**
 * \brief Disable a module clock derived from the PBB clock
 * \param index Index of the module clock in the PBBMASK register
 */
void sysclk_disable_pbb_module(unsigned int index)
{
	unsigned int pbus_id = 0;
	irqflags_t iflags;

	/* Disable the module */
	sysclk_priv_disable_module(3, index);

	/* The AES module is on PBC, others are on PBB */
	if (index == SYSCLK_AES)
		pbus_id = 1;

	/* Disable the bridge if possible */
	iflags = cpu_irq_save();
	sysclk_bus_refcount[pbus_id]--;
	if (!sysclk_bus_refcount[pbus_id])
		sysclk_disable_hsb_module(2 + 4 * pbus_id);
	cpu_irq_restore(iflags);
}

void sysclk_init(void)
{
	/* Turn off all synchronous clocks that are safe to turn off */
	pm_write_reg(CPUMASK, SYSCLK_INIT_CPUMASK);
	pm_write_reg(HSBMASK, SYSCLK_INIT_HSBMASK);
	pm_write_reg(PBAMASK, SYSCLK_INIT_PBAMASK);
	pm_write_reg(PBBMASK, SYSCLK_INIT_PBBMASK);

	build_assert(CONFIG_SYSCLK_PBA_DIV >= CONFIG_SYSCLK_CPU_DIV);

	/* Set up system clock dividers if different from defaults */
	if (CONFIG_SYSCLK_CPU_DIV > 0 || CONFIG_SYSCLK_PBA_DIV > 0) {
		uint32_t cksel = 0;

		if (CONFIG_SYSCLK_CPU_DIV > 0) {
			cksel |= PM_BIT(CKSEL_CPUDIV);
			cksel |= PM_BF(CKSEL_CPUSEL,
					(uint32_t)CONFIG_SYSCLK_CPU_DIV - 1);
		}
		if (CONFIG_SYSCLK_PBA_DIV > 0) {
			cksel |= PM_BIT(CKSEL_PBADIV);
			cksel |= PM_BF(CKSEL_PBASEL,
					(uint32_t)CONFIG_SYSCLK_PBA_DIV - 1);
		}
		pm_write_reg(CKSEL, cksel);
	}

	/*
	 * Switch to PLL0 if selected by the user. OSC0 is handled by
	 * early startup code.
	 */
#ifdef CONFIG_PLL0_SOURCE
	if (CONFIG_SYSCLK_SOURCE == SYSCLK_SRC_PLL0) {
		struct pll_config pllcfg;
		uint32_t mcctrl;

		pll_config_defaults(&pllcfg, 0);
		pll_enable(&pllcfg, 0);
		mcctrl = pm_read_reg(MCCTRL);
		mcctrl = PM_BFINS(MCCTRL_MCSEL, SYSCLK_SRC_PLL0, mcctrl);
		pll_wait_for_lock(0);
		pm_write_reg(MCCTRL, mcctrl);
	}
#endif
}

//! @}

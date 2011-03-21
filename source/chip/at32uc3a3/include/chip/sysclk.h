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
#ifndef CHIP_SYSCLK_H_INCLUDED
#define CHIP_SYSCLK_H_INCLUDED

/**
 * \weakgroup sysclk_group
 * @{
 */

//! \name System clock source
//@{
#define SYSCLK_SRC_RCOSC	0	//!< Use slow clock as main clock
#define SYSCLK_SRC_OSC0		1	//!< Use OSC0 as main clock
#define SYSCLK_SRC_PLL0		2	//!< Use PLL0 as main clock
//@}

//! \name Clocks derived from the CPU clock
//@{
#define SYSCLK_OCD		1	//!< On-Chip Debug system
#define SYSCLK_SYSTIMER		16	//!< COUNT/COMPARE registers
//@}

//! \name Clocks derived from the HSB clock
//@{
#define SYSCLK_FLASHC_DATA	0	//!< Flash data interface
#define SYSCLK_PBA_BRIDGE	1	//!< HSB<->PBA bridge
#define SYSCLK_PBB_BRIDGE	2	//!< HSB<->PBB bridge
#define SYSCLK_USBB_DATA	3	//!< USBB DMA and FIFO interface
#define SYSCLK_PDCA_HSB		4	//!< PDCA memory interface
#define SYSCLK_EBI		5	//!< External Bus Interface
#define SYSCLK_PBC_BRIDGE	6	//!< HSB<->PBC bridge
#define SYSCLK_DMACA		7	//!< DMACA data and config interface
#define SYSCLK_BUSMON_HSB	8	//!< Bus Performance Monitor
#define SYSCLK_HRAMC0		9	//!< HSB RAM block 0
#define SYSCLK_HRAMC1		10	//!< HSB RAM block 1
//@}

//! \name Clocks derived from the PBA clock
//@{
#define SYSCLK_INTC		0	//!< Internal interrupt controller
#define SYSCLK_GPIO		1	//!< General-Purpose I/O
#define SYSCLK_PDCA_PB		2	//!< PDCA periph bus interface
#define SYSCLK_PM		3	//!< PM/RTC/EIM configuration
#define SYSCLK_ADC		4	//!< A/D Converter
#define SYSCLK_SPI0		5	//!< SPI Controller 0
#define SYSCLK_SPI1		6	//!< SPI Controller 1
#define SYSCLK_TWIM0		7	//!< TWI Master 0
#define SYSCLK_TWIM1		8	//!< TWI Master 1
#define SYSCLK_TWIS0		9	//!< TWI Slave 0
#define SYSCLK_TWIS1		10	//!< TWI Slave 1
#define SYSCLK_USART0		11	//!< USART 0
#define SYSCLK_USART1		12	//!< USART 1
#define SYSCLK_USART2		13	//!< USART 2
#define SYSCLK_USART3		14	//!< USART 3
#define SYSCLK_SSC		15	//!< Synchronous Serial Controller
#define SYSCLK_TC0		16	//!< Timer/Counter 0
#define SYSCLK_TC1		17	//!< Timer/Counter 1
#define SYSCLK_DAC		18	//!< D/A Converter
//@}

//! \name Clocks derived from the PBB clock
//@{
#define SYSCLK_HMATRIX		0	//!< HSB Matrix configuration
#define SYSCLK_USBB_REGS	1	//!< USBB registers
#define SYSCLK_FLASHC_REGS	2	//!< Flash Controller registers
#define SYSCLK_SMC_REGS		3	//!< Static Memory Controller registers
#define SYSCLK_SDRAMC_REGS	4	//!< SDRAM Controller registers
#define SYSCLK_ECC		5	//!< ECC Controller
#define SYSCLK_MCI		6	//!< MMC Controller
#define SYSCLK_BUSMON		7	//!< Bus Performance Monitor
#define SYSCLK_MSI		8	//!< Memory Stick Interface
#define SYSCLK_AES		9	//!< Advanced Encryption Standard
//@}

#ifndef __ASSEMBLY__

#include <assert.h>
#include <clk/osc.h>
#include <clk/pll.h>

/**
 * \name Querying the system clock and its derived clocks
 *
 * The following functions may be used to query the current frequency of
 * the system clock and the CPU and bus clocks derived from it.
 * sysclk_get_main_hz() and sysclk_get_cpu_hz() can be assumed to be
 * available on all platforms, although some platforms may define
 * additional accessors for various chip-internal bus clocks. These are
 * usually not intended to be queried directly by generic code.
 */
//@{

/**
 * \brief Return the current rate in Hz of the main system clock
 *
 * \todo This function assumes that the main clock source never changes
 * once it's been set up, and that PLL0 always runs at the compile-time
 * configured default rate. While this is probably the most common
 * configuration, which we want to support as a special case for
 * performance reasons, we will at some point need to support more
 * dynamic setups as well.
 */
static inline uint32_t sysclk_get_main_hz(void)
{
	switch (CONFIG_SYSCLK_SOURCE) {
	case SYSCLK_SRC_RCOSC:
		return OSC_SLOW_NOMINAL_HZ;
#ifdef BOARD_OSC0_HZ
	case SYSCLK_SRC_OSC0:
		return BOARD_OSC0_HZ;
#endif
#ifdef CONFIG_PLL0_SOURCE
	case SYSCLK_SRC_PLL0:
		return pll_get_default_rate(0);
#endif
	default:
		unhandled_case(CONFIG_SYSCLK_SOURCE);
		return 0;
	}
}

/**
 * \brief Return the current rate in Hz of the CPU clock
 *
 * \todo This function assumes that the CPU always runs at the system
 * clock frequency. We want to support at least two more scenarios:
 * Fixed CPU/bus clock dividers (config symbols) and dynamic CPU/bus
 * clock dividers (which may change at run time). Ditto for all the bus
 * clocks.
 */
static inline uint32_t sysclk_get_cpu_hz(void)
{
	return sysclk_get_main_hz();
}

/**
 * \brief Return the current rate in Hz of the High-Speed Bus clock
 */
static inline uint32_t sysclk_get_hsb_hz(void)
{
	return sysclk_get_main_hz();
}

/**
 * \brief Return the current rate in Hz of the Peripheral Bus A clock
 */
static inline uint32_t sysclk_get_pba_hz(void)
{
	return sysclk_get_main_hz();
}

/**
 * \brief Return the current rate in Hz of the Peripheral Bus B clock
 */
static inline uint32_t sysclk_get_pbb_hz(void)
{
	return sysclk_get_main_hz();
}

//@}

extern void sysclk_priv_enable_module(unsigned int bus_id,
		unsigned int module_index);
extern void sysclk_priv_disable_module(unsigned int bus_id,
		unsigned int module_index);
extern void sysclk_priv_enable_pbb_module(unsigned int bus_id,
		unsigned int module_index);
extern void sysclk_priv_disable_pbb_module(unsigned int bus_id,
		unsigned int module_index);

//! \name Enabling and disabling synchronous clocks
//@{

/**
 * \brief Enable a module clock derived from the CPU clock
 * \param index Index of the module clock in the CPUMASK register
 */
static inline void sysclk_enable_cpu_module(unsigned int index)
{
	sysclk_priv_enable_module(0, index);
}

/**
 * \brief Disable a module clock derived from the CPU clock
 * \param index Index of the module clock in the CPUMASK register
 */
static inline void sysclk_disable_cpu_module(unsigned int index)
{
	sysclk_priv_disable_module(0, index);
}

/**
 * \brief Enable a module clock derived from the HSB clock
 * \param index Index of the module clock in the HSBMASK register
 */
static inline void sysclk_enable_hsb_module(unsigned int index)
{
	sysclk_priv_enable_module(1, index);
}

/**
 * \brief Disable a module clock derived from the HSB clock
 * \param index Index of the module clock in the HSBMASK register
 */
static inline void sysclk_disable_hsb_module(unsigned int index)
{
	sysclk_priv_disable_module(1, index);
}

/**
 * \brief Enable a module clock derived from the PBA clock
 * \param index Index of the module clock in the PBAMASK register
 */
static inline void sysclk_enable_pba_module(unsigned int index)
{
	sysclk_priv_enable_module(2, index);
}

/**
 * \brief Disable a module clock derived from the PBA clock
 * \param index Index of the module clock in the PBAMASK register
 */
static inline void sysclk_disable_pba_module(unsigned int index)
{
	sysclk_priv_disable_module(2, index);
}

extern void sysclk_enable_pbb_module(unsigned int index);
extern void sysclk_disable_pbb_module(unsigned int index);

//@}

extern void sysclk_init(void);

#endif /* !__ASSEMBLY__ */

//! @}

#endif /* CHIP_SYSCLK_H_INCLUDED */

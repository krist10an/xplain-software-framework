/**
 * \file
 *
 * \brief Chip-specific system clock management functions
 *
 * Copyright (C) 2009 - 2010 Atmel Corporation. All rights reserved.
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

#include <compiler.h>
#include <clk/pll.h>
#include <regs/xmega_pr.h>
#include <regs/xmega_clk.h>
#include <stdint.h>
#include <stdbool.h>

//! \name System Clock Sources
//@{
#define SYSCLK_SRC_RC2MHZ       0x00
#define SYSCLK_SRC_RC32MHZ      0x01
#define SYSCLK_SRC_RC32KHZ      0x02
#define SYSCLK_SRC_XOSC         0x03
#define SYSCLK_SRC_PLL          0x04
//@}

//! \name System Clock Port Numbers
//@{
#define SYSCLK_PORT_GEN         XMEGA_PR_PRGEN  //!< No particular port
#define SYSCLK_PORT_A           XMEGA_PR_PRPA   //!< Devices on PORTA
#define SYSCLK_PORT_B           XMEGA_PR_PRPB   //!< Devices on PORTB
#define SYSCLK_PORT_C           XMEGA_PR_PRPC   //!< Devices on PORTC
#define SYSCLK_PORT_D           XMEGA_PR_PRPD   //!< Devices on PORTD
#define SYSCLK_PORT_E           XMEGA_PR_PRPE   //!< Devices on PORTE
#define SYSCLK_PORT_F           XMEGA_PR_PRPF   //!< Devices on PORTF
//@}

//! \name Clocks not associated with any port
//@{
#define SYSCLK_DMA              (1U << PR_DMA_BIT)      //!< DMA Controller
#define SYSCLK_EVSYS            (1U << PR_EVSYS_BIT)    //!< Event System
#define SYSCLK_RTC              (1U << PR_RTC_BIT)      //!< Real-Time Counter
#define SYSCLK_EBI              (1U << PR_EBI_BIT)      //!< Ext Bus Interface
#define SYSCLK_AES              (1U << PR_AES_BIT)      //!< AES Module
#ifdef CONFIG_XMEGA_USB
# define SYSCLK_USB             (1U << PR_USB_BIT)      //!< USB Module
#endif
//@}

//! \name Clocks on PORTA and PORTB
//@{
#define SYSCLK_AC               (1U << PR_AC_BIT)       //!< Analog Comparator
#define SYSCLK_ADC              (1U << PR_ADC_BIT)      //!< A/D Converter
#define SYSCLK_DAC              (1U << PR_DAC_BIT)      //!< D/A Converter
//@}

//! \name Clocks on PORTC, PORTD, PORTE and PORTF
//@{
#define SYSCLK_TC0              (1U << PR_TC0_BIT)      //!< Timer/Counter 0
#define SYSCLK_TC1              (1U << PR_TC1_BIT)      //!< Timer/Counter 1
#define SYSCLK_HIRES            (1U << PR_HIRES_BIT)    //!< Hi-Res Extension
#define SYSCLK_SPI              (1U << PR_SPI_BIT)      //!< SPI controller
#define SYSCLK_USART0           (1U << PR_USART0_BIT)   //!< USART 0
#define SYSCLK_USART1           (1U << PR_USART1_BIT)   //!< USART 1
#define SYSCLK_TWI              (1U << PR_TWI_BIT)      //!< TWI controller
//@}

/**
 * \weakgroup sysclk_group
 * @{
 */

#ifndef __ASSEMBLY__

#include <assert.h>
#include <chip/memory-map.h>
#include <io.h>

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
	case SYSCLK_SRC_RC2MHZ:
		return 2000000UL;
	case SYSCLK_SRC_RC32MHZ:
		return 32000000UL;
	case SYSCLK_SRC_RC32KHZ:
		return 32768UL;
#ifdef BOARD_XOSC_HZ
	case SYSCLK_SRC_XOSC:
		return BOARD_XOSC_HZ;
#endif
#ifdef CONFIG_PLL0_SOURCE
	case SYSCLK_SRC_PLL:
		return pll_get_default_rate(0);
#endif
	default:
		unhandled_case(CONFIG_SYSCLK_SOURCE);
		return 0;
	}
}

/**
 * \brief Return the current rate in Hz of clk_PER4.
 *
 * This clock can run up to four times faster than the CPU clock.
 */
static inline uint32_t sysclk_get_per4_hz(void)
{
	uint8_t shift = 0;

	if (CONFIG_SYSCLK_PSADIV & 0x01)
		shift = (CONFIG_SYSCLK_PSADIV >> 1) + 1;

	return sysclk_get_main_hz() >> shift;
}

/**
 * \brief Return the current rate in Hz of clk_PER2.
 *
 * This clock can run up to two times faster than the CPU clock.
 */
static inline uint32_t sysclk_get_per2_hz(void)
{
	switch (CONFIG_SYSCLK_PSBCDIV) {
	case XMEGA_CLK_PSBCDIV_1_1:
	case XMEGA_CLK_PSBCDIV_1_2:
		return sysclk_get_per4_hz();
	case XMEGA_CLK_PSBCDIV_4_1:
		return sysclk_get_per4_hz() / 4;
	case XMEGA_CLK_PSBCDIV_2_2:
		return sysclk_get_per4_hz() / 2;
	default:
		unhandled_case(CONFIG_SYSCLK_PSBCDIV);
		return 0;
	}
}

/**
 * \brief Return the current rate in Hz of clk_PER.
 *
 * This clock always runs at the same rate as the CPU clock.
 */
static inline uint32_t sysclk_get_per_hz(void)
{
	if (CONFIG_SYSCLK_PSBCDIV & 0x01)
		return sysclk_get_per2_hz() / 2;
	else
		return sysclk_get_per2_hz();
}

/**
 * \brief Return the current rate in Hz of the CPU clock.
 */
static inline uint32_t sysclk_get_cpu_hz(void)
{
	return sysclk_get_per_hz();
}

//@}

//! \name Enabling and disabling synchronous clocks
//@{

extern void sysclk_enable_module(uint8_t port, uint8_t id);
extern void sysclk_disable_module(uint8_t port, uint8_t id);

//@}

/**
 * \brief Check if the synchronous clock is enabled for a module
 *
 * \param port ID of the port to which the module is connected (one of
 * the SYSCLK_PORT_* definitions).
 * \param id The ID (bitmask) of the peripheral module to check (one of
 * the SYSCLK_* module definitions).
 *
 * \retval true If the clock for module \a id on \a port is enabled.
 * \retval false If the clock for module \a id on \a port is disabled.
 */
__always_inline static bool sysclk_module_is_enabled(uint8_t port, uint8_t id)
{
	uint8_t mask = mmio_read8((void *)(PR_BASE + port));
	return (mask & id) == 0;
}

//! \name System Clock Initialization
//@{

extern void sysclk_init(void);

//@}

#endif /* !__ASSEMBLY__ */

//! @}

#endif /* CHIP_SYSCLK_H_INCLUDED */

/**
 * \file
 *
 * \brief Chip-specific oscillator management functions
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
#ifndef CHIP_OSC_H_INCLUDED
#define CHIP_OSC_H_INCLUDED

#include <board/osc.h>

/**
 * \weakgroup avr32_pm_v2_regs_group
 * @{
 */

//! \name OSC0/OSC1 mode values
//@{
#define OSC_MODE_EXTERNAL	0	//!< External clock connected to XIN
#define OSC_MODE_XTAL		1	//!< Crystal connected to XIN/XOUT
//@}

//! \name OSC32 mode values
//@{
#define OSC32_MODE_EXTERNAL	0	//!< External clock connected to XIN32
#define OSC32_MODE_XTAL		1	//!< Crystal connected to XIN32/XOUT32
//@}

//! \name OSC0/OSC1 startup values
//@{
#define OSC_STARTUP_0		0	//!< 0 cycles
#define OSC_STARTUP_64		1	//!< 64 cycles (560 us)
#define OSC_STARTUP_128		2	//!< 128 cycles (1.1 ms)
#define OSC_STARTUP_2048	3	//!< 2048 cycles (18 ms)
#define OSC_STARTUP_4096	4	//!< 4096 cycles (36 ms)
#define OSC_STARTUP_8192	5	//!< 8192 cycles (71 ms)
#define OSC_STARTUP_16384	6	//!< 16384 cycles (142 ms)
//@}

//! \name OSC32 startup values
//@{
#define OSC32_STARTUP_0		0	//!< 0 cycles
#define OSC32_STARTUP_128	1	//!< 128 cycles
#define OSC32_STARTUP_8192	2	//!< 8192 cycles
#define OSC32_STARTUP_16384	3	//!< 16384 cycles
#define OSC32_STARTUP_65536	4	//!< 65536 cycles
#define OSC32_STARTUP_131072	5	//!< 131072 cycles
#define OSC32_STARTUP_262144	6	//!< 262144 cycles
#define OSC32_STARTUP_524288	7	//!< 524288 cycles
//@}

/**
 * \def OSC0_STARTUP_TIMEOUT
 * \brief Number of slow clock cycles to wait for OSC0 to start
 *
 * This is the number of slow clock cycles corresponding to
 * OSC0_STARTUP_VALUE with an additional 25% safety margin. If the
 * oscillator isn't running when this timeout has expired, it is assumed
 * to have failed to start.
 */
/**
 * \def OSC0_MODE_VALUE
 * \brief Board-dependent value written to the MODE bitfield of
 * PM_OSCCTRL(0)
 */
/**
 * \def OSC0_STARTUP_VALUE
 * \brief Board-dependent value written to the STARTUP bitfield of
 * PM_OSCCTRL(0)
 */

#if defined(BOARD_OSC0_STARTUP_US)
# if BOARD_OSC0_STARTUP_US == 0
#  define OSC0_STARTUP_VALUE	OSC_STARTUP_0
#  define OSC0_STARTUP_TIMEOUT	8
# elif BOARD_OSC0_STARTUP_US <= 560
#  define OSC0_STARTUP_VALUE	OSC_STARTUP_64
#  define OSC0_STARTUP_TIMEOUT	80
# elif BOARD_OSC0_STARTUP_US <= 1100
#  define OSC0_STARTUP_VALUE	OSC_STARTUP_128
#  define OSC0_STARTUP_TIMEOUT	160
# elif BOARD_OSC0_STARTUP_US <= 18000
#  define OSC0_STARTUP_VALUE	OSC_STARTUP_2048
#  define OSC0_STARTUP_TIMEOUT	2560
# elif BOARD_OSC0_STARTUP_US <= 36000
#  define OSC0_STARTUP_VALUE	OSC_STARTUP_4096
#  define OSC0_STARTUP_TIMEOUT	5120
# elif BOARD_OSC0_STARTUP_US <= 71000
#  define OSC0_STARTUP_VALUE	OSC_STARTUP_8192
#  define OSC0_STARTUP_TIMEOUT	10240
# elif BOARD_OSC0_STARTUP_US <= 142000
#  define OSC0_STARTUP_VALUE	OSC_STARTUP_16384
#  define OSC0_STARTUP_TIMEOUT	20480
# else
#  error BOARD_OSC0_STARTUP_US is too high
# endif
# ifdef BOARD_OSC0_XTAL
#  define OSC0_MODE_VALUE	OSC_MODE_XTAL
# else
#  define OSC0_MODE_VALUE	OSC_MODE_EXTERNAL
# endif
#else
# if defined(CONFIG_MAIN_CLK_OSC0)
#  error BOARD_OSC0_STARTUP_US must be defined by the board code
# endif
# ifdef __DOXYGEN__
#  define OSC0_STARTUP_VALUE     UNDEFINED
#  define OSC0_STARTUP_TIMEOUT   UNDEFINED
#  define OSC0_MODE_VALUE        UNDEFINED
# endif
#endif

/**
 * \def OSC1_STARTUP_VALUE
 * \brief Board-dependent value written to the STARTUP bitfield of
 * PM_OSCCTRL(1)
 */
/**
 * \def OSC1_STARTUP_TIMEOUT
 * \brief Number of slow clock cycles to wait for OSC1 to start
 *
 * This is the number of slow clock cycles corresponding to
 * OSC1_STARTUP_VALUE with an additional 25% safety margin. If the
 * oscillator isn't running when this timeout has expired, it is assumed
 * to have failed to start.
 */
/**
 * \def OSC1_MODE_VALUE
 * \brief Board-dependent value written to the MODE bitfield of
 * PM_OSCCTRL(1)
 */
#if defined(BOARD_OSC1_STARTUP_US)
# if BOARD_OSC1_STARTUP_US == 0
#  define OSC1_STARTUP_VALUE	OSC_STARTUP_0
#  define OSC1_STARTUP_TIMEOUT	8
# elif BOARD_OSC1_STARTUP_US <= 560
#  define OSC1_STARTUP_VALUE	OSC_STARTUP_64
#  define OSC1_STARTUP_TIMEOUT	80
# elif BOARD_OSC1_STARTUP_US <= 1100
#  define OSC1_STARTUP_VALUE	OSC_STARTUP_128
#  define OSC1_STARTUP_TIMEOUT	160
# elif BOARD_OSC1_STARTUP_US <= 18000
#  define OSC1_STARTUP_VALUE	OSC_STARTUP_2048
#  define OSC1_STARTUP_TIMEOUT	2560
# elif BOARD_OSC1_STARTUP_US <= 36000
#  define OSC1_STARTUP_VALUE	OSC_STARTUP_4096
#  define OSC1_STARTUP_TIMEOUT	5120
# elif BOARD_OSC1_STARTUP_US <= 71000
#  define OSC1_STARTUP_VALUE	OSC_STARTUP_8192
#  define OSC1_STARTUP_TIMEOUT	10240
# elif BOARD_OSC1_STARTUP_US <= 142000
#  define OSC1_STARTUP_VALUE	OSC_STARTUP_16384
#  define OSC1_STARTUP_TIMEOUT	20480
# else
#  error BOARD_OSC1_STARTUP_US is too high
# endif
# ifdef BOARD_OSC1_XTAL
#  define OSC1_MODE_VALUE	OSC_MODE_XTAL
# else
#  define OSC1_MODE_VALUE	OSC_MODE_EXTERNAL
# endif
#else
# ifdef __DOXYGEN__
#  define OSC1_STARTUP_VALUE     UNDEFINED
#  define OSC1_STARTUP_TIMEOUT   UNDEFINED
#  define OSC1_MODE_VALUE        UNDEFINED
# endif
#endif

//! @}

/**
 * \weakgroup osc_group
 * @{
 */

/**
 * \name Board-specific configuration parameters
 * The following definitions must be provided by the board code for all
 * working oscillators on the board.
 */
//@{
/**
 * \def BOARD_OSC0_HZ
 * \brief Clock frequency of OSC0 in Hz
 */
/**
 * \def BOARD_OSC0_STARTUP_US
 * \brief Startup time of OSC0 in microseconds
 */
/**
 * \def BOARD_OSC0_XTAL
 * \brief OSC0 uses a crystal, not an external clock
 */
/**
 * \def BOARD_OSC1_HZ
 * \brief Clock frequency of OSC1 in Hz
 */
/**
 * \def BOARD_OSC1_STARTUP_US
 * \brief Startup time of OSC1 in microseconds
 */
/**
 * \def BOARD_OSC1_XTAL
 * \brief OSC1 uses a crystal, not an external clock
 */
/**
 * \def BOARD_OSC32_HZ
 * \brief Clock frequency of OSC32 in Hz
 */
/**
 * \def BOARD_OSC32_STARTUP_US
 * \brief Startup time of OSC32 in microseconds
 */
/**
 * \def BOARD_OSC32_XTAL
 * \brief OSC32 uses a crystal, not an external clock
 */

/**
 * \name Slow clock frequency limits
 * The slow clock is an internal RC oscillator whose frequency may drift
 * a bit as a result of temperature changes. These definitions provide
 * upper and lower limits which may be used to calculate upper and lower
 * limits of timeouts, derived clock frequencies, etc.
 */
//@{
//! Nominal frequency of the slow clock in Hz
#define OSC_SLOW_NOMINAL_HZ	115000
//! Minimum frequency of the slow clock in Hz
#define OSC_SLOW_MIN_HZ		100000
//! Maximum frequency of the slow clock in Hz
#define OSC_SLOW_MAX_HZ		120000
//@}

/* Include functionality common to all chips with a PM module */
#include <clk/avr32_pm/osc.h>

//! @}

#endif /* CHIP_OSC_H_INCLUDED */

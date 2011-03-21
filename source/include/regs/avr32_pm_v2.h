/**
 * \file
 *
 * \brief AVR32 Power Manager v2 Register Definitions
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
#ifndef REGS_AVR32_PM_V2_H_INCLUDED
#define REGS_AVR32_PM_V2_H_INCLUDED

#include <chip/memory-map.h>

/**
 * \ingroup regs_group
 * \defgroup avr32_pm_v2_regs_group Power Manager Register Definitions
 *
 * This is the register interface to version 2.x of the AVR32 Power
 * Manager module. Some of the definitions are chip-specific and
 * therefore provided by the appropriate chip-specific header file.
 *
 * @{
 */

//! \name Registers, Bitfields and Constants
//@{
#define PM_MCCTRL			0x0000	//!< Main Clock Control
# define PM_MCCTRL_MCSEL_START		0	//!< Main Clock Select
# define PM_MCCTRL_MCSEL_SIZE		2	//!< Main Clock Select
# define PM_MCCTRL_OSC0EN_BIT		2	//!< Oscillator 0 Enable
# define PM_MCCTRL_OSC1EN_BIT		3	//!< Oscillator 1 Enable
#define PM_CKSEL			0x0004	//!< Clock Select
# define PM_CKSEL_CPUSEL_START		0	//!< CPU Clock Divider
# define PM_CKSEL_CPUSEL_SIZE		3	//!< CPU Clock Divider
# define PM_CKSEL_CPUDIV_BIT		7	//!< CPU Divider Enable
# define PM_CKSEL_HSBSEL_START		8	//!< HSB Clock Divider
# define PM_CKSEL_HSBSEL_SIZE		3	//!< HSB Clock Divider
# define PM_CKSEL_HSBDIV_BIT		15	//!< HSB Divider Enable
# define PM_CKSEL_PBASEL_START		16	//!< PBA Clock Divider
# define PM_CKSEL_PBASEL_SIZE		3	//!< PBA Clock Divider
# define PM_CKSEL_PBADIV_BIT		23	//!< PBA Divider Enable
# define PM_CKSEL_PBBSEL_START		24	//!< PBB Clock Divider
# define PM_CKSEL_PBBSEL_SIZE		3	//!< PBB Clock Divider
# define PM_CKSEL_PBBDIV_BIT		31	//!< PBB Divider Enable
#define PM_CPUMASK			0x0008	//!< CPU Mask
#define PM_HSBMASK			0x000c	//!< HSB Mask
#define PM_PBAMASK			0x0010	//!< PBA Mask
#define PM_PBBMASK			0x0014	//!< PBB Mask
#define PM_PLL0				0x0020	//!< PLL0 Control
#define PM_PLL1				0x0024	//!< PLL1 Control
# define PM_PLL_EN_BIT			0	//!< PLL Enable
# define PM_PLL_OSC_BIT			1	//!< PLL Oscillator select
# define PM_PLL_OPT_START		2	//!< PLL Option
# define PM_PLL_OPT_SIZE		3	//!< PLL Option
# define PM_PLL_DIV_START		8	//!< PLL Division factor
# define PM_PLL_DIV_SIZE		4	//!< PLL Division factor
# define PM_PLL_MUL_START		16	//!< PLL Multiply factor
# define PM_PLL_MUL_SIZE		4	//!< PLL Multiply factor
# define PM_PLL_COUNT_START		24	//!< PLL startup count
# define PM_PLL_COUNT_SIZE		6	//!< PLL startup count
#define PM_OSC0CTRL			0x0028	//!< OSC0 Control
#define PM_OSC1CTRL			0x002c	//!< OSC1 Control
# define PM_OSCCTRL_MODE_START		0	//!< Oscillator Mode
# define PM_OSCCTRL_MODE_SIZE		3	//!< Oscillator Mode
# define PM_OSCCTRL_STARTUP_START	8	//!< Oscillator Startup Time
# define PM_OSCCTRL_STARTUP_SIZE	3	//!< Oscillator Startup Time
#define PM_OSCCTRL32			0x0030	//!< 32 kHz Oscillator Control
# define PM_OSCCTRL32_OSC32EN_BIT	0	//!< Enable the 32KHz oscillator
# define PM_OSCCTRL32_MODE_START	8	//!< 32KHz OSC Mode
# define PM_OSCCTRL32_MODE_SIZE		3	//!< 32KHz OSC Mode
# define PM_OSCCTRL32_STARTUP_START	16	//!< 32KHz OSC Startup Time
# define PM_OSCCTRL32_STARTUP_SIZE	3	//!< 32KHz OSC Startup Time
#define PM_IER				0x0040	//!< Interrupt Enable Register
#define PM_IDR				0x0044	//!< Interrupt Disable Register
#define PM_IMR				0x0048	//!< Interrupt Mask Register
#define PM_ISR				0x004c	//!< Interrupt Status Register
#define PM_ICR				0x0050	//!< Interrupt Clear Register
#define PM_POSCSR			0x0054	//!< Power/Oscillator Status
# define PM_LOCK0_BIT			0	//!< PLL 0 locked
# define PM_LOCK1_BIT			1	//!< PLL 1 locked
# define PM_WAKE_BIT			2	//!< Wake pin asserted
# define PM_VOK_BIT			3	//!< Voltage OK
# define PM_VMRDY_BIT			4	//!< Voltage Monitor Ready
# define PM_CKRDY_BIT			5	//!< Clock Ready
# define PM_MSKRDY_BIT			6	//!< Mask ready
# define PM_OSC0RDY_BIT			7	//!< OSC0 ready
# define PM_OSC1RDY_BIT			8	//!< OSC1 ready
# define PM_OSC32RDY_BIT		9	//!< OSC32 ready
# define PM_BODDET_BIT			16	//!< Brown out detection
# define PM_BOD33DET_BIT		17	//!< Brown out 33 detection
#define PM_GCCTRL0			0x0060	//!< Generic Clock Control
# define PM_GCCTRL_OSCSEL_BIT		0	//!< Oscillator Select
# define PM_GCCTRL_PLLSEL_BIT		1	//!< PLL Select
# define PM_GCCTRL_CEN_BIT		2	//!< Clock Enable
# define PM_GCCTRL_DIVEN_BIT		4	//!< Divide enable
# define PM_GCCTRL_DIV_START		8	//!< Division Factor
# define PM_GCCTRL_DIV_SIZE		8	//!< Division Factor
#define PM_RCCR				0x00c0	//!< RC Oscillator Calibration
# define PM_RCCR_CALIB_START		0	//!< Calibration Value
# define PM_RCCR_CALIB_SIZE		10	//!< Calibration Value
# define PM_RCCR_FCD_BIT		16	//!< Flash Calibration Done
# define PM_RCCR_KEY_START		24	//!< Register Write protection
# define PM_RCCR_KEY_SIZE		8	//!< Register Write protection
#define PM_BGCR				0x00c4	//!< Bandgap Calibration
# define PM_BGCR_CALIB_START		0	//!< Calibration value
# define PM_BGCR_CALIB_SIZE		3	//!< Calibration value
# define PM_BGCR_FCD_BIT		16	//!< Flash Calibration Done
# define PM_BGCR_KEY_START		24	//!< Register Write protection
# define PM_BGCR_KEY_SIZE		8	//!< Register Write protection
#define PM_VREGCR			0x00c8	//!< Buck Regulator Calibration
# define PM_VREGCR_CALIB_START		0
# define PM_VREGCR_CALIB_SIZE		3
# define PM_VREGCR_FCD_BIT		16	//!< Flash calibration done
# define PM_VREGCR_KEY_START		24
# define PM_VREGCR_KEY_SIZE		8
#define PM_BOD				0x00d0	//!< BOD Level Register
#define PM_BOD33			0x00d4	//!< BOD33 Level Register
# define PM_BOD_LEVEL_START		0	//!< BOD Level
# define PM_BOD_LEVEL_SIZE		6	//!< BOD Level
# define PM_BOD_HYST_BIT		6	//!< BOD Hysteresis
# define PM_BOD_CTRL_START		8	//!< BOD Control
# define PM_BOD_CTRL_SIZE		2	//!< BOD Control
#  define PM_BOD_CTRL_OFF1		0	//!< BOD is off
//! BOD is enabled and can reset the chip
#  define PM_BOD_CTRL_ENABLED		1
//! BOD is enabled but can't reset the chip
#  define PM_BOD_CTRL_ENABLED_NORESET	2
#  define PM_BOD_CTRL_OFF2		3	//!< BOD is off
# define PM_BOD_FCD_BIT			16	//!< BOD Fuse calibration done
# define PM_BOD_KEY_START		24	//!< Register Write protection
# define PM_BOD_KEY_SIZE		8	//!< Register Write protection
#define PM_RCAUSE			0x0140	//!< Reset Cause
# define PM_RCAUSE_POR_BIT		0	//!< Power-on Reset
# define PM_RCAUSE_BOD_BIT		1	//!< Brown-out Reset
# define PM_RCAUSE_EXT_BIT		2	//!< External Reset
# define PM_RCAUSE_WDT_BIT		3	//!< Watchdog Reset
# define PM_RCAUSE_JTAG_BIT		4	//!< JTAG reset
# define PM_RCAUSE_SLEEP_BIT		6	//!< Sleep
# define PM_RCAUSE_CPUERR_BIT		7	//!< CPU Error
# define PM_RCAUSE_OCDRST_BIT		8	//!< OCD Reset
# define PM_RCAUSE_JTAGHARD_BIT		9	//!< JTAG Hard Reset
# define PM_RCAUSE_BOD33_BIT		10	//!< Brown-out 33 Reset
#define PM_AWEN				0x0144	//!< Asynchronous Wake Enable
# define PM_AWEN_USB_WAKEN_BIT		0	//!< Async wake up by USB
//! General Purpose Low-Power register
#define PM_GPLP				0x0200
//@}

//! \name Bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set
#define PM_BIT(name)							\
	(1U << PM_##name##_BIT)
//! \brief Create a mask with bitfield \a name set to \a value
#define PM_BF(name, value)						\
	((value) << PM_##name##_START)
//! \brief Create a mask of bitfield with \a name
#define PM_BFMASK(name)							\
	(((1U << PM_##name##_SIZE) - 1) << PM_##name##_START)
//! \brief Extract the value of bitfield \a name from \a regval
#define PM_BFEXT(name, regval)						\
	(((regval) >> PM_##name##_START)				\
		& ((1U << PM_##name##_SIZE) - 1))
//! \brief Return \a regval with bitfield \a name set to \a value
#define PM_BFINS(name, value, regval)					\
	(((regval) & ~(((1U << PM_##name##_SIZE) - 1)			\
			<< PM_##name##_START))				\
		| PM_BF(name, value))
//@}

#ifndef __ASSEMBLY__

#include <io.h>

//! \name Register access macros
//@{
//! \brief Read the value of PM register \a reg
#define pm_read_reg(reg)						\
	mmio_read32((void *)(PM_BASE + PM_##reg))
//! \brief Write \a value to PM register \a reg
#define pm_write_reg(reg, value)					\
	mmio_write32((void *)(PM_BASE + PM_##reg), (value))
//@}

#endif /* !__ASSEMBLY__ */

//! @}

#endif /* REGS_AVR32_PM_V2_H_INCLUDED */

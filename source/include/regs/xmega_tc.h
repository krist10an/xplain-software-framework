/**
 * \file
 *
 * \brief XMEGA Timer/Counter register definitions.
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

#ifndef REGS_XMEGA_TC_H_INCLUDED
#define REGS_XMEGA_TC_H_INCLUDED

#include <io.h>

/**
 * \ingroup regs_group
 * \defgroup xmega_tc_regs_group XMEGA TC Register Definitions
 *
 * This is the register interface to the XMEGA TC. The registers are
 * defined as offsets relative to the base address of the module, so they may
 * be easily reused across several modules on the same chip, or across
 * different types of chips.
 *
 * @{
 */

//! \name Register Offsets
//@{
#define TC_CTRLA         0x00 //!< Control Register A
#define TC_CTRLB         0x01 //!< Control Register B
#define TC_CTRLC         0x02 //!< Control Register C
#define TC_CTRLD         0x03 //!< Control Register D
#define TC_CTRLE         0x04 //!< Control Register E
#define TC_INTCTRLA      0x06 //!< Interrupt Enable Register A
#define TC_INTCTRLB      0x07 //!< Interrupt Enable Register B
#define TC_CTRLFCRL      0x08 //!< Control Register F Clear
#define TC_CTRLFSET      0x09 //!< Control Register F Set
#define TC_CTRLGCLR      0x0a //!< Control Register G Clear
#define TC_CTRLGSET      0x0b //!< Control Register G Set
#define TC_INTFLAGS      0x0c //!< Interrupt Flag Register
#define TC_TEMP          0x0f //!< Temporary Register for 16-bit Access
#define TC_CNTL          0x20 //!< Counter Register H
#define TC_CNTH          0x21 //!< Counter Register L
#define TC_PERL          0x26 //!< Period Register H
#define TC_PERH          0x27 //!< Period Register L
#define TC_CCAL          0x28 //!< Compare or Capture Register A L
#define TC_CCAH          0x29 //!< Compare or Capture Register A H
#define TC_CCBL          0x2a //!< Compare or Capture Register B L
#define TC_CCBH          0x2b //!< Compare or Capture Register B H
#define TC_CCCL          0x2c //!< Compare or Capture Register C L
#define TC_CCCH          0x2d //!< Compare or Capture Register C H
#define TC_CCDL          0x2e //!< Compare or Capture Register D L
#define TC_CCDH          0x2f //!< Compare or Capture Register D H
#define TC_PERBUFL       0x36 //!< Timer/Counter Period Buffer L
#define TC_PERBUFH       0x37 //!< Timer/Counter Period Buffer H
#define TC_CCABUFL       0x38 //!< Compare or Capture A Buffer Register L
#define TC_CCABUFH       0x39 //!< Compare or Capture A Buffer Register H
#define TC_CCBBUFL       0x3a //!< Compare or Capture B Buffer Register L
#define TC_CCBBUFH       0x3b //!< Compare or Capture B Buffer Register H
#define TC_CCCBUFL       0x3c //!< Compare or Capture C Buffer Register L
#define TC_CCCBUFH       0x3d //!< Compare or Capture C Buffer Register H
#define TC_CCDBUFL       0x3e //!< Compare or Capture D Buffer Register L
#define TC_CCDBUFH       0x3f //!< Compare or Capture D Buffer Register H
//@}

//! \name CTRLA register bits
//@{
#define TC_CTRLA_CLKSEL_START           0 //!< Clock Select
#define TC_CTRLA_CLKSEL_SIZE            4 //!< Clock Select
//@}

//! \name CTRLB register bits
//@{
#define TC_CTRLB_WGMODE_START           0 //!< Waveform Generation Mode
#define TC_CTRLB_WGMODE_SIZE            3 //!< Waveform Generation Mode
#define TC_CTRLB_CCAEN_BIT              4 //!< Compare or Capture A Enable
#define TC_CTRLB_CCBEN_BIT              5 //!< Compare or Capture B Enable
#define TC_CTRLB_CCCEN_BIT              6 //!< Compare or Capture C Enable
#define TC_CTRLB_CCDEN_BIT              7 //!< Compare or Capture D Enable
//@}

//! \name CTRLC register bits
//@{
#define TC_CTRLC_CMPA_BIT               0 //!< Compare Output Value A
#define TC_CTRLC_CMPB_BIT               1 //!< Compare Output Value B
#define TC_CTRLC_CMPC_BIT               2 //!< Compare Output Value C
#define TC_CTRLC_CMPD_BIT               3 //!< Compare Output Value D
//@}

//! \name CTRLD register bits
//@{
#define TC_CTRLD_EVSEL_START            0 //!< Event Source Select
#define TC_CTRLD_EVSEL_SIZE             4 //!< Event Source Select
#define TC_CTRLD_EVDLY_BIT              4 //!< Timer Delay Event
#define TC_CTRLD_EVACT_START            5 //!< Event Action
#define TC_CTRLD_EVACT_SIZE             3 //!< Event Action
//@}

//! \name CTRLE register bits
//@{
#define TC_CTRLE_BYTEM_BIT              0 //!< Byte Mode
//@}

//! \name INTCTRLA register bits
//@{
//! Timer Overflow/Underflow Interrupt Level
#define TC_INTCTRLA_OVFINTLVL_START      0
//! Timer Overflow/Underflow Interrupt Level
#define TC_INTCTRLA_OVFINTLVL_SIZE       2
//! Timer Error Interrupt Level
#define TC_INTCTRLA_ERRINTLVL_START      2
//! Timer Error Interrupt Level
#define TC_INTCTRLA_ERRINTLVL_SIZE       2
//@}

//! \name INTCTRLB register bits
//@{
//! Compare or Capture A Interrupt Level
#define TC_INTCTRLB_CCAINTLVL_START      0
//! Compare or Capture A Interrupt Level
#define TC_INTCTRLB_CCAINTLVL_SIZE       2
//! Compare or Capture B Interrupt Level
#define TC_INTCTRLB_CCBINTLVL_START      2
//! Compare or Capture B Interrupt Level
#define TC_INTCTRLB_CCBINTLVL_SIZE       2
//! Compare or Capture C Interrupt Level
#define TC_INTCTRLB_CCCINTLVL_START      4
//! Compare or Capture C Interrupt Level
#define TC_INTCTRLB_CCCINTLVL_SIZE       2
//! Compare or Capture D Interrupt Level
#define TC_INTCTRLB_CCDINTLVL_START      6
//! Compare or Capture D Interrupt Level
#define TC_INTCTRLB_CCDINTLVL_SIZE       2
//@}

//! \name CTRLFCLR and CTRLCSET register bits
//@{
#define TC_DIR_BIT              0 //!< Counter Direction
#define TC_LUPD_BIT             1 //!< Lock Update
#define TC_CMD_START            2 //!< Timer/Counter Command
#define TC_CMD_SIZE             2 //!< Timer/Counter Command
//@}

//! \name CTRLGCLR and CTRLGSET register bits
//@{
#define TC_PERBV_BIT            0 //!< Period Buffer Valid
#define TC_CCABV_BIT            1 //!< Compare or Capture A Buffer Valid
#define TC_CCBBV_BIT            2 //!< Compare or Capture B Buffer Valid
#define TC_CCCBV_BIT            3 //!< Compare or Capture C Buffer Valid
#define TC_CCDBV_BIT            4 //!< Compare or Capture D Buffer Valid
//@}

//! \name INTFLAGS register bits
//@{
#define TC_INTFLAGS_OVFIF_BIT            0
#define TC_INTFLAGS_ERRIF_BIT            1
#define TC_INTFLAGS_CCAIF_BIT            4
#define TC_INTFLAGS_CCBIF_BIT            5
#define TC_INTFLAGS_CCCIF_BIT            6
#define TC_INTFLAGS_CCDIF_BIT            7
//@}

//! \name CTRLA_CLKSEL Bitfield Values
//@{
#define TC_CLKSEL_OFF           0x0 //!< None, Timer/Counter is in off state
#define TC_CLKSEL_DIV1          0x1 //!< Prescaler: clk
#define TC_CLKSEL_DIV2          0x2 //!< Prescaler: clk/2
#define TC_CLKSEL_DIV4          0x3 //!< Prescaler: clk/4
#define TC_CLKSEL_DIV8          0x4 //!< Prescaler: clk/8
#define TC_CLKSEL_DIV64         0x5 //!< Prescaler: clk/64
#define TC_CLKSEL_DIV256        0x6 //!< Prescaler: clk/256
#define TC_CLKSEL_DIV1024       0x7 //!< Prescaler: clk/1024
#define TC_CLKSEL_EVCHAN0       0x8 //!< Event Channel 0
#define TC_CLKSEL_EVCHAN1       0x9 //!< Event Channel 1
#define TC_CLKSEL_EVCHAN2       0xa //!< Event Channel 2
#define TC_CLKSEL_EVCHAN3       0xb //!< Event Channel 3
#define TC_CLKSEL_EVCHAN4       0xc //!< Event Channel 4
#define TC_CLKSEL_EVCHAN5       0xd //!< Event Channel 5
#define TC_CLKSEL_EVCHAN6       0xe //!< Event Channel 6
#define TC_CLKSEL_EVCHAN7       0xf //!< Event Channel 7
//@}

//! \name CTRLB_WGMODE Bitfield Values
//@{
#define TC_WGMODE_NORMAL        0x0 //!< Normal
#define TC_WGMODE_FRQ           0x1 //!< Frequency
#define TC_WGMODE_SS            0x3 //!< Single Slope PWM
#define TC_WGMODE_DS_T          0x5 //!< Dual Slope PWM, Event on TOP
#define TC_WGMODE_DS_TB         0x6 //!< Dual Slope PWM, Event on TOP and BOTTOM
#define TC_WGMODE_DS_B          0x7 //!< Dual Slope PWM, Event on BOTTOM
//@}

//! \name CTRLFCLR_CMD, CTRLFSET_CMD Bitfield Values
//@{
#define TC_CMD_NONE             0x0 //!< None
#define TC_CMD_UPDATE           0x1 //!< Force Update
#define TC_CMD_RESTART          0x2 //!< Force Restart
#define TC_CMD_RESET            0x3 //!< Force Hard Reset
//@}

//! \name CTRLD_EVACT Bitfield Values
//@{
#define TC_EVACT_OFF            0x0 //!< None
#define TC_EVACT_CAPT           0x1 //!< Input Capture
#define TC_EVACT_UPDOWN         0x2 //!< Externally Controlled Up/Down Count
#define TC_EVACT_QDEC           0x3 //!< Quadrature Decode
#define TC_EVACT_RESTART        0x4 //!< Restart Waveform Periode
#define TC_EVACT_FRQ            0x5 //!< Frequency Capture
#define TC_EVACT_PW             0x6 //!< Pulse Width Capture
//@}

//! \name CTRLD_EVSEL Bitfield Values
//@{
#define TC_EVSEL_OFF            0x0 //!< Off
#define TC_EVSEL_CH0            0x8 //!< Channel 0
#define TC_EVSEL_CH1            0x9 //!< Channel 1
#define TC_EVSEL_CH2            0xa //!< Channel 2
#define TC_EVSEL_CH3            0xb //!< Channel 3
#define TC_EVSEL_CH4            0xc //!< Channel 4
#define TC_EVSEL_CH5            0xd //!< Channel 5
#define TC_EVSEL_CH6            0xe //!< Channel 6
#define TC_EVSEL_CH7            0xf //!< Channel 7
//@}

//! \name Bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set
#define TC_BIT(name)							\
	(1U << TC_##name##_BIT)
//! \brief Create a mask with bitfield \a name set to \a value
#define TC_BF(name, value)						\
	((value) << TC_##name##_START)
//! \brief Create a mask of bitfield with \a name
#define TC_BFMASK(name)							\
	(((1U << TC_##name##_SIZE) - 1) << TC_##name##_START)
//! \brief Extract the value of bitfield \a name from \a regval
#define TC_BFEXT(name, regval)						\
	(((regval) >> TC_##name##_START)				\
		& ((1U << TC_##name##_SIZE) - 1))
//! \brief Return \a regval with bitfield \a name set to \a value
#define TC_BFINS(name, value, regval)					\
	(((regval) & ~(((1U << TC_##name##_SIZE) - 1)			\
			<< TC_##name##_START))				\
		| TC_BF(name, value))
//@}

//! \name Register access macros
//@{
//! \brief Write \a value to 8-bit TC register \a reg
#define tc_write_reg8(tc, reg, value) \
	mmio_write8((void *) ((uintptr_t)(tc) + TC_##reg), value)
//! \brief Read the value of 8-bit TC register \a reg
#define tc_read_reg8(tc, reg) \
	mmio_read8((void *) ((uintptr_t)(tc) + TC_##reg))
//! \brief Write \a value to 16-bit TC register \a reg
#define tc_write_reg16(tc, reg, value) \
	mmio_write16((void *) ((uintptr_t)(tc) + TC_##reg##L), value)
//! \brief Read the value of 16-bit TC register \a reg
#define tc_read_reg16(tc, reg) \
	mmio_read16((void *) ((uintptr_t)(tc) + TC_##reg##L))
//@}

//! @}
#endif /* REGS_XMEGA_TC_H_INCLUDED */

/**
 * \file
 *
 * \brief AVR32 Static Memory Controller Register Definitions
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
#ifndef REGS_SMC_H_INCLUDED
#define REGS_SMC_H_INCLUDED

/**
 * \ingroup regs_group
 * \defgroup smc_regs_group Static Memory Controller Register Definitions
 *
 * @{
 */

//! \name Register Offsets
//! These are relative to the start address of each chip select
//@{
#define SMC_SETUP                       0x0000  //!< SMC Setup Register
#define SMC_PULSE                       0x0004  //!< SMC Pulse Register
#define SMC_CYCLE                       0x0008  //!< SMC Cycle Register
#define SMC_MODE                        0x000c  //!< SMC Mode Register
//@}

//! \name Bitfields in SETUP
//@{
#define SMC_NWE_SETUP_START      0       //!< nwe setup length
#define SMC_NWE_SETUP_SIZE       6       //!< nwe setup length
#define SMC_NCS_WR_SETUP_START   8       //!< ncs setup length in WRITE access
#define SMC_NCS_WR_SETUP_SIZE    6       //!< ncs setup length in WRITE access
#define SMC_NRD_SETUP_START      16      //!< nrd setup length
#define SMC_NRD_SETUP_SIZE       6       //!< nrd setup length
#define SMC_NCS_RD_SETUP_START   24      //!< ncs setup length in READ access
#define SMC_NCS_RD_SETUP_SIZE    6       //!< ncs setup length in READ access
//@}

//! \name Bitfields in PULSE
//@{
#define SMC_NWE_PULSE_START      0       //!< nwe pulse length
#define SMC_NWE_PULSE_SIZE       7       //!< nwe pulse length
#define SMC_NCS_WR_PULSE_START   8       //!< ncs pulse length in WRITE access
#define SMC_NCS_WR_PULSE_SIZE    7       //!< ncs pulse length in WRITE access
#define SMC_NRD_PULSE_START      16      //!< nrd pulse length
#define SMC_NRD_PULSE_SIZE       7       //!< nrd pulse length
#define SMC_NCS_RD_PULSE_START   24      //!< ncs pulse length in READ access
#define SMC_NCS_RD_PULSE_SIZE    7       //!< ncs pulse length in READ access
//@}

//! \name Bitfields in CYCLE
//@{
#define SMC_NWE_CYCLE_START      0       //!< total write cycle length
#define SMC_NWE_CYCLE_SIZE       9       //!< total write cycle length
#define SMC_NRD_CYCLE_START      16      //!< total read cycle length
#define SMC_NRD_CYCLE_SIZE       9       //!< total read cycle length
//@}

//! \name Bitfields in MODE
//@{
#define SMC_READ_MODE_BIT          0       //!< Read Mode
#define SMC_WRITE_MODE_BIT         1       //!< Write Mode
#define SMC_EXNW_MODE_START        4       //!< External Wait Mode
#define SMC_EXNW_MODE_SIZE         2       //!< External Wait Mode
#define SMC_BAT_BIT                8       //!< Byte Access Type
#define SMC_DBW_START              12      //!< Data Bus Width
#define SMC_DBW_SIZE               2       //!< Data Bus Width
#define SMC_TDF_CYCLES_START       16      //!< Data Float Time
#define SMC_TDF_CYCLES_SIZE        4       //!< Data Float Time
#define SMC_TDF_MODE_BIT           20      //!< TDF optimisation
#define SMC_PMEN_BIT               24      //!< Page Mode enabled
#define SMC_PS_START               28      //!< Page Size
#define SMC_PS_SIZE                2       //!< Page Size
//@}

//! \name MODE0_EXNW_MODE Bitfield Values
//@{
#define SMC_EXNW_MODE_DISABLED          0       //!< 00 Disabled
#define SMC_EXNW_MODE_RESERVED          1       //!< 01 Reserved
#define SMC_EXNW_MODE_FROZEN            2       //!< 10 Frozen Mode
#define SMC_EXNW_MODE_READY             3       //!< 11 Ready Mode
//@}

//! \name MODE0_DBW Bitfield Values
//@{
#define SMC_DBW_8_BITS                  0       //!< 8-bit bus
#define SMC_DBW_16_BITS         1       //!< 16-bit bus
#define SMC_DBW_32_BITS         2       //!< 32-bit bus
//@}

//! \name Bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set
#define SMC_BIT(name)                                                   \
	(1U << SMC_##name##_BIT)
//! \brief Create a mask with bitfield \a name set to \a value
#define SMC_BF(name, value)                                             \
	((value) << SMC_##name##_START)
//! \brief Create a mask of the bitfield \a name
#define SMC_BFMASK(name)                                                \
	(((1U << %s_##name##_SIZE) - 1) << %s_##name##_START)
//! \brief Extract the value of bitfield \a name from \a regval
#define SMC_BFEXT(name, regval)                                         \
	(((regval) >> SMC_##name##_START)                               \
		& ((1U << SMC_##name##_SIZE) - 1))
//! \brief Return \a regval with bitfield \a name set to \a value
#define SMC_BFINS(name, value, regval)                                  \
	(((regval) & ~(((1U << SMC_##name##_SIZE) - 1)                  \
			<< SMC_##name##_START))                         \
		| SMC_BF(name, value))
//@}

//! \name Register access macros
//@{
//! \brief Read the value of SMC register \a reg
#define smc_read_reg(base, reg)                                         \
	mmio_read32((void *)((uintptr_t)(base) + SMC_##reg))
//! \brief Write \a value to SMC register \a reg
#define smc_write_reg(base, reg, value)                                 \
	mmio_write32((void *)((uintptr_t)(base) + SMC_##reg), (value))
//@}

//! @}

#endif /* REGS_SMC_H_INCLUDED */

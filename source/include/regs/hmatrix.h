/**
 * \file
 *
 * \brief High-speed Bus Matrix register definitions
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
#ifndef REGS_HMATRIX_H_INCLUDED
#define REGS_HMATRIX_H_INCLUDED

/**
 * \ingroup regs_group
 * \defgroup hmatrix_regs_group HMATRIX Register Definitions
 *
 * This is the register interface to the HSB Bus Matrix present on all
 * current AVR32 devices. The meaning of many bitfields, as well as the
 * master and slave interface numbers, are chip-specific and defined in
 * chip/hsb.h.
 *
 * This file deviates a bit from the rules laid out in \ref
 * regs_group_conventions. Instead of having constants defining the
 * offset of each register relative to the base address and common read
 * and write macros, the HMATRIX has one read and one write macro for
 * each register type. This makes it easier to access e.g. MCFG8 without
 * requiring the master ID (8 in this example) to be a literal.
 *
 * @{
 */

//! \name Register Access Macros
//@{
//! Read the value of Master Configuration Register \a x
#define hmatrix_read_mcfg(x)                                    \
        mmio_read32((void *)(HMATRIX_BASE + 0x0000 + (x) * 4))
//! Write \a value to Master Configuration Register \a x
#define hmatrix_write_mcfg(x, value)                            \
        mmio_write32((void *)(HMATRIX_BASE + 0x0000 + (x) * 4), value)
//! Read the value of Slave Configuration Register \a x
#define hmatrix_read_scfg(x)                                    \
        mmio_read32((void *)(HMATRIX_BASE + 0x0040 + (x) * 4))
//! Write \a value to Slave Configuration Register \a x
#define hmatrix_write_scfg(x, value)                            \
        mmio_write32((void *)(HMATRIX_BASE + 0x0040 + (x) * 4), value)
//! Read the value of Priority Register A (master 0..7) of HSB slave \a x
#define hmatrix_read_pras(x)                                    \
        mmio_read32((void *)(HMATRIX_BASE + 0x0080 + (x) * 8))
//! Write \a value to Priority Register A (master 0..7) of HSB slave \a x
#define hmatrix_write_pras(x, value)                            \
        mmio_write32((void *)(HMATRIX_BASE + 0x0080 + (x) * 8), value)
//! Read the value of Priority Register B (master 8..15) of HSB slave \a x
#define hmatrix_read_prbs(x)                                    \
        mmio_read32((void *)(HMATRIX_BASE + 0x0084 + (x) * 8))
//! Write \a value to Priority Register B (master 8..15) of HSB slave \a x
#define hmatrix_write_prbs(x, value)                            \
        mmio_write32((void *)(HMATRIX_BASE + 0x0084 + (x) * 8), value)
//! Read the value of the Master Remap Control Register
#define hmatrix_read_mrcr()                                     \
        mmio_read32((void *)(HMATRIX_BASE + 0x0100))
//! Write \a value to the Master Remap Control Register
#define hmatrix_write_mrcr(value)                               \
        mmio_write32((void *)(HMATRIX_BASE + 0x0100), value)
//! Read the value of the Special Function Register of HSB slave \a x
#define hmatrix_read_sfr(x)                                     \
        mmio_read32((void *)(HMATRIX_BASE + 0x0110 + (x) * 4))
//! Write \a value to the Special Function Register of HSB slave \a x
#define hmatrix_write_sfr(x, value)                             \
        mmio_write32((void *)(HMATRIX_BASE + 0x0110 + (x) * 4), value)
//@}

//! \name Bitfields in MCFG
//@{
#define HMATRIX_MCFG_ULBT_START         0       //!< Undef Length Burst Type
#define HMATRIX_MCFG_ULBT_SIZE          3       //!< Undef Length Burst Type
//@}

//! \name Bitfields in SCFG
//@{
//! Maximum Number of Allowed Cycles for a Burst
#define HMATRIX_SCFG_SLOT_CYCLE_START   0
//! Maximum Number of Allowed Cycles for a Burst
#define HMATRIX_SCFG_SLOT_CYCLE_SIZE    8
#define HMATRIX_SCFG_DEFMSTR_TYPE_START 16      //!< Default Master Type
#define HMATRIX_SCFG_DEFMSTR_TYPE_SIZE  2       //!< Default Master Type
#define HMATRIX_SCFG_FIXED_DEFMSTR_START 18     //!< Fixed Default Master ID
#define HMATRIX_SCFG_FIXED_DEFMSTR_SIZE  4      //!< Fixed Default Master ID
#define HMATRIX_SCFG_ARBT_START         24      //!< Arbitration Type
#define HMATRIX_SCFG_ARBT_SIZE          1       //!< Arbitration Type
//@}

/**
 * \name Bitfields in PRAS and PRBS
 * These definitions differ a bit from other bitfield definitions in
 * that they don't follow the usual START/SIZE convention. This is
 * because in this particular case, the regular convention leads to
 * definitions which are not very useful for the HSB configuration code.
 */
//@{
//! Set priority of master \a x to \a pri for (0 <= x <= 7)
#define HMATRIX_PRAS_MPR(x, pri)        ((pri) << ((x) * 4))
//! Bitmask corresponding to master \a x for (0 <= x <= 7)
#define HMATRIX_PRAS_MPR_MASK(x)        (0xf << ((x) * 4))
//! Set priority of master \a x to \a pri for (8 <= x <= 15)
#define HMATRIX_PRBS_MPR(x, pri)        ((pri) << (((x) - 8) * 4))
//! Bitmask corresponding to master \a x for (8 <= x <= 15)
#define HMATRIX_PRBS_MPR_MASK(x)        (0xf << (((x) - 8) * 4))
//@}

//! \name MCFG_ULBT Bitfield Values
//@{
#define HMATRIX_ULBT_INF                0       //!< Infinite
#define HMATRIX_ULBT_1                  1       //!< Single Access
#define HMATRIX_ULBT_4                  2       //!< 4-beat Burst
#define HMATRIX_ULBT_8                  3       //!< 8-beat Burst
#define HMATRIX_ULBT_16                 4       //!< 16-beat Burst
//@}

//! \name SCFG_DEFMSTR_TYPE Bitfield Values
//@{
#define HMATRIX_DEFMSTR_TYPE_NONE       0       //!< No Default Master
#define HMATRIX_DEFMSTR_TYPE_LAST       1       //!< Last Default Master
#define HMATRIX_DEFMSTR_TYPE_FIXED      2       //!< Fixed Default Master
//@}

//! \name SCFG_ARBT Bitfield Values
//@{
#define HMATRIX_ARBT_ROUND_ROBIN        0       //!< Round-Robin Arbitration
#define HMATRIX_ARBT_FIXED_PRIO         1       //!< Fixed Priority Arbitration
//@}

//! \name Bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set.
#define HMATRIX_BIT(name)                                       \
	(1U << HMATRIX_##name##_BIT)
//! \brief Create a mask with bitfield \a name set to \a value.
#define HMATRIX_BF(name, value)                                 \
	((value) << HMATRIX_##name##_START)
//! \brief Create a mask of the bitfield \a name.
#define HMATRIX_BFMASK(name)                                    \
	HMATRIX_BF(name, (1U << HMATRIX_##name##_SIZE) - 1)
//! \brief Extract the value of bitfield \a name from \a regval.
#define HMATRIX_BFEXT(name, regval)                             \
	(((regval) >> HMATRIX_##name##_START)                   \
		& ((1U << HMATRIX_##name##_SIZE) - 1))
//! \brief Return \a regval with bitfield \a name set to \a value.
#define HMATRIX_BFINS(name, value, regval)                      \
	(((regval) & ~(((1U << HMATRIX_##name##_SIZE) - 1)      \
			<< HMATRIX_##name##_START))             \
		| HMATRIX_BF(name, value))
//@}

//! @}

#endif /* REGS_HMATRIX_H_INCLUDED */

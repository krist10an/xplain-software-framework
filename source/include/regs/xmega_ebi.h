/**
 * \file
 *
 * \brief XMEGA EBI Register Interface.
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

#ifndef REGS_XMEGA_EBI_H_INCLUDED
#define REGS_XMEGA_EBI_H_INCLUDED

#include <chip/memory-map.h>
#include <io.h>

/**
 * \ingroup regs_group
 * \defgroup ebi_regs_group EBI Register Definitions
 * @{
 */

//! \name Register offsets
//@{
#define EBI_CTRL                0x00  //!< Control register
#define EBI_SDRAMCTRLA          0x01  //!< SDRAM control register A
#define EBI_SDRAMCTRLB          0x08  //!< SDRAM control register B
#define EBI_SDRAMCTRLC          0x09  //!< SDRAM control register C
#define EBI_REFRESHL            0x04  //!< SDRAM refresh period low byte
#define EBI_REFRESHH            0x05  //!< SDRAM refresh period high byte
#define EBI_INITDLYL            0x06  //!< SDRAM init. delay low byte
#define EBI_INITDLYH            0x07  //!< SDRAM init. delay high byte
//@}

//! \name Bitfields in CTRL
//@{
#define EBI_IFMODE_START        0  //!< EBI mode
#define EBI_IFMODE_SIZE         2  //!< EBI mode
#define EBI_SRMODE_START        2  //!< SRAM mode
#define EBI_SRMODE_SIZE         2  //!< SRAM mode
#define EBI_LPCMODE_START       4  //!< SRAM Low Pin-Count mode
#define EBI_LPCMODE_SIZE        2  //!< SRAM Low Pin-Count mode
#define EBI_SDDATAW_START       6  //!< SDRAM data width setting
#define EBI_SDDATAW_SIZE        2  //!< SDRAM data width setting
//@}

//! \name IFMODE bitfield values
//@{
#define EBI_IFMODE_DISABLED     0  //!< EBI disabled
#define EBI_IFMODE_3PORT        1  //!< EBI enabled w/ 3-port interface
#define EBI_IFMODE_4PORT        2  //!< EBI enabled w/ 4-port interface
#define EBI_IFMODE_2PORT        3  //!< EBI enabled w/ 2-port interface
//@}

//! \name SRMODE bitfield values
//@{
#define EBI_SRMODE_ALE1         0  //!< Address bytes 0 and 1 multiplexed
#define EBI_SRMODE_ALE2         1  //!< Address bytes 0 and 2 multiplexed
#define EBI_SRMODE_ALE12        2  //!< Address bytes 0, 1 and 2 multiplexed
#define EBI_SRMODE_NOALE        3  //!< No address multiplexing
//@}

//! \name LPCMODE bitfield values
//@{
//! Data multiplexed with address byte 0
#define EBI_LPCMODE_ALE1        0
//! Data multiplexed with address bytes 0 and 1
#define EBI_LPCMODE_ALE12       2
//@}

//! \name SDDATAW bitfield values
//@{
#define EBI_SDDATAW_4BIT        0  //!< 4-bit data bus
#define EBI_SDDATAW_8BIT        1  //!< 8-bit data bus
//@}


//! \name Bitfields in SDRAMCTRLA
//@{
#define EBI_SDCOL_START         0  //!< SDRAM column bits
#define EBI_SDCOL_SIZE          2  //!< SDRAM column bits
#define EBI_SDROW_BIT           2  //!< SDRAM row bits (11/12 bits)
#define EBI_SDCAS_BIT           3  //!< SDRAM CAS latency (2/3 cycles)
//@}

//! \name SDCOL bitfield values
//@{
#define EBI_SDCOL_8BIT          0  //!< 8 column bits
#define EBI_SDCOL_9BIT          1  //!< 9 column bits
#define EBI_SDCOL_10BIT         2  //!< 10 column bits
#define EBI_SDCOL_11BIT         3  //!< 11 column bits
//@}


//! \name Bitfields in SDRAMCTRLB
//@{
//! SDRAM row to precharge delay in number of CLKper2 cycles (0-7)
#define EBI_RPDLY_START         0
//! SDRAM row to precharge delay in number of CLKper2 cycles (0-7)
#define EBI_RPDLY_SIZE          3
//! SDRAM row cycle delay in number of CLKper2 cycles (0-7)
#define EBI_ROWCYCDLY_START     3
//! SDRAM row cycle delay in number of CLKper2 cycles (0-7)
#define EBI_ROWCYCDLY_SIZE      3
//! SDRAM mode register delay in number of CLKper2 cycles (0-3)
#define EBI_MRDLY_START         6
//! SDRAM mode register delay in number of CLKper2 cycles (0-3)
#define EBI_MRDLY_SIZE          2
//@}

//! \name Bitfields in SDRAMCTRLC
//@{
//! SDRAM row to column delay in number of CLKper2 cycles (0-7)
#define EBI_ROWCOLDLY_START     0
//! SDRAM row to column delay in number of CLKper2 cycles (0-7)
#define EBI_ROWCOLDLY_SIZE      3
//! SDRAM exit self refresh to active delay in number of CLKper2 cycles (0-7)
#define EBI_ESRDLY_START        3
//! SDRAM exit self refresh to active delay in number of CLKper2 cycles (0-7)
#define EBI_ESRDLY_SIZE         3
//! SDRAM write recovery delay in number of CLKper2 cycles (0-3)
#define EBI_WRDLY_START         6
//! SDRAM write recovery delay in number of CLKper2 cycles (0-3)
#define EBI_WRDLY_SIZE          2
//@}

//! \name Bitfields in REFRESH
//@{
//! SDRAM refresh period in number of CLKper2 cycles
#define EBI_REFRESH_START      0
//! SDRAM refresh period in number of CLKper2 cycles
#define EBI_REFRESH_SIZE       10
//@}

//! \name Bitfields in REFRESHL
//@{
//! SDRAM refresh period in number of CLKper2 cycles (LSB)
#define EBI_REFRESHL_START      0
//! SDRAM refresh period in number of CLKper2 cycles (LSB)
#define EBI_REFRESHL_SIZE       8
//@}

//! \name Bitfields in REFRESHH
//@{
//! SDRAM refresh period in number of CLKper2 cycles (MSB)
#define EBI_REFRESHH_START      0
//! SDRAM refresh period in number of CLKper2 cycles (MSB)
#define EBI_REFRESHH_SIZE       2
//@}

//! \name Bitfields in INITDLY
//@{
//! SDRAM initialization delay in number of CLKper2 cycles
#define EBI_INITDLY_START        0
//! SDRAM initialization delay in number of CLKper2 cycles
#define EBI_INITDLY_SIZE         14
//@}

//! \name Bitfields in INITDLYL
//@{
//! SDRAM initialization delay in number of CLKper2 cycles (LSB)
#define EBI_INITDLYL_START       0
//! SDRAM initialization delay in number of CLKper2 cycles (LSB)
#define EBI_INITDLYL_SIZE        8
//@}

//! \name Bitfields in INITDLYH
//@{
//! SDRAM initialization delay in number of CLKper2 cycles (MSB)
#define EBI_INITDLYH_START       0
//! SDRAM initialization delay in number of CLKper2 cycles (MSB)
#define EBI_INITDLYH_SIZE        6
//@}

//! \name Bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set.
#define EBI_BIT(name) \
	(1U << EBI_##name##_BIT)
//! \brief Create a mask with bitfield \a name set to \a value.
#define EBI_BF(name, value) \
	((value) << EBI_##name##_START)
//! \brief Create a mask of the bitfield \a name.
#define EBI_BFMASK(name) \
	(((1U << EBI_##name##_SIZE) - 1) << EBI_##name##_START)
//! \brief Extract the value of bitfield \a name from \a regval.
#define EBI_BFEXT(name, regval) \
	(((regval) >> EBI_##name##_START) \
		& ((1U << EBI_##name##_SIZE) - 1))
//! \brief Return \a regval with bitfield \a name set to \a value.
#define EBI_BFINS(name, value, regval) \
	(((regval) & ~(((1U << EBI_##name##_SIZE) - 1) \
			<< EBI_##name##_START)) \
	| EBI_BF(name, value))
//@}

//! \name Register access macros
//@{
//! \brief Read the value of EBI register \a reg.
#define ebi_read_reg(reg) \
	mmio_read8((void *)((uintptr_t)(EBI_BASE) + EBI_##reg))
//! \brief Write \a value to EBI register \a reg.
#define ebi_write_reg(reg, value) \
	mmio_write8((void *)((uintptr_t)(EBI_BASE) + EBI_##reg), (value))
/**
 *  \brief Read the value of EBI word-register \a reg.
 *
 *  For use with REFRESH and INITDLY.
 */
#define ebi_read_word_reg(reg) \
	mmio_read16((void *)((uintptr_t)(EBI_BASE) + EBI_##reg##L))
/**
 * \brief Write \a value to EBI word-register \a reg.
 *
 * For use with REFRESH and INITDLY.
 */
#define ebi_write_word_reg(reg, value) \
	mmio_write16((void *)((uintptr_t)(EBI_BASE) + EBI_##reg##L), (value))
//@}

//@}


/**
 * \ingroup regs_group
 * \defgroup ebics_regs_group EBI Chip Select Register Definitions
 * @{
 */

//! \name Channel register group offsets
//{@
#define EBICS_CS0             0x10  //!< EBI Chip Select 0
#define EBICS_CS1             0x14  //!< EBI Chip Select 1
#define EBICS_CS2             0x18  //!< EBI Chip Select 2
#define EBICS_CS3             0x1c  //!< EBI Chip Select 3
//@}

//! \name Chip Select register offsets
//@{
#define EBICS_CTRLA           0x00  //!< Control register A
#define EBICS_CTRLB           0x01  //!< Control register B
#define EBICS_BASEADDRL       0x02  //!< Base address low byte
#define EBICS_BASEADDRH       0x03  //!< Base address high byte
//@}


//! \name Bitfields in channel CTRLA
//@{
//! CS mode
#define EBICS_MODE_START      0
//! CS mode
#define EBICS_MODE_SIZE       2
//! CS address size in number of bits, minus 8
#define EBICS_ASIZE_START     2
//! CS address size in number of bits, minus 8
#define EBICS_ASIZE_SIZE      5
//@}

//! \name MODE bitfield values
//@{
#define EBICS_MODE_DISABLE    0  //!< CS disabled
#define EBICS_MODE_SRAM       1  //!< CS enabled for SRAM
#define EBICS_MODE_LPC        2  //!< CS enabled for low pin-count SRAM
#define EBICS_MODE_SDRAM      3  //!< CS enabled for SDRAM
//@}

//! \name Bitfields in channel CTRLB (SRAM)
//@{
//! SRAM wait states in number of CLKper2 cycles
#define EBICS_SRWS_START      0
//! SRAM wait states in number of CLKper2 cycles
#define EBICS_SRWS_SIZE       3
//@}

//! \name Bitfields in channel CTRLB (SDRAM)
//@{
#define EBICS_SDMODE_START    0  //!< SDRAM mode
#define EBICS_SDMODE_SIZE     2  //!< SDRAM mode
#define EBICS_SDSREN_BIT      2  //!< SDRAM self-refresh enable
#define EBICS_SDINITDONE_BIT  7  //!< SDRAM initialization done
//@}

//! \name SDMODE bitfield values
//@{
#define EBICS_SDMODE_NORMAL   0  //!< Normal mode
#define EBICS_SDMODE_LOAD     1  //!< Load mode
//@}

//! \name Bitfields in BASEADDR
//@{
//! CS base address in number of 4 kB pages
#define EBICS_BASEADDR_START  4
//! CS base address in number of 4 kB pages
#define EBICS_BASEADDR_SIZE   12
//@}

//! \name Bitfields in BASEADDRL
//@{
//! CS base address in number of 4 kB pages (LSB)
#define EBICS_BASEADDRL_START  4
//! CS base address in number of 4 kB pages (LSB)
#define EBICS_BASEADDRL_SIZE   4
//@}

//! \name Bitfields in BASEADDRH
//@{
//! CS base address in number of 4 kB pages (MSB)
#define EBICS_BASEADDRH_START  0
//! CS base address in number of 4 kB pages (MSB)
#define EBICS_BASEADDRH_SIZE   8
//@}

//! \name Chip Select bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set.
#define EBICS_BIT(name) \
	(1U << EBICS_##name##_BIT)
//! \brief Create a mask with bitfield \a name set to \a value.
#define EBICS_BF(name, value) \
	((value) << EBICS_##name##_START)
//! \brief Create a mask of the bitfield \a name.
#define EBICS_BFMASK(name) \
	(((1U << EBICS_##name##_SIZE) - 1) << EBICS_##name##_START)
//! \brief Extract the value of bitfield \a name from \a regval.
#define EBICS_BFEXT(name, regval) \
	(((regval) >> EBICS_##name##_START) \
		& ((1U << EBICS_##name##_SIZE) - 1))
//! \brief Return \a regval with bitfield \a name set to \a value.
#define EBICS_BFINS(name, value, regval) \
	(((regval) & ~(((1U << EBICS_##name##_SIZE) - 1) \
			<< EBICS_##name##_START)) \
	| EBICS_BF(name, value))
//@}

//! \name Chip Select line register access macros
//@{
//! \brief Read value of EBI register \a reg for CS \a cs.
#define ebics_read_reg(cs, reg) \
	mmio_read8((void *)((uintptr_t)(EBI_BASE) + EBICS_##cs + EBICS_##reg))
//! \brief Write \a value to EBI register \a reg for CS \a cs.
#define ebics_write_reg(cs, reg, value) \
	mmio_write8((void *)((uintptr_t)(EBI_BASE) + EBICS_##cs \
			+ EBICS_##reg), (value))
/**
 * \brief Read value of EBI word-register \a reg for CS \a cs.
 *
 * For use with BASEADDR.
 */
#define ebics_read_word_reg(cs, reg) \
	mmio_read16((void *)((uintptr_t)(EBI_BASE) + EBICS_##cs \
			+ EBICS_##reg##L))
/**
 * \brief Write \a value to EBI word-register \a reg for CS \a cs.
 *
 * For use with BASEADDR.
 */
#define ebics_write_word_reg(cs, reg, value) \
	mmio_write16((void *)((uintptr_t)(EBI_BASE) + EBICS_##cs \
			+ EBICS_##reg##L), (value))
//@}

//@}

#endif /* REGS_XMEGA_EBI_H_INCLUDED */

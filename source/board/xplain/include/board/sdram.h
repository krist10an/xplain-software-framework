/**
 * \file
 *
 * \brief Board specific external RAM control.
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
#ifndef BOARD_SDRAM_H_INCLUDED
#define BOARD_SDRAM_H_INCLUDED

#include <util.h>
#include <interrupt.h>

#include <clk/sys.h>

#include <regs/xmega_ebi.h>

/**
 * \internal
 * \defgroup board_ram_group Board RAM control
 *
 * This module contains functions for initalizing the EBI for use with the
 * Xplain's onboard SDRAM, as well as enabling/disabling SDRAM self-refresh
 * mode upon XMEGA sleep/wake-up.
 *
 * The initialization assumes that the symbol \ref CONFIG_CPU_HZ is half of
 * what CLKper2 is, i.e., the CPU clock is prescaled to half of CLKper2.
 *
 * Further, the initialization requires \ref CONFIG_EXTRAM_BASE and \ref
 * CONFIG_EXTRAM_END to be defined, which are also needed by the application
 * for addressing the hugemem section (external RAM).
 *
 * \note If \ref CONFIG_EXTRAM_SDRAM is defined, the initialization is done
 * in \ref board_init().
 *
 * \todo Remove dependency on \ref CONFIG_CPU_HZ, implement sysclk_get_xxx_hz()
 *
 * \todo Revisit when EBI driver for XMEGA is ready and this can be moved into
 * include/bus/ebi/params.
 *
 * @{
 */

//! SDRAM initialization delay in number of CLKper2 cycles (100 us)
#define BOARD_SDRAM_INITDLY     (100 * 2 * CONFIG_CPU_HZ / 1000000)

//! SDRAM refresh interval in number of CLKper2 cycles (16 us)
#define BOARD_SDRAM_REFRESH     (16 * 2 * CONFIG_CPU_HZ / 1000000)

#define BOARD_SDRAM_BASE        CONFIG_EXTRAM_BASE

#define BOARD_SDRAM_SIZE        (CONFIG_EXTRAM_END - CONFIG_EXTRAM_BASE + 1)

/**
 * \brief Configure the EBI for the 8 MB onboard SDRAM
 */
static inline void board_enable_sdram(void)
{
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EBI);

	// Configure SDRAM mode (4 bit data width, 3 port interface).
	ebi_write_reg(CTRL, EBI_BF(SDDATAW, EBI_SDDATAW_4BIT)
			| EBI_BF(IFMODE, EBI_IFMODE_3PORT));

	// Configure SDRAM interface and timing.
	ebi_write_reg(SDRAMCTRLA, EBI_BIT(SDCAS) | EBI_BIT(SDROW)
			| EBI_BF(SDCOL, EBI_SDCOL_10BIT));
	ebi_write_reg(SDRAMCTRLB, EBI_BF(MRDLY, 2) | EBI_BF(ROWCYCDLY, 7)
			| EBI_BF(RPDLY, 7));
	ebi_write_reg(SDRAMCTRLC, EBI_BF(WRDLY, 1) | EBI_BF(ESRDLY, 7)
			| EBI_BF(ROWCOLDLY, 7));
	ebi_write_word_reg(REFRESH, EBI_BF(REFRESH, BOARD_SDRAM_REFRESH));
	ebi_write_word_reg(INITDLY, EBI_BF(INITDLY, BOARD_SDRAM_INITDLY));

	// Configure CS module 3 in normal SDRAM mode.
	ebics_write_reg(CS3, CTRLB, EBICS_BF(SDMODE, EBICS_SDMODE_NORMAL));
	ebics_write_word_reg(CS3, BASEADDR, EBICS_BF(BASEADDR,
			(BOARD_SDRAM_BASE >> 12)));
	ebics_write_reg(CS3, CTRLA, EBICS_BF(ASIZE,
			ilog2(BOARD_SDRAM_SIZE) - 8)
			| EBICS_BF(MODE, EBICS_MODE_SDRAM));

	do {
		// Wait for SDRAM to initialize.
	} while (!(ebics_read_reg(CS3, CTRLB) & EBICS_BIT(SDINITDONE)));
}

/**
 * \brief Disable the SDRAM by disabling the EBI
 */
static inline void board_disable_sdram(void)
{
	// Disable CS module.
	ebics_write_reg(CS3, CTRLA, EBICS_BF(MODE, EBICS_MODE_DISABLE));

	// Disable EBI.
	ebi_write_reg(CTRL, EBI_BF(IFMODE, EBI_IFMODE_DISABLED));

	sysclk_disable_module(SYSCLK_PORT_GEN, SYSCLK_EBI);
}

/**
 * \brief Enable self-refresh mode for SDRAM
 *
 * Put SDRAM in self-refresh mode, leaving it inaccessible until this mode is
 * disabled again. This is necessary for retaining its contents when the XMEGA
 * is put in sleep modes where the peripheral clock is disabled.
 */
static inline void board_enable_sdram_selfrefresh(void)
{
	irqflags_t      flags;
	uint8_t         val;

	flags = cpu_irq_save();

	val = ebics_read_reg(CS3, CTRLB);
	val |= EBICS_BIT(SDSREN);
	ebics_write_reg(CS3, CTRLB, val);

	cpu_irq_restore(flags);
}

/**
 * \brief Disable self-refresh mode for SDRAM
 *
 * Bring SDRAM out of self-refresh mode when the peripheral clock is enabled.
 * This is needed to regain access.
 */
static inline void board_disable_sdram_selfrefresh(void)
{
	irqflags_t      flags;
	uint8_t         val;

	flags = cpu_irq_save();

	val = ebics_read_reg(CS3, CTRLB);
	val &= ~EBICS_BIT(SDSREN);
	ebics_write_reg(CS3, CTRLB, val);

	cpu_irq_restore(flags);
}

//! @}

#endif /* BOARD_SDRAM_H_INCLUDED */

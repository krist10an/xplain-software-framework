/**
 * \file
 *
 * \brief External Static Memory Interface
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
#ifndef CHIP_EBI_SRAM_H_INCLUDED
#define CHIP_EBI_SRAM_H_INCLUDED

#include <assert.h>
#include <debug.h>
#include <io.h>
#include <chip/memory-map.h>
#include <clk/sys.h>
#include <regs/smc.h>

enum ebi_sram_bus_width {
	EBI_SRAM_8_BITS         = SMC_DBW_8_BITS,
	EBI_SRAM_16_BITS        = SMC_DBW_16_BITS,
	EBI_SRAM_32_BITS        = SMC_DBW_32_BITS,
};

struct ebi_sram_params {
	uint32_t setup;
	uint32_t pulse;
	uint32_t cycle;
	uint32_t mode;
};

static inline void *ebi_sram_cs_cfg_addr(unsigned int cs)
{
	return (void *)(SMC_BASE + cs * 16);
}

static inline void ebi_sram_enable_cfg_clock(void)
{
	sysclk_enable_pbb_module(SYSCLK_SMC_REGS);
}

static inline void ebi_sram_disable_cfg_clock(void)
{
	sysclk_disable_pbb_module(SYSCLK_SMC_REGS);
}

static inline uint32_t ebi_sram_get_bus_hz(void)
{
	return sysclk_get_hsb_hz();
}

static inline void ebi_sram_params_init_defaults(
		struct ebi_sram_params *params,
		unsigned int cs)
{
	params->setup = 0x00010001;
	params->pulse = 0x04030402;
	params->cycle = 0x00050005;
	params->mode = 0x00001103;
}

static inline void ebi_sram_params_read(
		struct ebi_sram_params *params,
		unsigned int cs)
{
	const uint32_t *addr = ebi_sram_cs_cfg_addr(cs);

	ebi_sram_enable_cfg_clock();
	params->setup = mmio_read32(addr++);
	params->pulse = mmio_read32(addr++);
	params->cycle = mmio_read32(addr++);
	params->mode = mmio_read32(addr++);
	ebi_sram_disable_cfg_clock();
}

static inline void ebi_sram_params_write(
		const struct ebi_sram_params *params,
		unsigned int cs)
{
	uint32_t *addr = ebi_sram_cs_cfg_addr(cs);

	dbg_verbose("smc[%u] @ %p: %08lx %08lx %08lx %08lx\n", cs, addr,
			params->setup, params->pulse,
			params->cycle, params->mode);

	ebi_sram_enable_cfg_clock();
	mmio_write32(addr++, params->setup);
	mmio_write32(addr++, params->pulse);
	mmio_write32(addr++, params->cycle);
	mmio_write32(addr++, params->mode);
	ebi_sram_disable_cfg_clock();
}

static inline void ebi_sram_set_setup_cycles(
		struct ebi_sram_params *params,
		uint8_t cs_rd_cycles, uint8_t oe_cycles,
		uint8_t cs_wr_cycles, uint8_t we_cycles)
{
	params->setup = SMC_BF(NCS_RD_SETUP, cs_rd_cycles)
		| SMC_BF(NRD_SETUP, oe_cycles)
		| SMC_BF(NCS_WR_SETUP, cs_wr_cycles)
		| SMC_BF(NWE_SETUP, we_cycles);
}

static inline void ebi_sram_set_pulse_cycles(
		struct ebi_sram_params *params,
		uint8_t cs_rd_cycles, uint8_t oe_cycles,
		uint8_t cs_wr_cycles, uint8_t we_cycles)
{
	params->pulse = SMC_BF(NCS_RD_PULSE, cs_rd_cycles)
		| SMC_BF(NRD_PULSE, oe_cycles)
		| SMC_BF(NCS_WR_PULSE, cs_wr_cycles)
		| SMC_BF(NWE_PULSE, we_cycles);
}

static inline void ebi_sram_set_total_cycles(
		struct ebi_sram_params *params,
		uint16_t read_cycles, uint16_t write_cycles)
{
	params->cycle = SMC_BF(NRD_CYCLE, read_cycles)
		| SMC_BF(NWE_CYCLE, write_cycles);
}

static inline void ebi_sram_set_bus_width(
		struct ebi_sram_params *params,
		enum ebi_sram_bus_width width,
		bool use_byte_sel)
{
	assert(width <= EBI_SRAM_16_BITS);

	params->mode = SMC_BFINS(DBW, width, params->mode);
	if (use_byte_sel || width == EBI_SRAM_8_BITS)
		params->mode &= ~SMC_BIT(BAT);
	else
		params->mode |= SMC_BIT(BAT);
}

#endif /* CHIP_EBI_SRAM_H_INCLUDED */

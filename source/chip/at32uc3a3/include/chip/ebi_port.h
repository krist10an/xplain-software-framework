/**
 * \file
 *
 * \brief External Bus Interface: Port/pin Configuration
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
#ifndef CHIP_EBI_PORT_H_INCLUDED
#define CHIP_EBI_PORT_H_INCLUDED

#include <bitops.h>
#include <chip/portmux.h>
#include <types.h>
#include <util.h>

/**
 * \weakgroup ebi_group
 * @{
 */

//! \name EBI Port Configuration Flags
//@{
#define EBI_PORT_SRAM   (1 <<  0)       //!< Enable SRAM interface
#define EBI_PORT_SDRAM  (1 <<  1)       //!< Enable SDRAM interface
#define EBI_PORT_NAND0  (1 <<  2)       //!< Enable NAND Flash interface 0
#define EBI_PORT_NAND1  (1 <<  3)       //!< Enable NAND Flash interface 1
#define EBI_PORT_CF0    (1 <<  4)       //!< Enable CompactFlash interface 0
#define EBI_PORT_CF1    (1 <<  5)       //!< Enable CompactFlash interface 1
#define EBI_PORT_NCS0   (1 <<  6)       //!< Enable Chip Select pin 0
#define EBI_PORT_NCS1   (1 <<  7)       //!< Enable Chip Select pin 1
#define EBI_PORT_NCS2   (1 <<  8)       //!< Enable Chip Select pin 2
#define EBI_PORT_NCS3   (1 <<  9)       //!< Enable Chip Select pin 3
#define EBI_PORT_NCS4   (1 << 10)       //!< Enable Chip Select pin 4
#define EBI_PORT_NCS5   (1 << 11)       //!< Enable Chip Select pin 5
#define EBI_PORT_NWAIT  (1 << 14)       //!< Enable NWAIT pin
//@}

/**
 * \brief Set up the I/O ports for use by the EBI.
 *
 * This function is meant to be called with constant parameters, in
 * which case it will generate a minimal amount of machine code.
 *
 * \param data_width Number of data lines to be used
 * \param addr_width Number of address lines to be used
 * \param flags A bitwise combination of the EBI Port Configuration Flags
 */
static inline void ebi_setup_port(unsigned int data_width,
		uint32_t addr_mask, uint32_t flags)
{
	uint32_t pin_mask;

	assert(!(addr_mask & 0xff000000));
	assert(data_width <= 16);

	/* Data lines on PORTB with buskeeper or pull-up enabled */
	pin_mask = 0x3fc00000;                  // DATA0..7
	if (data_width > 8)
		pin_mask |= 0x00380000;         // DATA8..10
	portmux_select_peripheral(PORTMUX_PORT_B, pin_mask,
			PORTMUX_FUNC_A, PORTMUX_BUSKEEPER);

	/* Control lines on PORTB: no buskeeper or pull-up */
	pin_mask = 0;
	if (flags & EBI_PORT_SRAM) {
		pin_mask |= 1U << 31;           // NWE0
		if (data_width > 8)
			pin_mask |= 1 << 30;    // NWE1
	}
	if (flags & (EBI_PORT_NCS4 | EBI_PORT_CF0))
		pin_mask |= 1 << 9;
	if (flags & (EBI_PORT_NCS5 | EBI_PORT_CF1))
		pin_mask |= 1 << 10;
	portmux_select_peripheral(PORTMUX_PORT_B, pin_mask,
			PORTMUX_FUNC_A, 0);

	/* Data lines on PORTC */
	if (data_width > 8)
		portmux_select_peripheral(PORTMUX_PORT_C, 0x07c00000,
				PORTMUX_FUNC_A, PORTMUX_BUSKEEPER);

	/* Control and address lines on PORTC */
	pin_mask = 0;
	if (flags & EBI_PORT_SRAM)
		pin_mask |= 1 << 0;             // NRD
	if (flags & (EBI_PORT_NCS1))
		pin_mask |= 1 << 1;             // NCS1
	if (addr_mask & (1 << 19))
		pin_mask |= 1 << 2;             // ADDR19
	if (addr_mask & (1 << 18))
		pin_mask |= 1 << 3;             // ADDR18
	if (addr_mask & (1 << 17))
		pin_mask |= 1 << 4;             // ADDR17
	if (addr_mask & (1 << 16))
		pin_mask |= 1 << 5;             // ADDR16
	if (addr_mask & (1 << 15))
		pin_mask |= 1 << 6;             // ADDR15
	if (addr_mask & (1 << 14))
		pin_mask |= 1 << 7;             // ADDR14
	if (addr_mask & (1 << 13))
		pin_mask |= 1 << 8;             // ADDR13
	if (addr_mask & (1 << 12))
		pin_mask |= 1 << 9;             // ADDR12
	if (addr_mask & (1 << 11))
		pin_mask |= 1 << 10;            // ADDR11
	if (addr_mask & (1 << 10))
		pin_mask |= 1 << 11;            // ADDR10
	if (addr_mask & (1 << 9))
		pin_mask |= 1 << 12;            // ADDR9
	if (addr_mask & (1 << 8))
		pin_mask |= 1 << 13;            // ADDR8
	if (addr_mask & (1 << 7))
		pin_mask |= 1 << 14;            // ADDR7
	if (addr_mask & (1 << 6))
		pin_mask |= 1 << 15;            // ADDR6
	if (addr_mask & (1 << 5))
		pin_mask |= 1 << 16;            // ADDR5
	if (addr_mask & (1 << 4))
		pin_mask |= 1 << 17;            // ADDR4
	if (addr_mask & (1 << 3))
		pin_mask |= 1 << 18;            // ADDR3
	if (addr_mask & (1 << 2))
		pin_mask |= 1 << 19;            // ADDR2
	if (addr_mask & (1 << 1))
		pin_mask |= 1 << 20;            // ADDR1
	if (addr_mask & (1 << 0))
		pin_mask |= 1 << 21;            // ADDR0
	if (flags & EBI_PORT_SDRAM)
		pin_mask |= 0xf8000000;         // SDCS/CAS/RAS/SDA10/SDWE
	portmux_select_peripheral(PORTMUX_PORT_C, pin_mask,
			PORTMUX_FUNC_A, 0);

	/* Control and address lines on PORTX0 */
	pin_mask = 0;
	if (flags & EBI_PORT_SDRAM)
		pin_mask |= 0x00000003;         // SDCK/SDCKE
	if (flags & (EBI_PORT_NAND0 | EBI_PORT_NAND1))
		pin_mask |= 0x00004004;         // NANDOE/NANDWE
	if (addr_mask & (1 << 23))
		pin_mask |= 1 << 3;             // ADDR23
	if (flags & (EBI_PORT_CF0 | EBI_PORT_CF1))
		pin_mask |= 0x00000070;         // CFRNW/CFCE2/CFCE1
	if (flags & (EBI_PORT_NCS3 | EBI_PORT_NAND1))
		pin_mask |= 1 << 7;             // NCS3
	if (flags & (EBI_PORT_NCS2 | EBI_PORT_NAND0))
		pin_mask |= 1 << 8;             // NCS2
	if (addr_mask & (1 << 22))
		pin_mask |= 1 << 10;            // ADDR22
	if (addr_mask & (1 << 21))
		pin_mask |= 1 << 11;            // ADDR21
	if (addr_mask & (1 << 20))
		pin_mask |= 1 << 12;            // ADDR20
	if (flags & EBI_PORT_NCS0)
		pin_mask |= 1 << 13;            // NCS0
	portmux_select_peripheral(PORTMUX_PORT_X0, pin_mask,
			PORTMUX_FUNC_A, 0);

	/* NWAIT should have pull-up enabled */
	if (flags & EBI_PORT_NWAIT)
		portmux_select_peripheral(PORTMUX_PORT_X0, 1 << 9,
				PORTMUX_FUNC_A, PORTMUX_PULL_UP);
}

#endif /* CHIP_EBI_PORT_H_INCLUDED */

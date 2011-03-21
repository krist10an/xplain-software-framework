/**
 * \file
 *
 * \brief GPIO portmux register interface
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
#ifndef GPIO_PORTMUX_GPIO_REGS_H_INCLUDED
#define GPIO_PORTMUX_GPIO_REGS_H_INCLUDED

#include <chip/memory-map.h>
#include <io.h>

/* Register offsets */
#define GPIO_GPER			0x0000	//!< GPIO Enable
#define GPIO_GPERS			0x0004	//!< GPIO Enable Set
#define GPIO_GPERC			0x0008	//!< GPIO Enable Clear
#define GPIO_GPERT			0x000c	//!< GPIO Enable Toggle
#define GPIO_PMR0			0x0010	//!< Periph Mux 0
#define GPIO_PMR0S			0x0014	//!< Periph Mux 0 Set
#define GPIO_PMR0C			0x0018	//!< Periph Mux 0 Clear
#define GPIO_PMR0T			0x001c	//!< Periph Mux 0 Toggle
#define GPIO_PMR1			0x0020	//!< Periph Mux 1
#define GPIO_PMR1S			0x0024	//!< Periph Mux 1 Set
#define GPIO_PMR1C			0x0028	//!< Periph Mux 1 Clear
#define GPIO_PMR1T			0x002c	//!< Periph Mux 1 Toggle
#define GPIO_ODER			0x0040	//!< Output Drv Enable
#define GPIO_ODERS			0x0044	//!< Output Drv Enable Set
#define GPIO_ODERC			0x0048	//!< Output Drv Enable Clear
#define GPIO_ODERT			0x004c	//!< Output Drv Enable Toggle
#define GPIO_OVR			0x0050	//!< Output Value
#define GPIO_OVRS			0x0054	//!< Output Value Set
#define GPIO_OVRC			0x0058	//!< Output Value Clear
#define GPIO_OVRT			0x005c	//!< Output Value Toggle
#define GPIO_PVR			0x0060	//!< Pin Value
#define GPIO_PUER			0x0070	//!< Pull-up Enable
#define GPIO_PUERS			0x0074	//!< Pull-up Enable Set
#define GPIO_PUERC			0x0078	//!< Pull-up Enable Clear
#define GPIO_PUERT			0x007c	//!< Pull-up Enable Toggle
#define GPIO_PDER			0x0080	//!< Pull-down Enable
#define GPIO_PDERS			0x0084	//!< Pull-down Enable Set
#define GPIO_PDERC			0x0088	//!< Pull-down Enable Clear
#define GPIO_PDERT			0x008c	//!< Pull-down Enable Toggle
#define GPIO_IER			0x0090	//!< Interrupt Enable
#define GPIO_IERS			0x0094	//!< Interrupt Enable Set
#define GPIO_IERC			0x0098	//!< Interrupt Enable Clear
#define GPIO_IERT			0x009c	//!< Interrupt Enable Toggle
#define GPIO_IMR0			0x00a0	//!< Interrupt Mode 0
#define GPIO_IMR0S			0x00a4	//!< Interrupt Mode 0 Set
#define GPIO_IMR0C			0x00a8	//!< Interrupt Mode 0 Clear
#define GPIO_IMR0T			0x00ac	//!< Interrupt Mode 0 Toggle
#define GPIO_IMR1			0x00b0	//!< Interrupt Mode 1
#define GPIO_IMR1S			0x00b4	//!< Interrupt Mode 1 Set
#define GPIO_IMR1C			0x00b8	//!< Interrupt Mode 1 Clear
#define GPIO_IMR1T			0x00bc	//!< Interrupt Mode 1 Toggle
#define GPIO_GFER			0x00c0	//!< Glitch Filter
#define GPIO_GFERS			0x00c4	//!< Glitch Filter Set
#define GPIO_GFERC			0x00c8	//!< Glitch Filter Clear
#define GPIO_GFERT			0x00cc	//!< Glitch Filter Toggle
#define GPIO_IFR			0x00d0	//!< Interrupt Flag
#define GPIO_IFRC			0x00d8	//!< Interrupt Flag Clear
#define GPIO_ODMER			0x00e0	//!< Open Drain Mode
#define GPIO_ODMERS			0x00e4	//!< Open Drain Mode Set
#define GPIO_ODMERC			0x00e8	//!< Open Drain Mode Clear
#define GPIO_ODMERT			0x00ec	//!< Open Drain Mode Toggle
#define GPIO_ODCR0			0x0100	//!< Drive Strength 0
#define GPIO_ODCR0S			0x0104	//!< Drive Strength 0 Set
#define GPIO_ODCR0C			0x0108	//!< Drive Strength 0 Clear
#define GPIO_ODCR0T			0x010c	//!< Drive Strength 0 Toggle
#define GPIO_ODCR1			0x0110	//!< Drive Strength 1
#define GPIO_ODCR1S			0x0114	//!< Drive Strength 1 Set
#define GPIO_ODCR1C			0x0118	//!< Drive Strength 1 Clear
#define GPIO_ODCR1T			0x011c	//!< Drive Strength 1 Toggle
#define GPIO_OSRR0			0x0130	//!< Slew Rate 0
#define GPIO_OSRR0S			0x0134	//!< Slew Rate 0 Set
#define GPIO_OSRR0C			0x0138	//!< Slew Rate 0 Clear
#define GPIO_OSRR0T			0x013c	//!< Slew Rate 0 Toggle
#define GPIO_STER			0x0160	//!< Schmitt Trigger
#define GPIO_STERS			0x0164	//!< Schmitt Trigger Set
#define GPIO_STERC			0x0168	//!< Schmitt Trigger Clear
#define GPIO_STERT			0x016c	//!< Schmitt Trigger Toggle
#define GPIO_VERSION			0x01fc	//!< Controller Version

/** \brief Read GPIO register \a reg on \a port */
#define gpio_read_reg(port, reg)				\
	mmio_read32((void *)((unsigned long)(port) + GPIO_##reg))
/** \brief Write \a value to GPIO register \a reg on \a port */
#define gpio_write_reg(port, reg, value)			\
	mmio_write32((void *)((unsigned long)(port) + GPIO_##reg), value)

#endif /* GPIO_PORTMUX_GPIO_REGS_H_INCLUDED */

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

#include <clk/xmega/osc.h>

#define OSC_ID_RC2M             0
#define OSC_ID_RC32M            1
#define OSC_ID_RC32K            2
#define OSC_ID_XOSC             3

__always_inline static void osc_enable(uint8_t id)
{
	if (id < OSC_ID_XOSC) {
		osc_enable_internal(id);
	} else {
		assert(id == OSC_ID_XOSC);
		osc_enable_external(id);
	}
}

__always_inline static uint32_t osc_get_rate(uint8_t id)
{
	switch (id) {
	case OSC_ID_RC2M:
		return 2000000UL;
	case OSC_ID_RC32M:
		return 32000000UL;
	case OSC_ID_RC32K:
		return 32768UL;
#ifdef BOARD_XOSC_HZ
	case OSC_ID_XOSC:
		return BOARD_XOSC_HZ;
#endif
	default:
		unhandled_case(id);
		return 0;
	}
}

#endif /* CHIP_OSC_H_INCLUDED */

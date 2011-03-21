/**
 * \file
 *
 * \brief AT90USB1287 clock control
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
#ifndef CHIP_CLK_H_INCLUDED
#define CHIP_CLK_H_INCLUDED

#include <chip/regs.h>

/**
 * \brief Enable the AT90USB USB clock, sourced from the internal PLL.
 */
static inline void clk_enable_at90usb(void)
{
	avr_write_reg8(PLLCSR, AVR_BIT(PLLCSR_PLLP1)
#if CONFIG_CPU_HZ == 8000000UL
			| AVR_BIT(PLLCSR_PLLP0)
#elif CONFIG_CPU_HZ == 16000000UL
			| AVR_BIT(PLLCSR_PLLP2)
#else
# error Chip CPU frequency must be 8 MHz or 16 MHz when using the USB peripheral
#endif
			| AVR_BIT(PLLCSR_PLLE));
	do {
		barrier();
	} while (!(avr_read_reg8(PLLCSR) & AVR_BIT(PLLCSR_PLOCK)));
}

#endif /* CHIP_CLK_H_INCLUDED */

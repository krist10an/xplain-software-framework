/**
 * \file
 *
 * \brief Delay functions for the AVR architecture
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
#include <assert.h>
#include <delay.h>
#include <types.h>
#include <compiler.h>

static void delay_loop(uint16_t iterations)
{
#ifdef __GNUC__
	asm volatile(
		"1:	sbiw	%0, 1\n"
		"	brne	1b"
		: "+w"(iterations));
#else
	while (iterations--) {
		barrier();
	}
#endif
}

static void udelay_internal(unsigned int us)
{
	delay_loop(((CONFIG_CPU_HZ / 1000000) * us) / 4);
}

/**
 * \brief Delay for a specified number of microseconds.
 *
 * \param us Number of microseconds to delay.
 */
void udelay(unsigned int us)
{
	assert(us != 0);

	udelay_internal(us);
}

/**
 * \brief Delay for a specified number of milliseconds.
 *
 * \param ms Number of milliseconds to delay.
 */
void mdelay(unsigned int ms)
{
	assert(ms != 0);

	do {
		udelay_internal(1000);
		ms--;
	} while (ms != 0);
}

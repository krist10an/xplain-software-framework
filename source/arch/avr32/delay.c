/**
 * \file
 *
 * \brief Delay functions for the AVR32 architecture
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
#include <delay.h>
#include <types.h>
#include <util.h>

#include <arch/sysreg.h>
#include <clk/sys.h>

/**
 * \brief Delay for a specified number of microseconds.
 *
 * Since the max CPU frequency at this point is 66 MHz, this function
 * supports delays up to
 *
 * \f[
 *	\frac{2^{31}}{66000000} \approx 32 \textnormal{s}
 * \f]
 *
 * \param us Number of microseconds to delay.
 */
void udelay(unsigned int us)
{
	uint32_t count, count_end;

	count = sysreg_read(COUNT);
	count_end = count + div_ceil(sysclk_get_cpu_hz(), 1000000) * us;

	while ((int32_t)(count_end - count) > 0)
		count = sysreg_read(COUNT);
}

/**
 * \brief Delay for a specified number of milliseconds.
 *
 * \param ms Number of milliseconds to delay.
 */
void mdelay(unsigned int ms)
{
	udelay(ms * 1000);
}

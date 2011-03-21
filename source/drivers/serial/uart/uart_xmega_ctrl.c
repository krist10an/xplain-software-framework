/**
 * \file
 *
 * \brief AVR XMega UART control interface
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
#include <types.h>
#include <util.h>
#include <uart/ctrl.h>

/**
 * \internal
 * \brief Non-inline uart_baud_set_rate() with fewer parameters
 *
 * This function is called via an inline wrapper to preserve the
 * standard API while eliminating some unnecessary argument setup
 * overhead.
 */
void uart_priv_baud_set_rate(struct uart_baud *baud, uint32_t rate)
{
	int8_t		exp;
	uint32_t	div;
	uint32_t	fper = CONFIG_CPU_HZ;
	uint32_t	limit;
	uint32_t	ratio;

	/*! \todo Make use of the UART_BAUD_CLK2X flag. */
	baud->flags = 0;

	/*
	 * Implement the non-constant case in the same way as the
	 * constant case, but without unrolling the loop.
	 *
	 * The below expressions assume that the result is going to be
	 * divided by 8. So we need to (a) scale the target baud rate if
	 * 16x oversampling is used, and (b) shift all the limits left
	 * by 3 bits.
	 */
	if (!(baud->flags & USART_BIT(CLK2X)))
		rate *= 2;

	/* Find the lowest possible exponent */
	limit = 0xfffU >> 4;
	ratio = fper / rate;
	for (exp = -7; exp < 7; exp++) {
		if (ratio < limit)
			break;
		limit <<= 1;
		if (exp < -3)
			limit |= 1;
	}

	/*
	 * Depending on the value of exp, scale either the input
	 * frequency or the target baud rate. By always scaling upwards,
	 * we never introduce any additional inaccuracy.
	 *
	 * We're including the final divide-by-8 (aka. right-shift-by-3)
	 * in this operation as it ensures that we never exceeed 2**32
	 * at any point.
	 *
	 * The formula for calculating BSEL is slightly different when
	 * exp is negative than it is when exp is positive.
	 */
	if (exp < 0) {
		/*
		 * We're supposed to subtract 1, then apply BSCALE. We
		 * want to apply BSCALE first, so we need to turn
		 * everything inside the parenthesis into a single
		 * fractional expression.
		 */
		fper -= 8 * rate;

		/*
		 * If we end up with a left-shift after taking the final
		 * divide-by-8 into account, do the shift before the
		 * divide. Otherwise, left-shift the denominator instead
		 * (effectively resulting in an overall right shift.)
		 */
		if (exp <= -3) {
			div = ((fper << (-exp - 3)) + rate / 2)
				/ rate;
		} else {
			rate <<= exp + 3;
			div = (fper + rate / 2) / rate;
		}
	} else {
		/*
		 * We will always do a right shift in this case, but we
		 * need to shift three extra positions because of the
		 * divide-by-8.
		 */
		rate <<= exp + 3;
		div = (fper + rate / 2) / rate - 1;
	}

	assert(div > 0 && div < (1 << 12));
	assert(exp > -8 && exp < 8);

	baud->divider = (exp << 12) | div;
}

/**
 * \internal
 * \brief Non-inline uart_baud_rate_is_valid() with fewer parameters
 *
 * This function is called via an inline wrapper to preserve the
 * standard API while eliminating some unnecessary argument setup
 * overhead.
 */
bool uart_priv_baud_rate_is_valid(uint8_t flags, uint32_t rate)
{
	uint32_t	max_rate;
	uint32_t	min_rate;

	max_rate = CONFIG_CPU_HZ / 8;
	min_rate = div_ceil(CONFIG_CPU_HZ, 128UL * 8 * 4096);

	if (!(flags & USART_BIT(CLK2X))) {
		max_rate /= 2;
		min_rate = div_ceil(min_rate, 2);
	}

	return rate <= max_rate && rate >= min_rate;
}

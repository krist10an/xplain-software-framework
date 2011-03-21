/**
 * \file
 *
 * \brief AVR Mega UART control interface
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
	/*
	 * This will expand the non-constant version, which is large and
	 * slow, just once. If it is never called, the linker will
	 * eliminate it.
	 */
	uart_priv_baud_set_constant_rate(baud, rate);
}

/**
 * \internal
 * \brief Non-inline uart_baud_set_rate() with fewer parameters
 *
 * This function is called via an inline wrapper to preserve the
 * standard API while eliminating some unnecessary argument setup
 * overhead.
 */
bool uart_priv_baud_rate_is_valid(struct uart_baud *baud, uint32_t rate)
{
	uint32_t	max_rate;
	uint32_t	min_rate;

	max_rate = CONFIG_CPU_HZ / 8;
	min_rate = div_ceil(CONFIG_CPU_HZ, 8U * 4096);

	if (!(baud->flags & UART_BAUD_CLK2X)) {
		max_rate /= 2;
		min_rate = div_ceil(min_rate, 2);
	}

	return rate <= max_rate && rate >= min_rate;
}

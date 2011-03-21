/**
 * \file
 *
 * \brief Debug console backend using a UART for output
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
#include <debug.h>
#include <interrupt.h>
#include <stream.h>
#include <uart.h>
#include <uart/ctrl.h>

#define DEBUG_UART	CONFIG_DEBUG_UART_ID

/*
 * This function may be called from any context, so we need to be very
 * careful about races here.
 *
 * If possible, we don't want to busy-wait for the UART with interrupts
 * disabled, so after an unsuccessful attempt to send a byte, we restore
 * the interrupt state and wait for the UART to become ready.
 *
 * Then, we disable interrupts and try again. If an interrupt handler
 * beat us to it, we may fail again and do another round of waiting.
 *
 * Note that if we're called with interrupts disabled, we will end up
 * busy-waiting with interrupts disabled too. This may be bad for system
 * performance, but we really want this stuff to be safe to use anywhere.
 */
static void dbg_commit(struct stream *stream)
{
	irqflags_t	iflags;
	char		c;

	iflags = cpu_irq_save();
	while (stream_buf_has_data(stream)) {
		c = stream_buf_extract_char(stream);

		while (!uart_put_byte(DEBUG_UART, c)) {
			cpu_irq_restore(iflags);
			while (!uart_tx_buffer_is_empty(DEBUG_UART))
				barrier();
			cpu_irq_disable();
		}
	}
	cpu_irq_restore(iflags);
}

static bool dbg_make_room(struct stream *stream, unsigned int goal)
{
	/* Keep it simple for now */
	dbg_commit(stream);

	/* We are always able to make room */
	return true;
}

static const struct stream_ops dbg_stream_ops = {
	.commit		= dbg_commit,
	.make_room	= dbg_make_room,
};

const struct stream_ops *dbg_backend_init(void)
{
	uart_enable_clock(DEBUG_UART);
	uart_ctrl_init_defaults(DEBUG_UART);
	uart_enable(DEBUG_UART, UART_FLAG_TX);

	return &dbg_stream_ops;
}

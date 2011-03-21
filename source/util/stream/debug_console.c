/**
 * \file
 *
 * \brief Debug console implementation
 *
 * Copyright (C) 2009 Atmel Corporation. All rights reserved.
 *
 * This is the core implementation of the debug console, providing
 * support for dbg_printf_level() and friends. It relies on a debug
 * stream backend, e.g. a UART driver, which will do the actual data
 * transfer.
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
#include <debug.h>
#include <stream.h>
#include <util.h>

/**
 * \weakgroup debug_console
 * @{
 */

#ifdef CONFIG_DEBUG_CONSOLE_BUF_SIZE
# define DEBUG_BUF_SIZE	CONFIG_DEBUG_CONSOLE_BUF_SIZE
#else
# define DEBUG_BUF_SIZE	64
#endif

static char	debug_console_buffer[DEBUG_BUF_SIZE];

struct stream	debug_stream = {
	.ring_mask	= DEBUG_BUF_SIZE - 1,
	.data		= debug_console_buffer,
};

/**
 * \internal
 * \brief Formatted output conversion to the debug console.
 *
 * This is a simple wrapper around stream_vprintf() using the debug
 * console as the output stream.
 */
int dbg_priv_vprintf(const char *format, va_list ap)
{
	return stream_vprintf(&debug_stream, format, ap);
}

/**
 * \internal
 * \brief Formatted output conversion to the debug console.
 *
 * This is a simple wrapper around stream_vprintf() using the debug
 * console as the output stream.
 */
int dbg_priv_printf(const char *format, ...)
{
	va_list ap;
	int n;

	va_start(ap, format);
	n = stream_vprintf(&debug_stream, format, ap);
	va_end(ap);

	return n;
}

/**
 * \internal
 * \brief Write \a str to the debug console
 *
 * This is a simple wrapper around stream_putstr() using the debug
 * console as the output stream.
 */
int dbg_priv_putstr(const char *str)
{
	return stream_putstr(&debug_stream, str);
}

/**
 * \internal
 * \brief Write the character \a c to the debug console
 *
 * This is a simple wrapper around stream_putchar() using the debug
 * console as the output stream.
 */
int dbg_priv_putchar(int c)
{
	return stream_putchar(&debug_stream, c);
}

/**
 * \brief Initialize the debug console
 *
 * This function must be called before any of the other debug console
 * functions may be called. If CONFIG_DEBUG_CONSOLE is not set, calling
 * this function will not generate any code.
 */
void dbg_init(void)
{
	build_assert(is_power_of_two(DEBUG_BUF_SIZE));

	debug_stream.ops = dbg_backend_init();
}

//! @}

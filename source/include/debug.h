/**
 * \file
 *
 * \brief Debug console
 *
 * Copyright (C) 2009 Atmel Corporation. All rights reserved.
 *
 * This is a simple debug console which can be used to dump
 * information e.g. over a serial line when debugging a new application
 * or driver.
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
#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <compiler.h>
#include <stdarg.h>

/**
 * \ingroup stream
 * \defgroup debug_console Debug Console
 *
 * The debug console allows applications, drivers or pretty much
 * anything to log messages to a predefined console backend. The backend
 * may be a UART driver which sends the message over the serial line, or
 * a simple ring buffer which allows the messages to be read out for
 * forensics.
 *
 * Each message sent to the debug console has a \em severity associated
 * with it, specified as one of the values in #debug_level. A lower
 * number indicates higher severity, and if the severity level of the
 * message is numberically higher than the current debug level, the
 * message is dropped. The current debug level is determined as follows:
 *   - If the configuration symbol CONFIG_DEBUG_CONSOLE is not set, all
 *     messages are dropped. In this case, the initialization function,
 *     dbg_init() will do nothing.
 *   - Otherwise, if the preprocessor symbol DEBUG_LEVEL is defined, it
 *     is used as the current debug level.
 *   - Otherwise, if the configuration symbol CONFIG_DEBUG_LEVEL is set,
 *     it is used as the current debug level.
 *   - Otherwise, the current debug level is #DEBUG_INFO.
 *
 * If a message is dropped, the compiler should be able to eliminate the
 * debug function call as well as any references to its parameters
 * (format strings, etc.) This allows drivers, etc. to use dbg_verbose()
 * liberally without affecting the performance or code size when verbose
 * debugging is disabled.
 *
 * \note When the current debug level is set to #DEBUG_NONE, no debug
 * messages will be sent to the backend, not even messages with severity
 * #DEBUG_NONE.
 *
 * @{
 */

/**
 * \brief Severity level of debugging messages
 */
enum debug_level {
	DEBUG_NONE	= 0,	//!< No debugging messages
	DEBUG_PANIC,		//!< System panic (fatal exceptions, etc.)
	DEBUG_ASSERT,		//!< Assertion failures
	DEBUG_ERROR,		//!< Major errors which may be recoverable
	DEBUG_WARNING,		//!< Things that might cause problems
	DEBUG_INFO,		//!< Informational messages
	DEBUG_VERBOSE,		//!< Verbose debugging messages
};

#ifndef CONFIG_DEBUG_CONSOLE
# define dbg_init()		do { } while (0)
# undef DEBUG_LEVEL
# define DEBUG_LEVEL		DEBUG_NONE
#else
extern const struct stream_ops *dbg_backend_init(void);
extern void dbg_init(void);
# ifndef DEBUG_LEVEL
#  ifdef CONFIG_DEBUG_LEVEL
#   define DEBUG_LEVEL		CONFIG_DEBUG_LEVEL
#  else
#   define DEBUG_LEVEL		DEBUG_INFO
#  endif
# endif
#endif

extern int dbg_priv_vprintf(const char *format, va_list ap);
extern int dbg_priv_printf(const char *format, ...) __printf_format(1, 2);
extern int dbg_priv_putstr(const char *str);
extern int dbg_priv_putchar(int c);

/**
 * \internal
 * Dummy function which simply returns 0. Simply having a macro
 * evaluating to zero produces "statement with no effect" warnings with
 * gcc, so we're using an inline function call instead.
 */
static inline int dbg_priv_retzero(void)
{
	return 0;
}

/**
 * \internal
 * Return true if \a level is less than or equal to the current debug
 * level.
 */
#define dbg_priv_check_level(level)					\
	(DEBUG_LEVEL != DEBUG_NONE && (level) <= DEBUG_LEVEL)

/**
 * \brief Formatted output conversion to the debug console.
 *
 * Produce output according to \a format on the debug console if the
 * current debug level is higher or equal to \a level. \a format is
 * interpreted as a regular printf()-style format string with a few
 * limitations (some specifiers are accepted but ignored.)
 *
 * \param level Debug level at which to show the message
 * \param format Format specification.
 * \param ap Format arguments.
 *
 * \return The number of characters printed.
 */
#define dbg_vprintf_level(level, format, ap)			\
	(dbg_priv_check_level(level)				\
		? dbg_priv_vprintf(format, ap)			\
		: dbg_priv_retzero())

/**
 * \brief Formatted output conversion to the debug console.
 *
 * Produce output according to \a format on the debug console if the
 * current debug level is higher or equal to \a level. \a format is
 * interpreted as a regular printf()-style format string with a few
 * limitations (some specifiers are accepted but ignored.)
 *
 * \param level Debug level at which to show the message
 * \param ... Format specification and arguments
 *
 * \return The number of characters printed.
 */
#define dbg_printf_level(level, ...)				\
	(dbg_priv_check_level(level)				\
		? dbg_priv_printf(__VA_ARGS__)			\
		: dbg_priv_retzero())

/**
 * \brief Write a string to the debug console if the current debug level
 * is higher or equal to \a level.
 *
 * \param level Debug level at which to show the message
 * \param str NUL-terminated string.
 *
 * \return The number of characters written.
 */
#define dbg_putstr_level(level, str)				\
	(dbg_priv_check_level(level)				\
		? dbg_priv_putstr(str)				\
		: dbg_priv_retzero())

/**
 * \brief Write a single character to the debug console if the current
 * debug level is higher or equal to \a level.
 *
 * \param level Debug level at which to show the message
 * \param c The character to write.
 *
 * \return \a c as an unsigned char cast to an int.
 */
#define dbg_putchar_level(level, c)				\
	(dbg_priv_check_level(level)				\
		? dbg_priv_putchar(c)				\
		: dbg_priv_retzero())

/**
 * \brief Output string at DEBUG_VERBOSE level.
 * \sa dbg_putstr_level()
 */
#define dbg_putstr(str)						\
	dbg_putstr_level(DEBUG_VERBOSE, str)
/**
 * \brief Output character at DEBUG_VERBOSE level.
 * \sa dbg_putchar_level()
 */
#define dbg_putchar(c)						\
	dbg_putchar_level(DEBUG_VERBOSE, c)

/**
 * \brief Display a panic message
 * \sa dbg_printf_level()
 */
#define dbg_panic(...)						\
	dbg_printf_level(DEBUG_PANIC, __VA_ARGS__)
/**
 * \brief Display a error message
 * \sa dbg_printf_level()
 */
#define dbg_error(...)						\
	dbg_printf_level(DEBUG_ERROR, __VA_ARGS__)
/**
 * \brief Display a warning message
 * \sa dbg_printf_level()
 */
#define dbg_warning(...)					\
	dbg_printf_level(DEBUG_WARNING, __VA_ARGS__)
/**
 * \brief Display an informational message
 * \sa dbg_printf_level()
 */
#define dbg_info(...)						\
	dbg_printf_level(DEBUG_INFO, __VA_ARGS__)
/**
 * \brief Display a verbose debugging message
 * \sa dbg_printf_level()
 */
#define dbg_verbose(...)					\
	dbg_printf_level(DEBUG_VERBOSE, __VA_ARGS__)

//! @}

#endif /* DEBUG_H_INCLUDED */

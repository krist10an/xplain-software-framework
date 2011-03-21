/**
 * \file
 *
 * \brief Run-time and build-time assertion support.
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
#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED

#include <debug.h>

/**
 * \ingroup utility_group
 * \defgroup assert_group Assertions
 *
 * This module provides support for run-time and build-time assertions,
 * to help testing and debugging.
 *
 * @{
 */

#ifdef CONFIG_ASSERT
# define ASSERT_ENABLED 1
#else
# define ASSERT_ENABLED 0
#endif

/**
 * \brief Abort execution of the program
 *
 * When some part of the program finds itself in an impossible
 * situation, it may call this macro to halt execution and thus
 * facilitiate debugging.
 *
 * If CONFIG_ASSERT is not set, this macro does nothing.
 */
#define abort() do { } while (ASSERT_ENABLED)

/**
 * \brief Assert that \a condition is true at run time.
 *
 * If \a condition is false, output a diagnostic message and hang.
 *
 * If CONFIG_ASSERT is not set, this macro does nothing.
 */
#ifdef CONFIG_ASSERT
# define assert(condition)                                              \
	do {                                                            \
		if (ASSERT_ENABLED && unlikely(!(condition))) {         \
			cpu_irq_disable();                              \
			dbg_printf_level(DEBUG_ASSERT,                  \
				"%s:%d: Assertion \"%s\" failed!\n",    \
				__FILE__, __LINE__, #condition);        \
			abort();                                        \
		}                                                       \
	} while (0)
#else
# define assert(condition)
#endif

/**
 * \brief Assert that the case \a value will never need to be handled.
 *
 * If the code expanded from this macro is ever reached, output a
 * diagnostic message and hang.
 *
 * If CONFIG_ASSERT is not set, this macro does nothing.
 */
#define unhandled_case(value)                                           \
	do {                                                            \
		if (ASSERT_ENABLED) {                                   \
			dbg_printf_level(DEBUG_ASSERT,                  \
				"%s:%d: Unhandled case value %d\n",     \
				__FILE__, __LINE__, (value));           \
			abort();                                        \
		}                                                       \
	} while (0)

ERROR_FUNC(build_assert_failed, "Build assertion failed");

/**
 * \brief Assert that \a condition is true at build time.
 *
 * If \a condition is false, the compilation will abort with an error
 * message.
 */
#define build_assert(condition)                                         \
	do {                                                            \
		if (!(condition))                                       \
			build_assert_failed();                          \
	} while (0)

//! @}

#endif /* ASSERT_H_INCLUDED */

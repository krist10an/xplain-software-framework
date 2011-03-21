/**
 * \file
 *
 * \brief Compiler abstraction layer: GCC specifics
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
#ifndef COMPILER_GCC_H_INCLUDED
#define COMPILER_GCC_H_INCLUDED

#define __noreturn __attribute__((noreturn))
#define __must_check __attribute__((warn_unused_result))
#define __used __attribute__((used))
#define __always_inline inline __attribute__((always_inline))
#define __nonnull(...) __attribute__((nonnull (__VA_ARGS__)))
#define __printf_format(fmt_index, first_arg_index) \
	__attribute__((format(printf, fmt_index, first_arg_index)))

#define compiler_align_data(byte_alignment) \
	__attribute__((aligned(byte_alignment)))

#define likely(exp)             __builtin_expect(!!(exp), 1)
#define unlikely(exp)           __builtin_expect(!!(exp), 0)
#define is_constant(exp)        __builtin_constant_p(exp)
#define barrier()               asm volatile("" ::: "memory")

#if defined(__GNUC__)
# if __NO_INLINE__
#  define ERROR_FUNC(name, msg) \
	  extern int name(void) {}
# else
#  if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)
#   define ERROR_FUNC(name, msg) \
	   extern __attribute__((error(msg))) int name(void)
#  endif
# endif
#endif

/* Pull in arch-specific definitions and overrides */
#include <arch/compiler-gcc.h>

#endif /* COMPILER_GCC_H_INCLUDED */

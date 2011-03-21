/**
 * \file
 *
 * \brief C99 integer type definitions for AVR
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
#ifndef ARCH_STDINT_H_INCLUDED
#define ARCH_STDINT_H_INCLUDED

typedef signed char             int8_t;
typedef signed short            int16_t;
typedef signed long             int32_t;
typedef signed long long        int64_t;

typedef unsigned char           uint8_t;
typedef unsigned short          uint16_t;
typedef unsigned long           uint32_t;
typedef unsigned long long      uint64_t;

typedef int8_t                  int_least8_t;
typedef int16_t                 int_least16_t;
typedef int32_t                 int_least32_t;
typedef int64_t                 int_least64_t;

typedef uint8_t                 uint_least8_t;
typedef uint16_t                uint_least16_t;
typedef uint32_t                uint_least32_t;
typedef uint64_t                uint_least64_t;

typedef int8_t                  int_fast8_t;
typedef int16_t                 int_fast16_t;
typedef int32_t                 int_fast32_t;
typedef int64_t                 int_fast64_t;

typedef uint8_t                 uint_fast8_t;
typedef uint16_t                uint_fast16_t;
typedef uint32_t                uint_fast32_t;
typedef uint64_t                uint_fast64_t;

#ifdef __ICCAVR__
typedef __INTPTR_T_TYPE__       intptr_t;
typedef __UINTPTR_T_TYPE__      uintptr_t;
#else
typedef int16_t                 intptr_t;
typedef uint16_t                uintptr_t;
#endif

typedef int64_t                 intmax_t;
typedef uint64_t                uintmax_t;

#define INT8_MIN                (-128)
#define INT16_MIN               (-32767 - 1)
#define INT32_MIN               (-2147483647L - 1)
#define INT64_MIN               (-9223372036854775807LL - 1)
#define INT8_MAX                (127)
#define INT16_MAX               (32767)
#define INT32_MAX               (2147483647L)
#define INT64_MAX               (9223372036854775807LL)
#define UINT8_MAX               (255)
#define UINT16_MAX              (65535U)
#define UINT32_MAX              (4294967295UL)
#define UINT64_MAX              (18446744073709551615ULL)

#define INT_LEAST8_MIN          INT8_MIN
#define INT_LEAST16_MIN         INT16_MIN
#define INT_LEAST32_MIN         INT32_MIN
#define INT_LEAST64_MIN         INT64_MIN
#define INT_LEAST8_MAX          INT8_MAX
#define INT_LEAST16_MAX         INT16_MAX
#define INT_LEAST32_MAX         INT32_MAX
#define INT_LEAST64_MAX         INT64_MAX
#define UINT_LEAST8_MAX         UINT8_MAX
#define UINT_LEAST16_MAX        UINT16_MAX
#define UINT_LEAST32_MAX        UINT32_MAX
#define UINT_LEAST64_MAX        UINT64_MAX

#define INT_FAST8_MIN           INT8_MIN
#define INT_FAST16_MIN          INT16_MIN
#define INT_FAST32_MIN          INT32_MIN
#define INT_FAST64_MIN          INT64_MIN
#define INT_FAST8_MAX           INT8_MAX
#define INT_FAST16_MAX          INT16_MAX
#define INT_FAST32_MAX          INT32_MAX
#define INT_FAST64_MAX          INT64_MAX
#define UINT_FAST8_MAX          UINT8_MAX
#define UINT_FAST16_MAX         UINT16_MAX
#define UINT_FAST32_MAX         UINT32_MAX
#define UINT_FAST64_MAX         UINT64_MAX

#ifdef __ICCAVR__
# define INTPTR_MIN             __INTPTR_T_MIN__
# define INTPTR_MAX             __INTPTR_T_MAX__
# define UINTPTR_MAX            __UINTPTR_T_MAX__
#else
# define INTPTR_MIN             INT16_MIN
# define INTPTR_MAX             INT16_MAX
# define UINTPTR_MAX            UINT16_MAX
#endif

#define INTMAX_MIN              INT64_MIN
#define INTMAX_MAX              INT64_MAX
#define UINTMAX_MAX             UINT64_MAX

#endif /* ARCH_STDINT_H_INCLUDED */

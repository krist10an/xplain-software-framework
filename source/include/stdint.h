/**
 * \file
 *
 * \brief C99 integer type definitions
 *
 * Copyright (C) 2009 Atmel Corporation. All rights reserved.
 *
 * This file defines the standard C99 integer types like int8_t and
 * friends. These are normally provided by the C library, but since
 * this library is designed for running without one, we need to
 * provide them ourselves.
 *
 * Note that this file does not define all the types specified by the
 * C99 standard.
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
#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED

#include <arch/stdint.h>

/*
 * The rest of this file consists of documentation common to all
 * architectures.
 */

/**
 * \ingroup utility_group
 * \defgroup stdint_group Standard integer types
 *
 * @{
 */

//! \name Exact-width integer types
//@{
/**
 * \typedef int8_t
 * \brief 8-bit signed integer
 */
/**
 * \typedef int16_t
 * \brief 16-bit signed integer
 */
/**
 * \typedef int32_t
 * \brief 32-bit signed integer
 */
/**
 * \typedef int64_t
 * \brief 64-bit signed integer
 */
/**
 * \typedef uint8_t
 * \brief 8-bit unsigned integer
 */
/**
 * \typedef uint16_t
 * \brief 16-bit unsigned integer
 */
/**
 * \typedef uint32_t
 * \brief 32-bit unsigned integer
 */
/**
 * \typedef uint64_t
 * \brief 64-bit unsigned integer
 */
//@}

//! \name Minimum-width integer types
//@{
/**
 * \typedef int_least8_t
 * \brief Signed integer with at least 8-bit width
 */
/**
 * \typedef int_least16_t
 * \brief Signed integer with at least 16-bit width
 */
/**
 * \typedef int_least32_t
 * \brief Signed integer with at least 32-bit width
 */
/**
 * \typedef int_least64_t
 * \brief Signed integer with at least 64-bit width
 */
/**
 * \typedef uint_least8_t
 * \brief Unsigned integer with at least 8-bit width
 */
/**
 * \typedef uint_least16_t
 * \brief Unsigned integer with at least 16-bit width
 */
/**
 * \typedef uint_least32_t
 * \brief Unsigned integer with at least 32-bit width
 */
/**
 * \typedef uint_least64_t
 * \brief Unsigned integer with at least 64-bit width
 */
//@}

//! \name Fastest minimum-width integer types
/**
 * \typedef int_fast8_t
 * \brief Fastest signed integer with at least 8-bit width
 */
/**
 * \typedef int_fast16_t
 * \brief Fastest signed integer with at least 16-bit width
 */
/**
 * \typedef int_fast32_t
 * \brief Fastest signed integer with at least 32-bit width
 */
/**
 * \typedef int_fast64_t
 * \brief Fastest signed integer with at least 64-bit width
 */
/**
 * \typedef uint_fast8_t
 * \brief Fastest unsigned integer with at least 8-bit width
 */
/**
 * \typedef uint_fast16_t
 * \brief Fastest unsigned integer with at least 16-bit width
 */
/**
 * \typedef uint_fast32_t
 * \brief Fastest unsigned integer with at least 32-bit width
 */
/**
 * \typedef uint_fast64_t
 * \brief Fastest unsigned integer with at least 64-bit width
 */
//@}

//! \name Integer types capable of holding object pointers
//@{
/**
 * \typedef intptr_t
 * \brief Signed integer type capable of holding a pointer.
 *
 * If a valid pointer to void is converted to this type, then converted
 * back to a pointer to void, the result will compare equal to the
 * original pointer.
 */
/**
 * \typedef uintptr_t
 * \brief Unsigned integer type capable of holding a pointer.
 *
 * If a valid pointer to void is converted to this type, then converted
 * back to a pointer to void, the result will compare equal to the
 * original pointer.
 */
//@}

//! \name Greatest-width integer types
//@{
/**
 * \typedef intmax_t
 * \brief Signed integer type capable of representing any supported
 * signed integer type.
 */
/**
 * \typedef uintmax_t
 * \brief Unsigned integer type capable of representing any supported
 * unsigned integer type.
 */
//@}

//! \name Limits of Specified-Width Integer Types
//@{
/**
 * \def INT8_MIN
 * \brief Minimum value of int8_t variables.
 */
/**
 * \def INT16_MIN
 * \brief Minimum value of int16_t variables.
 */
/**
 * \def INT32_MIN
 * \brief Minimum value of int32_t variables.
 */
/**
 * \def INT64_MIN
 * \brief Minimum value of int64_t variables.
 */

/**
 * \def INT8_MAX
 * \brief Maximum value of int8_t variables.
 */
/**
 * \def INT16_MAX
 * \brief Maximum value of int16_t variables.
 */
/**
 * \def INT32_MAX
 * \brief Maximum value of int32_t variables.
 */
/**
 * \def INT64_MAX
 * \brief Maximum value of int64_t variables.
 */

/**
 * \def UINT8_MAX
 * \brief Maximum value of uint8_t variables.
 */
/**
 * \def UINT16_MAX
 * \brief Maximum value of uint16_t variables.
 */
/**
 * \def UINT32_MAX
 * \brief Maximum value of uint32_t variables.
 */
/**
 * \def UINT64_MAX
 * \brief Maximum value of uint64_t variables.
 */

/**
 * \def INT_LEAST8_MIN
 * \brief Minimum value of int_least8_t variables.
 */
/**
 * \def INT_LEAST16_MIN
 * \brief Minimum value of int_least16_t variables.
 */
/**
 * \def INT_LEAST32_MIN
 * \brief Minimum value of int_least32_t variables.
 */
/**
 * \def INT_LEAST64_MIN
 * \brief Minimum value of int_least64_t variables.
 */

/**
 * \def INT_LEAST8_MAX
 * \brief Maximum value of int_least8_t variables.
 */
/**
 * \def INT_LEAST16_MAX
 * \brief Maximum value of int_least16_t variables.
 */
/**
 * \def INT_LEAST32_MAX
 * \brief Maximum value of int_least32_t variables.
 */
/**
 * \def INT_LEAST64_MAX
 * \brief Maximum value of int_least64_t variables.
 */

/**
 * \def UINT_LEAST8_MAX
 * \brief Maximum value of uint_least8_t variables.
 */
/**
 * \def UINT_LEAST16_MAX
 * \brief Maximum value of uint_least16_t variables.
 */
/**
 * \def UINT_LEAST32_MAX
 * \brief Maximum value of uint_least32_t variables.
 */
/**
 * \def UINT_LEAST64_MAX
 * \brief Maximum value of uint_least64_t variables.
 */

/**
 * \def INT_FAST8_MIN
 * \brief Minimum value of int_fast8_t variables.
 */
/**
 * \def INT_FAST16_MIN
 * \brief Minimum value of int_fast16_t variables.
 */
/**
 * \def INT_FAST32_MIN
 * \brief Minimum value of int_fast32_t variables.
 */
/**
 * \def INT_FAST64_MIN
 * \brief Minimum value of int_fast64_t variables.
 */

/**
 * \def INT_FAST8_MAX
 * \brief Maximum value of int_fast8_t variables.
 */
/**
 * \def INT_FAST16_MAX
 * \brief Maximum value of int_fast16_t variables.
 */
/**
 * \def INT_FAST32_MAX
 * \brief Maximum value of int_fast32_t variables.
 */
/**
 * \def INT_FAST64_MAX
 * \brief Maximum value of int_fast64_t variables.
 */

/**
 * \def UINT_FAST8_MAX
 * \brief Maximum value of uint_fast8_t variables.
 */
/**
 * \def UINT_FAST16_MAX
 * \brief Maximum value of uint_fast16_t variables.
 */
/**
 * \def UINT_FAST32_MAX
 * \brief Maximum value of uint_fast32_t variables.
 */
/**
 * \def UINT_FAST64_MAX
 * \brief Maximum value of uint_fast64_t variables.
 */

/**
 * \def INTPTR_MIN
 * \brief Minimum value of intptr_t variables.
 */
/**
 * \def INTPTR_MAX
 * \brief Maximum value of intptr_t variables.
 */
/**
 * \def UINTPTR_MAX
 * \brief Maximum value of uintptr_t variables.
 */

/**
 * \def INTMAX_MIN
 * \brief Minimum value of intmax_t variables.
 */
/**
 * \def INTMAX_MAX
 * \brief Maximum value of intmax_t variables.
 */
/**
 * \def UINTMAX_MAX
 * \brief Maximum value of uintmax_t variables.
 */
//@}

//! @}

#endif /* STDINT_H_INCLUDED */

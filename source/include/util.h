/**
 * \file
 *
 * \brief Misc utility functions and definitions
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
#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <compiler.h>
#include <stdint.h>
#include <types.h>

/**
 * \defgroup utility_group Utility Library
 *
 * This is a collection of utility functions and macros which may be
 * useful when dealing with certain common problems, e.g. accessing data
 * from a byte stream, simple mathematical operations, etc.
 *
 * @{
 */

/**
 * \brief Stringify the result after expansion of a macro argument.
 */
#define xstr(s)		str(s)

/**
 * \brief Stringify a macro argument without expansion.
 */
#define str(s)		#s

/**
 * \brief Get the number of elements in array @a a.
 */
#define ARRAY_LEN(a)	(sizeof(a) / sizeof((a)[0]))

/**
 * \brief Determine whether or not the character @a c is a digit.
 *
 * \param c The character to consider.
 * \retval 0 The character \a c is not a digit.
 * \retval 1 The character \a c is a digit.
 */
static inline int isdigit(int c)
{
	return (c >= '0') && (c <= '9');
}

/**
 * \brief Determine whether or not the character \a c is a control
 * character.
 *
 * \param c The character to consider.
 * \retval 0 The character \a c is not a control character.
 * \retval 1 The character \a c is a control character.
 */
static inline int iscntrl(int c)
{
	return (c < 32) || (c >= 127);
}

/**
 * \brief Determine whether or not the character \a c is a space.
 *
 * \note This implementation is very limited in that it doesn't
 * consider a bunch of control characters that probably should be
 * interpreted as space.
 *
 * \param c The character to consider.
 * \retval 0 The character \a c is not a space.
 * \retval 1 The character \a c is a space.
 */
static inline int isspace(int c)
{
	return c == ' ';
}

/**
 * \brief Get the containing object.
 *
 * \param ptr Pointer to the contained object.
 * \param type Type of the containing object.
 * \param member Member name of the contained object inside the
 *	containing object.
 * \return Pointer to the containing object.
 */
#define container_of(ptr, type, member)					\
	((type *)((uintptr_t)(ptr) - offsetof(type, member)))

//! \name Minimum and Maximum
//@{

/**
 * \brief Get the lowest of two signed values.
 * \param a A signed integer
 * \param b Another signed integer
 * \return The numerically lowest value of \a a and \a b.
 */
#define min_s(a, b)                                                     \
	((sizeof(a) == 1) && (sizeof(b) == 1) ? compiler_min_s8(a, b)   \
	: (sizeof(a) <= 2) && (sizeof(b) <= 2) ? compiler_min_s16(a, b) \
	: (sizeof(a) <= 4) && (sizeof(b) <= 4) ? compiler_min_s32(a, b) \
	: compiler_min_s64(a, b))

/**
 * \brief Get the lowest of two unsigned values.
 * \param a An unsigned integer
 * \param b Another unsigned integer
 * \return The numerically lowest value of \a a and \a b.
 */
#define min_u(a, b)                                                     \
	((sizeof(a) == 1) && (sizeof(b) == 1) ? compiler_min_u8(a, b)   \
	: (sizeof(a) <= 2) && (sizeof(b) <= 2) ? compiler_min_u16(a, b) \
	: (sizeof(a) <= 4) && (sizeof(b) <= 4) ? compiler_min_u32(a, b) \
	: compiler_min_u64(a, b))

/**
 * \brief Get the highest of two signed values.
 * \param a A signed integer
 * \param b Another signed integer
 * \return The numerically highest value of \a a and \a b.
 */
#define max_s(a, b)                                                     \
	((sizeof(a) == 1) && (sizeof(b) == 1) ? compiler_max_s8(a, b)   \
	: (sizeof(a) <= 2) && (sizeof(b) <= 2) ? compiler_max_s16(a, b) \
	: (sizeof(a) <= 4) && (sizeof(b) <= 4) ? compiler_max_s32(a, b) \
	: compiler_max_s64(a, b))

/**
 * \brief Get the highest of two unsigned values.
 * \param a An unsigned integer
 * \param b Another unsigned integer
 * \return The numerically highest value of \a a and \a b.
 */
#define max_u(a, b)                                                     \
	((sizeof(a) == 1) && (sizeof(b) == 1) ? compiler_max_u8(a, b)   \
	: (sizeof(a) <= 2) && (sizeof(b) <= 2) ? compiler_max_u16(a, b) \
	: (sizeof(a) <= 4) && (sizeof(b) <= 4) ? compiler_max_u32(a, b) \
	: compiler_max_u64(a, b))

//@}

/**
 * \internal
 * Undefined function. Will cause a link failure if ilog2() is called
 * with an invalid constant value.
 */
int_fast8_t ilog2_undefined(void);

/**
 * \brief Calculate the base-2 logarithm of a number rounded down to
 * the nearest integer.
 *
 * \param x A 32-bit value
 * \return The base-2 logarithm of \a x, or -1 if \a x is 0.
 */
__always_inline static int_fast8_t ilog2(uint32_t x)
{
	if (is_constant(x))
		return ((x) & (1ULL << 31) ? 31 :
			(x) & (1ULL << 30) ? 30 :
			(x) & (1ULL << 29) ? 29 :
			(x) & (1ULL << 28) ? 28 :
			(x) & (1ULL << 27) ? 27 :
			(x) & (1ULL << 26) ? 26 :
			(x) & (1ULL << 25) ? 25 :
			(x) & (1ULL << 24) ? 24 :
			(x) & (1ULL << 23) ? 23 :
			(x) & (1ULL << 22) ? 22 :
			(x) & (1ULL << 21) ? 21 :
			(x) & (1ULL << 20) ? 20 :
			(x) & (1ULL << 19) ? 19 :
			(x) & (1ULL << 18) ? 18 :
			(x) & (1ULL << 17) ? 17 :
			(x) & (1ULL << 16) ? 16 :
			(x) & (1ULL << 15) ? 15 :
			(x) & (1ULL << 14) ? 14 :
			(x) & (1ULL << 13) ? 13 :
			(x) & (1ULL << 12) ? 12 :
			(x) & (1ULL << 11) ? 11 :
			(x) & (1ULL << 10) ? 10 :
			(x) & (1ULL <<  9) ?  9 :
			(x) & (1ULL <<  8) ?  8 :
			(x) & (1ULL <<  7) ?  7 :
			(x) & (1ULL <<  6) ?  6 :
			(x) & (1ULL <<  5) ?  5 :
			(x) & (1ULL <<  4) ?  4 :
			(x) & (1ULL <<  3) ?  3 :
			(x) & (1ULL <<  2) ?  2 :
			(x) & (1ULL <<  1) ?  1 :
			(x) & (1ULL <<  0) ?  0 :
			ilog2_undefined());

	return 31 - compiler_clz(x);
}

/**
 * \brief Test if a given value is a power of two.
 *
 * \param x The value to test
 * \return true if \a x is a power of two, false otherwise
 */
__always_inline static bool is_power_of_two(unsigned long x)
{
	return x && !(x & (x - 1));
}

ERROR_FUNC(priv_round_down_bad_type, "Invalid type passed to round_down");

/**
 * \brief Round down to the nearest power of two boundary.
 *
 * \param x A positive integer
 * \param order log2 of the required boundary.
 * \return \a x rounded down to the nearest multiple of (1 << \a order)
 */
#define round_down(x, order)							\
		(sizeof(x) == 4 ? round_down32((uint32_t)(x), (order)) :	\
		 sizeof(x) == 2 ? round_down16((uint16_t)(x), (order)) :	\
		 sizeof(x) == 1 ? round_down8 (( uint8_t)(x), (order)) :	\
		(priv_round_down_bad_type(),1))

static inline uint8_t round_down8(uint8_t x, unsigned int order)
{
	return (x & ~((1U << order) - 1));
}

static inline uint16_t round_down16(uint16_t x, unsigned int order)
{
	return (x & ~((1U << order) - 1));
}

static inline uint32_t round_down32(uint32_t x, unsigned int order)
{
	return (x & ~((1UL << order) - 1));
}

ERROR_FUNC(priv_round_up_bad_type, "Invalid type passed to round_up");

/**
 * \brief Round up to the nearest power of two boundary.
 *
 * \param x A positive integer
 * \param order log2 of the required boundary.
 * \return \a x rounded up to the next multiple of (1 << \a order)
 */
#define round_up(x, order)						\
		(sizeof(x) == 4 ? round_up32((uint32_t)(x), (order)) :	\
		 sizeof(x) == 2 ? round_up16((uint16_t)(x), (order)) :	\
		 sizeof(x) == 1 ? round_up8 (( uint8_t)(x), (order)) :	\
		(priv_round_up_bad_type(),1))

static inline uint8_t round_up8(uint8_t x, unsigned int order)
{
	return round_down8(x + (1U << order) - 1, order);
}

static inline uint16_t round_up16(uint16_t x, unsigned int order)
{
	return round_down16(x + (1U << order) - 1, order);
}

static inline uint32_t round_up32(uint32_t x, unsigned int order)
{
	return round_down32(x + (1UL << order) - 1, order);
}

/**
 * \brief Round up to the nearest word-aligned boundary.
 *
 * \param x Address or offset to be word-aligned
 * \return The smallest number \a y where \a y >= \a x and \a y & 3 == 0.
 */
static inline unsigned long word_align(unsigned long x)
{
	return round_up(x, 2);
}

#ifdef CONFIG_PAGE_SIZE
/**
 * Round up to the nearest multiple of #CONFIG_PAGE_SIZE.
 *
 * \param x Address or offset to be page aligned
 * \return The smallest number \a y where \a y >= @a x and
 * \a y % #CONFIG_PAGE_SIZE == 0.
 */
static inline unsigned long page_align(unsigned long x)
{
	return (x + CONFIG_PAGE_SIZE - 1) & ~(CONFIG_PAGE_SIZE - 1);
}
#endif

/**
 * \brief Calculate \f$ \left\lceil \frac{a}{b} \right\rceil \f$ using
 * integer arithmetic.
 *
 * \param a An integer
 * \param b Another integer
 *
 * \return (\a a / \a b) rounded up to the nearest integer.
 */
#define div_ceil(a, b)	(((a) + (b) - 1) / (b))

//! @}

#endif /* UTIL_H_INCLUDED */

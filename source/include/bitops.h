/**
 * \file
 *
 * \brief Generic bit operations
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
#ifndef BITOPS_H_INCLUDED
#define BITOPS_H_INCLUDED

#include <assert.h>
#include <compiler.h>
#include <stdbool.h>
#include <types.h>
#include <util.h>

/**
 * \ingroup utility_group
 * \defgroup bitops_group Bit Operations
 * @{
 *
 * These bit operations are helper functions for accessing individual
 * bits in a bitmaps. Most operations are defined for arbitrary-length
 * bitmaps, but there may be architecture-specific constraints
 * associated with e.g. the atomic operations.
 */

/**
 * \defgroup atomic_bitops_group Atomic Bit Operations
 *
 * Atomic bit operations have the same semantics as the regular bit
 * operations, except that the bitmaps are accessed atomically so it is
 * safe to use atomic bit operations on a bitmap which may also be
 * altered by an interrupt handler, even without disabling interrupt.
 *
 * The architecture-specific implementation of atomic bit operations may
 * disable interrupts internally in order to ensure atomicity, but this
 * is purely an implementation detail.
 */

/**
 * \brief Generate a \a ws-bit mask with only bit \a nr set
 *
 * \param ws The word size, i.e. number of bits that fits in the mask.
 * \param nr The index of the bit that is to be set.
 *
 * \pre \a ws must be a power of two
 */
#define bit_mask(ws, nr)	(1 << ((nr) & ((ws) - 1)))

/**
 * \brief Return the offset of the \a ws-bit word containing bit \a nr
 * in a multi-word bitfield.
 *
 * \param ws The word size in bits
 * \param nr The index of the bit
 *
 * \pre \a ws must be a power of two
 */
#define bit_word(ws, nr)	((nr) / (ws))

/**
 * \brief Set bit \a nr in \a bitmap
 *
 * \a bitmap may be of arbitrary length and type. The caller must ensure
 * that \a nr is within the bounds of the bitmap.
 */
#define set_bit(nr, bitmap)						\
	do {								\
		unsigned int priv_bit_ws = 8 * sizeof(*(bitmap));	\
		unsigned int priv_bit_nr = (nr);			\
		(bitmap)[bit_word(priv_bit_ws, priv_bit_nr)]		\
			|= bit_mask(priv_bit_ws, priv_bit_nr);		\
	} while (0)

/**
 * \brief Clear bit \a nr in \a bitmap
 *
 * \a bitmap may be of arbitrary length and type. The caller must ensure
 * that \a nr is within the bounds of the bitmap.
 */
#define clear_bit(nr, bitmap)						\
	do {								\
		unsigned int priv_bit_ws = 8 * sizeof(*(bitmap));	\
		unsigned int priv_bit_nr = (nr);			\
		(bitmap)[bit_word(priv_bit_ws, priv_bit_nr)]		\
			&= ~bit_mask(priv_bit_ws, priv_bit_nr);		\
	} while (0)

/**
 * \brief Toggle bit \a nr in \a bitmap
 *
 * \a bitmap may be of arbitrary length and type. The caller must ensure
 * that \a nr is within the bounds of the bitmap.
 */
#define toggle_bit(nr, bitmap)						\
	do {								\
		unsigned int priv_bit_ws = 8 * sizeof(*(bitmap));	\
		unsigned int priv_bit_nr = (nr);			\
		(bitmap)[bit_word(priv_bit_ws, priv_bit_nr)]		\
			^= bit_mask(priv_bit_ws, priv_bit_nr);		\
	} while (0)

ERROR_FUNC(priv_test_bit_bad_type, "Invalid type passed to test_bit()");

__always_inline static bool priv_test_bit_ws(unsigned int nr, const void *bp,
		unsigned int ws)
{
	union {
		const uint8_t *b;
		const uint16_t *s;
		const uint32_t *l;
	} p = { bp };

	switch (ws) {
	case 8:
		return 1U & (p.b[bit_word(8, nr)] >> (nr & (8 - 1)));
	case 16:
		return 1U & (p.s[bit_word(16, nr)] >> (nr & (16 - 1)));
	case 32:
		return 1U & (p.l[bit_word(32, nr)] >> (nr & (32 - 1)));
	default:
		priv_test_bit_bad_type();
		return false;
	}
}

/**
 * \brief Test bit \a nr in \a bitmap
 *
 * \a bitmap may be of arbitrary length and type. The caller must ensure
 * that \a nr is within the bounds of the bitmap.
 *
 * \retval true if the bit is set
 * \retval false if the bit is clear
 */
#define test_bit(nr, bitmap)						\
	priv_test_bit_ws(nr, bitmap, 8 * sizeof(*(bitmap)))

#include <arch/bitops.h>

/**
 * \brief Reverse the order of the bits in \a word
 */
#define bit_word_reverse(word)			compiler_brev(word)

/**
 * \brief Find the first bit set in \a word, counting from the LSB
 */
#define bit_word_find_first_one_bit(word)	compiler_ctz(word)

/**
 * \brief Find the last bit set in \a word, counting from the LSB
 */
#define bit_word_find_last_one_bit(word)	(31 - compiler_clz(word))

/**
 * \brief Find the first bit cleared in \a word, counting from the LSB
 */
#define bit_word_find_first_zero_bit(word)		\
	bit_word_find_first_one_bit(~word)

/**
 * \brief Find the last bit cleared in \a word, counting from the LSB
 */
#define bit_word_find_last_zero_bit(word)		\
	bit_word_find_last_one_bit(~word)

/**
 * \brief Find the first bit set in \a bitmap, counting from the LSB of
 * each word.
 *
 * \note Even though \a len does not need to be a multiple of the size
 * of bit_word_t in bits, the caller must ensure that the space
 * allocated for \a bitmap covers an integral number of bit_word_t
 * objects which is enough to hold \a len bits.
 *
 * \param bitmap An array of bit words making up a contiguous bitmap
 * \param len The length of \a bitmap in bits
 *
 * \return The index of the first `1' bit in \a bitmap counting from the
 * LSB of bitmap[0], or a value >= \a len if not found.
 */
static inline unsigned int bit_array_find_first_one_bit(
		const bit_word_t *bitmap, unsigned int len)
{
	unsigned int	i;
	unsigned int	nr_words;

	nr_words = div_ceil(len, 8 * sizeof(*bitmap));

	for (i = 0; i < nr_words; i++) {
		if (bitmap[i])
			return bit_word_find_first_one_bit(bitmap[i]);
	}

	return len;
}

//! @}

#endif /* BITOPS_H_INCLUDED */

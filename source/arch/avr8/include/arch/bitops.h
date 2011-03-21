/**
 * \file
 *
 * \brief AVR-specific implementation of bit operations
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
#ifndef ARCH_BITOPS_H_INCLUDED
#define ARCH_BITOPS_H_INCLUDED

#include <interrupt.h>

/**
 * \addtogroup bitops_group
 * @{
 */

/**
 * \brief Optimum type for bit operations
 *
 * Declaring a bitmap array using this type will ensure that the bitmap
 * is manipulated using the optimum word size for the processor.
 *
 * This is the only type which is guaranteed to work with atomic bit
 * operations.
 */
typedef uint8_t bit_word_t;

/**
 * \addtogroup atomic_bitops_group
 * @{
 */

/**
 * \brief Atomically set bit \a nr in \a bitmap
 */
static inline void atomic_set_bit(unsigned int nr, bit_word_t *bitmap)
{
	irqflags_t	iflags;

	iflags = cpu_irq_save();
	set_bit(nr, bitmap);
	cpu_irq_restore(iflags);
}

/**
 * \brief Atomically clear bit \a nr in \a bitmap
 */
static inline void atomic_clear_bit(unsigned int nr, bit_word_t *bitmap)
{
	irqflags_t	iflags;

	iflags = cpu_irq_save();
	clear_bit(nr, bitmap);
	cpu_irq_restore(iflags);
}

/**
 * \brief Atomically toggle bit \a nr in \a bitmap
 */
static inline void atomic_toggle_bit(unsigned int nr, bit_word_t *bitmap)
{
	irqflags_t	iflags;

	iflags = cpu_irq_save();
	toggle_bit(nr, bitmap);
	cpu_irq_restore(iflags);
}

/**
 * \brief Atomically set bit \a nr in \a bitmap and return its
 * previous state.
 */
static inline bool atomic_test_and_set_bit(unsigned int nr, bit_word_t *bitmap)
{
	bool		is_set;
	irqflags_t	iflags;

	iflags = cpu_irq_save();
	is_set = test_bit(nr, bitmap);
	set_bit(nr, bitmap);
	cpu_irq_restore(iflags);

	return is_set;
}

/**
 * \brief Atomically clear bit \a nr in \a bitmap and return its
 * previous state.
 */
static inline bool atomic_test_and_clear_bit(unsigned int nr,
		bit_word_t *bitmap)
{
	bool		is_set;
	irqflags_t	iflags;

	iflags = cpu_irq_save();
	is_set = test_bit(nr, bitmap);
	clear_bit(nr, bitmap);
	cpu_irq_restore(iflags);

	return is_set;
}

//! @}
//! @}

#endif /* ARCH_BITOPS_H_INCLUDED */

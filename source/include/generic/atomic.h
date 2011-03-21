/**
 * \file
 *
 * \brief Atomic Operations: Generic implementation
 *
 * Architectures which don't have any special mechanisms for atomic
 * operations may include this file after defining the type
 * atomic_value_t to something suitable for the architecture.
 *
 * The generic implementation disables interrupts to achieve atomicity.
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
#ifndef GENERIC_ATOMIC_H_INCLUDED
#define GENERIC_ATOMIC_H_INCLUDED

#include <compiler.h>
#include <interrupt.h>

/**
 * \weakgroup atomic_group
 * @{
 */

/**
 * \struct atomic_object
 * \brief Implementation-specific structure holding an atomic object
 *
 * Do not rely on this structure. Use atomic_t instead.
 */
typedef struct atomic_object {
	//! The value of the atomic object. Do not access directly.
	atomic_value_t	value;
} atomic_t;

static inline atomic_value_t atomic_read(atomic_t *ptr)
{
	barrier();
	return ptr->value;
}

static inline void atomic_write(atomic_t *ptr, atomic_value_t value)
{
	ptr->value = value;
	barrier();
}

static inline atomic_t atomic_add(atomic_t *ptr, atomic_value_t value)
{
	atomic_t	result;
	irqflags_t	iflags;

	iflags = cpu_irq_save();
	result.value = ptr->value + value;
	ptr->value = result.value;
	cpu_irq_restore(iflags);

	return result;
}

static inline atomic_t atomic_sub(atomic_t *ptr, atomic_value_t value)
{
	atomic_t	result;
	irqflags_t	iflags;

	iflags = cpu_irq_save();
	result.value = ptr->value - value;
	ptr->value = result.value;
	cpu_irq_restore(iflags);

	return result;
}

//! @}

#endif /* GENERIC_ATOMIC_H_INCLUDED */

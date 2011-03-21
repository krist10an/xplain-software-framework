/**
 * \file
 *
 * \brief Atomic Operations
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
#ifndef ATOMIC_H_INCLUDED
#define ATOMIC_H_INCLUDED

/**
 * \ingroup utility_group
 * \defgroup atomic_group Atomic Operations
 *
 * Atomic operations allow you to do certain arithmetic
 * read-modify-write operations atomically. The atomic operations
 * guarantee that the memory object being modified will not be altered
 * by any other code while the operation is being carried out.
 *
 * Normally, when incrementing a variable stored in memory, the
 * processor will have to load the current value from memory into a
 * register, increment the value in the register and store the result
 * back into memory. If for example an interrupt comes along while this
 * happens, and the interrupt handler modifies the same variable, the
 * result will usually be wrong since the value stored by the interrupt
 * handler will be ignored. If the variable is incremented using an
 * atomic operation, the read-modify-write operation will happen in its
 * entirety either before or after the interrupt handler runs, thus
 * avoiding this problem.
 *
 * One common way to implement atomic operations is to disable
 * interrupts around the operation. However, some architectures may have
 * specialized instructions which allow interrupts to be enabled while
 * the read-modify-write operation is being carried out. It is therefore
 * recommended to use atomic operations instead of disabling interrupts
 * "manually" wherever possible.
 *
 * @{
 */

#include <arch/atomic.h>

/**
 * \brief Increment the memory object at \a ptr atomically
 * \return The new value stored at \a ptr
 */
#define atomic_inc(ptr)		atomic_add(ptr, 1)
/**
 * \brief Decrement the memory object at \a ptr atomically
 * \return The new value stored at \a ptr
 */
#define atomic_dec(ptr)		atomic_sub(ptr, 1)

/*
 * Documentation for arch-specific types and functions are included
 * below in order to avoid duplication.
 */

/**
 * \typedef struct atomic_object atomic_t
 * \brief An atomic object representing a value which is manipulated
 * atomically.
 */
/**
 * \fn atomic_value_t atomic_read(atomic_t *ptr)
 * \brief Return the value of the atomic object at \a ptr
 */
/**
 * \fn void atomic_write(atomic_t *ptr, atomic_value_t value)
 * \brief Write \a value to the atomic object at \a ptr
 */
/**
 * \fn atomic_t atomic_add(atomic_t *ptr, atomic_value_t value)
 * \brief Atomically add \a value to the value stored at \a ptr
 * \return The result of the addition
 */
/**
 * \fn atomic_t atomic_sub(atomic_t *ptr, atomic_value_t value)
 * \brief Atomically subtract \a value from the value stored at \a ptr
 * \return The result of the subtraction
 */

//! @}

#endif /* ATOMIC_H_INCLUDED */

/**
 * \file
 *
 * \brief Compiler abstraction layer: IAR/AVR32 specifics
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
#ifndef ARCH_COMPILER_IAR_H_INCLUDED
#define ARCH_COMPILER_IAR_H_INCLUDED

/**
 * \weakgroup compiler_group
 * @{
 */

/* These are documented in arch/compiler-gcc.h */
#define avr32_write_sr(x)       __set_system_register(0, x)
#define avr32_set_sr_bit(x)     __set_status_flag(x)
#define avr32_clear_sr_bit(x)   __clear_status_flag(x)

/**
 * \name Atomic Builtin Emulation for IAR
 *
 * These operations are available as builtins on gcc. We rely on these
 * in order to provide support for various atomic operations, so we need
 * to provide a compatible implementation when using IAR as well.
 *
 * All of these functions atomically update the memory location passed
 * as an argument even if interrupts are enabled.
 */
//@{

/**
 * \brief Atomic OR-and-fetch on memory
 *
 * Load a 32-bit word from \a addr, OR it with \a mask and store it back
 * to \a addr.
 *
 * \return The value which was stored to \a addr
 */
static inline unsigned int __sync_or_and_fetch(unsigned int *addr,
		unsigned int mask)
{
	unsigned int word;

	do {
		__set_status_flag(5);
		barrier();
		word = *addr;
		word |= mask;
	} while (!__store_conditional(addr, word));

	barrier();

	return word;
}

/**
 * \brief Atomic AND-and-fetch on memory
 *
 * Load a 32-bit word from \a addr, AND it with \a mask and store it
 * back to \a addr.
 *
 * \return The value which was stored to \a addr
 */
static inline unsigned int __sync_and_and_fetch(unsigned int *addr,
		unsigned int mask)
{
	unsigned int word;

	do {
		__set_status_flag(5);
		barrier();
		word = *addr;
		word &= mask;
	} while (!__store_conditional(addr, word));

	barrier();

	return word;
}

/**
 * \brief Atomic XOR-and-fetch on memory
 *
 * Load a 32-bit word from \a addr, XOR it with \a mask and store it
 * back to \a addr.
 *
 * \return The value which was stored to \a addr
 */
static inline unsigned int __sync_xor_and_fetch(unsigned int *addr,
		unsigned int mask)
{
	unsigned int word;

	do {
		__set_status_flag(5);
		barrier();
		word = *addr;
		word ^= mask;
	} while (!__store_conditional(addr, word));

	barrier();

	return word;
}

/**
 * \brief Atomic fetch-and-OR on memory
 *
 * Load a 32-bit word from \a addr, OR it with \a mask and store it
 * back to \a addr.
 *
 * \return The value stored at \a addr before this function changed it.
 */
static inline unsigned int __sync_fetch_and_or(unsigned int *addr,
		unsigned int mask)
{
	unsigned int word;
	unsigned int old;

	do {
		__set_status_flag(5);
		barrier();
		word = *addr;
		old = word;
		word |= mask;
	} while (!__store_conditional(addr, word));

	barrier();

	return old;
}

/**
 * \brief Atomic fetch-and-AND on memory
 *
 * Load a 32-bit word from \a addr, AND it with \a mask and store it
 * back to \a addr.
 *
 * \return The value stored at \a addr before this function changed it.
 */
static inline unsigned int __sync_fetch_and_and(unsigned int *addr,
		unsigned int mask)
{
	unsigned int word;
	unsigned int old;

	do {
		__set_status_flag(5);
		barrier();
		word = *addr;
		old = word;
		word &= mask;
	} while (!__store_conditional(addr, word));

	barrier();

	return old;
}

/**
 * \brief Atomic fetch-and-XOR on memory
 *
 * Load a 32-bit word from \a addr, XOR it with \a mask and store it
 * back to \a addr.
 *
 * \return The value stored at \a addr before this function changed it.
 */
static inline unsigned int __sync_fetch_and_xor(unsigned int *addr,
		unsigned int mask)
{
	unsigned int word;
	unsigned int old;

	do {
		__set_status_flag(5);
		barrier();
		word = *addr;
		old = word;
		word ^= mask;
	} while (!__store_conditional(addr, word));

	barrier();

	return old;
}

//@}

//! @}

#endif /* ARCH_COMPILER_IAR_H_INCLUDED */

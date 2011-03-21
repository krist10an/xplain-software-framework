/**
 * \file
 *
 * \brief AVR-specific internal interrupt handling
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
#ifndef ARCH_INTC_H_INCLUDED
#define ARCH_INTC_H_INCLUDED

#include <compiler.h>

/**
 * \weakgroup intc_group
 * @{
 */

#define intc_priv_entry_sym(id)		intc_priv_entry_irq##id
#define intc_priv_data_sym(id)		intc_priv_data_irq##id

/* Declaration of interrupts differs between compilers. */
#if defined(__GNUC__) || defined(__DOXYGEN__)

#define INTC_DEFINE_HANDLER(id, handler, level)         \
	extern void *intc_priv_data_sym(id);            \
	extern void __attribute__((__signal__))         \
			intc_priv_entry_sym(id)(void);  \
	void intc_priv_entry_sym(id)(void)              \
	{                                               \
		handler(intc_priv_data_sym(id));        \
	}                                               \
	void *intc_priv_data_sym(id)

#elif defined(__ICCAVR__)

#define IAR_VECTOR(x) (4 * (x - 1))
#define INTC_DEFINE_HANDLER(id, handler, level)         \
	extern void *intc_priv_data_sym(id);            \
	COMPILER_PRAGMA(vector=IAR_VECTOR(id))          \
	__interrupt void intc_priv_entry_sym(id)(void)  \
	{                                               \
		handler(intc_priv_data_sym(id));        \
	}                                               \
	void *intc_priv_data_sym(id)

#endif /* __ICCAVR__ */

#define intc_set_irq_data(id, data)                     \
	do {                                            \
		extern void *intc_priv_data_sym(id);    \
		intc_priv_data_sym(id) = (data);        \
	} while (0)

#define intc_get_irq_data(id, pdata)                    \
	do {                                            \
		extern void *intc_priv_data_sym(id);    \
		*(pdata) = intc_priv_data_sym(id);      \
	} while (0)

#define intc_setup_handler(id, level, data)             \
	do {                                            \
		intc_set_irq_data(id, data);            \
	} while (0)

#define intc_remove_handler(id)                         \
	do {                                            \
		extern void *intc_priv_data_sym(id);    \
		intc_priv_data_sym(id) = 0;             \
	} while (0)

//! @}

#endif /* ARCH_INTC_H_INCLUDED */

/**
 * \file
 *
 * \brief AVR32-specific internal interrupt handling
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

#include <io.h>
#include <util.h>
#include <chip/memory-map.h>
#include <cpu/intc.h>

/**
 * \weakgroup intc_group
 * @{
 */

#define intc_priv_entry_sym(id)		intc_priv_irq##id##_entry
#define intc_priv_entry_sym_str(id)	xstr(intc_priv_irq##id##_entry)
#define intc_priv_data_sym(id)		intc_priv_irq##id##_data
#define intc_priv_data_sym_str(id)	xstr(intc_priv_irq##id##_data)
#define intc_priv_dummy_ref(id)		intc_priv_irq##id##_dummy_ref
#define intc_priv_level_str(level)	xstr(intc_priv_level##level##_return)

/* Doxygen needs to see one definition, doesn't matter which one */
#if defined(__GNUC__) || defined(__DOXYGEN__)

#define intc_priv_entry_section(id)					\
	".asf.text.intc_priv_irq" #id "_entry, \"ax\", @progbits"
#define intc_priv_data_section(id)					\
	".asf.bss.intc_priv_irq" #id "_data, \"aw\", @nobits"
#define intc_priv_entry_size(id)					\
	intc_priv_entry_sym_str(id) ", . - " intc_priv_entry_sym_str(id)
#define intc_priv_data_size(id)						\
	intc_priv_data_sym_str(id) ", . - " intc_priv_data_sym_str(id)

#define INTC_DEFINE_HANDLER(id, handler, level)				\
	extern void intc_priv_entry_sym(id)(void);			\
	extern void *intc_priv_data_sym(id);				\
	static void __used intc_priv_dummy_ref(id)(void)		\
	{								\
		handler(&handler);					\
	}								\
	asm(".section " intc_priv_entry_section(id) "\n"		\
		"	.global	" intc_priv_entry_sym_str(id) "\n"	\
		"	.type	" intc_priv_entry_sym_str(id) ", @function\n" \
		intc_priv_entry_sym_str(id) ":\n"			\
		intc_priv_cpu_save_regs(level)				\
		"	lda.w	r8, " intc_priv_data_sym_str(id) "\n"	\
		"	ld.w	r12, r8[0]\n"				\
		"	rcall	" #handler "\n"				\
		"	rjmp	" intc_priv_level_str(level) "\n"	\
		"	.size	" intc_priv_entry_size(id) "\n"		\
		"	.previous\n"					\
		"	.section " intc_priv_data_section(id) "\n"	\
		"	.global	" intc_priv_data_sym_str(id) "\n"	\
		"	.type	" intc_priv_data_sym_str(id) ", @object\n" \
		intc_priv_data_sym_str(id) ":\n"			\
		"	.long	0\n"					\
		"	.size	" intc_priv_data_size(id) "\n"		\
		"	.previous")

#elif defined(__ICCAVR32__)

#define INTC_DEFINE_HANDLER(id, handler, level)				\
	extern void *intc_priv_data_sym(id);				\
	__interrupt void intc_priv_entry_sym(id)(void)			\
	{								\
		handler(intc_priv_data_sym(id));			\
	}								\
	void *intc_priv_data_sym(id)

#endif /* __ICCAVR32__ */

#define intc_set_irq_data(id, data)					\
	do {								\
		intc_priv_data_sym(id) = (data);			\
	} while (0)

#define intc_get_irq_data(id, pdata)					\
	do {								\
		extern void *intc_priv_data_sym(id);			\
		*(pdata) = intc_priv_data_sym(id);			\
	} while (0)

extern void intc_priv_setup_handler(int irq, void (*entry)(void),
		void **pdata, unsigned int level, void *data);

/**
 * \pre \a level must be between 0 and 3 inclusive.
 * \pre The low-level entry point must be placed less than 16k after EVBA.
 */
#define intc_setup_handler(id, level, data)				\
	do {								\
		extern void intc_priv_entry_sym(id)(void);		\
		extern void *intc_priv_data_sym(id);			\
		intc_priv_setup_handler(id, &intc_priv_entry_sym(id),	\
				&intc_priv_data_sym(id), level, data);	\
	} while (0)

#define intc_remove_handler(id)						\
	do {								\
		extern void *intc_priv_data_sym(id);			\
		intc_priv_data_sym(id) = 0;				\
	} while (0)

/**
 * \brief Get the pending interrupt mask for \a group
 */
static inline uint32_t intc_get_group_requests(unsigned int group)
{
	return mmio_read32((void *)(INTC_BASE + 256 + 4 * group));
}

//! @}

#endif /* ARCH_INTC_H_INCLUDED */

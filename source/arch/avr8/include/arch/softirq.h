/**
 * \file
 *
 * \brief AVR-specific soft interrupt definitions
 *
 * \see softirq_group
 *
 * Most of the softirq-related documentation is in include/softirq.h
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
#ifndef ARCH_SOFTIRQ_H_INCLUDED
#define ARCH_SOFTIRQ_H_INCLUDED

#include <bitops.h>
#include <compiler.h>
#include <interrupt.h>
#include <types.h>

/**
 * \weakgroup softirq_group
 * @{
 */

//! Type for holding the current softirq state (enabled/disabled)
typedef bool softirq_flags_t;

static inline void softirq_poll(void)
{
	assert(!cpu_irq_is_enabled());

	while (1) {
		struct softirq_desc	*desc;
		unsigned int		id;

		id = bit_array_find_first_one_bit(softirq_priv_status,
				SOFTIRQ_NR_IDS);
		if (id >= SOFTIRQ_NR_IDS)
			break;
		clear_bit(id, softirq_priv_status);

		cpu_irq_enable();

		desc = &softirq_priv_table[id];
		assert(desc->handler);
		desc->handler(desc->data);

		cpu_irq_disable();
	}
}

static inline void softirq_disable(void)
{
	/* Nothing to do on AVR since softirqs are run synchronously */
	barrier();
}

static inline void softirq_enable(void)
{
	/* Nothing to do on AVR since softirqs are run synchronously */
	barrier();
}

static inline bool softirq_is_enabled_flags(softirq_flags_t flags)
{
	return flags;
}

static inline bool softirq_is_enabled(void)
{
	/*
	 * Softirq on AVR are always enabled since softirqs are run
	 * synchronously.
	 */
	return true;
}

static inline softirq_flags_t softirq_save(void)
{
	/*
	 * Softirq on AVR are always enabled since softirqs are run
	 * synchronously.
	 */
	return true;
}

static inline void softirq_restore(softirq_flags_t flags)
{
	if (softirq_is_enabled_flags(flags))
		softirq_enable();
}

//! @}

#endif /* ARCH_SOFTIRQ_H_INCLUDED */

/**
 * \file
 *
 * \brief Architecture-independent soft interrupt implementation
 *
 * \see softirq_group
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
#include <softirq.h>

/**
 * \weakgroup softirq_group
 * @{
 */

/**
 * \internal
 * \brief Bitmask indicating the active status of each soft interrupt.
 */
bit_word_t softirq_priv_status[SOFTIRQ_BITMAP_WORDS];

/**
 * \internal
 * \brief Table with a soft interrupt handler for each softirq
 */
struct softirq_desc softirq_priv_table[SOFTIRQ_NR_IDS];

/**
 * \brief Install a soft interrupt handler
 *
 * \param id Soft interrupt number for which to install a handler
 * \param handler Function to be called when soft interrupt is active
 * \param data Arbitrary data to be passed to \a handler
 */
void softirq_set_handler(enum softirq_id id, softirq_handler_t handler,
		void *data)
{
	struct softirq_desc	*desc;

	build_assert(ARRAY_LEN(softirq_priv_table)
			<= 8 * sizeof(softirq_priv_status));
	assert(id < ARRAY_LEN(softirq_priv_table));
	assert(handler);

	desc = &softirq_priv_table[id];
	desc->handler = handler;
	desc->data = data;
}

//! @}

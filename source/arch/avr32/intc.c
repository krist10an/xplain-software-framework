/**
 * \file
 *
 * \brief Internal Interrupt Controller driver
 *
 * This file implements support for setting up an internal interrupt
 * handler using the INTC module.
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
#include <assert.h>
#include <io.h>
#include <intc.h>
#include <linker.h>
#include <stdint.h>
#include <chip/memory-map.h>

#define HANDLER_OFFSET_MASK	0x00ffffff
#define INTLEV_MASK		0x00000003
#define INTLEV_SHIFT		30

void intc_priv_setup_handler(int irq, void (*entry)(void), void **pdata,
		unsigned int level, void *data)
{
	unsigned long	handler_offset;
	uint32_t	intpr;

	/* Do not attempt to initialize the same irq twice */
	assert(*pdata == NULL);

	/* Level must be a number between 0 and 3 inclusive */
	assert(!(level & ~INTLEV_MASK));

	handler_offset = (unsigned long)entry - (unsigned long)_evba;

	/* The low-level handler must not be placed too far from EVBA */
	assert(!(handler_offset & ~HANDLER_OFFSET_MASK));

	intpr = handler_offset;
	intpr |= level << INTLEV_SHIFT;
	mmio_write32((void *)(INTC_BASE + 4 * irq), intpr);
	*pdata = data;
}

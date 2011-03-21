/**
 * \file
 *
 * \brief XMEGA Power Reduction support
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
#include <clk/sys.h>
#include <chip/memory-map.h>
#include <interrupt.h>

/**
 * \weakgroup sysclk_group
 * @{
 */

/**
 * \brief Enable the clock to peripheral \a id on port \a port
 *
 * \param port ID of the port to which the module is connected (one of
 * the SYSCLK_PORT_* definitions).
 * \param id The ID (bitmask) of the peripheral module to be enabled
 */
void sysclk_enable_module(uint8_t port, uint8_t id)
{
	irqflags_t iflags;
	uint8_t mask;

	iflags = cpu_irq_save();
	mask = mmio_read8((void *)(PR_BASE + port));
	mask &= ~id;
	mmio_write8((void *)(PR_BASE + port), mask);
	cpu_irq_restore(iflags);
}

/**
 * \brief Disable the clock to peripheral \a id on port \a port
 *
 * \param port ID of the port to which the module is connected (one of
 * the SYSCLK_PORT_* definitions).
 * \param id The ID (bitmask) of the peripheral module to be disabled
 */
void sysclk_disable_module(uint8_t port, uint8_t id)
{
	irqflags_t iflags;
	uint8_t mask;

	iflags = cpu_irq_save();
	mask = mmio_read8((void *)(PR_BASE + port));
	mask |= id;
	mmio_write8((void *)(PR_BASE + port), mask);
	cpu_irq_restore(iflags);
}

//! @}

/**
 * \file
 *
 * \brief AVR-specific interrupt masking/unmasking
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
#ifndef ARCH_INTERRUPT_H_INCLUDED
#define ARCH_INTERRUPT_H_INCLUDED

#include <compiler.h>
#include <cpu/regs.h>
#include <types.h>

/**
 * \brief Type used for holding the current interrupt state
 */
typedef uint8_t		irqflags_t;

/**
 * \brief Save the current interrupt state and disable interrupts
 * \return A cookie indicating the interrupt state before interrupts
 * were disabled.
 */
__always_inline static irqflags_t cpu_irq_save(void)
{
	irqflags_t flags;

	flags = cpu_read_reg8(SREG);
	cpu_irq_disable();

	return flags;
}

/**
 * \brief Restore saved interrupt state
 * \param flags A cookie returned by cpu_irq_save()
 */
__always_inline static void cpu_irq_restore(irqflags_t flags)
{
	barrier();
	cpu_write_reg8(SREG, flags);
}

/**
 * \brief Test if interrupts are enabled in \a flags
 * \param flags Saved interrupt state
 * \retval true If \a flags indicates interrups are enabled
 * \retval false If \a flags indicates interrupts are disabled
 */
__always_inline static bool cpu_irq_is_enabled_flags(irqflags_t flags)
{
	return (flags & CPUR_SREG_I);
}

/**
 * \brief Test if interrupts are enabled
 * \retval true If interrups are enabled
 * \retval false If interrupts are disabled
 */
#define cpu_irq_is_enabled()			\
	cpu_irq_is_enabled_flags(cpu_read_reg8(SREG))

#endif /* ARCH_INTERRUPT_H_INCLUDED */

/**
 * \file
 *
 * \brief Common code for low-level interrupt entrypoints.
 *
 * This file provides the common return path for all low-level
 * interrupt entrypoints.
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
#include <arch/sysreg.h>

/*
 * If any of the following bits are set in the status register of the
 * return context, soft interrupts are masked.
 */
#define SOFTIRQ_MASK	(SYSREG_SR_GM | SYSREG_SR_IxM | SYSREG_SR_T)

	.section .asf.text.intc_priv_level0_return, "ax", @progbits
	.global	intc_priv_level0_return
	.type	intc_priv_level0_return, @function
intc_priv_level0_return:
	ld.w	r8, sp[0]		// load return context's SR
	cbr	r8, SYSREG_SR_L_BIT	// atomic errata workaround
	st.w	sp[0], r8
#ifdef CONFIG_SOFTIRQ
	andl	r8, lo(SOFTIRQ_MASK)
	andh	r8, hi(SOFTIRQ_MASK)	// if no mask bits set, ...
	breq	handle_softirqs		// ... process softirqs
#endif
.Lret_from_irq:
	rete				// otherwise, return
	.size	intc_priv_level0_return, . - intc_priv_level0_return

#ifdef CONFIG_SOFTIRQ
	.section .asf.text.softirq, "ax", @progbits
	.type	handle_softirqs, @function
handle_softirqs:
	lda.w	r8, softirq_priv_status	// locate softirq "status register"
	ssrf	SYSREG_SR_GM_BIT	// mask interrupts
	ld.w	r12, r8[0]		// check pending softirqs
	cp.w	r12, 0			// if nothing pending, ...
	breq	.Lret_from_irq		// ... return

	mfsr	r10, SYSREG_SR		// read cpu status register
	andh	r10, hi(~SYSREG_SR_IxM)	// clear all IxM bits
	sbr	r10, SYSREG_SR_T_BIT	// mask softirqs
	mtsr	SYSREG_SR, r10		// apply the changes to SR
					// -- interrupts are still masked

	rcall	softirq_priv_do_pending	// process pending softirqs

	rete
	.size	handle_softirqs, . - handle_softirqs

#endif /* CONFIG_SOFTIRQ */

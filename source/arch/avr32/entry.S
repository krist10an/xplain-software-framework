/**
 * \file
 *
 * \brief Low-level startup code and exception vectors for AVR32
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
#include <assembler.h>
#include <arch/sysreg.h>

	/*
	 * To save some space, we use the same entry point for
	 * exceptions and reset. This avoids lots of alignment padding
	 * since the reset vector is always suitably aligned.
	 */
#if defined(__GNUC__)
	.section .avr.text.exception, "ax", @progbits
	.global	_start
	.global	_evba
	.type	_start, @function
	.type	_evba, @function
	.size	_start, 0
#elif defined(__IAR_SYSTEMS_ASM__)
	MODULE	_evba
	RSEG	EVTAB:CODE:ROOT
	PUBLIC	_start
	PUBLIC	_evba
#endif
	EXTERN_SYMBOL(unknown_exception)
_start:
_evba:
	SET_LOC(0x00)
	rjmp	unknown_exception	/* Unrecoverable exception */
	SET_LOC(0x04)
	rjmp	unknown_exception	/* TLB multiple hit */
	SET_LOC(0x08)
	rjmp	unknown_exception	/* Bus error data fetch */
	SET_LOC(0x0c)
	rjmp	unknown_exception	/* Bus error instruction fetch */
	SET_LOC(0x10)
	rjmp	unknown_exception	/* NMI */
	SET_LOC(0x14)
	rjmp	unknown_exception	/* Instruction address */
	SET_LOC(0x18)
	rjmp	unknown_exception	/* ITLB protection */
	SET_LOC(0x1c)
	rjmp	unknown_exception	/* Breakpoint */
	SET_LOC(0x20)
	rjmp	unknown_exception	/* Illegal opcode */
	SET_LOC(0x24)
	rjmp	unknown_exception	/* Unimplemented instruction */
	SET_LOC(0x28)
	rjmp	unknown_exception	/* Privilege violation */
	SET_LOC(0x2c)
	rjmp	unknown_exception	/* Floating-point */
	SET_LOC(0x30)
	rjmp	unknown_exception	/* Coprocessor absent */
	SET_LOC(0x34)
	rjmp	unknown_exception	/* Data Address (read) */
	SET_LOC(0x38)
	rjmp	unknown_exception	/* Data Address (write) */

	END_FUNC(_evba)

	PUBLIC_FUNCTION(unknown_exception)
	EXTERN_SYMBOL(osc_setup)
	/* Figure out whether we're handling an exception (Exception
	 * mode) or just booting (Supervisor mode). */
	csrfcz	SYSREG_SR_MODE_BIT(1)
	brcc	osc_setup

#ifdef CONFIG_EXCEPTION_DEBUG
	/* This is an exception. Complain. */
	pushm	r0-r12
	sub	r8, sp, -15 * 4
	mov	r9, lr
	pushm	r8-r9
	mfsr	r12, SYSREG_ECR
	mov	r11, sp
	rcall	do_unknown_exception
#endif
L(exception_loop):
	rjmp	L(exception_loop)
	END_FUNC(unknown_exception)
	END_FILE();

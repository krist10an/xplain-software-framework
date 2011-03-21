/**
 * \file
 *
 * \brief Runtime system initialization
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

	PUBLIC_FUNCTION(avr32_runtime_init)
	EXTERN_SYMBOL(main)
	EXTERN_SYMBOL(_evba)

	/* Set up the exception handlers */
	ld_addr	r1, _evba
	mtsr	SYSREG_EVBA, r1

	/* Copy the .data section and zero out the .bss section. */
	ld_addr	r8, _etext
	ld_addr	r9, _data
	ld_addr	r10, _edata
	cp.w	r9, r10
	breq	L(copy_data_done)

L(copy_data_loop):
	ldm	r8++, r0-r7
	stm	r9, r0-r7
	sub	r9, -32
	cp.w	r9, r10
	brlt	L(copy_data_loop)

L(copy_data_done):
	ld_addr	r8, _ebss
	mov	r0, 0
	mov	r1, 0
	mov	r2, 0
	mov	r3, 0
	cp.w	r9, r8
	breq	L(copy_bss_done)

L(copy_bss_loop):
	stm	r9, r0-r3
	sub	r9, -16
	cp.w	r9, r8
	brlt	L(copy_bss_loop)

	/* Initialize the stack pointer */
L(copy_bss_done):
	ld_addr	sp, _estack

	/* Run the application */
	call	main
L(exit_loop):
	rjmp	L(exit_loop)

	END_FUNC(avr32_runtime_init)

#ifdef __IAR_SYSTEMS_ASM__
	/* I have no idea what this is for */
	PUBLIC	?need_segment_init
?need_segment_init:
#endif

	/* IAR totally blows at this */
#ifndef __IAR_SYSTEMS_ASM__
	/* If oscillator setup isn't needed, run avr32_runtime_init directly */
	WEAK_FUNCTION_ALIAS(osc_setup, avr32_runtime_init)
#endif

#ifdef CONFIG_NANOTRACE_SIZE
	.section .nanotrace, "aw", @nobits
	.global	NANOTRACE
	.type	NANOTRACE, @object
NANOTRACE:
	.fill	CONFIG_NANOTRACE_SIZE
	.size	NANOTRACE, . - NANOTRACE
#endif

	END_FILE()

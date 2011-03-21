/**
 * \file
 *
 * \brief Low-level startup code for the AVR architecture
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

	.macro	irq_vector id
	.if	\id < (CONFIG_NR_IRQS + 1)
	.org	(\id - 1) * 4
	.global	intc_priv_entry_irq\id
	.weak	intc_priv_entry_irq\id
	.set	intc_priv_entry_irq\id, intc_priv_unhandled_irq
	jmp	intc_priv_entry_irq\id
	.endif
	.endm

	.section .vectors, "ax", @progbits
	.global	_start
	.type	_start, @function
_start:
	rjmp	avr_runtime_init
	irq_vector 2
	irq_vector 3
	irq_vector 4
	irq_vector 5
	irq_vector 6
	irq_vector 7
	irq_vector 8
	irq_vector 9
	irq_vector 10
	irq_vector 11
	irq_vector 12
	irq_vector 13
	irq_vector 14
	irq_vector 15
	irq_vector 16
	irq_vector 17
	irq_vector 18
	irq_vector 19
	irq_vector 20
	irq_vector 21
	irq_vector 22
	irq_vector 23
	irq_vector 24
	irq_vector 25
	irq_vector 26
	irq_vector 27
	irq_vector 28
	irq_vector 29
	irq_vector 30
	irq_vector 31
	irq_vector 32
	irq_vector 33
	irq_vector 34
	irq_vector 35
	irq_vector 36
	irq_vector 37
	irq_vector 38
	irq_vector 39
	irq_vector 40
	irq_vector 41
	irq_vector 42
	irq_vector 43
	irq_vector 44
	irq_vector 45
	irq_vector 46
	irq_vector 47
	irq_vector 48
	irq_vector 49
	irq_vector 50
	irq_vector 51
	irq_vector 52
	irq_vector 53
	irq_vector 54
	irq_vector 55
	irq_vector 56
	irq_vector 57
	irq_vector 58
	irq_vector 59
	irq_vector 60
	irq_vector 61
	irq_vector 62
	irq_vector 63
	irq_vector 64
	irq_vector 65
	irq_vector 66
	irq_vector 67
	irq_vector 68
	irq_vector 69
	irq_vector 70
	irq_vector 71
	irq_vector 72
	irq_vector 73
	irq_vector 74
	irq_vector 75
	irq_vector 76
	irq_vector 77
	irq_vector 78
	irq_vector 79
	irq_vector 80
	irq_vector 81
	irq_vector 82
	irq_vector 83
	irq_vector 84
	irq_vector 85
	irq_vector 86
	irq_vector 87
	irq_vector 88
	irq_vector 89
	irq_vector 90
	irq_vector 91
	irq_vector 92
	irq_vector 93
	irq_vector 94
	irq_vector 95
	irq_vector 96
	irq_vector 97
	irq_vector 98
	irq_vector 99
	irq_vector 100
	irq_vector 101
	irq_vector 102
	irq_vector 103
	irq_vector 104
	irq_vector 105
	irq_vector 106
	irq_vector 107
	irq_vector 108
	irq_vector 109
	irq_vector 110
	irq_vector 111
	irq_vector 112
	irq_vector 113
	irq_vector 114
	irq_vector 115
	irq_vector 116
	irq_vector 117
	irq_vector 118
	irq_vector 119
	irq_vector 120
	irq_vector 121
	irq_vector 122
	irq_vector 123
	irq_vector 124
	irq_vector 125
	irq_vector 126
	irq_vector 127
	irq_vector 128
	.size	_start, . - _start

	.section .text.intc_priv_unhandled_irq, "ax", @progbits
	.global	intc_priv_unhandled_irq
	.type	intc_priv_unhandled_irq, @function
intc_priv_unhandled_irq:
	jmp	_start
	.size	intc_priv_unhandled_irq, . - intc_priv_unhandled_irq

	.section .avr.text.avr_runtime_init, "ax", @progbits
	.type	avr_runtime_init, @function
avr_runtime_init:
	clr	r1			; r1 is expected to contain 0
	out	0x3f, r1		; clear status register
	ldi	r26, lo8(_estack - 1)
	ldi	r27, hi8(_estack - 1)
	out	0x3e, r27
	out	0x3d, r26

	; On XMEGA the interrupt levels are disabled by default. Enable the
	; levels, but keep the global interrupt flag cleared.
#if defined(CONFIG_CPU_XMEGA)
#include <chip/memory-map.h>
#include <pmic.h>
	ldi     r28, lo8(PMIC_BASE + PMIC_CTRL)
	ldi     r29, hi8(PMIC_BASE + PMIC_CTRL)
	ld      r16, Y
	ldi     r17, PMIC_CTRL_LOLVLEN | PMIC_CTRL_MEDLVLEN | PMIC_CTRL_HILVLEN
	or      r16, r17
	st      Y, r16
#endif

	; Copy .data section
	out	0x3b, r1
	ldi	r30, lo8(_etext)
	ldi	r31, hi8(_etext)
	ldi	r16, hh8(_etext)
	out	0x3B, r16
	ldi	r26, lo8(_data)
	ldi	r27, hi8(_data)
	ldi	r28, lo8(_edata)
	ldi	r29, hi8(_edata)
	rjmp	2f
1:	elpm	r0, Z+
	st	X+, r0
2:	cp	r26, r28
	cpc	r27, r29
	brne	1b

	clr	r16
	out	0x3B, r16		;reset the RAMPZ to zero

	; Clear .bss section
	ldi	r28, lo8(_end)
	ldi	r29, hi8(_end)
	rjmp	2f
1:	st	X+, r1
2:	cp	r26, r28
	cpc	r27, r29
	brne	1b

	call	main
1:	rjmp	1b
	.size	avr_runtime_init, . - avr_runtime_init

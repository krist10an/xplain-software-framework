/**
 * \file
 *
 * \brief Optimized memset() implementation for AVR32
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

#define s r12
#define c r11
#define n r10

	PUBLIC_FUNCTION(memset)

	cp.w	n, 32
	mov	r9, s
	brge	L(large_memset)

	sub	n, 1
	retlt	s
L(small_memset_loop):
	st.b	s++, c
	sub	n, 1
	brge	L(small_memset_loop)

	retal	r9

L(large_memset):
	mov	r8, r11
	mov	r11, 3
	bfins	r8, r8, 8, 8
	bfins	r8, r8, 16, 16
	tst	s, r11
	breq	L(ptr_aligned)

L(initial_byte_loop):
	st.b	s++, r8
	sub	n, 1
	tst	s, r11
	brne	L(initial_byte_loop)

L(ptr_aligned):
	mov	r11, r9
	mov	r9, r8
	sub	n, 8

L(dword_loop):
	st.d	s++, r8
	sub	n, 8
	brge	L(dword_loop)

	/* If we are done, n == -8 and we'll skip all st.b insns below */
	neg	n
	lsl	n, 1
	add	pc, n
	REPEAT(7)
	st.b	s++, r8
	END_REPEAT()
	retal	r11

	END_FUNC(memset)
	END_FILE()

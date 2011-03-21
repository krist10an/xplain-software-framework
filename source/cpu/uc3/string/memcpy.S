/**
 * \file
 *
 * \brief Optimized memcpy() implementation for AVR32 UC3
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

/* Don't use r12 as dst since we must return it unmodified */
#define dst r9
#define src r11
#define len r10

	PUBLIC_FUNCTION(memcpy)

	/* If we have less than 8 bytes, don't do anything fancy */
	mov	dst, r12
	cp.w	len, 8
	brlt	L(byte_copy)

	/* If src and dst have different alignment, we must do a
	 * byte-for-byte copy */
	eor	r8, r11, r12
	andl	r8, 3
	brne	L(byte_copy)

	/* If src is unaligned, dst must be equally unaligned */
	mov	r8, src
	andl	r8, 3
	brne	L(unaligned)

L(aligned):
	pushm	r0-r7

	sub	len, 32
	brlt	L(less_than_32)
L(aligned_copy_loop):
	ldm	src, r0-r7
	sub	src, -32
	stm	dst, r0-r7
	sub	dst, -32
	sub	len, 32
	brge	L(aligned_copy_loop)

L(less_than_32):
	/* Copy 16 more bytes if possible */
	sub	len, -16
	brlt	L(less_than_16)
	ldm	src, r0-r3
	sub	src, -16
	sub	len, 16
	stm	dst, r0-r3
	sub	dst, -16

L(less_than_16):
	/* Do the remaining, if anything, as byte copies */
	sub	len, -16
	popm	r0-r7

L(byte_copy):
	sub	len, 1
	retlt	r12
L(byte_copy_loop):
	ld.ub	r8, src++
	st.b	dst++, r8
	sub	len, 1
	brge	L(byte_copy_loop)
	retal	r12

L(unaligned):
	/* Make src 8-byte aligned */
	mov	r8, 7
	st.w	--sp, r0
	and	r8, src
	rsub	r8, r8, 8
	sub	len, r8
L(unaligned_copy_loop):
	ld.ub	r0, src++
	st.b	dst++, r0
	sub	r8, 1
	brne	L(unaligned_copy_loop)
	ld.w	r0, sp++
	rjmp	L(aligned)

	END_FUNC(memcpy)
	END_FILE()

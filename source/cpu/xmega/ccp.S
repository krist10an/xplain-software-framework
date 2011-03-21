/**
 * \file
 *
 * \brief Configuration Change Protection
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
#include <cpu/regs.h>

	/*
	 * GNU and IAR use different calling conventions. Since this is
	 * a very small and simple function to begin with, it's easier
	 * to implement it twice than to deal with the differences
	 * within a single implementation.
	 *
	 * Interrupts are disabled by hardware during the timed
	 * sequence, so there's no need to save/restore interrupt state.
	 */

	PUBLIC_FUNCTION(mmio_ccp_write8)

#if defined(__GNUC__)

	ST_CPUREG(r1, RAMPZ)            // Reset bits 23:16 of Z
	movw    r30, r24                // Load addr into Z
	ldi     r18, CCP_IOREG          // Load magic CCP value
	ST_CPUREG(r18, CCP)             // Start CCP handshake
	st      Z, r22                  // Write value to I/O register
	ret                             // Return to caller

#elif defined(__IAR_SYSTEMS_ASM__)

	ldi     r20, 0
	ST_CPUREG(r20, RAMPZ)           // Reset bits 23:16 of Z
# if defined(CONFIG_MEMORY_MODEL_TINY)
	mov     r31, r20                // Reset bits 8:15 of Z
	mov     r30, r16                // Load addr into Z
# else
	movw    r30, r16                // Load addr into Z
# endif
	ldi     r21, CCP_IOREG          // Load magic CCP value
	ST_CPUREG(r21, CCP)             // Start CCP handshake
# if defined(CONFIG_MEMORY_MODEL_TINY)
	st      Z, r17                  // Write value to I/O register
# elif defined(CONFIG_MEMORY_MODEL_SMALL)
	st      Z, r18                  // Write value to I/O register
# elif defined(CONFIG_MEMORY_MODEL_LARGE)
	st      Z, r19                  // Write value to I/O register
# else
#  error Unknown memory model in use, no idea how registers should be accessed
# endif
	ret                             // Return to caller

#else
# error Unknown assembler
#endif

	END_FUNC(mmio_ccp_write8)
	END_FILE()

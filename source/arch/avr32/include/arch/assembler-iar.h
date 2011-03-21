/**
 * \file
 *
 * \brief Assembler abstraction layer: IAR AVR32 Assembler specifics
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
#ifndef ARCH_ASSEMBLER_IAR_H_INCLUDED
#define ARCH_ASSEMBLER_IAR_H_INCLUDED

/* Declare all the segments we use below */
	RSEG    DATA21_ID:CONST:NOROOT
	RSEG    DATA21_I:CONST:NOROOT
	RSEG    DATA32_I:DATA:NOROOT
	RSEG    DATA32_N:DATA:NOROOT
	RSEG    SSTACK:DATA:NOROOT

/* These need to be carefully synced up with the linker script */
#define _etext  SFB(DATA21_ID)
#define _data   SFB(DATA21_I)
#define _edata  SFE(DATA32_I)
#define _ebss   SFE(DATA32_N)
#define _estack SFE(SSTACK)

#endif /* ARCH_ASSEMBLER_IAR_H_INCLUDED */

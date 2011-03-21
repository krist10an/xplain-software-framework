/**
 * \file
 *
 * \brief Compiler abstraction layer: sparse specifics
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
#ifndef COMPILER_SPARSE_H_INCLUDED
#define COMPILER_SPARSE_H_INCLUDED

/* sparse understands most gcc attributes */
#include <compiler/gcc.h>

#define __bitwise	__attribute__((bitwise))
#define __virtual	/* default address space */
#define __physical	__attribute__((noderef, address_space(1)))
#define __force		__attribute__((force))

/*
 * Sparse doesn't know these builtins, so declare them as regular
 * functions to avoid warnings.
 */
unsigned short __builtin_bswap_16(unsigned short x);
unsigned long __builtin_bswap_32(unsigned long x);
unsigned int __builtin_clz(unsigned int x);
unsigned long __builtin_mfsr(unsigned long addr);
void __builtin_mtsr(unsigned long addr, unsigned long value);
void __builtin_tlbw(void);
void __builtin_cache(void *addr, int op);
void __builtin_sync(unsigned int unused);

#endif /* COMPILER_SPARSE_H_INCLUDED */

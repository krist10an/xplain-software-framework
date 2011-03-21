/**
 * \file
 *
 * \brief Linker script definitions
 *
 * This file contains common linker script elements which are the same
 * regardless of how the image ends up being booted.
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
#ifndef LINKER_H_INCLUDED
#define LINKER_H_INCLUDED

#include <chip/memory-map.h>

#ifndef __ASSEMBLY__

/**
 * \brief Exception Vector Base Address
 *
 * This symbol marks the beginning of the exception handler table.
 */
extern void _evba(void);

/**
 * \brief Start address of the stack
 *
 * This symbol is located at the lowest address usable as stack. The
 * dynamically allocatable memory (heap area) will be below this
 * address.
 */
#if defined(__ICCAVR32__)
__no_init static const unsigned char stack_area[CONFIG_STACK_SIZE] @ "SSTACK";
#elif defined(__ICCAVR__)
__no_init static const unsigned char stack_area[CONFIG_CSTACK_SIZE] @ "CSTACK";
#else
extern unsigned char stack_area[];
#endif

/**
 * \brief The end of the static data.
 *
 * This symbol marks the end of the statically allocated data in RAM.
 */
#if defined(__ICCAVR32__)
__no_init static const unsigned char heap_area[1] @ "HEAP";
#elif defined(__ICCAVR__)
__no_init static const unsigned char heap_area[1] @ "NEAR_HEAP";
#else
extern unsigned char heap_area[];
#endif

#endif /* __ASSEMBLY__ */

/*
 * The startup code copies 32-byte chunks, so we need to align the
 * .data section on a 32-byte boundary.
 */
#if DATA_SRAM_BASE < 32
# define DATA_VMA	32
#else
# define DATA_VMA	DATA_SRAM_BASE
#endif

#ifdef CONFIG_APPLICATION_LMA
# define TEXT_LMA	CONFIG_APPLICATION_LMA
# define TEXT_VMA	CACHEABLE_VMA(TEXT_LMA)
#else
# define TEXT_LMA	CHIP_ENTRY_LMA
# define TEXT_VMA	CHIP_ENTRY_VMA
#endif

#endif /* LINKER_H_INCLUDED */

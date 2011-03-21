/**
 * \file
 *
 * \brief Program memory access for 8-bit AVR
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
#ifndef ARCH_PROGMEM_H_INCLUDED
#define ARCH_PROGMEM_H_INCLUDED

#include <stdint.h>

/**
 * \weakgroup progmem_group
 * @{
 */

/**
 * \internal
 * \defgroup progmem_avr8_group Progmem Implementation for 8-bit AVR
 *
 * 8-bit AVR is a Harvard architecture, so program memory can not be
 * accessed as normal data. However, there are special instructions
 * available for accessing program memory, so it is still possible to
 * support it through dedicated accessors.
 *
 * Unfortunately, program memory access is highly compiler-specific.
 */

#if defined(__GNUC__)
# define __progmem                      __attribute__((__progmem__))
# define __progmem_arg
# define DECLARE_PROGMEM(type, name)    const type name __progmem
# define DEFINE_PROGMEM(type, name)     const type name __progmem

static inline uint8_t progmem_read8(const uint8_t *p)
{
	uint8_t value;

	asm("lpm %0, Z" : "=r"(value) : "z"(p));

	return value;
}

static inline uint16_t progmem_read16(const uint16_t *p)
{
	uint16_t value;

	asm("lpm %A0, Z+\n\t"
		"lpm %B0, Z"
		: "=&r"(value), "+z"(p));

	return value;
}

static inline uint32_t progmem_read32(const uint32_t *p)
{
	uint32_t value;

	asm("lpm %A0, Z+\n\t"
		"lpm %B0, Z+\n\t"
		"lpm %C0, Z+\n\t"
		"lpm %D0, Z"
		: "=&r"(value), "+z"(p));

	return value;
}

#elif defined(__ICCAVR__)

# if defined(CONFIG_MEMORY_MODEL_TINY) || defined(CONFIG_MEMORY_MODEL_SMALL)
#  define __progmem                      __flash
#  define __progmem_arg                  __flash
# elif defined(CONFIG_MEMORY_MODEL_LARGE)
#  define __progmem                      __farflash
#  define __progmem_arg                  __farflash
# endif

# define DECLARE_PROGMEM(type, name)    const __progmem type name
# define DEFINE_PROGMEM(type, name)     const __progmem type name

static inline uint8_t progmem_read8(const uint8_t __progmem *p)
{
	return *p;
}

static inline uint16_t progmem_read16(const uint16_t __progmem *p)
{
	return *p;
}

static inline uint32_t progmem_read32(const uint32_t __progmem *p)
{
	return *p;
}

#elif defined(__DOXYGEN__)

/*
 * Basically this is a copy of GCCs implementation, tuned for making doxygen
 * not output warnings while generating the documentation.
 */

# define __progmem                      __attribute__((__progmem__))
# define __progmem_arg
# define DECLARE_PROGMEM(type, name)    const type name __progmem
# define DEFINE_PROGMEM(type, name)     const type name __progmem

static inline uint8_t progmem_read8(const uint8_t __progmem *p)
{
}

static inline uint16_t progmem_read16(const uint16_t __progmem *p)
{
}

static inline uint32_t progmem_read32(const uint32_t __progmem *p)
{
}

#endif /* __DOXYGEN__ */

//! @}

#endif /* ARCH_PROGMEM_H_INCLUDED */

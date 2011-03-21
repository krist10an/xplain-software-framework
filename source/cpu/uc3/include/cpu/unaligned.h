/**
 * \file
 *
 * \brief Portable unaligned access helpers: AVR32 UC3 implementation
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
#ifndef CPU_UNALIGNED_H_INCLUDED
#define CPU_UNALIGNED_H_INCLUDED

#include <compiler.h>
#include <stdint.h>

/**
 * \weakgroup unaligned_group
 * @{
 */

__always_inline static uint16_t read_unaligned_16(const void *ptr)
{
	const uint8_t	*p = ptr;

	return (p[0] << 8) | p[1];
}

__always_inline static uint32_t read_unaligned_32(const void *ptr)
{
	const uint8_t	*p = ptr;

	return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}

__always_inline static void write_unaligned_16(void *ptr, uint16_t value)
{
	uint8_t	*p = ptr;

	p[0] = value >> 8;
	p[1] = value;
}

__always_inline static void write_unaligned_32(void *ptr, uint32_t value)
{
	uint8_t	*p = ptr;

	p[0] = value >> 24;
	p[1] = value >> 16;
	p[2] = value >> 8;
	p[3] = value;
}

#define read_unaligned_be16(ptr)	be16_to_cpu(read_unaligned_16(ptr))
#define read_unaligned_be32(ptr)	be32_to_cpu(read_unaligned_32(ptr))
#define write_unaligned_be16(ptr, value)			\
	write_unaligned_16(ptr, cpu_to_be16(value))
#define write_unaligned_be32(ptr, value)			\
	write_unaligned_32(ptr, cpu_to_be32(value))

#define read_unaligned_le16(ptr)	le16_to_cpu(read_unaligned_16(ptr))
#define read_unaligned_le32(ptr)	le32_to_cpu(read_unaligned_32(ptr))
#define write_unaligned_le16(ptr, value)			\
	write_unaligned_16(ptr, cpu_to_le16(value))
#define write_unaligned_le32(ptr, value)			\
	write_unaligned_32(ptr, cpu_to_le32(value))

//! @}

#endif /* CPU_UNALIGNED_H_INCLUDED */

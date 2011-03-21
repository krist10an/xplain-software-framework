/**
 * \file
 *
 * \brief Byte order conversion helpers
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
#ifndef BYTEORDER_H_INCLUDED
#define BYTEORDER_H_INCLUDED

#include <compiler.h>
#include <types.h>

#include <arch/byteorder.h>

/**
 * \ingroup utility_group
 * \defgroup byte_order_group Byte Order Conversion
 *
 * When communicating with the world outside the CPU on which the code
 * is running, it is important that multi-byte values are interpreted
 * the same way by all parties. Unfortunately, the CPUs of the world do
 * interpret things differently unless special care is taken by the
 * programmer; the vast majority of CPUs can be divided into two groups,
 * depending on how multi-byte values are interpreted:
 *
 *   - Big Endian: The most significant byte comes first
 *   - Little Endian: The least significant byte comes first
 *
 * Every time two processors need to communicate, or need to access the
 * same on-disk filesystem, or share any data in any way, the byte
 * ordering (or \em endianness) must be clearly defined, and all
 * processors must accept this regardless of their native byte ordering.
 *
 * This module provides a set of helper functions which can be used to
 * read and write data using a pre-defined byte order, regardless of the
 * native byte ordering of the CPU. Byte swapping is performed as
 * necessary, but if the pre-defined byte order matches the native byte
 * order, no extra overhead is added.
 *
 * \par Example
 * In the following example, a packet is received over some kind of
 * network from some other, unknown CPU. The network protocol dictates
 * that the sequence number is transfered as little endian (i.e. least
 * significant byte first). The function get_seq_number() below will
 * return the correct sequence number regardless of the CPU's native
 * byte order.
 * \code
struct packet {
	le32_t seq_number;
	le16_t size;
	uint8_t data[];
};

uint32_t get_seq_number(const struct packet *pkt)
{
	return le32_to_cpu(pkt->seq_number);
} \endcode
 *
 * Note that the helper functions in this module is the only legal way
 * to convert between endian-aware types and native types. For example,
 * to convert from a le16_t variable to a uint16_t variable, the
 * function le16_to_cpu() must be used.
 *
 * @{
 */

//! \name Byte Swapping
//@{
/**
 * \def swab32(x)
 * \brief Swap the order of bytes in a 32-bit word
 */
/**
 * \def swab16(x)
 * \brief Swap the order of bytes in a 16-bit word
 */
//@}

//! \name Conversion to/from Little Endian values
//@{
/**
 * \def LE16(x)
 * \brief Create a 16-bit little endian constant
 */
/**
 * \def LE32(x)
 * \brief Create a 32-bit little endian constant
 */
/**
 * \fn uint16_t le16_to_cpu(le16_t x)
 * \brief Convert a 16-bit word from little endian to native byte order
 */
/**
 * \fn uint32_t le32_to_cpu(le32_t x)
 * \brief Convert a 32-bit word from little endian to native byte order
 */
/**
 * \fn le16_t cpu_to_le16(uint16_t x)
 * \brief Convert a 16-bit word from native to little endian byte order
 */
/**
 * \fn le32_t cpu_to_le32(uint32_t x)
 * \brief Convert a 32-bit word from native to little endian byte order
 */
//@}

//! \name Conversion to/from Big Endian values
//@{
/**
 * \def BE16(x)
 * \brief Create a 16-bit big endian constant
 */
/**
 * \def BE32(x)
 * \brief Create a 32-bit big endian constant
 */
/**
 * \fn uint16_t be16_to_cpu(be16_t x)
 * \brief Convert a 16-bit word from big endian to native byte order
 */
/**
 * \fn uint32_t be32_to_cpu(be32_t x)
 * \brief Convert a 32-bit word from big endian to native byte order
 */
/**
 * \fn be16_t cpu_to_be16(uint16_t x)
 * \brief Convert a 16-bit word from native to big endian byte order
 */
/**
 * \fn be32_t cpu_to_be32(uint32_t x)
 * \brief Convert a 32-bit word from native to big endian byte order
 */
//@}

#ifndef swab32
# define swab32(x)					\
	((((x) & 0xff) << 24) | (((x) & 0xff00) << 8)	\
	 | (((x) >> 8) & 0xff00) | (((x) >> 24) & 0xff))
#endif
#ifndef swab16
# define swab16(x)	((((x) & 0xff) << 8) | (((x) >> 8) & 0xff))
#endif

#ifdef CPU_IS_BIG_ENDIAN

#define LE16(x)		((le16_t __force)((((x) & 0xff) << 8)		\
					| (((x) >> 8) & 0xff)))
#define LE32(x)		((le32_t __force)((((x) & 0xff) << 24)		\
					| (((x) & 0xff00) << 8)		\
					| (((x) >> 8) & 0xff00)		\
					| (((x) >> 24) & 0xff)))
#define BE16(x)		((be16_t __force)(x))
#define BE32(x)		((be32_t __force)(x))

static inline uint16_t le16_to_cpu(le16_t x)
{
	return swab16((uint16_t __force)x);
}
static inline uint32_t le32_to_cpu(le32_t x)
{
	return swab32((uint32_t __force)x);
}
static inline le16_t cpu_to_le16(uint16_t x)
{
	return (le16_t __force)swab16(x);
}
static inline le32_t cpu_to_le32(uint32_t x)
{
	return (le32_t __force)swab32(x);
}

static inline uint16_t be16_to_cpu(be16_t x)
{
	return (uint16_t __force)x;
}
static inline uint32_t be32_to_cpu(be32_t x)
{
	return (uint32_t __force)x;
}
static inline be16_t cpu_to_be16(uint16_t x)
{
	return (be16_t __force)x;
}
static inline be32_t cpu_to_be32(uint32_t x)
{
	return (be32_t __force)x;
}

#else /* BIG_ENDIAN */

#define LE16(x)		((le16_t __force)(x))
#define LE32(x)		((le32_t __force)(x))
#define BE16(x)		((be16_t __force)((((x) & 0xff) << 8)		\
					| (((x) >> 8) & 0xff)))
#define BE32(x)		((be32_t __force)((((x) & 0xff) << 24)		\
					| (((x) & 0xff00) << 8)		\
					| (((x) >> 8) & 0xff00)		\
					| (((x) >> 24) & 0xff)))

static inline uint16_t le16_to_cpu(le16_t x)
{
	return (uint16_t __force)x;
}
static inline uint32_t le32_to_cpu(le32_t x)
{
	return (uint32_t __force)x;
}
static inline le16_t cpu_to_le16(uint16_t x)
{
	return (le16_t __force)x;
}
static inline le32_t cpu_to_le32(uint32_t x)
{
	return (le32_t __force)x;
}

static inline uint16_t be16_to_cpu(be16_t x)
{
	return swab16((uint16_t __force)x);
}
static inline uint32_t be32_to_cpu(be32_t x)
{
	return swab32((uint32_t __force)x);
}
static inline be16_t cpu_to_be16(uint16_t x)
{
	return (be16_t __force)swab16(x);
}
static inline be32_t cpu_to_be32(uint32_t x)
{
	return (be32_t __force)swab32(x);
}

#endif /* BIG_ENDIAN */

//! @}

#endif /* BYTEORDER_H_INCLUDED */

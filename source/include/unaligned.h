/**
 * \file
 *
 * \brief Portable unaligned access helpers
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
#ifndef UNALIGNED_H_INCLUDED
#define UNALIGNED_H_INCLUDED

/**
 * \ingroup utility_group
 * \defgroup unaligned_group Unaligned Access Helpers
 *
 * Different CPUs may have different addressing limitations on memory
 * accesses. In particular, many CPUs cannot load a 16-bit value from an
 * address which isn't a multiple of two and similarly for larger
 * values. The helper functions and macros defined here will ensure that
 * the optimum method of accessing unaligned variables is selected based
 * on the target platform.
 *
 * Since unaligned fields are commonly found in protocol-mandated
 * structures, such fields may often need to be accessed using
 * non-native endian as well, so endian-aware versions of the accessors
 * are also included.
 *
 * \par Example
 * Consider a file system which stores the inode number and file size at
 * some arbitrary offset within a block of data. Both numbers are stored
 * in little endian byte order regardless of the CPU, so that disks,
 * memory cards, etc. created on one system may be easily read on
 * another.
 *
 * \par
 * The code below will access those numbers correctly regardless of the
 * CPU's byte order and addressing limitations.
 * \code
uint16_t read_inode_number(const uint8_t *data, unsigned int offset)
{
	return read_unaligned_le16(&data[offset]);
}

void write_file_size(uint8_t *data, unsigned int offset, uint32_t size)
{
	write_unaligned_le16(&data[offset], size);
} \endcode
 *
 * \see byte_order_group
 *
 * @{
 */

#include <cpu/unaligned.h>

//! \name Reading and Writing Unaligned Native Endian Values
//@{
/**
 * \fn uint16_t read_unaligned_16(const void *ptr)
 * \brief Read 16-bit value from a possibly unaligned address
 */
/**
 * \fn uint32_t read_unaligned_32(const void *ptr)
 * \brief Read 32-bit value from a possibly unaligned address
 */
/**
 * \fn void write_unaligned_16(void *ptr, uint16_t value)
 * \brief Write 16-bit value to a possibly unaligned address
 */
/**
 * \fn void write_unaligned_32(void *ptr, uint32_t value)
 * \brief Write 32-bit value to a possibly unaligned address
 */
//@}

//! \name Reading and Writing Unaligned Big Endian Values
//@{
/**
 * \def read_unaligned_be16
 * \brief Read 16-bit Big Endian value from a possibly unaligned address
 */
/**
 * \def read_unaligned_be32
 * \brief Read 32-bit Big Endian value from a possibly unaligned address
 */
/**
 * \def write_unaligned_be16
 * \brief Write 16-bit Big Endian value to a possibly unaligned address
 */
/**
 * \def write_unaligned_be32
 * \brief Write 32-bit Big Endian value to a possibly unaligned address
 */
//@}

//! \name Reading and Writing Unaligned Little Endian Values
//@{
/**
 * \def read_unaligned_le16
 * \brief Read 16-bit Little Endian value from a possibly unaligned address
 */
/**
 * \def read_unaligned_le32
 * \brief Read 32-bit Little Endian value from a possibly unaligned address
 */
/**
 * \def write_unaligned_le16
 * \brief Write 16-bit Little Endian value to a possibly unaligned address
 */
/**
 * \def write_unaligned_le32
 * \brief Write 32-bit Little Endian value to a possibly unaligned address
 */
//@}

//! @}

#endif /* UNALIGNED_H_INCLUDED */

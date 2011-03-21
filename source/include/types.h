/**
 * \file
 *
 * \brief Type definitions used throughout the library
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
#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <compiler.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * \brief Status return value.
 *
 * This type can hold small positive values (up to 127) as well as the
 * status codes defined by the status_code enum. It is the most
 * efficient return type for all platforms.
 */
typedef int_fast8_t             status_t;

/** \brief Unsigned 16-bit quantity, little endian byte order */
typedef uint16_t __bitwise	le16_t;

/** \brief Unsigned 32-bit quantity, little endian byte order */
typedef uint32_t __bitwise	le32_t;

/** \brief Unsigned 16-bit quantity, big endian byte order */
typedef uint16_t __bitwise	be16_t;

/** \brief Unsigned 32-bit quantity, big endian byte order */
typedef uint32_t __bitwise	be32_t;

#endif /* TYPES_H_INCLUDED */

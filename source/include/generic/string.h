/**
 * \file
 *
 * \brief Generic, non-optimized string operations.
 *
 * These functions may be used by all architectures which don't have
 * their own hand-optimized implementations of one or more string
 * operations. They can also be useful when testing the optimized string
 * operations.
 *
 * Copyright (C) 2009 - 2010 Atmel Corporation. All rights reserved.
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
#ifndef GENERIC_STRING_H_INCLUDED
#define GENERIC_STRING_H_INCLUDED

#include <types.h>
#include <compiler.h>

/**
 * \addtogroup string_group
 * @{
 * \name Generic String Implementation
 *
 * For testing purposes, and for architectures which don't have or need
 * optimized implementations of the standard string operations, a
 * generic, non-optimized implementation is available for the all string
 * operations. If you need access to these generic string operations,
 * you need to set \c CONFIG_GENERIC_STRING_IMPL to 'y'. Some
 * architectures do this unconditionally, but it is not safe to rely on
 * this.
 */
//@{

//! \copydoc memcpy()
void *generic_memcpy(void *dest, const void *src, size_t n);
//! \copydoc memset()
void *generic_memset(void *s, int c, size_t n);
//! \copydoc strlen()
size_t generic_strlen(const char *str);
//! \copydoc strcmp()
int generic_strcmp(const char *str1, const char *str2) __nonnull(1, 2);
//! \copydoc strncmp()
int generic_strncmp(const char *str1, const char *str2, size_t n) __nonnull(1, 2);

//@}
//! @}

#endif /* GENERIC_STRING_H_INCLUDED */

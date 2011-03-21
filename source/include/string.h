/**
 * \file
 *
 * \brief Standard string operations.
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
#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

#include <arch/string.h>

/**
 * \ingroup utility_group
 * \defgroup string_group String Operations
 * @{
 *
 * String operations are standard functions for copying, initializing
 * and searching memory. They tend to get used a lot, so they are
 * usually highly optimized for each particular architecture or even CPU
 * implementation.
 */

/*
 * String operations often have several implementations, so we keep the
 * documentation here in order to avoid redundancy.
 */

/**
 * \fn void *memcpy(void *dest, const void *src, size_t n)
 *
 * \brief Copy memory area.
 *
 * Copy \a n bytes from \a src to \a dest.
 *
 * \param dest Pointer to the destination memory area.
 * \param src Pointer to the source memory area.
 * \param n The number of bytes to copy.
 *
 * \return The \a dest parameter.
 */

/**
 * \fn void *memset(void *s, int c, size_t n)
 *
 * \brief Initialize memory area.
 *
 * Fill the first \a n bytes of the memory area \a s with the constant
 * byte value \a c.
 *
 * \param s Pointer to the memory area to be filled.
 * \param c The byte value to use as fill value.
 * \param n The number of bytes to initialize.
 *
 * \return The \a s parameter.
 */

/**
 * \fn int strcmp(const char *str1, const char *str2)
 *
 * \brief Compare two strings \a str1 and \a str2.
 *
 * \param str1 A '\\+'-terminated string
 * \param str2 A '\\+'-terminated string
 *
 * \retval Negative if \a str1 is smaller than \a str2
 * \retval Zero if \a str1 is equal to \a str2
 * \retval Positive if \a str1 is greater than \a str2
 */

/**
 * \fn int strncmp(const char *str1, const char *str2, size_t n)
 *
 * \brief Compare the first \a n characters of two strings \a str1 and \a str2.
 *
 * \param str1 A '\\+'-terminated string
 * \param str2 A '\\+'-terminated string
 * \param n Numbers of characters to compare
 *
 * \retval Negative if \a str1 is smaller than \a str2
 * \retval Zero if \a str1 is equal to \a str2
 * \retval Positive if \a str1 is greater than \a str2
 */

/**
 * \fn size_t strlen(const char *str)
 *
 * \brief Calculate the length of a string
 *
 * \param str A '\\0'-terminated string
 *
 * \return The length of \a s up to but not including the terminating
 * '\\0' character.
 */

//! @}

#endif /* STRING_H_INCLUDED */

/**
 * \file
 *
 * \brief Extremely simple malloc() implementation
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
#ifndef MALLOC_SIMPLE_H_INCLUDED
#define MALLOC_SIMPLE_H_INCLUDED

#include <types.h>

/**
 * \ingroup mem_alloc_group
 * \defgroup malloc_group Simple Malloc implementation
 *
 * This implementation of malloc() is very lightweight, but it does not
 * include free(), realloc() or any other fancy features. It is useful
 * when including drivers that rely on malloc() during initialization,
 * but not for general dynamic memory
 * management.
 * @{
 */

extern void *malloc(size_t size);
extern void *zalloc(size_t size);

/**
 * \brief Free a memory object previously allocated by malloc().
 *
 * This will return the memory at \a ptr back to the dynamic memory
 * allocator. \a ptr must point to a block of memory previously returned
 * by malloc() or any of the associated functions, and must not have
 * been passed to free() since then.
 *
 * \note This particular implementation of free() does nothing.
 *
 * \param ptr Pointer to the memory object to be freed.
 */
static inline void free(void *ptr)
{

}

//! @}

#endif /* MALLOC_SIMPLE_H_INCLUDED */

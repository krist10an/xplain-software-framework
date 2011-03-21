/**
 * \file
 *
 * \brief Program Memory Access
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
#ifndef PROGMEM_H_INCLUDED
#define PROGMEM_H_INCLUDED

/* The real implementation is architecture-specific */
#include <arch/progmem.h>

/**
 * \ingroup utility_group
 * \defgroup progmem_group Data in Program Space
 *
 * On devices with a Harvard architecture, the program memory (typically
 * flash) and data memory are in separate address spaces. In order to
 * allow optimization of constant data storage, these devices often
 * allow accessing data stored in program space through special
 * instructions.
 *
 * This module provides mechanisms for defining data stored in program
 * space, and for accessing this data. On Von Neumann architecture
 * devices, these attributes and functions have no effect.
 *
 * @{
 */

/**
 * \def __progmem
 * \brief Attribute for indicating that a data object is stored in
 * program memory.
 *
 * This is used on pointers that refer to objects stored in program
 * memory. On Von Neumann architectures, this attribute has no effect.
 */
/**
 * \def __progmem_arg
 * \brief Attribute for indicating that a pointer argument points into
 * program memory rather than data memory.
 *
 * This is used on pointers that refer to objects stored in program
 * memory. On Von Neumann architectures, this attribute has no effect.
 */
/**
 * \def DECLARE_PROGMEM(type, name)
 * \brief Declare a variable stored in program memory.
 *
 * This macro will ensure that the correct pragmas, attributes, extended
 * keywords, etc. are inserted in the correct place, depending on the
 * compiler.
 */
/**
 * \def DEFINE_PROGMEM(type, name)
 * \brief Define a variable stored in program memory.
 *
 * This macro will ensure that the correct pragmas, attributes, extended
 * keywords, etc. are inserted in the correct place, depending on the
 * compiler. The macro invocation may be followed by '=', followed by
 * the initializer, if necessary.
 */

/**
 * \fn uint8_t progmem_read8(const uint8_t __progmem *p)
 * \brief Read a byte stored at address \a p in program memory.
 */
/**
 * \fn uint16_t progmem_read16(const uint16_t __progmem *p)
 * \brief Read a 16-bit word stored at address \a p in program memory.
 */
/**
 * \fn uint32_t progmem_read32(const uint32_t __progmem *p)
 * \brief Read a 32-bit word stored at address \a p in program memory.
 */

//! @}

#endif /* PROGMEM_H_INCLUDED */

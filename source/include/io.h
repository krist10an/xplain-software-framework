/**
 * \file
 *
 * \brief I/O accessor functions
 *
 * Copyright (C) 2009 Atmel Corporation. All rights reserved.
 *
 * The actual implementation of the accessors is architecture-specific.
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
#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED

/**
 * \defgroup mmio_group Memory-Mapped Input/Output (MMIO)
 *
 * These accessors are used to access memory-mapped I/O registers on the
 * chip as well as external devices hooked up through the external bus
 * interface. Accessing such registers usually have side-effects, so
 * these accessors ensure that the registers are accessed exactly as
 * many times as specified.
 *
 * Traditionally, such registers are accessed through pointers declared
 * as "volatile", but using dedicated accessors has a number of
 * advantages:
 *   - Conceptually, it makes much more sense to provide special access
 *     semantics as part of an operation instead of the data associated
 *     with it.
 *   - The optimal way to access a register may vary from compiler to
 *     compiler (for example, gcc appears to generate slightly better
 *     code from inline assembly than from volatile access.) Such
 *     details are best hidden behind an accessor layer like this.
 *   - When dedicated accessors are used, it becomes very obvious that
 *     the code using them are in fact dealing with I/O. A volatile
 *     struct access looks just like any other struct access.
 *
 * @{
 */

#include <arch/io.h>

/**
 * \defgroup regs_group Internal Register Definitions
 *
 * This group contains definitions of the internal registers on all
 * supported chips. Some of the register interfaces are highly
 * chip-dependent, but most of them are usable with only minor
 * variations over a wide range of chips.
 *
 * \section regs_group_conventions Common Conventions
 *
 * Each module may define convenience wrappers for the MMIO accessor
 * functions which will paste in the correct module prefix and do other
 * transformations as appropriate. Most modules use the form
 * \verbatim
    <module>_read_reg(base, reg)
    <module>_write_reg(base, reg, value)\endverbatim
 * where \a module is the short name of the module in all lowercase, \a
 * base is the base address of the module instance, \a reg is the name
 * of the register without the module prefix, and \a value is the value
 * to be written to the register. Modules of which there will never be
 * more than one instance (e.g. the Power Manager), may leave out the \a
 * base argument and provide the correct value automatically using the
 * definitions in chip/memory-map.h.
 *
 * All registers are defined as an offset relative to the base address
 * of the module. This ensures that the same register definitions may be
 * used for all modules on a chip with multiple modules of the same
 * type. The constants are named in all uppercase using the short name
 * of the module and the name of the register as specified by the data
 * sheet, separated by an underscore character.
 *
 * Single bits in the register are specified as a single bit number
 * indicating the position of the bit counting from the LSB. The name is
 * on the form <em>module</em>_<em>register</em>_<em>name</em>_BIT, in
 * all uppercase. A bit definition may be turned into a bit mask by
 * passing it through the <em>module</em>_BIT() macro.
 *
 * Multi-bit fields in the register are specified by two definitions:
 *   - <em>module</em>_<em>register</em>_<em>name</em>_START: The
 *     position of the first bit in the field.
 *   - <em>module</em>_<em>register</em>_<em>name</em>_SIZE: The number
 *     of bits in the field.
 *
 * Bit and bitfield definitions which are used with multiple registers
 * (for example interrupt flags) may drop the \a reg part of the name.
 *
 * The single- and multi-bit field definitions may be used as is in
 * manual shift and mask operations. However, the most common bit
 * manipulations are supported by a set of helper macros:
 *   - <em>module</em>_BIT(): Create a bitmask with a single, named bit
 *     set to one.
 *   - <em>module</em>_BF(): Create a bitmask with a single, named
 *     bitfield set to a particular value.
 *   - <em>module</em>_BFEXT(): Extract the value of a named bitfield
 *     from a register value.
 *   - <em>module</em>_BFINS(): Insert a new value into a named bitfield
 *     without changing any other parts of the register value.
 *
 * Constants indicating special values that certain bitfields may be set
 * to have names on the form
 * <em>module</em>_<em>bitfield</em>_<em>name</em> in all uppercase. The
 * value is not shifted, so the user is responsible for shifting the
 * value into the correct bitfield, for example by passing it to one of
 * the bit manipulation macros above.
 */

//! @}

#endif /* IO_H_INCLUDED */

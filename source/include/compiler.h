/**
 * \file
 *
 * \brief Compiler abstraction layer
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
#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED

#include <stdint.h>

/**
 * \ingroup utility_group
 * \defgroup compiler_group Compiler Support
 *
 * This group provides various mechanisms which are not defined by the C
 * standard, but supported by most compilers anyway. Each compiler
 * typically defines its own set of extensions for providing these
 * mechanisms, so we need a common abstraction layer to use them in
 * applications and drivers.
 *
 * @{
 */

/* The "sparse" syntax checker defines __CHECKER__ */
#if defined(__CHECKER__)
# include <compiler/sparse.h>
#elif defined(__GNUC__)
# include <compiler/gcc.h>
#elif defined(__ICCAVR32__) || defined(__ICCAVR__)
# include <compiler/iar.h>
#endif

//! \name Extended type checking
//@{

/**
 * \def __bitwise
 * \brief Make a type incompatible with all other types.
 *
 * This is a sparse attribute which has no effect on other compilers.
 * Declaring a type as __bitwise will cause sparse to complain if it
 * is implicitly or explicitly cast to a different type. This is
 * useful when dealing with foreign-endian types, for example, since
 * it is always a bug if they are accessed without proper byte
 * swapping.
 */
#ifndef __bitwise
# define __bitwise
#endif

/**
 * \def __virtual
 * \brief Mark a pointer as virtual
 *
 * This annotation does nothing on any compiler, since the virtual
 * address space is the default one.
 */
#ifndef __virtual
# define __virtual
#endif

/**
 * \def __physical
 * \brief Mark a pointer as physical
 *
 * A pointer marked with this annotation cannot be dereferenced since
 * it belongs to an address space which cannot be accessed directly on
 * all platforms. It must be mapped to a virtual address by calling an
 * appropriate mapping function.
 */
#ifndef __physical
# define __physical
#endif

/**
 * \def __force
 * \brief Force a type cast
 *
 * This modifier can be used to silence any warnings from sparse with
 * regards to casting of bitwise types or casting from one address
 * space to another. This is typically used by the functions that
 * provides the mappings between those types, e.g. byte swapping
 * functions.
 */
#ifndef __force
# define __force
#endif
//@}

//! \name Function and Data Attributes
//@{

/**
 * \def __noreturn
 * \brief The function does not return
 *
 * This function annotation gives a hint to the compiler that the
 * function being annotated does not return. This may give the
 * compiler some more room for optimizations.
 */
#if !defined(__noreturn) || defined(__DOXYGEN__)
# define __noreturn
#endif

/**
 * \def __must_check
 * \brief The return value must be checked
 *
 * This annotation may cause the compiler to warn if someone calls the
 * annotated function without checking its return value.
 */
#if !defined(__must_check) || defined(__DOXYGEN__)
# define __must_check
#endif

/**
 * \def __used
 * \brief Mark a function or data object as being used.
 *
 * The compiler will normally warn about functions or variables
 * defined as static but not referenced anywhere. By annotating them
 * with __used, the compiler will shut up and emit the symbol. This is
 * useful when a symbol is only referenced by inline assembly code,
 * for example.
 */
#if !defined(__used) || defined(__DOXYGEN__)
# define __used
#endif

/**
 * \def __always_inline
 * \brief The function should always be inlined.
 *
 * This annotation instructs the compiler to ignore its inlining
 * heuristics and inline the function no matter how big it thinks it
 * becomes.
 */
#ifndef __always_inline
# define __always_inline inline
#endif

/**
 * \def __nonnull(...)
 * \brief Define non-null function attributes.
 *
 * This function attribute specifies that some function parameters must
 * be non-null pointers.
 */
#ifndef __nonnull
# define __nonnull(...)
#endif

/**
 * \def __printf_format
 * \brief The function takes a printf-like format string.
 *
 * This annotation may cause the compiler to emit a warning if the
 * format string doesn't match the parameters given.
 */
#ifndef __printf_format
# define __printf_format(fmt_index, first_arg_index)
#endif

/**
 * \def compiler_align_data(byte_alignment)
 * \brief Align data variables to a give byte value
 *
 * \param byte_alignment Data alignment given in bytes
 *
 * This function will align a data variable in memory to the supplied number of
 * bytes. The number of bytes must be a power of two, e.g., 1, 2, 4, 8, 16,
 * etc.
 *
 * The compiler_align_data attribute must be placed at the very beginning of
 * the declaration of the variable.
 *
 * \par Example:
 *   compiler_align_data(4) static uint32_t dma_buffer[64];
 */
#ifndef compiler_align_data
# define compiler_align_data(byte_alignment)
#endif

//@}

//! \name Optimization Control
//@{

/**
 * \def likely(exp)
 * \brief The expression \a exp is likely to be true
 */
#ifndef likely
# define likely(exp)    (exp)
#endif

/**
 * \def unlikely(exp)
 * \brief The expression \a exp is unlikely to be true
 */
#ifndef unlikely
# define unlikely(exp)  (exp)
#endif

/**
 * \def barrier()
 * \brief Optimization barrier.
 *
 * This macro forces the compiler to reload any variables from memory.
 * Local variables are left alone, unless they had their address taken
 * at some point.
 */
#ifndef barrier
# define barrier()      do { } while (0)
# warning Compiler does not support barrier() -- expect breakage!
#endif

/**
 * \def is_constant(exp)
 * \brief Determine if an expression evaluates to a constant value.
 *
 * \param exp Any expression
 *
 * \return true if \a exp is constant, false otherwise.
 */
#ifndef is_constant
# define is_constant(exp) (0)
#endif

//! @}

//! \name Top-level Directives
//@{

/**
 * \brief Emit the compiler pragma \a arg.
 *
 * \param arg The pragma directive as it would appear after \e \#pragma
 * (i.e. not stringified).
 */
#define COMPILER_PRAGMA(arg)            _Pragma(#arg)

/**
 * \def COMPILER_PACK_SET(alignment)
 * \brief Set maximum alignment for subsequent struct and union
 * definitions to \a alignment.
 */
#ifndef COMPILER_PACK_SET
# define COMPILER_PACK_SET(alignment)   COMPILER_PRAGMA(pack(alignment))
#endif

/**
 * \def COMPILER_PACK_RESET()
 * \brief Set default alignment for subsequent struct and union
 * definitions.
 */
#ifndef COMPILER_PACK_RESET
# define COMPILER_PACK_RESET()          COMPILER_PRAGMA(pack())
#endif

/**
 * \def ERROR_FUNC(name, msg)
 * \brief Fail compilation if function call isn't eliminated
 *
 * If the compiler fails to optimize away all calls to the function \a
 * name, terminate compilation and display \a msg to the user.
 *
 * Note that not all compilers support this, so this is best-effort
 * only. Sometimes, there may be a linker error instead, and when
 * optimization is disabled, this mechanism will be completely disabled.
 */
#ifndef ERROR_FUNC
# define ERROR_FUNC(name, msg)                                  \
	extern int name(void)
#endif

//@}

ERROR_FUNC(compiler_priv_bad_size, "Invalid parameter size");

#define compiler_priv_demux_size(size, func, ...)               \
	(((size) == 1) ? func##8(__VA_ARGS__) :                 \
	 ((size) == 2) ? func##16(__VA_ARGS__) :                \
	 ((size) == 4) ? func##32(__VA_ARGS__) :                \
	 compiler_priv_bad_size())

/**
 * \name Bit Operations
 * These functions are not meant to be called directly. Please see \ref
 * bitops_group instead.
 */
//@{

/**
 * \def compiler_ctz(x)
 * \brief Count Trailing Zeroes
 * \see bit_word_find_first_one_bit()
 */
#ifndef compiler_ctz
static inline int_fast8_t compiler_priv_ctz8(uint8_t x)
{
	int_fast8_t bit = 0;

	if (!(x & 0x0f)) {
		bit += 4;
		x >>= 4;
	}
	if (!(x & 0x03)) {
		bit += 2;
		x >>= 2;
	}
	if (!(x & 0x01))
		bit++;

	return bit;
}

static inline int_fast8_t compiler_priv_ctz16(uint16_t x)
{
	int_fast8_t bit = 0;

	if (!(x & 0x00ff)) {
		bit += 8;
		x >>= 8;
	}

	return bit + compiler_priv_ctz8(x);
}

static inline int_fast8_t compiler_priv_ctz32(uint32_t x)
{
	int_fast8_t bit = 0;

	if (!(x & 0x0000ffff)) {
		bit += 16;
		x >>= 16;
	}

	return bit + compiler_priv_ctz16(x);
}

#define compiler_ctz(x) \
	compiler_priv_demux_size(sizeof(x), compiler_priv_ctz, (x))
#endif

/**
 * \def compiler_clz(x)
 * \brief Count Leading Zeroes
 * \see bit_word_find_last_one_bit()
 */
#ifndef compiler_clz
static inline int_fast8_t compiler_priv_clz8(uint8_t x)
{
	int_fast8_t bit = 0;

	if (x & 0xf0)
		x >>= 4;
	else
		bit += 4;

	if (x & 0x0c)
		x >>= 2;
	else
		bit += 2;

	if (!(x & 0x02))
		bit++;

	return bit;
}

static inline int_fast8_t compiler_priv_clz16(uint16_t x)
{
	int_fast8_t bit = 0;

	if (x & 0xff00)
		x >>= 8;
	else
		bit += 8;

	return bit + compiler_priv_clz8(x);
}

static inline int_fast8_t compiler_priv_clz32(uint32_t x)
{
	int_fast8_t bit = 0;

	if (x & 0xffff0000)
		x >>= 16;
	else
		bit += 16;

	return bit + compiler_priv_clz16(x);
}

#define compiler_clz(x) \
	compiler_priv_demux_size(sizeof(x), compiler_priv_clz, (x))
#endif

/**
 * \def compiler_brev(x)
 * \brief Reverse the order of the bits in \a x
 * \see bit_word_reverse()
 */
#ifndef compiler_brev
static inline uint8_t compiler_priv_brev8(uint8_t x)
{
	/* swap odd and even bits */
	x = ((x >> 1) & 0x55) | ((x & 0x55) << 1);
	/* swap consecutive pairs */
	x = ((x >> 2) & 0x33) | ((x & 0x33) << 2);
	/* swap nibbles */
	x = ((x >> 4) & 0x0f) | ((x & 0x0f) << 4);

	return x;
}

static inline uint16_t compiler_priv_brev16(uint16_t x)
{
	/* swap odd and even bits */
	x = ((x >> 1) & 0x5555) | ((x & 0x5555) << 1);
	/* swap consecutive pairs */
	x = ((x >> 2) & 0x3333) | ((x & 0x3333) << 2);
	/* swap nibbles */
	x = ((x >> 4) & 0x0f0f) | ((x & 0x0f0f) << 4);
	/* swap bytes */
	x = ((x >> 8) & 0x00ff) | ((x & 0x00ff) << 8);

	return x;
}

static inline uint32_t compiler_priv_brev32(uint32_t x)
{
	/* swap odd and even bits */
	x = ((x >> 1) & 0x55555555) | ((x & 0x55555555) << 1);
	/* swap consecutive pairs */
	x = ((x >> 2) & 0x33333333) | ((x & 0x33333333) << 2);
	/* swap nibbles */
	x = ((x >> 4) & 0x0f0f0f0f) | ((x & 0x0f0f0f0f) << 4);
	/* swap bytes */
	x = ((x >> 8) & 0x00ff00ff) | ((x & 0x00ff00ff) << 8);
	/* swap 2-byte pairs */
	x = (x >> 16) | (x << 16);

	return x;
}

#define compiler_brev(x) \
	compiler_priv_demux_size(sizeof(x), compiler_priv_brev, (x))
#endif

static inline int8_t compiler_min_s8(int8_t a, int8_t b)
{
	return (a < b) ? a : b;
}

static inline int16_t compiler_min_s16(int16_t a, int16_t b)
{
	return (a < b) ? a : b;
}

static inline int32_t compiler_min_s32(int32_t a, int32_t b)
{
	return (a < b) ? a : b;
}

static inline int64_t compiler_min_s64(int64_t a, int64_t b)
{
	return (a < b) ? a : b;
}

static inline uint8_t compiler_min_u8(uint8_t a, uint8_t b)
{
	return (a < b) ? a : b;
}

static inline uint16_t compiler_min_u16(uint16_t a, uint16_t b)
{
	return (a < b) ? a : b;
}

static inline uint32_t compiler_min_u32(uint32_t a, uint32_t b)
{
	return (a < b) ? a : b;
}

static inline uint64_t compiler_min_u64(uint64_t a, uint64_t b)
{
	return (a < b) ? a : b;
}

static inline int8_t compiler_max_s8(int8_t a, int8_t b)
{
	return (a > b) ? a : b;
}

static inline int16_t compiler_max_s16(int16_t a, int16_t b)
{
	return (a > b) ? a : b;
}

static inline int32_t compiler_max_s32(int32_t a, int32_t b)
{
	return (a > b) ? a : b;
}

static inline int64_t compiler_max_s64(int64_t a, int64_t b)
{
	return (a > b) ? a : b;
}

static inline uint8_t compiler_max_u8(uint8_t a, uint8_t b)
{
	return (a > b) ? a : b;
}

static inline uint16_t compiler_max_u16(uint16_t a, uint16_t b)
{
	return (a > b) ? a : b;
}

static inline uint32_t compiler_max_u32(uint32_t a, uint32_t b)
{
	return (a > b) ? a : b;
}

static inline uint64_t compiler_max_u64(uint64_t a, uint64_t b)
{
	return (a > b) ? a : b;
}

//@}

//! \name Interrupt Control
//@{

/**
 * \def cpu_irq_enable()
 * \brief Enable interrupts globally
 *
 * This implies a barrier to ensure that memory operations don't
 * "leak" out of the critical section.
 */
/**
 * \def cpu_irq_disable()
 * \brief Disable interrupts globally
 *
 * This implies a barrier to ensure that memory operations don't
 * "leak" out of the critical section.
 */

//@}

//! @}

#endif /* COMPILER_H_INCLUDED */

/**
 * \file
 *
 * \brief AVR XMega UART control interface
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
#ifndef UART_CTRL_XMEGA_H_INCLUDED
#define UART_CTRL_XMEGA_H_INCLUDED

#include <types.h>
#include <chip/uart.h>
#include <regs/xmega_usart.h>

/**
 * \weakgroup uart_ctrl_group
 * @{
 */

#ifndef CONFIG_UART_DEFAULT_MODE
# define CONFIG_UART_DEFAULT_MODE       { USART_BF(CHSIZE, 8 - 5) }
#endif

//! Hardware-specific representation of a baud rate
struct uart_baud {
	uint16_t	divider;
	uint8_t		flags;
};

enum uart_baud_flag_t {
	//! Double the USART clock speed
	UART_BAUD_CLK2X = USART_BIT(CLK2X),
};

//! Hardware-specific representation of a set of communication parameters
struct uart_mode {
	uint8_t		ctrlc;
};

enum uart_mode_sb {
	UART_MODE_SB_1		= 0,	//!< 1 stop bit
	UART_MODE_SB_2		= 1,	//!< 2 stop bits
};

enum uart_mode_par {
	UART_MODE_PAR_NONE	= 0,	//!< No parity
	UART_MODE_PAR_EVEN	= 2,	//!< Even parity
	UART_MODE_PAR_ODD	= 3,	//!< Odd parity
};

static inline struct uart_mode uart_mode_defaults(unsigned int uart_id)
{
	return (struct uart_mode)CONFIG_UART_DEFAULT_MODE;
}

#define uart_mode_read(uart_id)						\
	(struct uart_mode){						\
		usart_read_reg(uart_get_regs(uart_id), CTRLC)		\
	}
#define uart_mode_write(uart_id, mode)					\
	do {								\
		void *regs = uart_get_regs(uart_id);			\
		usart_write_reg(regs, CTRLC, mode->ctrlc);		\
	} while (0)

static inline struct uart_mode *uart_mode_set_chrlen(unsigned int uart_id,
		struct uart_mode *mode, uint8_t bits)
{
	assert(bits >= 5 && bits <= 8);

	mode->ctrlc = USART_BFINS(CHSIZE, bits - 5, mode->ctrlc);
	return mode;
}

static inline uint8_t uart_mode_get_chrlen(unsigned int uart_id,
		struct uart_mode *mode)
{
	return USART_BFEXT(CHSIZE, mode->ctrlc) + 5;
}

static inline bool uart_mode_chrlen_is_valid(unsigned int uart_id,
		uint8_t bits)
{
	return bits >= 5 && bits <= 8;
}

static inline struct uart_mode *uart_mode_set_stop_bits(unsigned int uart_id,
		struct uart_mode *mode, enum uart_mode_sb value)
{
	assert(value >= UART_MODE_SB_1 && value <= UART_MODE_SB_2);

	mode->ctrlc = USART_BFINS(SBMODE, value, mode->ctrlc);
	return mode;
}

static inline enum uart_mode_sb uart_mode_get_stop_bits(unsigned int uart_id,
		struct uart_mode *mode)
{
	return USART_BFEXT(SBMODE, mode->ctrlc);
}

static inline struct uart_mode *uart_mode_set_parity(unsigned int uart_id,
		struct uart_mode *mode, enum uart_mode_par value)
{
	assert(value >= UART_MODE_PAR_NONE && value <= UART_MODE_PAR_ODD);

	mode->ctrlc = USART_BFINS(PMODE, value, mode->ctrlc);
	return mode;
}

static inline enum uart_mode_par uart_mode_get_parity(unsigned int uart_id,
		struct uart_mode *mode)
{
	return USART_BFEXT(PMODE, mode->ctrlc);
}

/**
 * \internal
 * \brief Calculate the hardware baud value corresponding to \a baud_rate
 *
 * This function is called by uart_baud_default(), with constant \a
 * baud_rate and usually constant \a mode, as well as by
 * uart_baud_set_rate(), with non-constant \a baud_rate and \a mode.
 *
 * Forcing this function to be inlined allows the constant case
 * to be small and fast, while the non-constant case will be defined
 * only once: inside of uart_baud_set_rate(). The latter will only be
 * linked if the application actually needs support for arbitrary baud
 * rates at run time.
 *
 * This function makes no attempt at guessing the best oversampling
 * setting, but it will calculate the dividers based on the setting
 * specified by \a mode.
 *
 * \param baud Pointer to the baud struct which contains details about baud
 * flags, exponent and divider.
 * \param baud_rate The desired baud rate
 */
__always_inline static void uart_priv_baud_set_constant_rate(
		struct uart_baud *baud, uint32_t baud_rate)
{
	int8_t		exp;
	uint32_t	div;
	uint32_t	fper = CONFIG_CPU_HZ;

	/*! \todo Make use of the UART_BAUD_CLK2X flag. */
	baud->flags = 0;

	/*
	 * The below expressions assume that the result is going to be
	 * divided by 8. So we need to (a) scale the target baud rate if
	 * 16x oversampling is used, and (b) shift all the limits left
	 * by 3 bits.
	 */
	if (!(baud->flags & UART_BAUD_CLK2X))
		baud_rate *= 2;

	if (fper / baud_rate < (0xfffU >> 4))
		exp = -7;
	else if (fper / baud_rate < (0xfffU >> 3))
		exp = -6;
	else if (fper / baud_rate < (0xfffU >> 2))
		exp = -5;
	else if (fper / baud_rate < (0xfffU >> 1))
		exp = -4;
	else if (fper / baud_rate < (0xfffU >> 0))
		exp = -3;
	else if (fper / baud_rate < (0xfffU << 1))
		exp = -2;
	else if (fper / baud_rate < (0xfffU << 2))
		exp = -1;
	else if (fper / baud_rate < (0xfffUL << 3))
		exp = 0;
	else if (fper / baud_rate < (0xfffUL << 4))
		exp = 1;
	else if (fper / baud_rate < (0xfffUL << 5))
		exp = 2;
	else if (fper / baud_rate < (0xfffUL << 6))
		exp = 3;
	else if (fper / baud_rate < (0xfffUL << 7))
		exp = 4;
	else if (fper / baud_rate < (0xfffUL << 8))
		exp = 5;
	else if (fper / baud_rate < (0xfffUL << 9))
		exp = 6;
	else
		exp = 7;

	/*
	 * Depending on the value of exp, scale either the input
	 * frequency or the target baud rate. By always scaling upwards,
	 * we never introduce any additional inaccuracy.
	 *
	 * We're including the final divide-by-8 (aka. right-shift-by-3)
	 * in this operation as it ensures that we never exceeed 2**32
	 * at any point.
	 *
	 * The formula for calculating BSEL is slightly different when
	 * exp is negative than it is when exp is positive.
	 */
	if (exp < 0) {
		/*
		 * We're supposed to subtract 1, then apply BSCALE. We
		 * want to apply BSCALE first, so we need to turn
		 * everything inside the parenthesis into a single
		 * fractional expression.
		 */
		fper -= 8 * baud_rate;

		/*
		 * If we end up with a left-shift after taking the final
		 * divide-by-8 into account, do the shift before the
		 * divide. Otherwise, left-shift the denominator instead
		 * (effectively resulting in an overall right shift.)
		 */
		if (exp <= -3) {
			div = ((fper << (-exp - 3)) + baud_rate / 2)
				/ baud_rate;
		} else {
			baud_rate <<= exp + 3;
			div = (fper + baud_rate / 2) / baud_rate;
		}
	} else {
		/*
		 * We will always do a right shift in this case, but we
		 * need to shift three extra positions because of the
		 * divide-by-8.
		 */
		baud_rate <<= exp + 3;
		div = (fper + baud_rate / 2) / baud_rate - 1;
	}

	assert(div > 0 && div < (1 << 12));
	assert(exp > -8 && exp < 8);

	baud->divider = (exp << 12) | div;
}

static inline void uart_baud_default(unsigned int uart_id,
		struct uart_baud *baud)
{
	uart_priv_baud_set_constant_rate(baud, CONFIG_UART_BAUD_RATE);
}

#define uart_baud_read(uart_id, baud)                                        \
	do {                                                                 \
		void *regs = uart_get_regs(uart_id);                         \
		baud->divider = usart_read_reg(regs, BAUDCTRLB) << 8;        \
		baud->divider |= usart_read_reg(regs, BAUDCTRLA);            \
		baud->flags = usart_read_reg(regs, CTRLB) & UART_BAUD_CLK2X; \
	} while (0)

#define uart_baud_write(uart_id, baud)					\
	do {								\
		void    *regs = uart_get_regs(uart_id);                 \
		uint8_t ctrlb = usart_read_reg(regs, CTRLB);            \
		if (baud->flags & UART_BAUD_CLK2X)                      \
			ctrlb |= USART_BIT(CLK2X);                      \
		else                                                    \
			ctrlb &= ~USART_BIT(CLK2X);                     \
		usart_write_reg(regs, CTRLB, ctrlb);                    \
		usart_write_reg(regs, BAUDCTRLB, (baud->divider) >> 8); \
		usart_write_reg(regs, BAUDCTRLA, (baud->divider));      \
	} while (0)

extern void uart_priv_baud_set_rate(struct uart_baud *baud, uint32_t rate);
extern bool uart_priv_baud_rate_is_valid(uint8_t flags, uint32_t rate);

static inline void uart_baud_set_rate(unsigned int uart_id,
		struct uart_baud *baud, uint32_t rate)
{
	uart_priv_baud_set_rate(baud, rate);
}

static inline bool uart_baud_rate_is_valid(unsigned int uart_id,
		struct uart_baud *baud, uint32_t rate)
{
	return uart_priv_baud_rate_is_valid(baud->flags, rate);
}

//! @}

#endif /* UART_CTRL_XMEGA_H_INCLUDED */

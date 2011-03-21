/**
 * \file
 *
 * \brief AVR Mega UART control interface
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
#ifndef UART_CTRL_MEGA_H_INCLUDED
#define UART_CTRL_MEGA_H_INCLUDED

#include <assert.h>
#include <chip/regs.h>
#include <chip/uart.h>

/**
 * \weakgroup uart_ctrl_group
 * @{
 */

#ifndef CONFIG_UART_DEFAULT_MODE
# define CONFIG_UART_DEFAULT_MODE					\
	{ 0, AVR_BF(UPM, UART_MODE_PAR_NONE) | AVR_BF(UCSZ, 8 - 5) }
#endif

//! Hardware-specific representation of a baud rate
struct uart_baud {
	uint16_t        divider;
	uint8_t         flags;
};

enum uart_baud_flag_t {
	//! Double the USART clock speed
	UART_BAUD_CLK2X = AVR_BIT(U2X),
};

//! Hardware-specific representation of a set of communication parameters
struct uart_mode {
	uint8_t	        csra;
	uint8_t	        csrc;
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

#define uart_mode_read(uart_id)		_uart_mode_read(uart_id)
#define _uart_mode_read(uart_id)					\
	{ avr_read_reg8(UCSR##id##A), avr_read_reg8(UCSR##id##C) }
#define uart_mode_write(uart_id, mode)	_uart_mode_write(uart_id, mode)
#define _uart_mode_write(uart_id, mode)					\
	do {								\
		avr_write_reg8(UCSR##uart_id##A, mode->csra);		\
		avr_write_reg8(UCSR##uart_id##C, mode->csrc);		\
	} while (0)

static inline struct uart_mode *uart_mode_set_chrlen(unsigned int uart_id,
		struct uart_mode *mode, uint8_t bits)
{
	assert(bits >= 5 && bits <= 8);

	mode->csrc = AVR_BFINS(UCSZ, bits - 5, mode->csrc);
	return mode;
}

static inline uint8_t uart_mode_get_chrlen(unsigned int uart_id,
		struct uart_mode *mode)
{
	return AVR_BFEXT(UCSZ, mode->csrc) + 5;
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

	if (value)
		mode->csrc |= AVR_BIT(USBS);
	else
		mode->csrc &= ~AVR_BIT(USBS);

	return mode;
}

static inline enum uart_mode_sb uart_mode_get_stop_bits(unsigned int uart_id,
		struct uart_mode *mode)
{
	return (mode->csrc & AVR_BIT(USBS)) ? UART_MODE_SB_2 : UART_MODE_SB_1;
}

static inline struct uart_mode *uart_mode_set_parity(unsigned int uart_id,
		struct uart_mode *mode, enum uart_mode_par value)
{
	assert(value >= UART_MODE_PAR_NONE && value <= UART_MODE_PAR_ODD);

	mode->csrc = AVR_BFINS(UPM, value, mode->csrc);
	return mode;
}

static inline enum uart_mode_par uart_mode_get_parity(unsigned int uart_id,
		struct uart_mode *mode)
{
	return AVR_BFEXT(UPM, mode->csrc);
}

/**
 * \internal
 * \brief Calculate the hardware baud value corresponding to \a baud_rate
 *
 * This function is called by uart_baud_default(), with constant \a
 * baud_rate and usually constant \a csra, as well as by
 * uart_baud_set_rate(), with non-constant \a baud_rate and \a csra.
 *
 * Forcing this function to be inlined allows the constant case
 * to be small and fast, while the non-constant case will be defined
 * only once: inside of uart_baud_set_rate(). The latter will only be
 * linked if the application actually needs support for arbitrary baud
 * rates at run time.
 *
 * This function makes no attempt at guessing the best oversampling
 * setting, but it will calculate the dividers based on the setting
 * specified by \a csra.
 *
 * \param csra The CSRA hardware register for UART which will use the
 *             new baud rate
 * \param baud_rate The desired baud rate
 *
 * \return A hardware representation of \a baud_rate
 */
__always_inline static void uart_priv_baud_set_constant_rate(
		struct uart_baud *baud, uint32_t baud_rate)
{
	uint16_t	div;

	/*! \todo Make use of the UART_BAUD_CLK2X flag. */
	baud->flags = 0;

	div = (CONFIG_CPU_HZ + (baud_rate / 2)) / baud_rate;
	if (baud->flags & UART_BAUD_CLK2X)
		div = (div + 4) / 8;
	else
		div = (div + 8) / 16;

	assert(div > 0 && div < (1 << 12));

	baud->divider = div;
}

static inline void uart_baud_default(unsigned int uart_id,
		struct uart_baud *baud)
{
	uart_priv_baud_set_constant_rate(baud, CONFIG_UART_BAUD_RATE);
}

#define uart_baud_read(uart_id)		_uart_baud_read(uart_id)
#define _uart_baud_read(uart_id)                                        \
	do {                                                            \
		baud->divider = avr_read_reg8(UBRR##uart_id##H) << 8;   \
		baud->divider |= avr_read_reg8(UBRR##uart_id##L);       \
		baud->flags = avr_read_reg8(UCSR##uart_id##A)           \
				& AVR_BIT(U2X);                         \
	} while (0)

#define uart_baud_write(uart_id, baud)	_uart_baud_write(uart_id, baud)
#define _uart_baud_write(uart_id, baud)                                 \
	do {                                                            \
		uint8_t ucsra = avr_read_reg8(UCSR##uart_id##A);        \
		if (baud->flags & UART_BAUD_CLK2X)                      \
			ucsra |= AVR_BIT(U2X);                          \
		else                                                    \
			ucsra &= ~AVR_BIT(U2X);                         \
		avr_write_reg8(UCSR##uart_id##A, ucsra);                \
		avr_write_reg8(UBRR##uart_id##H, baud->divider >> 8);   \
		avr_write_reg8(UBRR##uart_id##L, baud->divider);        \
	} while (0)

extern void uart_priv_baud_set_rate(struct uart_baud *baud, uint32_t rate);
extern bool uart_priv_baud_rate_is_valid(struct uart_baud *baud, uint32_t rate);

static inline void uart_baud_set_rate(unsigned int uart_id,
		struct uart_baud *baud, uint32_t rate)
{
	uart_priv_baud_set_rate(baud, rate);
}

static inline bool uart_baud_rate_is_valid(unsigned int uart_id,
		struct uart_baud *baud, uint32_t rate)
{
	return uart_priv_baud_rate_is_valid(baud, rate);
}

//! @}

#endif /* UART_CTRL_MEGA_H_INCLUDED */

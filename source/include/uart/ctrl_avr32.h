/**
 * \file
 *
 * \brief AVR32 UART control interface
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
#ifndef UART_CTRL_AVR32_H_INCLUDED
#define UART_CTRL_AVR32_H_INCLUDED

#include <regs/avr32_usart.h>
#include <chip/uart.h>

/**
 * \weakgroup uart_ctrl_group
 * @{
 */

#ifndef CONFIG_UART_DEFAULT_MODE
# define CONFIG_UART_DEFAULT_MODE				\
	{                                                       \
		USART_BF(MR_MODE, USART_MODE_NORMAL)		\
		| USART_BF(MR_CHRL, USART_CHRL_8)		\
		| USART_BF(MR_PAR, USART_PAR_NONE)		\
		| USART_BF(MR_NBSTOP, USART_NBSTOP_1)		\
		| USART_BF(MR_CHMODE, USART_MODE_NORMAL)        \
	}
#endif

//! Hardware-specific representation of a baud rate
struct uart_baud {
	uint32_t        divider;
	uint32_t        flags;
};

enum uart_baud_flag_t {
	//! Double the USART clock speed
	UART_BAUD_CLK2X = USART_BIT(MR_OVER),
};

//! Hardware-specific representation of a set of communication parameters
struct uart_mode {
	uint32_t        mr;
};

enum uart_mode_sb {
	UART_MODE_SB_1		= USART_NBSTOP_1,	//!< 1 stop bit
	UART_MODE_SB_1_5	= USART_NBSTOP_1_5,	//!< 1.5 stop bits
	UART_MODE_SB_2		= USART_NBSTOP_2,	//!< 2 stop bits
};

enum uart_mode_par {
	UART_MODE_PAR_EVEN	= USART_PAR_EVEN,	//!< Even parity
	UART_MODE_PAR_ODD	= USART_PAR_ODD,	//!< Odd parity
	UART_MODE_PAR_SPACE	= USART_PAR_SPACE,	//!< Forced to 0
	UART_MODE_PAR_MARK	= USART_PAR_MARK,	//!< Forced to 1
	UART_MODE_PAR_NONE	= USART_PAR_NONE,	//!< No parity
	UART_MODE_PAR_MDROP	= USART_PAR_MULTI,	//!< Multidrop mode
};

static inline struct uart_mode uart_mode_defaults(unsigned int uart_id)
{
	return (struct uart_mode)CONFIG_UART_DEFAULT_MODE;
}

#define uart_mode_read(uart_id)					\
	(struct uart_mode){                                     \
		usart_read_reg(uart_get_regs(uart_id), MR)      \
	}
#define uart_mode_write(uart_id, mode)				\
	usart_write_reg(uart_get_regs(uart_id), MR, mode->mr)

static inline struct uart_mode *uart_mode_set_chrlen(unsigned int uart_id,
		struct uart_mode *mode, uint8_t bits)
{
	assert(bits >= 5 && bits <= 8);

	mode->mr = USART_BFINS(MR_CHRL, bits - 5, mode->mr);
	return mode;
}

static inline uint8_t uart_mode_get_chrlen(unsigned int uart_id,
		struct uart_mode *mode)
{
	return USART_BFEXT(MR_CHRL, mode->mr) + 5;
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

	mode->mr = USART_BFINS(MR_NBSTOP, value, mode->mr);
	return mode;
}

static inline enum uart_mode_sb uart_mode_get_stop_bits(unsigned int uart_id,
		struct uart_mode *mode)
{
	return USART_BFEXT(MR_NBSTOP, mode->mr);
}

static inline struct uart_mode *uart_mode_set_parity(unsigned int uart_id,
		struct uart_mode *mode, enum uart_mode_par value)
{
	assert(value >= UART_MODE_PAR_EVEN && value <= UART_MODE_PAR_MDROP);

	mode->mr = USART_BFINS(MR_PAR, value, mode->mr);
	return mode;
}

static inline enum uart_mode_par uart_mode_get_parity(unsigned int uart_id,
		struct uart_mode *mode)
{
	return USART_BFEXT(MR_PAR, mode->mr);
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
 * \param uart_id The ID of the UART which will use the new baud rate
 * \param mode Mode settings to be used with the new baud rate
 * \param baud_rate The desired baud rate
 *
 * \return A hardware representation of \a baud_rate
 */
__always_inline static void uart_ctrl_priv_calc_baud_rate(unsigned int uart_id,
		struct uart_baud *baud, uint32_t baud_rate)
{
	unsigned long	pclk;
	unsigned long	div;
	uint32_t	cd;
	uint32_t	fp;

	pclk = uart_get_pclk_hz(uart_id);
	div = (pclk + (baud_rate / 2)) / baud_rate;

	/*! \todo Make use of the UART_BAUD_CLK2X flag. */
	baud->flags = 0;

	if (!(baud->flags & UART_BAUD_CLK2X))
		div = (div + 1) / 2;

	cd = div / 8;
	fp = div % 8;
	assert(cd > 0 && cd < 1 << USART_BRGR_CD_SIZE);

	baud->divider = USART_BF(BRGR_FP, fp) | USART_BF(BRGR_CD, cd);
}

static inline void uart_baud_default(unsigned int uart_id,
		struct uart_baud *baud)
{
	uart_ctrl_priv_calc_baud_rate(uart_id, baud,
			CONFIG_UART_BAUD_RATE);
}

#define uart_baud_read(uart_id, baud)				          \
	do {                                                              \
		void *regs = uart_get_regs(uart_id);                      \
		baud->divider = usart_read_reg(regs, BRGR);               \
		baud->flags = usart_read_reg(regs, MR) & UART_BAUD_CLK2X; \
	} while (0)

#define uart_baud_write(uart_id, baud)				          \
	do {                                                              \
		void *regs = uart_get_regs(uart_id);                      \
		uint32_t mr = usart_read_reg(regs, MR);                   \
		if (baud->flags & UART_BAUD_CLK2X)                        \
			mr |= USART_BIT(MR_OVER);                         \
		else                                                      \
			mr &= ~USART_BIT(MR_OVER);                        \
		usart_write_reg(regs, MR, mr);                            \
		usart_write_reg(regs, BRGR, baud->divider);               \
	} while (0)

extern void uart_baud_set_rate(unsigned int uart_id,
		struct uart_baud *baud, uint32_t rate);
extern bool uart_baud_rate_is_valid(unsigned int uart_id,
		struct uart_baud *baud, uint32_t rate);

//! @}

#endif /* UART_CTRL_AVR32_H_INCLUDED */

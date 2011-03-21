/**
 * \file
 *
 * \brief XMEGA UART definitions.
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

#ifndef UART_UART_XMEGA_H_INCLUDED
#define UART_UART_XMEGA_H_INCLUDED

#include <assert.h>
#include <types.h>
#include <chip/uart.h>
#include <regs/xmega_usart.h>

/**
 * \weakgroup uart_simple_group
 * @{
 */

typedef uint8_t uart_flags_t;
typedef void *uart_t;

#define UART_FLAG_TX		(1 << 0)
#define UART_FLAG_RX		(1 << 1)

#define uart_transmit_is_complete(uart_id) \
	uart_transmit_is_complete_priv(uart_t uart)
#define uart_tx_buffer_is_empty(uart_id) \
	uart_tx_buffer_is_empty_priv(uart_get_regs(uart_id))
#define uart_send_byte(uart_id, data) \
	uart_send_byte_priv(uart_get_regs(uart_id), data)
#define uart_put_byte(uart_id, data)\
	uart_put_byte_priv(uart_get_regs(uart_id), data)

#define uart_rx_buffer_is_full(uart_id) \
	uart_rx_buffer_is_full_priv(uart_get_regs(uart_id))
#define uart_get_byte(uart_id, data) \
	uart_get_byte_priv(uart_get_regs(uart_id), data)

#define uart_enable(uart_id, flags)			\
	do {						\
		uint8_t	ctrlb;				\
		uint8_t	_flags = (flags);		\
		void	*regs = uart_get_regs(uart_id);	\
		ctrlb = usart_read_reg(regs, CTRLB);	\
		if (_flags & UART_FLAG_TX)		\
			ctrlb |= USART_BIT(TXEN);	\
		if (_flags & UART_FLAG_RX)		\
			ctrlb |= USART_BIT(RXEN);	\
		usart_write_reg(regs, CTRLB, ctrlb);	\
	} while (0)


/**
 * \internal
 * \brief Initializes the UART.
 *
 * \param uart Pointer to UART module.
 * \param flags UART configuration flags.
 */
extern void uart_init_priv(uart_t uart, uart_flags_t flags);

/**
 * \internal
 * \brief Tests if all data has been transmitted.
 *
 * \param uart Pointer to UART module.
 * \retval true All data has been sent (shift register and THR is empty).
 * \retval false There is still data either in the shift register or in the THR
 * register, or the transmitter is disabled.
 *
 */
static inline bool uart_transmit_is_complete_priv(uart_t uart)
{
	if (usart_read_reg(uart, STATUS) & USART_BIT(TXCIF))
		return true;
	else
		return false;
}

/**
 * \internal
 * \brief Test if tx buffer is empty.
 *
 * \param uart Pointer to UART module.
 * \retval true if buffer is empty.
 * \retval false if buffer is full.
 */
static inline bool uart_tx_buffer_is_empty_priv(uart_t uart)
{
	if (usart_read_reg(uart, STATUS) & USART_BIT(DREIF))
		return true;
	else
		return false;
}

/**
 * \internal
 * \brief Test if rx buffer is full.
 *
 * \param uart Pointer to UART module.
 * \retval true if buffer is full.
 * \retval false if buffer is empty.
 */
static inline bool uart_rx_buffer_is_full_priv(uart_t uart)
{
	if (usart_read_reg(uart, STATUS) & USART_BIT(RXCIF))
		return true;
	else
		return false;
}

/**
 * \brief Send data without checking if the pipline is empty.
 *
 * \param uart Pointer to UART module.
 * \param data Data to be sent.
 */
static inline void uart_send_byte_priv(uart_t uart, uint8_t data)
{
	usart_write_reg(uart, DATA, data);
}

/**
 * \internal
 * \brief Send data if transmitter pipeline is empty.
 *
 * This function checks if there is room left to add new data (max is a byte)
 * in transmitter pipeline and copies the new data into it.
 *
 * \param uart Pointer to UART module.
 * \param data Data to be sent.
 * \retval true Data was written to send-buffer successfully.
 * \retval false Data could not be sent because data register is not empty.
 */
static inline bool uart_put_byte_priv(uart_t uart, uint8_t data)
{
	if (usart_read_reg(uart, STATUS) & USART_BIT(DREIF)) {
		usart_write_reg(uart, DATA, data);
		return true;
	}else {
		return false;
	}
}

/**
 * \internal
 * \brief Read data if data is available.
 *
 * This function checks if data is available in the receive queue and copies
 * it to the user supplied data pointer.
 *
 * \param uart Pointer to UART module.
 * \param data Pointer to memory where to store rx data.
 * \retval true Data available and written to b successfully.
 * \retval false  No data available.
 */
static inline bool uart_get_byte_priv(uart_t uart, uint8_t *data)
{
	if (usart_read_reg(uart, STATUS) & USART_BIT(RXCIF)) {
		*data = usart_read_reg(uart, DATA);
		return true;
	} else {
		return false;
	}
}

//@}

#endif /* UART_UART_XMEGA_H_INCLUDED */

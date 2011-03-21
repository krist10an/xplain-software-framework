/**
 * \file
 *
 * \brief ATmega UART definitions.
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

#ifndef UART_UART_MEGA_H_INCLUDED
#define UART_UART_MEGA_H_INCLUDED

#include <chip/regs.h>
#include <chip/uart.h>
#include <clk/sys.h>
#include <types.h>

/**
 * \weakgroup uart_simple_group
 * @{
 */

typedef uint8_t uart_flags_t;

#define UART_FLAG_TX		(1 << 0)
#define UART_FLAG_RX		(1 << 1)
#define UART_FLAG_UDRE		(1 << 2)

#define uart_enable_clock(id)                           \
	_uart_enable_clock(id)
#define _uart_enable_clock(id)                          \
	sysclk_enable_module(SYSCLK_USART##id)

#define uart_disable_clock(id)                          \
	_uart_disable_clock(id)
#define _uart_disable_clock(id)                         \
	sysclk_disable_module(SYSCLK_USART##id)

/* Multiplex layer to uart module specific functions */

/**
 * \internal
 * \brief Select a UART function.
 *
 * Select a UART function by building the function name from the UART id and
 * a function name.
 *
 * \param func Function name. (e.g uart0_"here comes the function name")
 * \param uart_id UART id.
 * \param ... Actual function parameters.
 */
#define UART_SELECT(func, uart_id, ...) \
	uart##uart_id##_##func(__VA_ARGS__)

#define uart_transmit_is_complete(uart_id) \
	UART_SELECT(transmit_is_complete, uart_id)
#define uart_tx_buffer_is_empty(uart_id)	\
	UART_SELECT(tx_buffer_is_empty, uart_id)
#define uart_send_byte(uart_id, data) \
	UART_SELECT(send_byte, uart_id, data)
#define uart_put_byte(uart_id, data) \
	UART_SELECT(put_byte, uart_id, data)

#define uart_rx_buffer_is_full(uart_id)	\
	UART_SELECT(rx_buffer_is_full, uart_id)
#define uart_get_byte(uart_id, data) \
	UART_SELECT(get_byte, uart_id, data)

#define uart_enable(uart_id, flags) \
	UART_SELECT(enable, uart_id, flags)
#define uart_enable_irq(uart_id, flags) \
	UART_SELECT(enable_irq, uart_id, flags)

static inline void uart0_enable(uart_flags_t flags)
{
		uint8_t	ucsrb;
		ucsrb = avr_read_reg8(UCSR0B);
		if (flags & UART_FLAG_TX)
			ucsrb |= AVR_BIT(TXEN);
		if (flags & UART_FLAG_RX)
			ucsrb |= AVR_BIT(RXEN);
		avr_write_reg8(UCSR0B, ucsrb);
}

static inline void uart0_enable_irq(uart_flags_t flags)
{
		uint8_t ucsrb;
		ucsrb = avr_read_reg8(UCSR0B);
		if (flags & UART_FLAG_TX)
			ucsrb |= AVR_BIT(TXCIE);
		if (flags & UART_FLAG_RX)
			ucsrb |= AVR_BIT(RXCIE);
		if (flags & UART_FLAG_UDRE)
			ucsrb |= AVR_BIT(UDRIE);
		avr_write_reg8(UCSR0B, ucsrb);
}

static inline void uart0_disable_irq(uart_flags_t flags)
{
		uint8_t ucsrb;
		ucsrb = avr_read_reg8(UCSR0B);
		if (flags & UART_FLAG_TX)
			ucsrb &= ~AVR_BIT(TXCIE);
		if (flags & UART_FLAG_RX)
			ucsrb &= ~AVR_BIT(RXCIE);
		if (flags & UART_FLAG_UDRE)
			ucsrb &= ~AVR_BIT(UDRIE);
		avr_write_reg8(UCSR0B, ucsrb);
}

#if (UART_MODULES > 1)
static inline void uart1_enable(uart_flags_t flags)
{
		uint8_t	ucsrb;
		ucsrb = avr_read_reg8(UCSR1B);
		if (flags & UART_FLAG_TX)
			ucsrb |= AVR_BIT(TXEN);
		if (flags & UART_FLAG_RX)
			ucsrb |= AVR_BIT(RXEN);
		avr_write_reg8(UCSR1B, ucsrb);
}

static inline void uart1_enable_irq(uart_flags_t flags)
{
		uint8_t ucsrb;
		ucsrb = avr_read_reg8(UCSR1B);
		if (flags & UART_FLAG_TX)
			ucsrb |= AVR_BIT(TXCIE);
		if (flags & UART_FLAG_RX)
			ucsrb |= AVR_BIT(RXCIE);
		if (flags & UART_FLAG_UDRE)
			ucsrb |= AVR_BIT(UDRIE);
		avr_write_reg8(UCSR1B, ucsrb);
}

static inline void uart1_disable_irq(uart_flags_t flags)
{
		uint8_t ucsrb;
		ucsrb = avr_read_reg8(UCSR1B);
		if (flags & UART_FLAG_TX)
			ucsrb &= ~AVR_BIT(TXCIE);
		if (flags & UART_FLAG_RX)
			ucsrb &= ~AVR_BIT(RXCIE);
		if (flags & UART_FLAG_UDRE)
			ucsrb &= ~AVR_BIT(UDRIE);
		avr_write_reg8(UCSR1B, ucsrb);
}
#endif

/**
 * \internal
 * \brief Tests if all data has been transmitted from UART 0.
 *
 * \retval true All data has been sent (shift register and THR is empty).
 * \retval false There is still data either in the shift register or in the THR
 * register, or the transmitter is disabled.
 *
 */
static inline bool uart0_transmit_is_complete(void)
{
	if (avr_read_reg8(UCSR0A) & AVR_BIT(TXC))
		return true;
	else
		return false;
}

/**
 * \internal
 * \brief Tests if the tx buffer of UART 0 is empty.
 *
 * \retval true if the buffer is empty
 * \retval false if buffer is full
 */
static inline bool uart0_tx_buffer_is_empty(void)
{
	if (avr_read_reg8(UCSR0A) & AVR_BIT(UDRE))
		return true;
	else
		return false;
}

/**
 * \internal
 * \brief Tests if the rx buffer of UART 0 is full.
 *
 * \retval true if buffer is full
 * \retval false if buffer is empty
 */
static inline bool uart0_rx_buffer_is_full(void)
{
	if (avr_read_reg8(UCSR0A) & AVR_BIT(RXC))
		return true;
	else
		return false;
}

/**
 * \internal
 * \brief Send data to UART 0 without checking if the pipline is empty.
 *
 * \param data Data to be sent.
 */
static inline void uart0_send_byte(uint8_t data)
{
	avr_write_reg8(UDR0, data);
}

/**
 * \internal
 * \brief Send data to UART 0 if transmitter pipeline is empty.
 *
 * This function checks if there is room left to add new data ( max is a byte)
 * in transmitter pipeline and copies the new data into it.
 *
 * \param data Data to be sent.
 * \retval true Data was written to send-buffer successfully.
 * \retval false Data could not be sent becausee data register is not empty.
 */
static inline bool uart0_put_byte(uint8_t data)
{
	if (avr_read_reg8(UCSR0A) & AVR_BIT(UDRE)) {
		avr_write_reg8(UDR0, data);
		return true;
	}else {
		return false ;
	}
}

/**
 * \internal
 * \brief Read data from UART 0 if data is available.
 *
 * This function checks if data is available in the receive queue and copies
 * it to the user supplied data pointer.
 *
 * \param data Pointer to memory where to store rx data.
 * \retval true Data available and written to b succesfully.
 * \retval false  No data available.
 */
static inline bool uart0_get_byte(uint8_t *data)
{
	if (avr_read_reg8(UCSR0A) & AVR_BIT(RXC)) {
		*data = avr_read_reg8(UDR0);
		return true;
	} else {
		return false;
	}
}

#if (UART_MODULES > 1)

/**
 * \internal
 * \brief Initializes the UART module 1.
 *
 * \param flags UART configuration flags
 */
extern void uart1_init(uart_flags_t flags);

/**
 * \internal
 * \brief Tests if all data has been transmitted from UART 1.
 *
 * \retval true All data has been sent (shift register and THR is empty).
 * \retval false There is still data either in the shift register or in the THR
 * register, or the transmitter is disabled.
 *
 */
static inline bool uart1_transmit_is_complete(void)
{
	if (avr_read_reg8(UCSR1A) & AVR_BIT(TXC))
		return true;
	else
		return false;
}

/**
 * \internal
 * \brief Tests if the tx buffer of UART 1 is empty.
 *
 * \retval true if the buffer is empty
 * \retval false if buffer is full
 */
static inline bool uart1_tx_buffer_is_empty(void)
{
	if (avr_read_reg8(UCSR1A) & AVR_BIT(UDRE))
		return true;
	else
		return false;
}

/**
 * \internal
 * \brief Tests if the rx buffer of UART 1 is full.
 *
 * \retval true if buffer is full
 * \retval false if buffer is empty
 */
static inline bool uart1_rx_buffer_is_full(void)
{
	if (avr_read_reg8(UCSR1A) & AVR_BIT(RXC))
		return true;
	else
		return false;
}

/**
 * \internal
 * \brief Send data to UART 1 without checking if the pipline is empty.
 *
 * \param data Data to be sent.
 */
static inline void uart1_send_byte(uint8_t data)
{
	avr_write_reg8(UDR1, data);
}

/**
 * \internal
 * \brief Send data to UART 1 if transmitter pipeline is empty.
 *
 * This function checks if there is room left to add new data ( max is a byte)
 * in transmitter pipeline and copies the new data into it.
 *
 * \param data Data to be sent.
 * \retval true Data was written to send-buffer successfully.
 * \retval false Data could not be sent becausee data register is not empty.
 */
static inline bool uart1_put_byte(uint8_t data)
{
	if (avr_read_reg8(UCSR1A) & AVR_BIT(UDRE)) {
		avr_write_reg8(UDR1, data);
		return true;
	}else {
		return false ;
	}
}

/**
 * \internal
 * \brief Read data from UART 1 if data is available.
 *
 * This function checks if data is available in the receive queue and copies
 * it to the user supplied data pointer.
 *
 * \param data Pointer to memory where to store rx data.
 * \retval true Data available and succesfully read.
 * \retval false  No data available.
 */
static inline bool uart1_get_byte(uint8_t *data)
{
	if (avr_read_reg8(UCSR1A) & AVR_BIT(RXC)) {
		*data = avr_read_reg8(UDR1);
		return true;
	} else {
		return false;
	}
}
#endif

//@}

#endif /* UART_UART_MEGA_H_INCLUDED */

/**
 * \file
 *
 * \brief UART definitions.
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
#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

#include <chip/uart.h>

/**
 * \ingroup uart_group
 * \defgroup uart_simple_group Simple UART Data Transfer interface
 *
 * This is a simple and lightweight data transfer interface for UARTs.
 * All transfers are polled, which means that neither the performance
 * nor the power consumption is optimal, but in many cases, it is good
 * enough.
 *
 * @{
 */

/**
 * \def uart_enable_clock(id)
 * \brief Enable the peripheral clock to UART \a id
 *
 * This function must be called before starting to initialize a UART
 * which is disabled.
 */
/**
 * \def uart_disable_clock(id)
 * \brief Disable the peripheral clock to UART \a id
 *
 * This function may be called after disabling a UART in order to
 * conserve a bit of power.
 */
#ifndef uart_enable_clock
# define uart_enable_clock(id)  do { } while (0)
# define uart_disable_clock(id)  do { } while (0)
#endif

//! \name UART enable/disable flags
//@{
/**
 * \def UART_FLAG_TX
 * \brief Enable/disable transmitter
 */
/**
 * \def UART_FLAG_RX
 * \brief Enable/disable receiver
 */
//@}

//! \name Initialization
//@{
/**
 * \def uart_enable
 * \brief Enable UART transmitter and/or receiver
 *
 * Note that this function is not IRQ safe in general (although it
 * happens to be on AVR32.)
 *
 * \param uart_id ID of the UART to be enabled
 * \param flags Bitwise combination of #UART_FLAG_TX and #UART_FLAG_RX
 * specifying whether TX, RX or both should be enabled.
 */
//@}

//! \name Transmitting Data
//@{
/**
 * \brief Send a string to the UART
 *
 * \param uart_id UART id.
 * \param data String to be sent.
 */
#define uart_put_string(uart_id, data)					\
	do {								\
		unsigned int i;						\
		for (i = 0; data[i] != 0; i++) {			\
			while (!uart_put_byte(uart_id, data[i]));	\
		}							\
	} while (0)

/**
 * \brief Send a given number of bytes to the UART
 *
 * \param uart_id UART id.
 * \param data bytes to be sent.
 * \param len the number of bytes to be sent.
 */
#define uart_put_bytes(uart_id, data, len)				\
	do {								\
		unsigned int i;						\
		for (i = 0; i < len; i++) {				\
			while (!uart_put_byte(uart_id, data[i]));	\
		}							\
	} while (0)

/**
 * \def uart_transmit_is_complete
 * \brief Tests if all data has been transmitted.
 *
 * \param uart_id UART id.
 * \retval true All data has been sent (shift and holding registers
 * empty.)
 * \retval false There is still data either in the shift register or in
 * the holding register, or the transmitter is disabled.
 */
/**
 * \def uart_tx_buffer_is_empty
 * \brief Test if tx buffer is empty.
 *
 * \param uart_id UART id.
 * \retval true The TX buffer is empty.
 * \retval false The TX buffer is full.
 */
/**
 * \def uart_send_byte
 * \brief Send data without checking if the pipline is empty.
 *
 * \param uart_id UART id.
 * \param data Data to be sent.
 */
/**
 * \def uart_put_byte
 * \brief Sends data if room is left in the tx pipeline.
 *
 * This function checks if there is room left to add new data (max is a
 * byte) in transmitter pipeline and copies the new data into it.
 *
 * \param uart_id UART id.
 * \param data Data to be sent.
 * \retval true Data was written to send-buffer successfully.
 * \retval false Data could not be sent becausee data register is not
 * empty.
 */
//@}

//! \name Receiving Data
//@{
/**
 * \def uart_rx_buffer_is_full
 * \brief Test if rx buffer is full.
 *
 * \param uart_id UART id.
 * \retval true if buffer is full.
 * \retval false if buffer is empty.
 */
/**
 * \def uart_get_byte
 * \brief Reads data if data is available in the rx buffer.
 *
 * This function checks if data is available in the receive queue and
 * copies it to the user supplied data pointer.
 *
 * \param uart_id UART id.
 * \param data Pointer to memory where to store rx data.
 * \retval true Data available and stored succesfully.
 * \retval false No data available.
 */
//@}

//! @}

#endif /* UART_H_INCLUDED */

/**
 * \file
 *
 * \brief UART Control interface
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
#ifndef UART_CTRL_H_INCLUDED
#define UART_CTRL_H_INCLUDED

/**
 * \defgroup uart_group UART Framework
 *
 * The UART Framework consists of one control interface and several
 * data transfer interfaces. The control interface, used for
 * initializing the UART and changing communication parameters at
 * run-time, should be lightweight and flexible enough for all
 * applications. Data transfers, on the other hand, is typically more
 * complicated and performance-sensitive, so the application will have
 * to pick the interface which best fits its needs.
 *
 * Because of this split between the control and data transfer logic,
 * initializing a UART is typically a two-step process (though more
 * advanced applications may want to do it in more steps, possibly with
 * some time in between and/or triggered by external events):
 *   - Set initial communication parameters. This is typically done by
 *     calling uart_ctrl_init_defaults().
 *   - Enable the appropriate data transfer interface. Some applications
 *     may need to enable TX and RX separately, and some data transfer
 *     interfaces have separate functions for enabling TX and RX, while
 *     others may be able to enable both in one go.
 */

/**
 * \ingroup uart_group
 * \defgroup uart_ctrl_group UART Control interface
 *
 * The UART Control interface can be used to alter the communication
 * parameters of a UART, e.g. baud rate, number of data bits, etc. The
 * interface consists of a large number of mostly simple functions,
 * which can be used to perform fairly powerful control operations when
 * combined together.
 *
 * When initializing a UART, it's usually a good idea to start out with
 * the default settings and alter them as necessary. The functions
 * uart_mode_defaults() and uart_baud_default() can be used to obtain
 * the default settings of a given UART (usually, all UARTs have the
 * same default settings for performance reasons, but the caller should
 * not rely on this.) One advantage of starting from defaults is that
 * the default value is usually a constant, so even after altering or
 * reading out various settings, the result is usually still constant,
 * so the compiler can optimize the whole init sequence into a small
 * handful of machine instructions.
 *
 * When altering the settings of an already-initialized UART, the
 * current settings can be read from the hardware by calling
 * uart_mode_read() and uart_baud_read(). The values returned by these
 * functions can then be altered by calling one or more of the mode or
 * baud manipulation functions.
 *
 * After all the necessary alterations have been made, the result can be
 * committed to hardware by calling uart_mode_write() and/or
 * uart_baud_write(). These are the only functions which actually alter
 * the hardware state.
 *
 * The UART control drivers do not initialize the hardware on their own.
 * Before the UART can be used, a valid mode and a valid baud rate must
 * both be set. The following code may be used to initialize the
 * hardware with default settings:
 * \verbatim
	struct uart_mode	mode;
	struct uart_baud	baud;

	mode = uart_mode_defaults(UART_ID);
	baud = uart_baud_default(UART_ID, &baud);
	uart_mode_write(UART_ID, &mode);
	uart_baud_write(UART_ID, &baud);\endverbatim
 *
 * If the default settings need to be stored as initial values of some
 * other data structure, any number of uart_mode_get_*() calls may be
 * performed on the 'mode' variable in the example above.
 *
 * The functions defined by this modules are generally not
 * interrupt-safe. If the UART control state is manipulated from both
 * interrupt and non-interrupt contexts, the caller must make sure to
 * disable (soft-)interrupts when necessary around these calls.
 *
 * @{
 */

#if defined(CONFIG_ARCH_AVR32)
# include <uart/ctrl_avr32.h>
#elif defined(CONFIG_CPU_MEGA)
# include <uart/ctrl_mega.h>
#elif defined(CONFIG_CPU_XMEGA)
# include <uart/ctrl_xmega.h>
#endif

/**
 * \brief Initialize a UART with default settings.
 */
#define uart_ctrl_init_defaults(uart_id)			\
	do {							\
		struct uart_mode	mode;			\
		struct uart_mode        *mode_ptr = &mode;      \
		struct uart_baud	baud;			\
		struct uart_baud        *baud_ptr = &baud;      \
								\
		mode = uart_mode_defaults(uart_id);		\
		uart_baud_default(uart_id, baud_ptr);	        \
		uart_mode_write(uart_id, mode_ptr);		\
		uart_baud_write(uart_id, baud_ptr);		\
	} while (0)

/**
 * \enum uart_mode_sb
 * \brief Number of stop bits
 *
 * These are the allowed values when specifying the number of stop bits
 * to use. It is safe to assume that #UART_MODE_SB_1 and #UART_MODE_SB_2
 * are always available, although some drivers may provide additional
 * choices.
 */
/**
 * \enum uart_mode_par
 * \brief Parity mode
 *
 * These are the allowed values when specifying the parity generation of
 * the UART. It is safe to assume that #UART_MODE_PAR_NONE,
 * #UART_MODE_PAR_EVEN and #UART_MODE_PAR_ODD are always available,
 * although some drivers may provide additional choices.
 */
/**
 * \fn struct uart_mode uart_mode_defaults(unsigned int uart_id)
 * \brief Return the default mode parameters of \a uart_id
 */
/**
 * \def uart_mode_read(uart_id)
 * \brief Return the currently active mode parameters of \a uart_id
 * \return A struct uart_mode representing the mode parameters
 */
/**
 * \def uart_mode_write(uart_id, mode)
 * \brief Activate the mode parameters \a mode on \a uart_id
 *
 * When this function returns, the hardware will be using the new
 * parameters specified by \a mode.
 *
 * \param uart_id The ID of the UART to be updated
 * \param mode A struct uart_mode representing the new mode parameters
 */
/**
 * \fn struct uart_mode uart_mode_set_chrlen(unsigned int uart_id,
 *		struct uart_mode *mode, uint8_t bits)
 * \brief Return a new mode value with the character length set to \a bits
 *
 * All mode parameters except for the character length is taken from \a
 * mode. This function does not change any hardware state.
 */
/**
 * \fn uint8_t uart_mode_get_chrlen(unsigned int uart_id, struct uart_mode mode)
 * \brief Return the current character length of \a mode
 *
 * This function returns the number of bits per character specified in
 * \a mode. Note that the settings currently used by the hardware may be
 * different.
 */
/**
 * \fn bool uart_mode_chrlen_is_valid(unsigned int uart_id, uint8_t bits)
 * \brief Check if the specified character length is supported by hardware
 * \param uart_id The ID of the UART to validate against
 * \param bits The bits-per-characters setting to be validated
 * \retval true The hardware supports a character length of \a bits
 * \retval false The hardware does not support a character length of \a bits
 */
/**
 * \fn struct uart_mode uart_mode_set_stop_bits(unsigned int uart_id,
 *		struct uart_mode *mode, enum uart_mode_sb value)
 * \brief Return a new mode value with number of stop bits set to \a value
 *
 * All mode parameters except for the number of stop bits is taken from
 * \a mode. This function does not change any hardware state.
 */
/**
 * \fn enum uart_mode_sb uart_mode_get_stop_bits(unsigned int uart_id,
 *		struct uart_mode mode)
 * \brief Return the number of stop bits set in \a mode
 *
 * This function returns the number of stop bits specified in \a mode.
 * Note that the settings currently used by the hardware may be
 * different.
 */
/**
 * \fn struct uart_mode uart_mode_set_parity(unsigned int uart_id,
 *		struct uart_mode *mode, enum uart_mode_par value)
 * \brief Return a new mode value with parity set to \a value
 *
 * All mode parameters except for the parity mode is taken from \a mode.
 * This function does not change any hardware state.
 */
/**
 * \fn enum uart_mode_par uart_mode_get_parity(unsigned int uart_id,
 *		struct uart_mode mode)
 * \brief Return the parity mode set in \a mode
 *
 * This function returns the parity generation mode specified in \a
 * mode. Note that the settings currently used by the hardware may be
 * different.
 */
/**
 * \fn struct uart_baud uart_baud_default(unsigned int uart_id,
 *              struct uart_baud *baud)
 * \brief Return the default baud value of \a uart_id
 */
/**
 * \def uart_baud_read(uart_id)
 * \brief Return the currently active baud value of \a uart_id
 * \return A struct uart_baud representing the current baud rate
 */
/**
 * \def uart_baud_write(uart_id, *baud)
 * \brief Activate the baud value \a baud on \a uart_id
 *
 * When this function returns, the hardware will be using the new
 * baud rate specified by \a baud. Note that \a baud is a
 * hardware-specific value which may be quite different from the
 * human-readable baud rate.
 *
 * \param uart_id The ID of the UART to be updated
 * \param baud A struct uart_baud representing the new baud rate
 */
/**
 * \fn void uart_baud_set_rate(unsigned int uart_id,
 *              struct uart_baud *baud, uint32_t rate);
 *
 * \note The baud rate specified by \a rate is assumed to be supported
 * by the hardware. If the new baud rate is received from any external
 * source, the caller must validate the new baud rate by calling
 * uart_baud_rate_is_valid() before calling this function.
 *
 * \param uart_id The ID of the UART which will use the new baud rate
 * \param baud Pointer to baud rate hardware definitions
 * \param rate The desired baud rate
 *
 * \return A hardware representation of \a rate to be used with \a mode
 */
/**
 * \fn bool uart_baud_rate_is_valid(unsigned int uart_id,
 *		struct uart_mode *mode, uint32_t rate)
 * \brief Check if the hardware supports a given baud rate
 * \param uart_id The ID of the UART to validate against
 * \param mode Mode value to be used with the new baud rate
 * \param rate The baud rate to be validated
 *
 * \retval true The hardware supports the baud rate
 * \retval false The hardware does not support the baud rate (i.e. it's
 * either too high or too low.)
 */

//! @}

#endif /* UART_CTRL_H_INCLUDED */

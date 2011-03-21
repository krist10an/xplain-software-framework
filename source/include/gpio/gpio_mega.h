/**
 * \file
 *
 * \brief ATmega specific GPIO implementation.
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
#ifndef GPIO_GPIO_MEGA_H_INCLUDED
#define GPIO_GPIO_MEGA_H_INCLUDED

#include <assert.h>

/**
 * \brief Pin number.
 *
 * A pin number is generated from the port and the pin on the port.
 * - Each port has 8 pins even if not available in the datasheet.
 * - The firs pin number is 0
 *
 * Examples:
 * The first pin on port A has the number 0.
 * The first pin on port B has the number 8.
 *
 */
typedef uint8_t gpio_pin_t;

/**
 * \brief Convert a pin number to a port number.
 *
 * \param pin Pin number
 *
 */
static inline port_t gpio_pin_to_port(gpio_pin_t pin)
{
	return (pin >> 3);
}

/**
 * \brief Generate pin mask from pin number.
 *
 * \param pin Pin number
 */
static inline uint8_t gpio_pin_to_mask(gpio_pin_t pin)
{
	return (1U << (pin & 0x07));
}

/**
 * \brief Create a GPIO pin number.
 *
 * This macro generates a pin number that can be used with the GPIO functions
 * For instance to create a pin number for pin 2 on PORTA use the macro with
 * the following parameters: CREATE_GPIO_PIN_NUMBER(PORTA, 2)
 *
 * \param port Port name, e.g. PORTB.
 * \param pin Pin number on the port. Valid values are 0 to 7.
 */
#define CREATE_GPIO_PIN(port, pin)	(GPIO_##port * 8 + (pin))

//! \name GPIO pin configuration flags
//!@{
#define GPIO_DIR_INPUT     (0 << 0)  //!< Set pin as input
#define GPIO_DIR_OUTPUT    (1 << 0)  //!< Set pin as output
#define GPIO_PULL_UP       (1 << 1)  //!< Enable pull-up
#define GPIO_INIT_HIGH     (1 << 1)  //!< Set initial pin state to high
#define GPIO_INIT_LOW      (0 << 1)  //!< Set initial pin state to low
//!@}

/**
 * \brief Sets a pin to high state.
 *
 * (High means drive the pin towards Vdd)
 *
 * \param pin Pin number.
 */
static inline void gpio_set_gpio_pin(gpio_pin_t pin)
{
	uint8_t portx;

	switch (gpio_pin_to_port(pin))
	{
#ifdef CONFIG_HAVE_GPIO_PORTA
	case GPIO_PORTA:
		portx = avr_read_reg8(PORTA);
		portx |= gpio_pin_to_mask(pin);
		avr_write_reg8(PORTA, portx);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTB
	case GPIO_PORTB:
		portx = avr_read_reg8(PORTB);
		portx |= gpio_pin_to_mask(pin);
		avr_write_reg8(PORTB, portx);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTC
	case GPIO_PORTC:
		portx = avr_read_reg8(PORTC);
		portx |= gpio_pin_to_mask(pin);
		avr_write_reg8(PORTC, portx);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTD
	case GPIO_PORTD:
		portx = avr_read_reg8(PORTD);
		portx |= gpio_pin_to_mask(pin);
		avr_write_reg8(PORTD, portx);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTE
	case GPIO_PORTE:
		portx = avr_read_reg8(PORTE);
		portx |= gpio_pin_to_mask(pin);
		avr_write_reg8(PORTE, portx);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTF
	case GPIO_PORTF:
		portx = avr_read_reg8(PORTF);
		portx |= gpio_pin_to_mask(pin);
		avr_write_reg8(PORTF, portx);
		break;
#endif
	}
}

/**
 * \brief Set pin to low state.
 *
 * Low in the sense of Vss.
 * \param pin Pin number.
 */
static inline void gpio_clear_gpio_pin(gpio_pin_t pin)
{
	uint8_t portx;

	switch (gpio_pin_to_port(pin))
	{
#ifdef CONFIG_HAVE_GPIO_PORTA
	case GPIO_PORTA:
		portx = avr_read_reg8(PORTA);
		portx &= ~gpio_pin_to_mask(pin);
		avr_write_reg8(PORTA, portx);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTB
	case GPIO_PORTB:
		portx = avr_read_reg8(PORTB);
		portx &= ~gpio_pin_to_mask(pin);
		avr_write_reg8(PORTB, portx);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTC
	case GPIO_PORTC:
		portx = avr_read_reg8(PORTC);
		portx &= ~gpio_pin_to_mask(pin);
		avr_write_reg8(PORTC, portx);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTD
	case GPIO_PORTD:
		portx = avr_read_reg8(PORTD);
		portx &= ~gpio_pin_to_mask(pin);
		avr_write_reg8(PORTD, portx);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTE
	case GPIO_PORTE:
		portx = avr_read_reg8(PORTE);
		portx &= ~gpio_pin_to_mask(pin);
		avr_write_reg8(PORTE, portx);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTF
	case GPIO_PORTF:
		portx = avr_read_reg8(PORTF);
		portx &= ~gpio_pin_to_mask(pin);
		avr_write_reg8(PORTF, portx);
		break;
#endif
	}
}

/**
 * \brief Set gpio pin value.
 *
 * \param pin GPIO pin number of the pin.
 * \param value Desired state of the pin. \a true means drive the pin high
 * (towards Vdd), while \a false means drive the pin low (towards Vss).
 *
 */
static inline void gpio_set_value(gpio_pin_t pin, bool value)
{
	if (value)
		gpio_set_gpio_pin(pin);
	else
		gpio_clear_gpio_pin(pin);
}

/**
 * \brief Read a GPIO pin value.
 *
 * Do not attempt to read a pin directly after you have set it.
 * To get the correct value in this case a nop() instruction needs to be added.
 *
 * \param pin GPIO pin number.
 * \returns true If pin is in high state (Vdd)
 * \returns false if pin is in low state (Vss)
 */
static inline bool gpio_get_value(gpio_pin_t pin)
{
	uint8_t pin_state = 0;

	switch (gpio_pin_to_port(pin))
	{
#ifdef CONFIG_HAVE_GPIO_PORTA
	case GPIO_PORTA:
		pin_state = avr_read_reg8(PINA);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTB
	case GPIO_PORTB:
		pin_state = avr_read_reg8(PINB);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTC
	case GPIO_PORTC:
		pin_state = avr_read_reg8(PINC);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTD
	case GPIO_PORTD:
		pin_state = avr_read_reg8(PIND);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTE
	case GPIO_PORTE:
		pin_state = avr_read_reg8(PINE);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTF
	case GPIO_PORTF:
		pin_state = avr_read_reg8(PINF);
		break;
#endif
	default:
		unhandled_case(gpio_pin_to_port(pin));
		break;
	}

	return pin_state & gpio_pin_to_mask(pin);
}

/**
 * \brief Configures a GPIO pin.
 *
 * \param pin Pin number of the GPIO pin.
 * \param flags Configuration options.
 *
 * GPIO_DIR_INPUT: Set GPIO pin as input.
 * GPIO_DIR_OUTPUT: Set GPIO pin as output.
 * GPIO_PULL_UP: Activate the pull up if configured as input.
 * GPIO_INIT_HIGH: Set output initial state as high.
 * GPIO_INIT_LOW: Set output initial state as low.
 */
__always_inline static void port_select_gpio_pin(gpio_pin_t pin, uint8_t flags)
{
	uint8_t ddrx;
	uint8_t portx;

	switch (gpio_pin_to_port(pin))
	{
#ifdef CONFIG_HAVE_GPIO_PORTA
	case GPIO_PORTA:
		ddrx = avr_read_reg8(DDRA);
		portx = avr_read_reg8(PORTA);

		if (flags & GPIO_DIR_OUTPUT)
			ddrx |= gpio_pin_to_mask(pin);
		else
			ddrx &= ~gpio_pin_to_mask(pin);

		if (flags & (GPIO_INIT_HIGH | GPIO_PULL_UP))
			portx |= gpio_pin_to_mask(pin);
		else
			portx &= ~gpio_pin_to_mask(pin);

		avr_write_reg8(PORTA, portx);
		avr_write_reg8(DDRA, ddrx);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTB
	case GPIO_PORTB:
		ddrx = avr_read_reg8(DDRB);
		portx = avr_read_reg8(PORTB);

		if (flags & GPIO_DIR_OUTPUT)
			ddrx |= gpio_pin_to_mask(pin);
		else
			ddrx &= ~gpio_pin_to_mask(pin);

		if (flags & (GPIO_INIT_HIGH | GPIO_PULL_UP))
			portx |= gpio_pin_to_mask(pin);
		else
			portx &= ~gpio_pin_to_mask(pin);

		avr_write_reg8(PORTB, portx);
		avr_write_reg8(DDRB, ddrx);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTC
	case GPIO_PORTC:
		ddrx = avr_read_reg8(DDRC);
		portx = avr_read_reg8(PORTC);

		if (flags & GPIO_DIR_OUTPUT)
			ddrx |= gpio_pin_to_mask(pin);
		else
			ddrx &= ~gpio_pin_to_mask(pin);

		if (flags & (GPIO_INIT_HIGH | GPIO_PULL_UP))
			portx |= gpio_pin_to_mask(pin);
		else
			portx &= ~gpio_pin_to_mask(pin);

		avr_write_reg8(PORTC, portx);
		avr_write_reg8(DDRC, ddrx);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTD
	case GPIO_PORTD:
		ddrx = avr_read_reg8(DDRD);
		portx = avr_read_reg8(PORTD);

		if (flags & GPIO_DIR_OUTPUT)
			ddrx |= gpio_pin_to_mask(pin);
		else
			ddrx &= ~gpio_pin_to_mask(pin);
		if (flags & (GPIO_INIT_HIGH |GPIO_PULL_UP))
			portx |= gpio_pin_to_mask(pin);
		else
			portx &= ~gpio_pin_to_mask(pin);

		avr_write_reg8(PORTD, portx);
		avr_write_reg8(DDRD, ddrx);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTE
	case GPIO_PORTE:
		ddrx = avr_read_reg8(DDRE);
		portx = avr_read_reg8(PORTE);

		if (flags & GPIO_DIR_OUTPUT)
			ddrx |= gpio_pin_to_mask(pin);
		else
			ddrx &= ~gpio_pin_to_mask(pin);
		if (flags & (GPIO_INIT_HIGH |GPIO_PULL_UP))
			portx |= gpio_pin_to_mask(pin);
		else
			portx &= ~gpio_pin_to_mask(pin);

		avr_write_reg8(PORTE, portx);
		avr_write_reg8(DDRE, ddrx);
		break;
#endif
#ifdef CONFIG_HAVE_GPIO_PORTF
	case GPIO_PORTF:
		ddrx = avr_read_reg8(DDRF);
		portx = avr_read_reg8(PORTF);

		if (flags & GPIO_DIR_OUTPUT)
			ddrx |= gpio_pin_to_mask(pin);
		else
			ddrx &= ~gpio_pin_to_mask(pin);
		if (flags & (GPIO_INIT_HIGH |GPIO_PULL_UP))
			portx |= gpio_pin_to_mask(pin);
		else
			portx &= ~gpio_pin_to_mask(pin);

		avr_write_reg8(PORTF, portx);
		avr_write_reg8(DDRF, ddrx);
		break;
#endif
	}
}
#endif /* GPIO_GPIO_MEGA_H_INCLUDED */

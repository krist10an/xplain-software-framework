/**
 * \file
 *
 * \brief GPIO portmux/gpio implementation
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
#ifndef GPIO_PORTMUX_GPIO_H_INCLUDED
#define GPIO_PORTMUX_GPIO_H_INCLUDED

#include <compiler.h>
#include <stdbool.h>
#include <gpio/portmux_gpio_regs.h>

/**
 * \brief A bitmask representing a set of pins on a port.
 *
 * Starting from pin 0 as the LSB, each bit in the mask corresponds to
 * a pin on some port. Each '1' bit includes the corresponding pin in
 * the set.
 */
typedef uint32_t	pin_mask_t;

/**
 * \brief A GPIO pin
 *
 * This type is used to describe the GPIO pins on the part.
 */
typedef unsigned int	gpio_pin_t;

/**
 * \brief Encode a GPIO portmux version number.
 *
 * \param major The major version number of the module.
 * \param minor The minor version number.
 *
 * \return An encoded version number which can be used to compare two
 * version numbers numerically. A higher major number will always
 * yield a numerically larger code. If the major number is the same, a
 * higher minor number will yield a numerically larger code.
 */
#define PORTMUX_GPIO_VER(major, minor)	(((major) << 8) | (minor))

/*
 * Portmux API starts here. See the Portmux-overview page for more
 * information
 */

enum portmux_function {
	PORTMUX_FUNC_A,
	PORTMUX_FUNC_B,
	PORTMUX_FUNC_C,
	PORTMUX_FUNC_D,
};

#define PORTMUX_DIR_INPUT	(0 << 0)
#define PORTMUX_DIR_OUTPUT	(1 << 0)
#define PORTMUX_INIT_LOW	(0 << 1)
#define PORTMUX_INIT_HIGH	(1 << 1)
#define PORTMUX_PULL_UP		(1 << 2)
#define PORTMUX_PULL_DOWN	(2 << 2)
#define PORTMUX_BUSKEEPER	(3 << 2)
#define PORTMUX_DRIVE_MIN	(0 << 4)
#define PORTMUX_DRIVE_LOW	(1 << 4)
#define PORTMUX_DRIVE_HIGH	(2 << 4)
#define PORTMUX_DRIVE_MAX	(3 << 4)
#define PORTMUX_OPEN_DRAIN	(1 << 6)

extern void portmux_select_peripheral(void *port, pin_mask_t pin_mask,
		enum portmux_function func, unsigned long flags);
extern void portmux_select_gpio(void *port, pin_mask_t pin_mask,
		unsigned long flags);

/* Portmux/GPIO conversion */
static inline void *gpio_pin_to_port(gpio_pin_t pin)
{
	return (void *)(GPIO_BASE + (pin >> 5) * 0x100);
}

static inline pin_mask_t gpio_pin_to_mask(gpio_pin_t pin)
{
	return 1U << (pin & 0x1f);
}

/**
 * \brief Select the GPIO function for a single pin.
 *
 * \param pin The pin to configure
 * \param flags Bitmask of flags specifying additional configuration
 * parameters.
 */
static inline void portmux_select_gpio_pin(gpio_pin_t pin, unsigned long flags)
{
	portmux_select_gpio(gpio_pin_to_port(pin), gpio_pin_to_mask(pin), flags);
}

/* Internal helper functions */

__always_inline static void gpio_set_value_inline(gpio_pin_t pin, bool value)
{
	pin_mask_t pin_mask = gpio_pin_to_mask(pin);
	void *port = gpio_pin_to_port(pin);

	if (value)
		gpio_write_reg(port, OVRS, pin_mask);
	else
		gpio_write_reg(port, OVRC, pin_mask);
}

__always_inline static bool gpio_get_value_inline(gpio_pin_t pin)
{
	return (gpio_read_reg(gpio_pin_to_port(pin), PVR) >> (pin & 0x1f)) & 1;
}

extern void gpio_set_value_noninline(gpio_pin_t pin, bool value);
extern bool gpio_get_value_noninline(gpio_pin_t pin);

/* GPIO API starts here */

/**
 * \brief Drive a GPIO pin to a given state.
 *
 * This function will only have an effect if \a pin is configured as
 * an output.
 *
 * \param pin A number identifying the pin to act on.
 * \param value The desired state of the pin. \a true means drive the
 * pin high (towards Vdd), while \a false means drive the pin low
 * (towards Vss).
 */
__always_inline static void gpio_set_value(gpio_pin_t pin, bool value)
{
	if (is_constant(pin))
		gpio_set_value_inline(pin, value);
	else
		gpio_set_value_noninline(pin, value);
}

/**
 * \brief Read the current state of a GPIO pin.
 *
 * \param pin A number identifying the pin to read.
 *
 * \retval true The pin is currently high (close to Vdd)
 * \retval false The pin is currently low (close to Vss)
 */
__always_inline static bool gpio_get_value(gpio_pin_t pin)
{
	if (is_constant(pin))
		return gpio_get_value_inline(pin);
	else
		return gpio_get_value_noninline(pin);
}

/**
 * \brief Toggle the current state of a GPIO pin.
 *
 * \param pin A number identifying the pin to toggle.
 */
__always_inline static void gpio_toggle_value(gpio_pin_t pin)
{
	pin_mask_t pin_mask = gpio_pin_to_mask(pin);
	void *port = gpio_pin_to_port(pin);

	gpio_write_reg(port, OVRT, pin_mask);
}
#endif /* GPIO_PORTMUX_GPIO_H_INCLUDED */

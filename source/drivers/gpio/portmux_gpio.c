/**
 * \file
 *
 * \brief GPIO portmux driver
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
#include <io.h>
#include <chip/memory-map.h>
#include <chip/portmux.h>

/**
 * \brief Select a peripheral function for a set of pins.
 *
 * \param port The GPIO port to which the pins belong.
 * \param pin_mask Bitmask of pins on \a port to be configured.
 * \param func The peripheral function to which the pins are to be
 * assigned.
 * \param flags Bitmask of flags specifying additional configuration
 * parameters.
 */
void portmux_select_peripheral(void *port, pin_mask_t pin_mask,
		enum portmux_function func, unsigned long flags)
{
	/* Both pull-up and pull-down set means buskeeper */
	if (portmux_gpio_version() >= PORTMUX_GPIO_VER(2, 0)) {
		if (flags & PORTMUX_PULL_DOWN)
			gpio_write_reg(port, PDERS, pin_mask);
		else
			gpio_write_reg(port, PDERC, pin_mask);
	}
	if (flags & PORTMUX_PULL_UP)
		gpio_write_reg(port, PUERS, pin_mask);
	else
		gpio_write_reg(port, PUERC, pin_mask);

	/* Select drive strength */
	if (portmux_gpio_version() >= PORTMUX_GPIO_VER(2, 0)) {
		if (flags & PORTMUX_DRIVE_LOW)
			gpio_write_reg(port, ODCR0S, pin_mask);
		else
			gpio_write_reg(port, ODCR0C, pin_mask);
		if (flags & PORTMUX_DRIVE_HIGH)
			gpio_write_reg(port, ODCR1S, pin_mask);
		else
			gpio_write_reg(port, ODCR1C, pin_mask);
	}

	/* Select function */
	if (func & PORTMUX_FUNC_B)
		gpio_write_reg(port, PMR0S, pin_mask);
	else
		gpio_write_reg(port, PMR0C, pin_mask);
	if (func & PORTMUX_FUNC_C)
		gpio_write_reg(port, PMR1S, pin_mask);
	else
		gpio_write_reg(port, PMR1C, pin_mask);

	/* Disable GPIO (i.e. enable peripheral) */
	gpio_write_reg(port, GPERC, pin_mask);
}

/**
 * \brief Select the GPIO function for a set of pins.
 *
 * \param port The GPIO port to which the pins belong.
 * \param pin_mask Bitmask of pins on \a port to be configured.
 * \param flags Bitmask of flags specifying additional configuration
 * parameters.
 */
void portmux_select_gpio(void *port, pin_mask_t pin_mask, unsigned long flags)
{
	/* Both pull-up and pull-down set means buskeeper */
	if (portmux_gpio_version() >= PORTMUX_GPIO_VER(2, 0)) {
		if (flags & PORTMUX_PULL_DOWN)
			gpio_write_reg(port, PDERS, pin_mask);
		else
			gpio_write_reg(port, PDERC, pin_mask);
	}
	if (flags & PORTMUX_PULL_UP)
		gpio_write_reg(port, PUERS, pin_mask);
	else
		gpio_write_reg(port, PUERC, pin_mask);

	/* Enable open-drain mode if requested */
	if (portmux_gpio_version() >= PORTMUX_GPIO_VER(2, 0)) {
		if (flags & PORTMUX_OPEN_DRAIN)
			gpio_write_reg(port, ODMERS, pin_mask);
		else
			gpio_write_reg(port, ODMERC, pin_mask);
	} else {
		if (flags & PORTMUX_OPEN_DRAIN)
			gpio_write_reg(port, PDERS, pin_mask);
		else
			gpio_write_reg(port, PDERC, pin_mask);
	}

	if (portmux_gpio_version() >= PORTMUX_GPIO_VER(2, 0)) {
		/* Select drive strength */
		if (flags & PORTMUX_DRIVE_LOW)
			gpio_write_reg(port, ODCR0S, pin_mask);
		else
			gpio_write_reg(port, ODCR0C, pin_mask);
		if (flags & PORTMUX_DRIVE_HIGH)
			gpio_write_reg(port, ODCR1S, pin_mask);
		else
			gpio_write_reg(port, ODCR1C, pin_mask);
	}

	/* Select direction and initial pin state */
	if (flags & PORTMUX_DIR_OUTPUT) {
		if (flags & PORTMUX_INIT_HIGH)
			gpio_write_reg(port, OVRS, pin_mask);
		else
			gpio_write_reg(port, OVRC, pin_mask);
		gpio_write_reg(port, ODERS, pin_mask);
	} else {
		gpio_write_reg(port, ODERC, pin_mask);
	}

	/* Enable GPIO */
	gpio_write_reg(port, GPERS, pin_mask);
}

/**
 * \brief Drive a GPIO pin to a given state (non-constant pin version).
 *
 * This function will only have an effect if \a pin is configured as
 * an output.
 *
 * \param pin A number identifying the pin to act on.
 * \param value The desired state of the pin. \a true means drive the
 * pin high (towards Vdd), while \a false means drive the pin low
 * (towards Vss).
 */
void gpio_set_value_noninline(gpio_pin_t pin, bool value)
{
	gpio_set_value_inline(pin, value);
}

/**
 * \brief Read the current state of a GPIO pin (non-constant pin version).
 *
 * \param pin A number identifying the pin to read.
 *
 * \retval true The pin is currently high (close to Vdd)
 * \retval false The pin is currently low (close to Vss)
 */
bool gpio_get_value_noninline(gpio_pin_t pin)
{
	return gpio_get_value_inline(pin);
}

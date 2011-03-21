/**
 * \file
 *
 * \brief I/O Ports Driver
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
#include <gpio/port.h>
#include <regs/xmega_port.h>


void port_select_gpio(void *port, pin_mask_t pin_mask,
	port_pin_flags_t flags)
{
	unsigned char pin;

	for (pin = 0; pin < 8; pin++) {
		if (pin_mask & (1 << pin))
			port_write_reg(port, PIN0CTRL + pin, flags);
	}

	/* Select direction and initial pin state */
	if (flags & PORT_DIR_OUTPUT) {
		if (flags & PORT_INIT_HIGH)
			port_write_reg(port, OUTSET, pin_mask);
		else
			port_write_reg(port, OUTCLR, pin_mask);
		port_write_reg(port, DIRSET, pin_mask);
	} else {
		port_write_reg(port, DIRCLR, pin_mask);
	}
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

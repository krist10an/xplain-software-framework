/**
 * \file
 *
 * \brief XMEGA specific functions for 4-wire resistive touch driver.
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
#ifndef CPU_TOUCH_H
#define CPU_TOUCH_H

#include <compiler.h>
#include <stdint.h>
#include <gpio.h>
#include <pmic.h>
#include <regs/xmega_adc.h>
#include <regs/xmega_portcfg.h>

/**
 * \ingroup touch_driver_4wres_group
 * \defgroup touch_driver_4wres_xmega_group XMEGA 4-wire resistive touch driver\
 * internals
 *
 * These functions default to pin interrupt type 0 for touch detection and
 * assume that the sense pins are on the same port with which the ADC is
 * associated, i.e., port A for ADCA and port B for ADCB.
 *
 * The ADC is configured for measurements from both "ends" of the tri-stated
 * touch surfaces for improved accuracy, and an interrupt is only generated
 * when both conversions have been made.
 *
 * @{
 */

/**
 * \name Pin mask macros for convenience
 *
 * @{
 */
#define TOUCH_XL_MASK         (1U << CONFIG_TOUCH_XL_PIN)
#define TOUCH_XR_MASK         (1U << CONFIG_TOUCH_XR_PIN)
#define TOUCH_YD_MASK         (1U << CONFIG_TOUCH_YD_PIN)
#define TOUCH_YU_MASK         (1U << CONFIG_TOUCH_YU_PIN)
#define TOUCH_X_MASK          (TOUCH_XL_MASK | TOUCH_XR_MASK)
#define TOUCH_Y_MASK          (TOUCH_YD_MASK | TOUCH_YU_MASK)
//@}

/**
 * \brief Initialize port for touch detection.
 *
 * Configure interrupt mask on Y pins, and prepare their output for voltage
 * gradient.
 *
 */
__always_inline static void touch_priv_port_init(void)
{
	// Mask touch detection pins for interrupt.
	port_write_reg(CONFIG_TOUCH_PORT_BASE, INT0MASK, TOUCH_Y_MASK);

	/* Set Y pin output for voltage gradient. This configuration
	 * is not ever changed by any of the other functions.
	 */
	port_write_reg(CONFIG_TOUCH_PORT_BASE, OUTCLR, TOUCH_YD_MASK);
	port_write_reg(CONFIG_TOUCH_PORT_BASE, OUTSET, TOUCH_YU_MASK);
}


/**
 * \brief Enable interrupt for touch detection.
 */
__always_inline static void touch_priv_port_enable_int(void)
{
	uint8_t int_bits;

	// Set level for interrupt type 0 on touch port as configured.
	int_bits = port_read_reg(CONFIG_TOUCH_PORT_BASE, INTCTRL);
	int_bits = PORT_BFINS(INTCTRL_INT0LVL, CONFIG_TOUCH_PORT_INTLVL,
			int_bits);
	port_write_reg(CONFIG_TOUCH_PORT_BASE, INTCTRL, int_bits);
}


/**
 * \brief Disable interrupt for touch detection.
 */
__always_inline static void touch_priv_port_disable_int(void)
{
	uint8_t int_bits;

	// Set level for interrupt type 0 on touch port to OFF.
	int_bits = port_read_reg(CONFIG_TOUCH_PORT_BASE, INTCTRL);
	int_bits = PORT_BFINS(INTCTRL_INT0LVL, PMIC_INTLVL_OFF, int_bits);
	port_write_reg(CONFIG_TOUCH_PORT_BASE, INTCTRL, int_bits);
}


/**
 * \brief Return current state of port interrupt flag.
 *
 * \retval true  Interrupt flag is set.
 * \retval false Interrupt flag is not set.
 */
__always_inline static bool touch_priv_port_is_int_flag_set(void)
{
	uint8_t int_flags;

	// Read out and check state of interrupt type 0 flag.
	int_flags = port_read_reg(CONFIG_TOUCH_PORT_BASE, INTFLAGS);
	int_flags &= PORT_BIT(INTFLAGS_INT0IF);

	return (int_flags != 0);
}


/**
 * \brief Clear the port interrupt flag.
 */
__always_inline static void touch_priv_port_clear_int_flag(void)
{
	port_write_reg(CONFIG_TOUCH_PORT_BASE, INTFLAGS, PORT_BIT(INTFLAGS_INT0IF));
}


/**
 * \brief Configure port for touch detection.
 *
 * Touch detection is done by use of a low-level interrupt.\n
 * This requires one touch surface to be grounded, and the other to have
 * pull-up and the interrupt sensing enabled.\n
 * In this function, X is grounded and Y used for the sensing.
 */
__always_inline static void touch_priv_port_set_detection(void)
{
	// Set YD and YU as inputs with totem pull-up and low level sensing.
	portcfg_write_reg(PORTCFG_BASE, MPCMASK, TOUCH_Y_MASK);
	port_write_reg(CONFIG_TOUCH_PORT_BASE, PIN0CTRL,
			PORT_BF(PINCTRL_OPC, PORT_PINCTRL_PULLUP)
			| PORT_BF(PINCTRL_ISC, PORT_PINCTRL_ISC_LEVEL));
	port_write_reg(CONFIG_TOUCH_PORT_BASE, DIRCLR, TOUCH_Y_MASK);

	// Drive XL and XR pins to ground.
	port_write_reg(CONFIG_TOUCH_PORT_BASE, OUTCLR, TOUCH_X_MASK);
	port_write_reg(CONFIG_TOUCH_PORT_BASE, DIRSET, TOUCH_X_MASK);
}


/**
 * \brief Configure port for gradient on X touch surface.
 */
__always_inline static void touch_priv_port_set_gradient_x(void)
{
	// Set surface Y pins as tri-stated inputs.
	portcfg_write_reg(PORTCFG_BASE, MPCMASK, TOUCH_Y_MASK);
	port_write_reg(CONFIG_TOUCH_PORT_BASE, PIN0CTRL,
			PORT_BF(PINCTRL_OPC, PORT_PINCTRL_TOTEM));
	port_write_reg(CONFIG_TOUCH_PORT_BASE, DIRCLR, TOUCH_Y_MASK);

	// Drive XR to VCC, set XL and XR as outputs to create gradient.
	port_write_reg(CONFIG_TOUCH_PORT_BASE, OUTSET, TOUCH_XR_MASK);
	port_write_reg(CONFIG_TOUCH_PORT_BASE, DIRSET, TOUCH_X_MASK);
}


/**
 * \brief Configure port for gradient on Y touch surface.
 *
 * To be able to detect the position along the Y axis, the YD and YU pins must
 * have different potentials, and the XL and XR pins be configured as tristated
 * inputs before using them for measurements with ADC.
 */
__always_inline static void touch_priv_port_set_gradient_y(void)
{
	// Set surface X pins as tri-stated inputs.
	portcfg_write_reg(PORTCFG_BASE, MPCMASK, TOUCH_X_MASK);
	port_write_reg(CONFIG_TOUCH_PORT_BASE, PIN0CTRL,
			PORT_BF(PINCTRL_OPC, PORT_PINCTRL_TOTEM));
	port_write_reg(CONFIG_TOUCH_PORT_BASE, DIRCLR, TOUCH_X_MASK);

	// Set YD and YU as outputs to create voltage gradient.
	port_write_reg(CONFIG_TOUCH_PORT_BASE, DIRSET, TOUCH_Y_MASK);
}


/**
 * \brief Initialize the ADC for readings from touch surfaces.
 *
 * Initializes ADC reference selection, channels 0 and 1, and clock prescaling.
 */
__always_inline static void touch_priv_adc_init(void)
{
	// Set 12 bit resolution, maximum prescaling and VCC/1.6 as reference.
	adc_write_reg(CONFIG_TOUCH_ADC_BASE, CTRLB,
			ADC_BF(RESOLUTION, ADC_RESOLUTION_12BIT));
	adc_write_reg(CONFIG_TOUCH_ADC_BASE, PRESCALER,
			ADC_BF(PRESCALER, ADC_PRESCALER_DIV512));
	adc_write_reg(CONFIG_TOUCH_ADC_BASE, REFCTRL,
			ADC_BF(REFSEL, ADC_REFSEL_INTVCC));

	// Set ADC channel 0 and 1 to single ended input mode.
	adcch_write_reg(CONFIG_TOUCH_ADC_BASE, CH0, CTRL,
			ADCCH_BF(INPUTMODE, ADCCH_INPUTMODE_SINGLEENDED));
	adcch_write_reg(CONFIG_TOUCH_ADC_BASE, CH1, CTRL,
			ADCCH_BF(INPUTMODE, ADCCH_INPUTMODE_SINGLEENDED));

	// Enable the ADC.
	adc_write_reg(CONFIG_TOUCH_ADC_BASE, CTRLA, ADC_BIT(ENABLE));
}


/**
 * \brief Enable ADC interrupts.
 *
 * Sets configured level for ADC interrupt on channel 1.
 */
__always_inline static void touch_priv_adc_enable_int(void)
{
	adcch_write_reg(CONFIG_TOUCH_ADC_BASE, CH1, INTCTRL,
			ADCCH_BF(INTLVL, CONFIG_TOUCH_ADC_INTLVL));
}


/**
 * \brief Disable ADC interrupts.
 *
 * Sets level for ADC interrupt on channel 1 to OFF.
 */
__always_inline static void touch_priv_adc_disable_int(void)
{
	adcch_write_reg(CONFIG_TOUCH_ADC_BASE, CH1, INTCTRL,
			ADCCH_BF(INTLVL, ADCCH_INTLVL_OFF));
}


/**
 * \brief Clear the interrupt flag for ADC.
 */
__always_inline static void touch_priv_adc_clear_int_flag(void)
{
	adc_write_reg(CONFIG_TOUCH_ADC_BASE, INTFLAGS, ADC_BIT(CH1IF));
}


/**
 * \brief Start ADC scan sequence to measure position on Y surface.
 *
 * Configure ADC channel 0 and 1 to measure from XL and XR, then trigger
 * conversions on both channels.
 *
 * The voltage on the XL and XR pins depends on the position along the Y axis.
 * By reading both XL and XR, the effect of touch proximity to either of these
 * pins is negated.
 */
__always_inline static void touch_priv_adc_set_surface_y(void)
{
	adcch_write_reg(CONFIG_TOUCH_ADC_BASE, CH0, MUXCTRL,
			ADCCH_BF(MUXPOS, CONFIG_TOUCH_XL_PIN));
	adcch_write_reg(CONFIG_TOUCH_ADC_BASE, CH1, MUXCTRL,
			ADCCH_BF(MUXPOS, CONFIG_TOUCH_XR_PIN));
}


/**
 * \brief Set ADC scan sequence to measure position on X surface.
 *
 * Configure ADC channel 0 and 1 to measure from YD and YU, then trigger
 * conversions on both channels.
 */
__always_inline static void touch_priv_adc_set_surface_x(void)
{
	adcch_write_reg(CONFIG_TOUCH_ADC_BASE, CH0, MUXCTRL,
			ADCCH_BF(MUXPOS, CONFIG_TOUCH_YD_PIN));
	adcch_write_reg(CONFIG_TOUCH_ADC_BASE, CH1, MUXCTRL,
			ADCCH_BF(MUXPOS, CONFIG_TOUCH_YU_PIN));
}


/**
 * \brief Trigger AD conversions.
 *
 * Starts conversions on ADC channels 0 and 1.
 */
__always_inline static void touch_priv_adc_start(void)
{
	adc_write_reg(CONFIG_TOUCH_ADC_BASE, CTRLA,
			ADC_BF(CHSTART, ADC_CHSTART_0 | ADC_CHSTART_1)
			| ADC_BIT(ENABLE));
}


/**
 * \brief Return mean of YD and YU ADC measurements.
 *
 * This function returns the mean of the last YD and YU ADC readings.
 */
__always_inline static uint16_t touch_priv_adc_get_x(void)
{
	uint16_t adc_result;

	// Read CH0 result, add CH1 result and divide by two.
	adc_result = adc_read_reg(CONFIG_TOUCH_ADC_BASE, CH0RESL);
	adc_result |= adc_read_reg(CONFIG_TOUCH_ADC_BASE, CH0RESH) << 8;
	adc_result += adc_read_reg(CONFIG_TOUCH_ADC_BASE, CH1RESL);
	adc_result += adc_read_reg(CONFIG_TOUCH_ADC_BASE, CH1RESH) << 8;

	// Divide by two to get the mean.
	adc_result >>= 1;

	return (adc_result);
}


/**
 * \brief Return mean of XL and XR ADC measurements.
 *
 * \note Measurements are computed in the same way for both X and Y surfaces.
 */
#define touch_priv_adc_get_y       touch_priv_adc_get_x

//! @}

#endif /* CPU_TOUCH_H */

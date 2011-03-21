/**
 * \file
 *
 * \brief Board specific ADC control for resistive touch driver
 *
 * This header file contains the default configuration for Display Xplained,
 * i.e., the pin masks, port and ADC base addresses, IRQ IDs and interrupt
 * levels to set for these.
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
#ifndef BOARD_TOUCH_H
#define BOARD_TOUCH_H

#include <pmic.h>
#include <clk/sys.h>
#include <regs/xmega_adc.h>
#include <regs/xmega_pr.h>

/**
 * \ingroup touch_driver_group
 * \defgroup touch_driver_board_group Board-specific ADC control for\
 * resistive touch driver
 *
 * This uses the configuration set in config-xplain.mk for the application.
 *
 * @{
 */

/**
 * \brief Clear PR bit for default touch ADC.
 *
 * \note The enable bits for neither the ADC nor the bandgap or temperature
 * references are restored if this function is called after
 * \ref board_disable_touch_adc() has been called.
 */
__always_inline static void board_enable_touch_adc(void)
{
	sysclk_enable_module(SYSCLK_PORT_A, SYSCLK_ADC);
}


/**
 * \brief Disable ADC and references, then set its PR bit.
 *
 * \note This leaves all configuration except the ADC and reference enable
 * bits intact.
 */
__always_inline static void board_disable_touch_adc(void)
{
	uint8_t temp;

	// Disable unnecessary references, leave selection.
	temp = adc_read_reg(CONFIG_TOUCH_ADC_BASE, REFCTRL);
	temp &= ADC_BFMASK(REFSEL);
	adc_write_reg(CONFIG_TOUCH_ADC_BASE, REFCTRL, temp);

	// Disable the ADC itself.
	temp = adc_read_reg(CONFIG_TOUCH_ADC_BASE, CTRLA);
	temp &= ~ADC_BIT(ENABLE);
	adc_write_reg(CONFIG_TOUCH_ADC_BASE, CTRLA, temp);

	// Finally, set the ADC's PR bit.
	sysclk_disable_module(SYSCLK_PORT_A, SYSCLK_ADC);
}

//! @}

#endif /* BOARD_TOUCH_H */

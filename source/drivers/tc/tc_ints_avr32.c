/**
 * \file
 *
 * \brief AVR32 TC channel interrupt support.
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

#include <intc.h>
#include <chip/irq-map.h>
#include <tc/tc_ints_avr32.h>
#include <assert.h>

/**
 * \defgroup tc_avr32_ints_internal_group Internal AVR32 TC Channel Interrupt \
 * Support
 * \ingroup tc_avr32_ints_group
 * \internal
 * @{
 */

/**
 * \brief Channel interrupt data.
 * \internal
 *
 */
struct tc_channel_int_data {
	/** Function pointer to callback function for channel interrupt */
	void (*channel_int_callback) (void *int_data);
	/** Pointer to data for channel interrupt */
	void *int_data;
};


#ifdef TC0_BASE
#ifndef CONFIG_TC0_INTLVL
#define CONFIG_TC0_INTLVL	0
#endif

/**
 * \brief Interrupt data for TC0 channels.
 * \internal
 */
static struct tc_channel_int_data tc0_int_data[3];

/**
 * \brief Timer 0 interrupt group handler.
 * \internal
 *
 * This interrupt handler reads out the group requests, determines which
 * group interrupt is pending and calls the installed callback function.
 *
 * \param int_data Pointer to interrupt data.
 */
static void tc0_irq_group_handler(void *int_data)
{
	uint32_t group_requests;
	int channel;

	group_requests = intc_get_group_requests(TC0_IRQ);
	channel = compiler_ctz(group_requests);

	tc0_int_data[channel].channel_int_callback(
		tc0_int_data[channel].int_data);

}
INTC_DEFINE_HANDLER(TC0_IRQ, tc0_irq_group_handler, CONFIG_TC0_INTLVL);
#endif

#ifdef TC1_BASE
#ifndef CONFIG_TC1_INTLVL
#define CONFIG_TC1_INTLVL	0
#endif

/**
 * \brief Interrupt data for TC1 channels
 * \internal
 */
static struct tc_channel_int_data tc1_int_data[3];

/**
 * \brief Timer 1 interrupt group handler.
 * \internal
 *
 * This interrupt handler reads out the group requests, determines which
 * group interrupt is pending and calls the installed callback function.
 *
 * \param int_data Pointer to interrupt data.
 */
static void tc1_irq_group_handler(void *int_data)
{
	uint32_t group_requests;
	int channel;

	group_requests = intc_get_group_requests(TC1_IRQ);
	channel = compiler_ctz(group_requests);

	tc1_int_data[channel].channel_int_callback(
		tc1_int_data[channel].int_data);

}
INTC_DEFINE_HANDLER(TC1_IRQ, tc1_irq_group_handler, CONFIG_TC1_INTLVL);
#endif

void tc_register_channel_int(int tc_id, int tc_channel,
		tc_callback_t callback, void *int_data)
{
	assert(tc_id < 2);
	assert(tc_channel < 3);
	assert(callback != NULL);

	switch(tc_id) {
#ifdef TC0_BASE
	case 0:
		intc_setup_handler(TC0_IRQ, CONFIG_TC0_INTLVL, NULL);
		tc0_int_data[tc_channel].channel_int_callback = callback;
		tc0_int_data[tc_channel].int_data = int_data;
		break;
#endif
#ifdef TC1_BASE
	case 1:
		intc_setup_handler(TC1_IRQ, CONFIG_TC1_INTLVL, NULL);
		tc1_int_data[tc_channel].channel_int_callback = callback;
		tc1_int_data[tc_channel].int_data = int_data;
		break;
#endif
	}
}
/** @} */

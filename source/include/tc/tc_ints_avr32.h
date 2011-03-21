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

#ifndef TC_TC_INTS_AVR32_H_INCLUDED
#define TC_TC_INTS_AVR32_H_INCLUDED
/**
 * \ingroup tc_avr32_group
 * \defgroup tc_avr32_ints_group AVR32 TC Channel Interrupt Support
 * @{
 *
 * The interrupt controller implementation does not support handling of
 * interrupts within a group. Because of that the TC must do this in order to
 * provide interrupt handling per TC channel.
 *
 * The TC interrupt level can be set by specifying the configuration symbol
 * CONFIG_TCx_INTLVL with the desired interrupt level, where x must be replaced
 * with the module index. For instance to set the interrupt level 1 for TC
 * module 0, CONFIG_TC0_INTLVL must be set to 1.
 *
 * By using the channel interrupt support module, different drivers like the
 * timer and the PWM can use the TC module at the same time. Each driver must
 * use a different channel and the block control interface should not be used.
 *
 */

/**
 * \brief TC callback function type.
 */
typedef void (*tc_callback_t) (void *);

/**
 * \brief Register an interrupt handler for a TC channel.
 *
 * \param tc_id Timer/counter ID.
 * \param tc_channel Timer/counter channel.
 * \param callback Function pointer to channel callback function.
 * \param int_data Pointer to data that should be passed to the interrupt
 * handler.
 *
 */
void tc_register_channel_int(int tc_id, int tc_channel,
		tc_callback_t callback, void *int_data);

/** @} */
#endif /* TC_TC_INTS_AVR32_H_INCLUDED */

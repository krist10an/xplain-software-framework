/**
 * \file
 *
 * \brief Oscillator management
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
#ifndef CLK_OSC_H_INCLUDED
#define CLK_OSC_H_INCLUDED

/**
 * \ingroup clk_group
 * \defgroup osc_group Oscillator Management
 *
 * This group contains functions and definitions related to configuring
 * and enabling/disabling on-chip oscillators.
 *
 * @{
 */

#include <chip/osc.h>

//! \name Oscillator Management
//@{
/**
 * \fn void osc_enable(uint8_t id)
 * \brief Enable oscillator \a id
 *
 * The startup time and mode value is automatically determined based on
 * definitions in the board code.
 */
/**
 * \fn void osc_disable(uint8_t id)
 * \brief Disable oscillator \a id
 */
/**
 * \fn osc_is_running(uint8_t id)
 * \brief Determine whether oscillator \a id is running
 * \retval true Oscillator \a id is running
 * \retval false Oscillator \a id is not running
 */
/**
 * \fn uint32_t osc_get_rate(uint8_t id)
 * \brief Return the frequency of oscillator \a id in Hz
 */
//@}

//! @}

#endif /* CLK_OSC_H_INCLUDED */

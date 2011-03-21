/**
 * \file
 *
 * \brief SRAM Interface Parameters for the HX8347A Display Controller
 *
 * Copyright (C) 2010 Atmel Corporation. All rights reserved.
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
#ifndef BUS_EBI_PARAMS_HX8347A_H_INCLUDED
#define BUS_EBI_PARAMS_HX8347A_H_INCLUDED

#include <bus/ebi/sram.h>

/**
 * \ingroup ebi_params_group
 * \brief Initialize EBI bus parameters for a HX8347A display controller
 *
 * \param cs Chip Select number to which the HX8347A chip is attached.
 */
static inline void ebi_init_hx8347a_params(unsigned int cs)
{
	struct ebi_sram_params params;

	ebi_sram_params_init_defaults(&params, cs);
	ebi_sram_set_setup_ns(&params, 0, 30, 0, 20);
	ebi_sram_set_pulse_ns(&params, 240, 210, 90, 60);
	ebi_sram_set_total_ns(&params, 270, 100);
	ebi_sram_set_bus_width(&params, EBI_SRAM_16_BITS, true);
	ebi_sram_params_write(&params, cs);
}

#endif /* BUS_EBI_PARAMS_HX8347A_H_INCLUDED */


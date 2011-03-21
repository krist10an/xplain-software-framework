/**
 * \file
 *
 * \brief AVR32 timer/counter TC.
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
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TC_TC_AVR32_H_INCLUDED
#define TC_TC_AVR32_H_INCLUDED

#include <chip/memory-map.h>

/**
 * \defgroup tc_avr32_group AVR32 TC
 * @{
 */

/**
 * \brief Get a pointer to the TC registers.
 * \internal
 *
 * \param id The ID of the TC module. This must be a constant value.
 * The first ID is 0 and it refers to the first TC in the peripheral module
 * address map. The other TCs that follow get increasing IDs  1, 2, 3 ...
 *
 * \return A pointer to a virtual address corresponding to the base
 * address of the selected TC peripheral.
 */
#define tc_get_regs(id)		((void *) (TC##id##_BASE))

/**
 * \brief Get a pointer to the TC channel registers.
 * \internal
 *
 * The register interface of the channels within a TC module are similar, so
 * it is possible to use the same register definitions with an offset to
 * work on all channles.
 * This function will provide the base address of the channel register
 * interface.
 *
 * \param id TC module ID. The first module on the chip has the ID 0.
 * \param channel Channel ID. Valid IDs are 0 to 2.
 */
#define tc_get_channel_regs(id, channel) \
	((void *) (TC##id##_BASE + (channel) * 0x40))

/** @} */
#endif /* TC_TC_AVR32_H_INCLUDED */

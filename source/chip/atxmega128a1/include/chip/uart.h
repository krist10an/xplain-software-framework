/**
 * \file
 *
 * \brief XMEGA chip-specific UART definitions.
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

#ifndef CHIP_UART_H_INCLUDED
#define CHIP_UART_H_INCLUDED

#include <chip/memory-map.h>
#include <clk/sys.h>
#include <uart/uart_xmega.h>

/**
 * \brief Get a pointer to the UART registers.
 *
 * \param id The id of the UART. The first id is 0 and it refers to UART0 on
 * port C which is also the first UART in the peripheral module address map.
 * The other UARTs that follow get increasing ids  1, 2, 3 ...,
 * So UART1 on port C has id=1, UART0 on port D gets id=2 ...
 *
 * Take a look at the chip/memory-map.h or the device datasheet for available
 * UARTs.
 *
 * \return A pointer to a virtual address corresponding to the base
 * address of the UART\a id registers.
 */
#define uart_get_regs(id)	((void *) USART##id##_BASE)

#define uart_enable_clock(id)                                           \
	do {                                                            \
		unsigned int _id = (id);                                \
									\
		sysclk_enable_module(SYSCLK_PORT_C + (_id >> 1),        \
				SYSCLK_USART0 + (_id & 1));             \
	} while (0)
#define uart_disable_clock(id)                                          \
	do {                                                            \
		unsigned int _id = (id);                                \
									\
		sysclk_disable_module(SYSCLK_PORT_C + (_id >> 1),       \
				SYSCLK_USART0 + (_id & 1));             \
	} while (0)

#endif /* CHIP_UART_H_INCLUDED */

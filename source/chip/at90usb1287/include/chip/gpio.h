/**
 * \file
 *
 * \brief GPIO chip specific implementation.
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
#ifndef CHIP_GPIO_H_INCLUDED
#define CHIP_GPIO_H_INCLUDED

#include <types.h>
#include <chip/regs.h>

/** \brief Ports on AT90USB1287.
 *
 * This device has 6 ports.
 * Each port has a port number according to the representing letter.
 * The first port A starts with number 0. Port B is 1, ...
 * The numbering is continuous, that means even if a port is not available
 * in the datasheet it is included in the port numbering.
 */
typedef enum {
	GPIO_PORTA = 0,
	GPIO_PORTB = 1,
	GPIO_PORTC = 2,
	GPIO_PORTD = 3,
	GPIO_PORTE = 4,
	GPIO_PORTF = 5,
} port_t;

#include <gpio/gpio_mega.h>

#endif /* CHIP_GPIO_H_INCLUDED */

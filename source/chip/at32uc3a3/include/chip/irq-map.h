/**
 * \file
 *
 * \brief Internal interrupt lines on the AT32UC3A3
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
#ifndef CHIP_IRQ_MAP_H_INCLUDED
#define CHIP_IRQ_MAP_H_INCLUDED

#define EIM_IRQ				 1
#define GPIO_IRQ			 2
#define PDCA_IRQ			 3
#define FLASHC_IRQ			 4
#define USART0_IRQ			 5
#define USART1_IRQ			 6
#define USART2_IRQ			 7
#define USART3_IRQ			 8
#define SPI0_IRQ			 9
#define SPI1_IRQ			10
#define TWIM0_IRQ			11
#define TWIM1_IRQ			12
#define SSC_IRQ				13
#define TC0_IRQ				14
#define ADC_IRQ				15
#define TC1_IRQ				16
#define USBB_IRQ			17
#define SDRAMC_IRQ			18
#define DAC_IRQ				19
#define MCI_IRQ				20
#define AES_IRQ				21
#define DMACA_IRQ			22
#define MSI_IRQ				26
#define TWIS0_IRQ			27
#define TWIS1_IRQ			28
#define ECCHRS_IRQ			29

#endif /* CHIP_IRQ_MAP_H_INCLUDED */

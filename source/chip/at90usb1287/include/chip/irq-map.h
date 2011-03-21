/**
 * \file
 *
 * \brief INTC chip specific interrupt vector IDs.
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
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

#define INT0_IRQ		2  /*! External interrupt 0 */
#define INT1_IRQ		3  /*! External interrupt 1 */
#define INT2_IRQ		4  /*! External interrupt 2 */
#define INT3_IRQ		5  /*! External interrupt 3 */
#define INT4_IRQ		6  /*! External interrupt 4 */
#define INT5_IRQ		7  /*! External interrupt 5 */
#define INT6_IRQ		8  /*! External interrupt 6 */
#define INT7_IRQ		9  /*! External interrupt 7 */
#define PCINT0_IRQ		10 /*! Pin change */
#define USB_GEN_IRQ		11 /*! USB general */
#define USB_EP_IRQ		12 /*! USB endpoint/pipe */
#define WDT_IRQ			13 /*! Watchdog time-out */
#define TIMER2_COMPA_IRQ	14 /*! Timer/Counter2 compare match A */
#define TIMER2_COMPB_IRQ	15 /*! Timer/Counter2 compare match B */
#define TIMER2_OVF_IRQ		16 /*! Timer/Counter2 overflow */
#define TIMER1_CAPT_IRQ		17 /*! Timer/Counter1 capture event */
#define TIMER1_COMPA_IRQ	18 /*! Timer/Counter1 compare match A */
#define TIMER1_COMPB_IRQ	19 /*! Timer/Counter1 compare match B */
#define TIMER1_COMPC_IRQ	20 /*! Timer/Counter1 compare match C */
#define TIMER1_OVF_IRQ		21 /*! Timer/Counter1 overflow */
#define TIMER0_COMPA_IRQ	22 /*! Timer/Counter0 compare match A */
#define TIMER0_COMPB_IRQ	23 /*! Timer/Counter0 compare match B */
#define TIMER0_OVF_IRQ		24 /*! Timer/Counter0 overflow */
#define SPI_STC_IRQ		25 /*! SPI serial transfer complete */
#define USART1_RX_IRQ		26 /*! USART1 RX complete */
#define USART1_UDRE_IRQ		27 /*! USART1 data register empty */
#define USART1_TX_IRQ		28 /*! USART1 TX complete */
/*! USART1 RX complete replicated due to chip naming convention */
#define USART0_RX_IRQ		USART1_RX_IRQ
/*! USART1 data register empty replicated due to chip naming convention */
#define USART0_UDRE_IRQ		USART1_UDRE_IRQ
/*! USART1 TX complete replicated due to chip naming convention */
#define USART0_TX_IRQ		USART1_TX_IRQ
#define ANALOG_COMP_IRQ		29 /*! Analog comparator */
#define ADC_IRQ			30 /*! ADC conversion complete */
#define EE_READY_IRQ		31 /*! EEPROM ready */
#define TIMER3_CAPT_IRQ		32 /*! Timer/Counter3 capture event */
#define TIMER3_COMPA_IRQ	33 /*! Timer/Counter3 compare match A */
#define TIMER3_COMPB_IRQ	34 /*! Timer/Counter3 compare match B */
#define TIMER3_COMPC_IRQ	35 /*! Timer/Counter3 compare match C */
#define TIMER3_OVF_IRQ		36 /*! Timer/Counter3 overflow */
#define TWI_IRQ			37 /*! 2-wire serial interface */
#define SPM_READY_IRQ		38 /*! Store program memory ready */

#endif /* CHIP_IRQ_MAP_H_INCLUDED */

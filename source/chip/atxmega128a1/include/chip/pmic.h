/**
 * \file
 *
 * \brief PMIC chip specific interrupt vector IDs.
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

#ifndef CHIP_PMIC_H_INCLUDED
#define CHIP_PMIC_H_INCLUDED

#define PMIC_OSCF_INT_IRQ      2
#define PMIC_PORTC_INT0_IRQ    3
#define PMIC_PORTC_INT1_IRQ    4
#define PMIC_PORTR_INT0_IRQ    5
#define PMIC_PORTR_INT1_IRQ    6
#define PMIC_DMA_INT_CH0_IRQ   7
#define PMIC_DMA_INT_CH1_IRQ   8
#define PMIC_DMA_INT_CH2_IRQ   9
#define PMIC_DMA_INT_CH3_IRQ   10
#define PMIC_RTC_OVF_IRQ       11
#define PMIC_RTC_COMP_IRQ      12
#define PMIC_TWI0_MASTER_IRQ   13 // TWI on port C
#define PMIC_TWI0_SLAVE_IRQ    14 // TWI on port C
#define PMIC_TC0_OVF_IRQ       15 // TC 0 on port C
#define PMIC_TC0_ERR_IRQ       16
#define PMIC_TC0_CCA_IRQ       17
#define PMIC_TC0_CCB_IRQ       18
#define PMIC_TC0_CCC_IRQ       19
#define PMIC_TC0_CCD_IRQ       20
#define PMIC_TC1_OVF_IRQ       21 // TC 1 on port C
#define PMIC_TC1_ERR_IRQ       22
#define PMIC_TC1_CCA_IRQ       23
#define PMIC_TC1_CCB_IRQ       24
#define PMIC_SPI0_IRQ          25 // SPI on port C
#define PMIC_USART0_RXC_IRQ    26 // USART 0 on port C
#define PMIC_USART0_DRE_IRQ    27
#define PMIC_USART0_TXC_IRQ    28
#define PMIC_USART1_RXC_IRQ    29 // USART 1 on port C
#define PMIC_USART1_DRE_IRQ    30
#define PMIC_USART1_TXC_IRQ    31
#define PMIC_AES_INT_IRQ       32
#define PMIC_NVM_SPM_IRQ       33
#define PMIC_NVM_EE_IRQ        34
#define PMIC_PORTB_INT0_IRQ    35
#define PMIC_PORTB_INT1_IRQ    36
#define PMIC_AC1_COMP0_IRQ     37 // AC on port B
#define PMIC_AC1_COMP1_IRQ     38
#define PMIC_AC1_WINDOW_IRQ    39
#define PMIC_ADC1_CH0_IRQ      40 // ADC on port B
#define PMIC_ADC1_CH1_IRQ      41
#define PMIC_ADC1_CH2_IRQ      42
#define PMIC_ADC1_CH3_IRQ      43
#define PMIC_PORTE_INT0_IRQ    44
#define PMIC_PORTE_INT1_IRQ    45
#define PMIC_TWI2_MASTER_IRQ   46 // TWI on port E
#define PMIC_TWI2_SLAVE_IRQ    47
#define PMIC_TC4_OVF_IRQ       48 // TC 0 on port E
#define PMIC_TC4_ERR_IRQ       49
#define PMIC_TC4_CCA_IRQ       50
#define PMIC_TC4_CCB_IRQ       51
#define PMIC_TC4_CCC_IRQ       52
#define PMIC_TC4_CCD_IRQ       53
#define PMIC_TC5_OVF_IRQ       54 // TC 1 on port E
#define PMIC_TC5_ERR_IRQ       55
#define PMIC_TC5_CCA_IRQ       56
#define PMIC_TC5_CCB_IRQ       57
#define PMIC_SPI2_IRQ          58 // SPI on port E
#define PMIC_USART4_RXC_IRQ    59 // USART 0 on port E
#define PMIC_USART4_DRE_IRQ    60
#define PMIC_USART4_TXC_IRQ    61
#define PMIC_USART5_RXC_IRQ    62 // USART 1 on port E
#define PMIC_USART5_DRE_IRQ    63
#define PMIC_USART5_TXC_IRQ    64
#define PMIC_PORTD_INT0_IRQ    65
#define PMIC_PORTD_INT1_IRQ    66
#define PMIC_PORTA_INT0_IRQ    67
#define PMIC_PORTA_INT1_IRQ    68
#define PMIC_AC0_COMP0_IRQ     69 // AC on port A
#define PMIC_AC0_COMP1_IRQ     70
#define PMIC_AC0_WINDOW_IRQ    71
#define PMIC_ADC0_CH0_IRQ      72 // ADC on port A
#define PMIC_ADC0_CH1_IRQ      73
#define PMIC_ADC0_CH2_IRQ      74
#define PMIC_ADC0_CH3_IRQ      75
#define PMIC_TWI1_MASTER_IRQ   76 // TWI on port D
#define PMIC_TWI1_SLAVE_IRQ    77
#define PMIC_TC2_OVF_IRQ       78 // TC 0 on port D
#define PMIC_TC2_ERR_IRQ       79
#define PMIC_TC2_CCA_IRQ       80
#define PMIC_TC2_CCB_IRQ       81
#define PMIC_TC2_CCC_IRQ       82
#define PMIC_TC2_CCD_IRQ       83
#define PMIC_TC3_OVF_IRQ       84 // TC 1 on port D
#define PMIC_TC3_ERR_IRQ       85
#define PMIC_TC3_CCA_IRQ       86
#define PMIC_TC3_CCB_IRQ       87
#define PMIC_SPI1_IRQ          88 // SPI on port D
#define PMIC_USART2_RXC_IRQ    89 // USART 0 on port D
#define PMIC_USART2_DRE_IRQ    90
#define PMIC_USART2_TXC_IRQ    91
#define PMIC_USART3_RXC_IRQ    92 // USART 1 on port D
#define PMIC_USART3_DRE_IRQ    93
#define PMIC_USART3_TXC_IRQ    94
#define PMIC_PORTQ_INT0_IRQ    95
#define PMIC_PORTQ_INT1_IRQ    96
#define PMIC_PORTH_INT0_IRQ    97
#define PMIC_PORTH_INT1_IRQ    98
#define PMIC_PORTJ_INT0_IRQ    99
#define PMIC_PORTJ_INT1_IRQ    100
#define PMIC_PORTK_INT0_IRQ    101
#define PMIC_PORTK_INT1_IRQ    102
/* Skipped. */
/* Skipped. */
#define PMIC_PORTF_INT0_IRQ    105
#define PMIC_PORTF_INT1_IRQ    106
#define PMIC_TWI3_MASTER_IRQ   107 // TWI on port F
#define PMIC_TWI3_SLAVE_IRQ    108
#define PMIC_TC6_OVF_IRQ       109 // TC 0 on port F
#define PMIC_TC6_ERR_IRQ       110
#define PMIC_TC6_CCA_IRQ       111
#define PMIC_TC6_CCB_IRQ       112
#define PMIC_TC6_CCC_IRQ       113
#define PMIC_TC6_CCD_IRQ       114
#define PMIC_TC7_OVF_IRQ       115 // TC 1 on port F
#define PMIC_TC7_ERR_IRQ       116
#define PMIC_TC7_CCA_IRQ       117
#define PMIC_TC7_CCB_IRQ       118
#define PMIC_SPI3_IRQ          119 // SPI on port F
#define PMIC_USART6_RXC_IRQ    120 // USART 0 on port F
#define PMIC_USART6_DRE_IRQ    121
#define PMIC_USART6_TXC_IRQ    122
#define PMIC_USART7_RXC_IRQ    123 // USART 1 on port F
#define PMIC_USART7_DRE_IRQ    124
#define PMIC_USART7_TXC_IRQ    125

#ifdef CONFIG_XMEGA_USB
# define PMIC_USB_BUSEVENT_IRQ 126 // XMEGA USB on port D
# define PMIC_USB_TRNCOMPL_IRQ 127
#endif

#endif /* CHIP_PMIC_H_INCLUDED */

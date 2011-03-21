/**
 * \file
 *
 * \brief Physical memory map for the ATxmega128A1.
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

#ifndef CHIP_MEMORY_MAP_H_INCLUDED
#define CHIP_MEMORY_MAP_H_INCLUDED

/* Internal memories */
#define CPU_SRAM_BASE                   0x2000
#define CPU_SRAM_SIZE                   0x2000
#define FLASH_BASE                      0x0000
#define FLASH_SIZE                      0x20000
#define FLASH_PAGE_SIZE                 512     // Bytes
#define EEPROM_BASE                     0x0000
#define EEPROM_MAPPED_BASE              0x1000
#define EEPROM_SIZE                     0x800
#define EEPROM_PAGE_SIZE                32      // Bytes
#define USER_SIGNATURE_SIZE             0x200
#define FUSES_LENGTH                    6       // Bytes

/* Some useful register locations */
#define DEVICE_ID_BASE                  0x90

/* Used by the linker script to place the writeable data sections */
#define DATA_SRAM_BASE                  CPU_SRAM_BASE

#define CLK_BASE                        0x0040 // Clock control
#define OSC_BASE                        0x0050 // Oscillator control

#define PR_BASE                         0x0070 //!< Power Reduction
#define PMIC_BASE                       0x00a0
#define USART0_BASE                     0x08a0 // USART 0 on port C
#define USART1_BASE                     0x08b0 // USART 1 on port C
#define USART2_BASE                     0x09a0 // USART 0 on port D
#define USART3_BASE                     0x09b0 // USART 1 on port D
#define USART4_BASE                     0x0aa0 // USART 0 on port E
#define USART5_BASE                     0x0ab0 // USART 1 on port E
#define USART6_BASE                     0x0ba0 // USART 0 on port F
#define USART7_BASE                     0x0bb0 // USART 1 on port F

#define DMA_BASE                        0x0100 // DMA controller
#define NVM_BASE                        0x01c0 // NVM controller
#define EBI_BASE                        0x0440 // External bus interface

#ifdef CONFIG_XMEGA_USB
# define USB_BASE                       0x04c0 // XMEGA USB controller
#endif

#define SPI0_BASE                       0x08c0 // SPI 0 on port C
#define SPI1_BASE                       0x09c0 // SPI 1 on port D
#define SPI2_BASE                       0x0ac0 // SPI 2 on port E
#define SPI3_BASE                       0x0bc0 // SPI 3 on port F

#define TC0_BASE                        0x0800 // TC0 on port C
#define TC1_BASE                        0x0840 // TC1 on port C
#define TC2_BASE                        0x0900 // TC0 on port D
#define TC3_BASE                        0x0940 // TC1 on port D
#define TC4_BASE                        0x0a00 // TC0 on port E
#define TC5_BASE                        0x0a40 // TC1 on port E
#define TC6_BASE                        0x0b00 // TC0 on port F
#define TC7_BASE                        0x0b40 // TC1 on port F
#define NVM_CONTROLLER_BASE             0x01C0 // NVM Controller

#define PORTCFG_BASE                    0x00B0 // Port Configuration
#define VPORT0_BASE                     0x0010 // Virtual Port 0
#define VPORT1_BASE                     0x0014 // Virtual Port 1
#define VPORT2_BASE                     0x0018 // Virtual Port 2
#define VPORT3_BASE                     0x001C // Virtual Port 3
#define PORTA_BASE                      0x0600 // Port A
#define PORTB_BASE                      0x0620 // Port B
#define PORTC_BASE                      0x0640 // Port C
#define PORTD_BASE                      0x0660 // Port D
#define PORTE_BASE                      0x0680 // Port E
#define PORTF_BASE                      0x06A0 // Port F
#define PORTH_BASE                      0x06E0 // Port H
#define PORTJ_BASE                      0x0700 // Port J
#define PORTK_BASE                      0x0720 // Port K
#define PORTQ_BASE                      0x07C0 // Port Q
#define PORTR_BASE                      0x07E0 // Port R

#define ADCA_BASE                       0x0200 // ADC on port A
#define ADCB_BASE                       0x0240 // ADC on port B
#define DACA_BASE                       0x0300 // DAC on port A
#define DACB_BASE                       0x0320 // DAC on port B
#define ACA_BASE                        0x0380 // AC on port A
#define ACB_BASE                        0x0390 // AC on port B

#endif /* CHIP_MEMORY_MAP_H_INCLUDED */


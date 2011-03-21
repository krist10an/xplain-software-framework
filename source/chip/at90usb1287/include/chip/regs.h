/**
 * \file
 *
 * \brief Register definitions for the AT90USB1287.
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

#ifndef CHIP_REGS_H_INCLUDED
#define CHIP_REGS_H_INCLUDED

#include <arch/io.h>

/* All register addresses are memory mapped */
#define AVR_REG_PINA	0x20
#define AVR_REG_DDRA	0x21
#define AVR_REG_PORTA	0x22
#define AVR_REG_PINB	0x23
#define AVR_REG_DDRB	0x24
#define AVR_REG_PORTB	0x25
#define AVR_REG_PINC	0x26
#define AVR_REG_DDRC	0x27
#define AVR_REG_PORTC	0x28
#define AVR_REG_PIND	0x29
#define AVR_REG_DDRD	0x2A
#define AVR_REG_PORTD	0x2B
#define AVR_REG_PINE	0x2C
#define AVR_REG_DDRE	0x2D
#define AVR_REG_PORTE	0x2E
#define AVR_REG_PINF	0x2F
#define AVR_REG_DDRF	0x30
#define AVR_REG_PORTF	0x31

#define AVR_REG_TIFR0	0x35
#define AVR_REG_TIFR1	0x36
#define AVR_REG_TIFR2	0x37
#define AVR_REG_TIFR3	0x38

#define AVR_REG_PCIFR	0x3B
#define AVR_REG_EIFR	0x3C
#define AVR_REG_EIMSK	0x3D

#define AVR_REG_GPIOR0	0x3E

#define AVR_REG_EECR	0x3F
#define AVR_REG_EEDR	0x40
#define AVR_REG_EEARL	0x41
#define AVR_REG_EEARH	0x42

#define AVR_REG_GTCCR	0x43

#define AVR_REG_TCCR0A	0x44
#define AVR_REG_TCCR0B	0x45
#define AVR_REG_TCNT0	0x46
#define AVR_REG_OCR0A	0x47
#define AVR_REG_OCR0B	0x48

#define AVR_REG_PLLCSR	0x49

#define AVR_REG_GPIOR1	0x4A
#define AVR_REG_GPIOR2	0x4B

#define AVR_REG_SPCR	0x4C
# define AVR_BF_SPCR_SPR_OFFSET		0
# define AVR_BF_SPCR_SPR_SIZE		2
# define AVR_BF_SPCR_CPHA_OFFSET	2
# define AVR_BF_SPCR_CPOL_OFFSET	3
# define AVR_BF_SPCR_MODE_OFFSET	2	//!< Mode covering CPHA and CPOL
# define AVR_BF_SPCR_MODE_SIZE		2
# define AVR_BF_SPCR_MSTR_OFFSET	4
# define AVR_BF_SPCR_DORD_OFFSET	5
# define AVR_BF_SPCR_SPE_OFFSET		6
# define AVR_BF_SPCR_SPIE_OFFSET	7
#define AVR_REG_SPSR	0x4D
# define AVR_BF_SPSR_SPI2X_OFFSET	0
# define AVR_BF_SPSR_WCOL_OFFSET	6
# define AVR_BF_SPSR_SPIF_OFFSET	7
#define AVR_REG_SPDR	0x4E

#define AVR_REG_ACSR	0x50

#define AVR_REG_OCDR	0x51
#define AVR_REG_MONDR	0x51

#define AVR_REG_SMCR	0x53

#define AVR_REG_MCUSR	0x54              //!< MCU reset cause register
# define AVR_BF_MCUSR_PORF_OFFSET       0 //!< Power-on reset flag
# define AVR_BF_MCUSR_EXTRF_OFFSET      1 //!< External reset flag
# define AVR_BF_MCUSR_BORF_OFFSET       2 //!< Brown-out reset flag
# define AVR_BF_MCUSR_WDRF_OFFSET       3 //!< Watchdog reset flag
# define AVR_BF_MCUSR_JTRF_OFFSET       4 //!< JTAG reset flag
#define AVR_REG_MCUCR	0x55              //!< MCU control register
# define AVR_BF_MCUCR_IVCE_OFFSET       0 //!< Interrupt vector change enable
# define AVR_BF_MCUCR_IVSL_OFFSET       1 //!< Interrupt vector select
# define AVR_BF_MCUCR_PUD_OFFSET        4 //!< Pull-up disable on all I/O pins
# define AVR_BF_MCUCR_JTD_OFFSET        7 //!< JTAG interface disable

#define AVR_REG_SPMCSR	0x57
#define AVR_REG_RAMPZ	0x5B
#define AVR_REG_SPL	0x5D
#define AVR_REG_SPH	0x5E
#define AVR_REG_SREG	0x5F

#define AVR_REG_WDTCSR	0x60

#define AVR_REG_CLKPR	0x61
#define AVR_REG_PRR0	0x64
#define AVR_REG_PRR1	0x65
#define AVR_REG_OSCCAL	0x66
#define AVR_REG_PCICR	0x68
#define AVR_REG_EICRA	0X69
#define AVR_REG_EICRB	0x6A

#define AVR_REG_PCMSK0	0x6B
#define AVR_REG_TIMSK0	0x6E
#define AVR_REG_TIMSK1	0x6F
#define AVR_REG_TIMSK2	0x70
#define AVR_REG_TIMSK3	0x71

#define AVR_REG_XMCRA	0x74
#define AVR_REG_XMCRB	0x75

#define AVR_REG_ADCL	0x78
#define AVR_REG_ADCH	0x79
#define AVR_REG_ADCSRA	0x7A
#define AVR_REG_ADCSRB	0x7B
#define AVR_REG_ADMUX	0x7C

#define AVR_REG_DIDR0	0x7E
#define AVR_REG_DIDR1	0x7F

#define AVR_REG_TCCR1A	0x80
#define AVR_REG_TCCR1B	0x81
#define AVR_REG_TCCR1C	0x82
#define AVR_REG_TCNT1L	0x84
#define AVR_REG_TCNT1H	0x85
#define AVR_REG_ICR1L	0x86
#define AVR_REG_ICR1H	0x87
#define AVR_REG_OCR1AL	0x88
#define AVR_REG_OCR1AH	0x89
#define AVR_REG_OCR1BL	0x8A
#define AVR_REG_OCR1BH	0x8B
#define AVR_REG_OCR1CL	0x8C
#define AVR_REG_OCR1CH	0x8D

#define AVR_REG_TCCR3A	0x90
#define AVR_REG_TCCR3B	0x91
#define AVR_REG_TCCR3C	0x92
#define AVR_REG_TCNT3L	0x94
#define AVR_REG_TCNT3H	0x95
#define AVR_REG_ICR3L	0x96
#define AVR_REG_ICR3H	0x97
#define AVR_REG_OCR3AL	0x98
#define AVR_REG_OCR3AH	0x99
#define AVR_REG_OCR3BL	0x9A
#define AVR_REG_OCR3BH	0x9B
#define AVR_REG_OCR3CL	0x9C
#define AVR_REG_OCR3CH	0x9D

#define AVR_REG_UHCON	0x9E
#define AVR_REG_UHINT	0x9F
#define AVR_REG_UHIEN	0xA0
#define AVR_REG_UHADDR	0xA1
#define AVR_REG_UHFNUML	0xA2
#define AVR_REG_UHFNUMH	0xA3
#define AVR_REG_UHFLEN	0xA4
#define AVR_REG_UPINRQX	0xA5
#define AVR_REG_UPINTX	0xA6
#define AVR_REG_UPNUM	0xA7
#define AVR_REG_UPRST	0xA8
#define AVR_REG_UPCONX	0xA9
#define AVR_REG_UPCFG0X	0xAA
#define AVR_REG_UPCFG1X	0xAB
#define AVR_REG_UPSTAX	0xAC
#define AVR_REG_UPCFG2X	0xAD
#define AVR_REG_UPIENX	0xAE
#define AVR_REG_UPDATX	0xAF

#define AVR_REG_TCCR2A	0xB0
#define AVR_REG_TCCR2B	0xB1
#define AVR_REG_TCNT2	0xB2
#define AVR_REG_OCR2A	0xB3
#define AVR_REG_OCR2B	0xB4

#define AVR_REG_ASSR	0xB6

#define AVR_REG_TWBR	0xB8
#define AVR_REG_TWSR	0xB9
#define AVR_REG_TWAR	0xBA
#define AVR_REG_TWDR	0xBB
#define AVR_REG_TWCR	0xBC
#define AVR_REG_TWAMR	0xBD

#define AVR_REG_UCSR1A	0xC8
#define AVR_REG_UCSR1B	0xC9
#define AVR_REG_UCSR1C	0xCA
#define AVR_REG_UBRR1L	0xCC
#define AVR_REG_UBRR1H	0xCD
#define AVR_REG_UDR1	0xCE

/* Hack for broken register naming scheme; USART1 with non-existing USART0. */
#define AVR_REG_UCSR0A	AVR_REG_UCSR1A
#define AVR_REG_UCSR0B	AVR_REG_UCSR1B
#define AVR_REG_UCSR0C	AVR_REG_UCSR1C
#define AVR_REG_UBRR0L	AVR_REG_UBRR1L
#define AVR_REG_UBRR0H	AVR_REG_UBRR1H
#define AVR_REG_UDR0	AVR_REG_UDR1

#define AVR_REG_UHWCON	0xD7
#define AVR_REG_USBCON	0xD8
#define AVR_REG_USBSTA	0xD9
#define AVR_REG_USBINT	0xDA
#define AVR_REG_OTGCON	0xDD
#define AVR_REG_OTGIEN	0xDE
#define AVR_REG_OTGINT	0xDF
#define AVR_REG_UDCON	0xE0
#define AVR_REG_UDINT	0xE1
#define AVR_REG_UDIEN	0xE2
#define AVR_REG_UDADDR	0xE3
#define AVR_REG_UDFNUML	0xE4
#define AVR_REG_UDFNUMH	0xE5
#define AVR_REG_UDMFN	0xE6
#define AVR_REG_UEINTX	0xE8
#define AVR_REG_UENUM	0xE9
#define AVR_REG_UERST	0xEA
#define AVR_REG_UECONX	0xEB
#define AVR_REG_UECFG0X	0xEC
#define AVR_REG_UECFG1X	0xED
#define AVR_REG_UESTA0X	0xEE
#define AVR_REG_UESTA1X	0xEF
#define AVR_REG_UEIENX	0xF0
#define AVR_REG_UEDATX	0xF1
#define AVR_REG_UEBCLX	0xF2
#define AVR_REG_UEBCHX	0xF3
#define AVR_REG_UEINT	0xF4
#define AVR_REG_UPERRX	0xF5
#define AVR_REG_UPBCLX	0xF6
#define AVR_REG_UPBCHX	0xF7
#define AVR_REG_UPINT	0xF8
#define AVR_REG_OTGTCON	0xF9

/* PLLCSR bitfield definitions */
#define AVR_BF_PLLCSR_PLOCK_SIZE	1
#define AVR_BF_PLLCSR_PLOCK_OFFSET	0
#define AVR_BF_PLLCSR_PLLE_SIZE		1
#define AVR_BF_PLLCSR_PLLE_OFFSET	1
#define AVR_BF_PLLCSR_PLLP0_SIZE	1
#define AVR_BF_PLLCSR_PLLP0_OFFSET	2
#define AVR_BF_PLLCSR_PLLP1_SIZE	1
#define AVR_BF_PLLCSR_PLLP1_OFFSET	3
#define AVR_BF_PLLCSR_PLLP2_SIZE	1
#define AVR_BF_PLLCSR_PLLP2_OFFSET	4

/* UCSRxA bitfield definitions */
#define AVR_BF_MPCM_SIZE	1
#define AVR_BF_MPCM_OFFSET	0
#define AVR_BF_U2X_SIZE		1
#define AVR_BF_U2X_OFFSET	1
#define AVR_BF_UPE_SIZE		1
#define AVR_BF_UPE_OFFSET	2
#define AVR_BF_DOR_SIZE		1
#define AVR_BF_DOR_OFFSET	3
#define AVR_BF_FE_SIZE		1
#define AVR_BF_FE_OFFSET	4
#define AVR_BF_UDRE_SIZE	1
#define AVR_BF_UDRE_OFFSET	5
#define AVR_BF_TXC_SIZE		1
#define AVR_BF_TXC_OFFSET	6
#define AVR_BF_RXC_SIZE		1
#define AVR_BF_RXC_OFFSET	7

/* UCSRxB bitfield definitions */
#define AVR_BF_TXEN_SIZE	1
#define AVR_BF_TXEN_OFFSET	3
#define AVR_BF_RXEN_SIZE	1
#define AVR_BF_RXEN_OFFSET	4
#define AVR_BF_UDRIE_SIZE	1
#define AVR_BF_UDRIE_OFFSET	5
#define AVR_BF_TXCIE_SIZE	1
#define AVR_BF_TXCIE_OFFSET	6
#define AVR_BF_RXCIE_SIZE	1
#define AVR_BF_RXCIE_OFFSET	7

/* UCSRxC bitfield definitions */
#define AVR_BF_UCSZ_SIZE	2
#define AVR_BF_UCSZ_OFFSET	1
#define AVR_BF_USBS_SIZE	1
#define AVR_BF_USBS_OFFSET	3
#define AVR_BF_UPM_SIZE		2
#define AVR_BF_UPM_OFFSET	4

/* Bit manipulation macros */
#define AVR_BIT(name) 					\
	(1 << AVR_BF_##name##_OFFSET)
#define AVR_BF(name,value)				\
	(((value) & ((1 << AVR_BF_##name##_SIZE) - 1))	\
	<< AVR_BF_##name##_OFFSET)
#define AVR_BFEXT(name,value)				\
	(((value) >> AVR_BF_##name##_OFFSET)		\
	& ((1 << AVR_BF_##name##_SIZE) - 1))
#define AVR_BFINS(name,value,old)			\
	(((old) & ~(((1 << AVR_BF_##name##_SIZE) - 1)	\
	<< AVR_BF_##name##_OFFSET))			\
	| AVR_BF(name,value))

#define avr_read_reg8(reg)                              \
	mmio_read8((void *)(AVR_REG_##reg))
#define avr_write_reg8(reg, value)                      \
	mmio_write8((void *)(AVR_REG_##reg), value)

#endif /* CHIP_REGS_H_INCLUDED */

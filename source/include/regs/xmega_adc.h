/**
 * \file
 *
 * \brief XMEGA ADC and ADC Channel Register Interface.
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

#ifndef REGS_XMEGA_ADC_H_INCLUDED
#define REGS_XMEGA_ADC_H_INCLUDED

#include <chip/memory-map.h>
#include <io.h>

/**
 * \ingroup regs_group
 * \defgroup adc_regs_group ADC Register Definitions
 * @{
 */

//! \name Register offsets
//@{
#define ADC_CTRLA               0x00  //!< Control register A
#define ADC_CTRLB               0x01  //!< Control register B
#define ADC_REFCTRL             0x02  //!< Reference control
#define ADC_EVCTRL              0x03  //!< Event control
#define ADC_PRESCALER           0x04  //!< ADC clock prescaling
/* Reserved */
#define ADC_INTFLAGS            0x06  //!< Interrupt flags for ADC channels
#define ADC_TEMP                0x07  //!< Temporary register for 16-bit reads
/* Reserved */
#define ADC_CALL                0x0C  //!< ADC calibration low byte
#define ADC_CALH                0x0D  //!< ADC calibration high byte
/* Reserved */
#define ADC_CH0RESL             0x10  //!< ADC channel 0 result low byte
#define ADC_CH0RESH             0x11  //!< ADC channel 0 result high byte
#define ADC_CH1RESL             0x12  //!< ADC channel 1 result low byte
#define ADC_CH1RESH             0x13  //!< ADC channel 1 result high byte
#define ADC_CH2RESL             0x14  //!< ADC channel 2 result low byte
#define ADC_CH2RESH             0x15  //!< ADC channel 2 result high byte
#define ADC_CH3RESL             0x16  //!< ADC channel 3 result low byte
#define ADC_CH3RESH             0x17  //!< ADC channel 3 result high byte
#define ADC_CMPL                0x18  //!< ADC compare low byte
#define ADC_CMPH                0x19  //!< ADC compare high byte
//@}

//! \name Bitfields in CTRLA
//@{
#define ADC_ENABLE_BIT          0  //!< ADC enable
#define ADC_FLUSH_BIT           1  //!< ADC flush

#define ADC_CHSTART_START       2  //!< ADC channel start single conversion
#define ADC_CHSTART_SIZE        4  //!< ADC channel start single conversion
#define ADC_DMASEL_START        6  //!< ADC channel DMA request selection
#define ADC_DMASEL_SIZE         2  //!< ADC channel DMA request selection
//@}

//! \name CHSTART bitfield values
//@{
#define ADC_CHSTART_0           1  //!< Start conversion on ADC channel 0
#define ADC_CHSTART_1           2  //!< Start conversion on ADC channel 1
#define ADC_CHSTART_2           4  //!< Start conversion on ADC channel 2
#define ADC_CHSTART_3           8  //!< Start conversion on ADC channel 3
//@}

//! \name DMASEL bitfield values
//@{
#define ADC_DMASEL_OFF          0  //!< No combined DMA request
#define ADC_DMASEL_CH01         1  //!< ADC channel 0 and 1 can trigger DMA
#define ADC_DMASEL_CH012        2  //!< ADC channel 0, 1 and 2 can trigger DMA
#define ADC_DMASEL_CH0123       3  //!< All ADC channels can trigger DMA
//@}

//! \name Bitfields in CTRLB
//@{
#define ADC_RESOLUTION_START    1  //!< ADC conversion resolution
#define ADC_RESOLUTION_SIZE     2  //!< ADC conversion resolution
#define ADC_FREERUN_BIT         3  //!< ADC free running mode
#define ADC_CONVMODE_BIT        4  //!< ADC signed/unsigned conversion mode
//@}

//! \name RESOLUTION bitfield values
//@{
#define ADC_RESOLUTION_12BIT      0  //!< 12-bit result, right adjusted
#define ADC_RESOLUTION_8BIT       2  //!< 8-bit result, right adjusted
#define ADC_RESOLUTION_LEFT12BIT  3  //!< 12-bit result, left adjusted
//@}

//! \name Bitfields in REFCTRL
//@{
#define ADC_TEMPREF_BIT        0  //!< Enable temperature reference for measurement
#define ADC_BANDGAP_BIT        1  //!< Enable bandgap for measurement

#define ADC_REFSEL_START       4  //!< ADC reference selection
#define ADC_REFSEL_SIZE        2  //!< ADC reference selection
//@}

//! \name REFSEL bitfield values
//@{
#define ADC_REFSEL_INT1V       0  //!< Internal 1.00 V as reference
#define ADC_REFSEL_INTVCC      1  //!< Internal VCC/1.6 V as reference
#define ADC_REFSEL_AREFA       2  //!< External reference from AREFA
#define ADC_REFSEL_AREFB       3  //!< External reference from AREFB
//@}

//! \name Bitfields in EVCTRL
//@{
#define ADC_EVACT_START        0  //!< ADC event mode
#define ADC_EVACT_SIZE         3  //!< ADC event mode
#define ADC_EVSEL_START        3  //!< ADC event line selection
#define ADC_EVSEL_SIZE         3  //!< ADC event line selection
#define ADC_SWEEP_START        6  //!< ADC channel sweep selection
#define ADC_SWEEP_SIZE         2  //!< ADC channel sweep selection
//@}

//! \name EVACT bitfield values
//@{
//!< No event action
#define ADC_EVACT_NONE         0
//! Lowest event line triggers conversion on ADC channel 0
#define ADC_EVACT_CH0          1
//! Two lowest event lines trigger conversions on ADC channels 0 and 1
#define ADC_EVACT_CH01         2
//! Three lowest event lines trigger conversions on ADC channels 0, 1 and 2
#define ADC_EVACT_CH012        3
//! All event lines trigger conversions on the respective ADC channels
#define ADC_EVACT_CH0123       4
//! Lowest event line triggers channel sweep
#define ADC_EVACT_SWEEP        5
//! Lowest event line triggers channel sweep with synchronization
#define ADC_EVACT_SYNCSWEEP    6
//@}

//! \name EVSEL bitfield values
//@{
#define ADC_EVSEL_0123       0  //!< Event channels 0, 1, 2 and 3 as event lines
#define ADC_EVSEL_1234       1  //!< Event channels 1, 2, 3 and 4 as event lines
#define ADC_EVSEL_2345       2  //!< Event channels 2, 3, 4 and 5 as event lines
#define ADC_EVSEL_3456       3  //!< Event channels 3, 4, 5 and 6 as event lines
#define ADC_EVSEL_4567       4  //!< Event channels 4, 5, 6 and 7 as event lines
#define ADC_EVSEL_567        5  //!< Event channels 5, 6, and 7 as event lines
#define ADC_EVSEL_67         6  //!< Event channels 6 and 7 as event lines
#define ADC_EVSEL_7          7  //!< Event channel 7 as event line
//@}

//! \name SWEEP bitfield values
//@{
#define ADC_SWEEP_0            0  //!< Sweep ADC channel 0 only
#define ADC_SWEEP_01           1  //!< Sweep ADC channels 0 and 1
#define ADC_SWEEP_012          2  //!< Sweep ADC channels 0, 1 and 2
#define ADC_SWEEP_0123         3  //!< Sweep all ADC channels
//@}

//! \name Bitfields in PRESCALER
//@{
#define ADC_PRESCALER_START    0  //!< ADC clock prescaling
#define ADC_PRESCALER_SIZE     3  //!< ADC clock prescaling
//@}

//! \name PRESCALER bitfield values
//@{
#define ADC_PRESCALER_DIV4     0  //!< Prescale peripheral clock by 4
#define ADC_PRESCALER_DIV8     1  //!< Prescale peripheral clock by 8
#define ADC_PRESCALER_DIV16    2  //!< Prescale peripheral clock by 16
#define ADC_PRESCALER_DIV32    3  //!< Prescale peripheral clock by 32
#define ADC_PRESCALER_DIV64    4  //!< Prescale peripheral clock by 64
#define ADC_PRESCALER_DIV128   5  //!< Prescale peripheral clock by 128
#define ADC_PRESCALER_DIV256   6  //!< Prescale peripheral clock by 256
#define ADC_PRESCALER_DIV512   7  //!< Prescale peripheral clock by 512
//@}

//! \name Bitfields in INTFLAGS
//@{
#define ADC_CH0IF_BIT          0  //!< ADC channel 0 interrupt flag
#define ADC_CH1IF_BIT          1  //!< ADC channel 1 interrupt flag
#define ADC_CH2IF_BIT          2  //!< ADC channel 2 interrupt flag
#define ADC_CH3IF_BIT          4  //!< ADC channel 3 interrupt flag
//@}

//! \name Bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set.
#define ADC_BIT(name) \
	(1U << ADC_##name##_BIT)
//! \brief Create a mask with bitfield \a name set to \a value.
#define ADC_BF(name, value) \
	((value) << ADC_##name##_START)
//! \brief Create a mask of the bitfield \a name.
#define ADC_BFMASK(name) \
	(((1U << ADC_##name##_SIZE) - 1) << ADC_##name##_START)
//! \brief Extract the value of bitfield \a name from \a regval.
#define ADC_BFEXT(name, regval) \
	(((regval) >> ADC_##name##_START) \
		& ((1U << ADC_##name##_SIZE) - 1))
//! \brief Return \a regval with bitfield \a name set to \a value.
#define ADC_BFINS(name, value, regval) \
	(((regval) & ~(((1U << ADC_##name##_SIZE) - 1) \
			<< ADC_##name##_START)) \
	| ADC_BF(name, value))
//@}

//! \name Register access macros
//@{
//! \brief Read the value of register \a reg on ADC with base address \a base.
#define adc_read_reg(base, reg) \
	mmio_read8((void *)((uintptr_t)(base) + ADC_##reg))
//! \brief Write \a value to register \a reg on ADC with base address \a base.
#define adc_write_reg(base, reg, value) \
	mmio_write8((void *)((uintptr_t)(base) + ADC_##reg), (value))
//@}

//@}


/**
 * \ingroup regs_group
 * \defgroup adcch_regs_group ADC Channel Register Definitions
 * @{
 */

//! \name Channel register group offsets
//{@
#define ADCCH_CH0             0x20  //!< ADC channel 0 register group
#define ADCCH_CH1             0x28  //!< ADC channel 1 register group
#define ADCCH_CH2             0x30  //!< ADC channel 2 register group
#define ADCCH_CH3             0x38  //!< ADC channel 3 register group
//@}

//! \name Channel register offsets
//@{
#define ADCCH_CTRL            0x00  //!< ADC channel control
#define ADCCH_MUXCTRL         0x01  //!< ADC channel MUX control
#define ADCCH_INTCTRL         0x02  //!< ADC channel interrupt control
#define ADCCH_INTFLAG         0x03  //!< ADC channel interrupt flag
#define ADCCH_RESL            0x04  //!< ADC channel result low byte
#define ADCCH_RESH            0x05  //!< ADC channel result high byte
//@}

//! \name Bitfields in channel CTRL
//@{
#define ADCCH_INPUTMODE_START  0  //!< Channel input mode
#define ADCCH_INPUTMODE_SIZE   2  //!< Channel input mode
#define ADCCH_GAIN_START       2  //!< Channel gain setting
#define ADCCH_GAIN_SIZE        3  //!< Channel gain setting
#define ADCCH_START_BIT        7  //!< Start conversion on channel
//@}

//! \name INPUTMODE bitfield values
//@{
#define ADCCH_INPUTMODE_INTERNAL     0  //!< Internal input signal
#define ADCCH_INPUTMODE_SINGLEENDED  1  //!< Single-ended input signal
#define ADCCH_INPUTMODE_DIFF         2  //!< Differential input signal
#define ADCCH_INPUTMODE_DIFFWGAIN    3  //!< Differential input signal with gain
//@}

//! \name GAIN bitfield values
//@{
#define ADCCH_GAIN_1X          0  //!< Unity gain for differential signal
#define ADCCH_GAIN_2X          1  //!< 2x gain for differential signal
#define ADCCH_GAIN_4X          2  //!< 4x gain for differential signal
#define ADCCH_GAIN_8X          3  //!< 8x gain for differential signal
#define ADCCH_GAIN_16X         4  //!< 16x gain for differential signal
#define ADCCH_GAIN_32X         5  //!< 32x gain for differential signal
#define ADCCH_GAIN_64X         6  //!< 64x gain for differential signal
//@}

//! \name Bitfields in channel MUXCTRL
//@{
#define ADCCH_MUXNEG_START     0  //!< Negative input signal MUX selection
#define ADCCH_MUXNEG_SIZE      2  //!< Negative input signal MUX selection
#define ADCCH_MUXPOS_START     3  //!< Positive input signal MUX selection
#define ADCCH_MUXPOS_SIZE      3  //!< Positive input signal MUX selection
#define ADDCH_MUXPOS3_BIT      6  //!< Use pin on secondary port
//@}

//! \name MUXNEG bitfield values
//@{
#define ADCCH_MUXNEG_PIN0      0  //!< ADC0 pin as negative input
#define ADCCH_MUXNEG_PIN1      1  //!< ADC1 pin as negative input
#define ADCCH_MUXNEG_PIN2      2  //!< ADC2 pin as negative input
#define ADCCH_MUXNEG_PIN3      3  //!< ADC3 pin as negative input
#define ADCCH_MUXNEG_PIN4      4  //!< ADC4 pin as negative input
#define ADCCH_MUXNEG_PIN5      5  //!< ADC5 pin as negative input
#define ADCCH_MUXNEG_PIN6      6  //!< ADC6 pin as negative input
#define ADCCH_MUXNEG_PIN7      7  //!< ADC7 pin as negative input
//@}

//! \name MUXPOS bitfield values
//@{
// Internal input mode
#define ADCCH_MUXPOS_TEMP      0  //!< Temperature reference as positive input
#define ADCCH_MUXPOS_BANDGAP   1  //!< Bandgap reference as positive input
#define ADCCH_MUXPOS_SCALEDVCC 2  //!< VCC/10 as as positive input
#define ADCCH_MUXPOS_DAC       3  //!< DAC output as positive input
// Single-ended and differential input modes
#define ADCCH_MUXPOS_PIN0      0  //!< ADC0 pin as positive input
#define ADCCH_MUXPOS_PIN1      1  //!< ADC1 pin as positive input
#define ADCCH_MUXPOS_PIN2      2  //!< ADC2 pin as positive input
#define ADCCH_MUXPOS_PIN3      3  //!< ADC3 pin as positive input
#define ADCCH_MUXPOS_PIN4      4  //!< ADC4 pin as positive input
#define ADCCH_MUXPOS_PIN5      5  //!< ADC5 pin as positive input
#define ADCCH_MUXPOS_PIN6      6  //!< ADC6 pin as positive input
#define ADCCH_MUXPOS_PIN7      7  //!< ADC7 pin as positive input
//@}

//! \name Bitfields in channel INTCTRL
//@{
#define ADCCH_INTLVL_START     0  //!< ADC channel interrupt level
#define ADCCH_INTLVL_SIZE      2  //!< ADC channel interrupt level
#define ADCCH_INTMODE_START    2  //!< ADC channel interrupt mode
#define ADCCH_INTMODE_SIZE     2  //!< ADC channel interrupt mode
//@}

//! \name INTLVL bitfield values
//@{
#define ADCCH_INTLVL_OFF       0  //!< Interrupt disabled
#define ADCCH_INTLVL_LO        1  //!< Low level interrupt
#define ADCCH_INTLVL_MED       2  //!< Medium level interrupt
#define ADCCH_INTLVL_HI        3  //!< High level interrupt
//@}

//! \name INTMODE bitfield values
//@{
//! Interrupt on conversion completion
#define ADCCH_INTMODE_COMPLETE  0
//! Interrupt if conversion result is below comparison value
#define ADCCH_INTMODE_BELOW     1
//! Interrupt if conversion result is above comparison value
#define ADCCH_INTMODE_ABOVE     3
//@}

//! \name Bitfields in channel INTFLAG
//@{
#define ADCCH_IF_BIT            0  //!< ADC channel interrupt flag
//@}

//! \name Channel bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set.
#define ADCCH_BIT(name) \
	(1U << ADCCH_##name##_BIT)
//! \brief Create a mask with bitfield \a name set to \a value.
#define ADCCH_BF(name, value) \
	((value) << ADCCH_##name##_START)
//! \brief Create a mask of the bitfield \a name.
#define ADCCH_BFMASK(name) \
	(((1U << ADCCH_##name##_SIZE) - 1) << ADCCH_##name##_START)
//! \brief Extract the value of bitfield \a name from \a regval.
#define ADCCH_BFEXT(name, regval) \
	(((regval) >> ADCCH_##name##_START) \
		& ((1U << ADCCH_##name##_SIZE) - 1))
//! \brief Return \a regval with bitfield \a name set to \a value.
#define ADCCH_BFINS(name, value, regval) \
	(((regval) & ~(((1U << ADCCH_##name##_SIZE) - 1) \
			<< ADCCH_##name##_START)) \
	| ADCCH_BF(name, value))
//@}

//! \name Channel register access macros
//@{
/**
 * \brief Read value of register \a reg of channel \a ch on ADC with
 * base address \a base.
 */
#define adcch_read_reg(base, ch, reg) \
	mmio_read8((void *)((uintptr_t)(base) + ADCCH_##ch + ADCCH_##reg))
/**
 * \brief Write \a value to register \a reg of channel \a ch on ADC with
 * base address \a base.
 */
#define adcch_write_reg(base, ch, reg, value) \
	mmio_write8((void *)((uintptr_t)(base) + ADCCH_##ch + ADCCH_##reg), (value))
//@}

//@}

#endif /* REGS_XMEGA_ADC_H_INCLUDED */

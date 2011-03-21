/**
 * \file
 *
 * \brief AVR32 Timer/Counter Register Definitions.
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
#ifndef REGS_TC_H_INCLUDED
#define REGS_TC_H_INCLUDED

/**
 * \ingroup regs_group
 * \defgroup tc_regs_group Timer/Counter Register Definitions
 *
 * @{
 */

//! \name Register Offsets
//@{
#define TC_CCR		0x0000	//!< Channel Control Register
#define TC_CMR		0x0004	//!< Channel Mode Register
#define TC_CV		0x0010	//!< Counter Value
#define TC_RA		0x0014	//!< Register A
#define TC_RB		0x0018	//!< Register B
#define TC_RC		0x001c	//!< Register C
#define TC_SR		0x0020	//!< Status Register
#define TC_IER		0x0024	//!< Interrupt Enable Register
#define TC_IDR		0x0028	//!< Interrupt Disable Register
#define TC_IMR		0x002c	//!< Interrupt Mask Register
#define TC_BCR		0x00c0	//!< TC Block Control Register
#define TC_BMR		0x00c4	//!< TC Block Mode Register
//@}

//! \name Bitfields in CCR
//@{
#define TC_CCR_CLKEN_BIT	0	//!< Counter Clock Enable Command
#define TC_CCR_CLKDIS_BIT	1	//!< Counter Clock Disable Command
#define TC_CCR_SWTRG_BIT	2	//!< Software Trigger Command
//@}

//! \name Bitfields in CMR
//@{
#define TC_CMR_TCCLKS_START	0 	//!< Clock Selection
#define TC_CMR_TCCLKS_SIZE	3 	//!< Clock Selection
#define TC_CMR_CLKI_BIT		3 	//!< Clock Invert
#define TC_CMR_BURST_START	4 	//!< Burst Signal Selection
#define TC_CMR_BURST_SIZE	2 	//!< Burst Signal Selection
#define TC_CMR_LDBSTOP_BIT	6 //!< Counter Clock Stopped with RB Loading
#define TC_CMR_CPCSTOP_BIT	6 //!< Counter Clock Stopped with RC Compare
#define TC_CMR_LDBDIS_BIT	7 //!< Counter Clock Disable with RB Loading
#define TC_CMR_CPCDIS_BIT	7 //!< Counter Clock Disable with RC Compare
#define TC_CMR_ETRGEDG_START	8 //!< External Trigger Edge Selection
#define TC_CMR_ETRGEDG_SIZE	2  //!< External Trigger Edge Selection
#define TC_CMR_EEVTEDG_START	8 //!< External Event Edge Selection
#define TC_CMR_EEVTEDG_SIZE	2 //!< External Event Edge Selection
#define TC_CMR_EEVT_START	10 //!< External Event Selection
#define TC_CMR_EEVT_SIZE	2 //!< External Event Selection
#define TC_CMR_ABETRG_BIT	10 //!< TIOA or TIOB External Trigger Selection
#define TC_CMR_ENETRG_BIT	12 //!< External Event Trigger Enable
#define TC_CMR_WAVSEL_START	13	//!< Waveform Selection
#define TC_CMR_WAVSEL_SIZE	2	//!< Waveform Selection
#define TC_CMR_CPCTRG_BIT	14 	//!< RC Compare Trigger Enable
#define TC_CMR_WAVE_BIT		15 	//!< Wave
#define TC_CMR_LDRA_START	16 	//!< RA Loading Selection
#define TC_CMR_LDRA_SIZE	2 	//!< RA Loading Selection
#define TC_CMR_ACPA_START	16	//!< RA Compare Effect on TIOA
#define TC_CMR_ACPA_SIZE	2	//!< RA Compare Effect on TIOA
#define TC_CMR_LDRB_START	18	//!< RB Loading Selection
#define TC_CMR_LDRB_SIZE	2	//!< RB Loading Selection
#define TC_CMR_ACPC_START	18	//!< RC Compare Effect on TIOA
#define TC_CMR_ACPC_SIZE	2	//!< RC Compare Effect on TIOA
#define TC_CMR_AEEVT_START	20	//!< External Event Effect on TIOA
#define TC_CMR_AEEVT_SIZE	2	//!< External Event Effect on TIOA
#define TC_CMR_ASWTRG_START	22	//!< Software Trigger Effect on TIOA
#define TC_CMR_ASWTRG_SIZE	2	//!< Software Trigger Effect on TIOA
#define TC_CMR_BCPB_START	24	//!< RB Compare Effect on TIOB
#define TC_CMR_BCPB_SIZE	2	//!< RB Compare Effect on TIOB
#define TC_CMR_BCPC_START	26	//!< RC Compare Effect on TIOB
#define TC_CMR_BCPC_SIZE	2	//!< RC Compare Effect on TIOB
#define TC_CMR_BEEVT_START	28	//!< External Event Effect on TIOB
#define TC_CMR_BEEVT_SIZE	2	//!< External Event Effect on TIOB
#define TC_CMR_BSWTRG_START	30	//!< Software Trigger Effect on TIOB
#define TC_CMR_BSWTRG_SIZE	2	//!< Software Trigger Effect on TIOB
//@}

//! \name Bitfields in SR, IER, IDR and IMR
//@{
#define TC_COVFS_BIT		0	//!< Counter Overflow Status
#define TC_LOVRS_BIT		1	//!< Load Overrun Status
#define TC_CPAS_BIT		2	//!< RA Compare Status
#define TC_CPBS_BIT		3	//!< RB Compare Status
#define TC_CPCS_BIT		4	//!< RC Compare Status
#define TC_LDRAS_BIT		5	//!< RA Loading Status
#define TC_LDRBS_BIT		6	//!< RB Loading Status
#define TC_ETRGS_BIT		7	//!< External Trigger Status
//@}

//! \name Additional Bitfields in SR
//@{
#define TC_SR_CLKSTA_BIT	16	//!< Clock Enabling Status
#define TC_SR_MTIOA_BIT		17	//!< TIOA Mirror
#define TC_SR_MTIOB_BIT		18	//!< TIOB Mirror
//@}

//! \name Bitfields in BCR
//@{
#define TC_BCR_SYNC_BIT		0	//!< Synchro Command
//@}

//! \name Bitfields in BMR
//@{
#define TC_BMR_TC0XC0S_START	0	//!< External Clock Signal 0 Selection
#define TC_BMR_TC0XC0S_SIZE	2	//!< External Clock Signal 0 Selection
#define TC_BMR_TC1XC1S_START	2	//!< External Clock Signal 1 Selection
#define TC_BMR_TC1XC1S_SIZE	2	//!< External Clock Signal 1 Selection
#define TC_BMR_TC2XC2S_START	4	//!< External Clock Signal 2 Selection
#define TC_BMR_TC2XC2S_SIZE	2	//!< External Clock Signal 2 Selection
//@}

//! \name CMR_TCCLKS Bitfield Values
//@{
#define TC_TCCLKS_TIMER_DIV1_CLOCK	0	//!< TIMER_DIV1_CLOCK
#define TC_TCCLKS_TIMER_DIV2_CLOCK	1	//!< TIMER_DIV2_CLOCK
#define TC_TCCLKS_TIMER_DIV3_CLOCK	2	//!< TIMER_DIV3_CLOCK
#define TC_TCCLKS_TIMER_DIV4_CLOCK	3	//!< TIMER_DIV4_CLOCK
#define TC_TCCLKS_TIMER_DIV5_CLOCK	4	//!< TIMER_DIV5_CLOCK
#define TC_TCCLKS_XC0			5	//!< XC0
#define TC_TCCLKS_XC1			6	//!< XC1
#define TC_TCCLKS_XC2			7	//!< XC2
//@}

//! \name CMR_BURST Bitfield Values
//@{
//! The clock is not gated by an external signal.
#define TC_BURST_NOT_GATED	0
#define TC_BURST_CLK_AND_XC0	1 //!< XC0 is ANDed with the selected clock.
#define TC_BURST_CLK_AND_XC1	2 //!< XC1 is ANDed with the selected clock.
#define TC_BURST_CLK_AND_XC2	3 //!< XC2 is ANDed with the selected clock.
//@}

//! \name CMR_ETRGEDG Bitfield Values
//@{
#define TC_ETRGEDG_NO_EDGE		0	//!< none
#define TC_ETRGEDG_POS_EDGE		1	//!< rising edge
#define TC_ETRGEDG_NEG_EDGE		2	//!< falling edge
#define TC_ETRGEDG_BOTH_EDGES		3	//!< each edge
//@}

//! \name CMR_EEVTEDG Bitfield Values
//@{
#define TC_EEVTEDG_NO_EDGE		0	//!< none
#define TC_EEVTEDG_POS_EDGE		1	//!< rising edge
#define TC_EEVTEDG_NEG_EDGE		2	//!< falling edge
#define TC_EEVTEDG_BOTH_EDGES		3	//!< each edge
//@}

//! \name CMR_LDRA Bitfield Values
//@{
#define TC_LDRA_NO_EDGE			0	//!< none
#define TC_LDRA_POS_EDGE_TIOA		1	//!< rising edge of TIOA
#define TC_LDRA_NEG_EDGE_TIOA		2	//!< falling edge of TIOA
#define TC_LDRA_BIOTH_EDGES_TIOA	3	//!< each edge of TIOA
//@}

//! \name CMR_LDRB Bitfield Values
//@{
#define TC_LDRB_NO_EDGE			0	//!< none
#define TC_LDRB_POS_EDGE_TIOA		1	//!< rising edge of TIOA
#define TC_LDRB_NEG_EDGE_TIOA		2	//!< falling edge of TIOA
#define TC_LDRB_BIOTH_EDGES_TIOA	3	//!< each edge of TIOA
//@}

//! \name CMR_EEVT Bitfield Values
//@{
#define TC_EEVT_TIOB		0	//!< External Event is TIOB
#define TC_EEVT_XC0		1	//!< External Event is XC0
#define TC_EEVT_XC1		2	//!< External Event is XC1
#define TC_EEVT_XC2		3	//!< External Event is XC2
//@}

//! \name CMR_WAVSEL Bitfield Values
//@{
#define TC_WAVSEL_UP			0 //!< UP Mode
#define TC_WAVSEL_UP_RC_TRIG		1 //!< UP Mode, Trigger on RC Compare
#define TC_WAVSEL_UPDOWN		2 //!< UPDOWN Mode
#define TC_WAVSEL_UPDOWN_RC_TRIG	3 //!< UPDOWN Mode, Trigger on RC Compare
//@}

//! \name CMR_ACPA,  Bitfield Values
//@{
#define TC_ACPA_NONE		0	//!< None
#define TC_ACPA_SET		1	//!< Set TIOA upon RA Compare
#define TC_ACPA_CLEAR		2	//!< Clear TIOA upon RA Compare
#define TC_ACPA_TOGGLE		3	//!< Toggle TIOA upon RA Compare
//@}

//! \name CMR_ACPC,  Bitfield Values
//@{
#define TC_ACPC_NONE		0	//!< None
#define TC_ACPC_SET		1	//!< Set TIOA upon RC Compare
#define TC_ACPC_CLEAR		2	//!< Clear TIOA upon RC Compare
#define TC_ACPC_TOGGLE		3	//!< Toggle TIOA upon RC Compare
//@}

//! \name CMR_AEEVT,  Bitfield Values
//@{
#define TC_AEEVT_NONE		0	//!< None
#define TC_AEEVT_SET		1	//!< Set TIOA upon External Event
#define TC_AEEVT_CLEAR		2	//!< Clear TIOA upon External Event
#define TC_AEEVT_TOGGLE		3	//!< Toggle TIOA upon External Event
//@}

//! \name CMR_ASWTRG,  Bitfield Values
//@{
#define TC_ASWTRG_NONE		0	//!< None
#define TC_ASWTRG_SET		1	//!< Set TIOA upon Software Trigger
#define TC_ASWTRG_CLEAR		2	//!< Clear TIOA upon Software Trigger
#define TC_ASWTRG_TOGGLE	3	//!< Toggle TIOA upon Software Trigger
//@}

//! \name CMR_BCPA,  Bitfield Values
//@{
#define TC_BCPB_NONE		0	//!< None
#define TC_BCPB_SET		1	//!< Set TIOB upon RB Compare
#define TC_BCPB_CLEAR		2	//!< Clear TIOB upon RB Compare
#define TC_BCPB_TOGGLE		3	//!< Toggle TIOB upon RB Compare
//@}

//! \name CMR_BCPC,  Bitfield Values
//@{
#define TC_BCPC_NONE		0	//!< None
#define TC_BCPC_SET		1	//!< Set TIOB upon RC Compare
#define TC_BCPC_CLEAR		2	//!< Clear TIOB upon RC Compare
#define TC_BCPC_TOGGLE		3	//!< Toggle TIOB upon RC Compare
//@}

//! \name CMR_BEEVT,  Bitfield Values
//@{
#define TC_BEEVT_NONE		0	//!< None
#define TC_BEEVT_SET		1	//!< Set TIOB upon External Event
#define TC_BEEVT_CLEAR		2	//!< Clear TIOB upon External Event
#define TC_BEEVT_TOGGLE		3	//!< Toggle TIOB upon External Event
//@}

//! \name CMR_BSWTRG,  Bitfield Values
//@{
#define TC_BSWTRG_NONE		0	//!< None
#define TC_BSWTRG_SET		1	//!< Set TIOB upon Software Trigger
#define TC_BSWTRG_CLEAR		2	//!< Clear TIOB upon Software Trigger
#define TC_BSWTRG_TOGGLE	3	//!< Toggle TIOB upon Software Trigger
//@}

//! \name BMR_TC0XC0S Bitfield Values
//@{
#define TC_TC0XC0S_TCLK0	0	//!< Select TCLK0 as clock signal 0.
#define TC_TC0XC0S_NO_CLK	1	//!< Select no clock as clock signal 0.
#define TC_TC0XC0S_TIOA1	2	//!< Select TIOA1 as clock signal 0.
#define TC_TC0XC0S_TIOA2	3	//!< Select TIOA2 as clock signal 0.
//@}

//! \name BMR_TC1XC1S Bitfield Values
//@{
#define TC_TC1XC1S_TCLK1	0	//!< Select TCLK1 as clock signal 1.
#define TC_TC1XC1S_NO_CLK	1	//!< Select no clock as clock signal 1.
#define TC_TC1XC1S_TIOA0	2	//!< Select TIOA0 as clock signal 1.
#define TC_TC1XC1S_TIOA2	3	//!< Select TIOA2 as clock signal 1.
//@}

//! \name BMR_TC2XC2S Bitfield Values
//@{
#define TC_TC2XC2S_TCLK2	0	//!< Select TCLK2 as clock signal 2.
#define TC_TC2XC2S_NO_CLK	1	//!< Select no clock as clock signal 2.
#define TC_TC2XC2S_TIOA0	2	//!< Select TIOA0 as clock signal 2.
#define TC_TC2XC2S_TIOA1	3	//!< Select TIOA1 as clock signal 2.
//@}

//! \name Bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set.
#define TC_BIT(name)							\
	(1U << TC_##name##_BIT)
//! \brief Create a mask with bitfield \a name set to \a value.
#define TC_BF(name, value)						\
	((value) << TC_##name##_START)
//! \brief Create a mask of the bitfield \a name.
#define TC_BFMASK(name)							\
	(((1U << %s_##name##_SIZE) - 1) << %s_##name##_START)
//! \brief Extract the value of bitfield \a name from \a regval.
#define TC_BFEXT(name, regval)						\
	(((regval) >> TC_##name##_START)				\
		& ((1U << TC_##name##_SIZE) - 1))
//! \brief Return \a regval with bitfield \a name set to \a value.
#define TC_BFINS(name, value, regval)					\
	(((regval) & ~(((1U << TC_##name##_SIZE) - 1)			\
			<< TC_##name##_START))				\
		| TC_BF(name, value))
//@}

//! \name Register access macros
//@{
//! \brief Read the value of TC register \a reg.
#define tc_read_reg(base, reg)						\
	mmio_read32((void *)((uintptr_t)(base) + TC_##reg))
//! \brief Write \a value to TC register \a reg.
#define tc_write_reg(base, reg, value)					\
	mmio_write32((void *)((uintptr_t)(base) + TC_##reg), (value))
/**
 * \brief Read from a TC channel register.
 *
 * \param base Base address of the TC peripheral.
 * \param channel Channel numbe. Valid values are 0, 1 and 2.
 * \param reg Register name.
 * \return Register value.
 */
#define tc_read_ch_reg(base, channel, reg)		\
	mmio_read32((void *) ((uintptr_t)(base) + TC_##reg + (channel) * 0x40))
/**
 * \brief Write a value into a channel register.
 *
 * \param base Base address of the TC peripheral
 * \param channel Channel number. Valid values are 0, 1 and 2.
 * \param reg Register name.
 * \param value Value to write into the register.
 */
#define tc_write_ch_reg(base, channel, reg, value)	\
	mmio_write32((void *)((uintptr_t)(base) + TC_##reg + (channel) * 0x40), value)
//@}

//! @}

#endif /* REGS_TC_H_INCLUDED */

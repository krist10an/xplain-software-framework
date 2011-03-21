/**
 * \file
 *
 * \brief AVR32 USART Register Definitions.
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
#ifndef REGS_AVR32_USART_H_INCLUDED
#define REGS_AVR32_USART_H_INCLUDED

/**
 * \ingroup regs_group
 * \defgroup avr32_usart_regs_group AVR32 USART Register Definitions
 *
 * This is the register interface to the on-chip USART present on all
 * current AVR32 devices. Note that not all chips may support all the
 * features defined in this file, and there may sometimes be differences
 * between various USARTs on the same chip as well.
 *
 * @{
 */

//! \name Register Offsets
//@{
#define USART_CR			0x0000	//!< Control Register
#define USART_MR			0x0004	//!< Mode Register
#define USART_IER			0x0008	//!< Interrupt Enable Register
#define USART_IDR			0x000c	//!< Interrupt Disable Register
#define USART_IMR			0x0010	//!< Interrupt Mask Register
#define USART_CSR			0x0014	//!< Channel Status Register
#define USART_RHR			0x0018	//!< Receiver Holding Register
#define USART_THR			0x001c	//!< Transmitter Holding Reg
#define USART_BRGR			0x0020	//!< Baud Rate Generator Reg
#define USART_RTOR			0x0024	//!< Receiver Time-out Register
#define USART_TTGR			0x0028	//!< Transmitter Timeguard
#define USART_FIDI			0x0040	//!< FI DI Ratio Register
#define USART_NER			0x0044	//!< Number of Errors Register
#define USART_XXR			0x0048	//!< XON_XOFF Register
#define USART_IFR			0x004c	//!< IrDA Filter Register
#define USART_MAN			0x0050	//!< Manchester Config Register
#define USART_LINMR			0x0054	//!< LIN Mode Register
#define USART_LINIR			0x0058	//!< LIN Identifier Register
#define USART_WPMR			0x00e4	//!< Write Protect Mode Register
#define USART_WPSR			0x00e8	//!< Write Prot Status Register
#define USART_VERSION			0x00fc	//!< Version Register
//@}

//! \name Bitfields in CR
//@{
#define USART_CR_RSTRX_BIT		2	//!< Reset Receiver
#define USART_CR_RSTTX_BIT		3	//!< Reset Transmitter
#define USART_CR_RXEN_BIT		4	//!< Receiver Enable
#define USART_CR_RXDIS_BIT		5	//!< Receiver Disable
#define USART_CR_TXEN_BIT		6	//!< Transmitter Enable
#define USART_CR_TXDIS_BIT		7	//!< Transmitter Disable
#define USART_CR_RSTSTA_BIT		8	//!< Reset Status Bits
#define USART_CR_STTBRK_BIT		9	//!< Start Break
#define USART_CR_STPBRK_BIT		10	//!< Stop Break
#define USART_CR_STTTO_BIT		11	//!< Start Time-out
#define USART_CR_SENDA_BIT		12	//!< Send Address
#define USART_CR_RSTIT_BIT		13	//!< Reset Iterations
#define USART_CR_RSTNACK_BIT		14	//!< Reset Non Acknowledge
#define USART_CR_RETTO_BIT		15	//!< Rearm Time-out
#define USART_CR_DTREN_BIT		16	//!< Data Terminal Ready Enable
#define USART_CR_DTRDIS_BIT		17	//!< Data Terminal Ready Disable
#define USART_CR_RTSEN_BIT		18	//!< Request to Send Enable
#define USART_CR_RTSDIS_BIT		19	//!< Request to Send Disable
#define USART_CR_LINABT_BIT		20	//!< Abort LIN transmission
#define USART_CR_LINWKUP_BIT		21	//!< Send LIN wakeup signal
#define USART_CR_COMM_TX_BIT		30	//!< Disable COMM_TX
#define USART_CR_COMM_RX_BIT		31	//!< Disable COMM_RX
//@}

//! \name Bitfields in MR
//@{
#define USART_MR_MODE_START		0	//!< Usart Mode
#define USART_MR_MODE_SIZE		4	//!< Usart Mode
#define USART_MR_USCLKS_START		4	//!< Clock Selection
#define USART_MR_USCLKS_SIZE		2	//!< Clock Selection
#define USART_MR_CHRL_START		6	//!< Character Length.
#define USART_MR_CHRL_SIZE		2	//!< Character Length.
#define USART_MR_CPHA_BIT		8	//!< SPI CLock Phase
#define USART_MR_PAR_START		9	//!< Parity Type
#define USART_MR_PAR_SIZE		3	//!< Parity Type
#define USART_MR_NBSTOP_START		12	//!< Number of Stop Bits
#define USART_MR_NBSTOP_SIZE		2	//!< Number of Stop Bits
#define USART_MR_CHMODE_START		14	//!< Channel Mode
#define USART_MR_CHMODE_SIZE		2	//!< Channel Mode
#define USART_MR_CPOL_BIT		16	//!< SPI Clock Polarity
#define USART_MR_MODE9_BIT		17	//!< 9-bit Character Length
#define USART_MR_CLKO_BIT		18	//!< Clock Output Select
#define USART_MR_OVER_BIT		19	//!< Oversampling Mode
#define USART_MR_INACK_BIT		20	//!< Inhibit Non Acknowledge
#define USART_MR_DSNACK_BIT		21	//!< Disable Successive NACK
#define USART_MR_MAX_ITERATION_START	24	//!< Max interation
#define USART_MR_MAX_ITERATION_SIZE	3	//!< Max interation
#define USART_MR_FILTER_BIT		28	//!< IR Receive Line Filter
//@}

//! \name Bitfields in IER, IDR, IMR and CSR
//@{
#define USART_RXRDY_BIT			0	//!< Receiver Ready
#define USART_TXRDY_BIT			1	//!< Transmitter Ready
#define USART_RXBRK_BIT			2	//!< Receiver Break
#define USART_ENDRX_BIT			3	//!< End of Receive Transfer
#define USART_ENDTX_BIT			4	//!< End of Transmit
#define USART_OVRE_BIT			5	//!< Overrun Error
#define USART_FRAME_BIT			6	//!< Framing Error
#define USART_PARE_BIT			7	//!< Parity Error
#define USART_TIMEOUT_BIT		8	//!< Time-out
#define USART_TXEMPTY_BIT		9	//!< Transmitter Empty
#define USART_ITER_BIT			10	//!< Iteration
#define USART_TXBUFE_BIT		11	//!< Buffer Empty
#define USART_RXBUFF_BIT		12	//!< Buffer Full
#define USART_NACK_BIT			13	//!< Non Acknowledge
#define USART_LINIR_BIT			14	//!< LIN Identifier Received
#define USART_LINTC_BIT			15	//!< LIN Transfer Completed
#define USART_RIIC_BIT			16	//!< Ring Indicator Input Change
#define USART_DSRIC_BIT			17	//!< Data Set Ready Input Change
//! Data Carrier Detect Input Change
#define USART_DCDIC_BIT			18
#define USART_CTSIC_BIT			19	//!< Clear to Send Input Change
#define USART_LINBE_BIT			25	//!< LIN Bus Error
//! LIN Inconsistent Synch Field Error
#define USART_LINISFE_BIT		26
#define USART_LINIPE_BIT		27	//!< LIN Identifier Parity
#define USART_LINCE_BIT			28	//!< LIN Checksum Error
#define USART_LINSNRE_BIT		29	//!< LIN Slave Not Responding
#define USART_COMM_TX_BIT		30	//!< Enable COMM_TX
#define USART_COMM_RX_BIT		31	//!< Enable COMM_RX
//@}

//! \name Bitfields in CSR
//@{
#define USART_CSR_RI_BIT		20	//!< Image of RI Input
#define USART_CSR_DSR_BIT		21	//!< Image of DSR Input
#define USART_CSR_DCD_BIT		22	//!< Image of DCD Input
#define USART_CSR_CTS_BIT		23	//!< Image of CTS Input
//@}

//! \name Bitfields in RHR
//@{
#define USART_RHR_RXCHR_START		0	//!< Received Character
#define USART_RHR_RXCHR_SIZE		9	//!< Received Character
#define USART_RHR_RXSYNH_BIT		15	//!< Received Sync
//@}

//! \name Bitfields in THR
//@{
#define USART_THR_TXCHR_START		0	//!< Character to be Transmitted
#define USART_THR_TXCHR_SIZE		9	//!< Character to be Transmitted
#define USART_THR_TXSYNH_BIT		15	//!< Sync to be Transmitted
//@}

//! \name Bitfields in BRGR
//@{
#define USART_BRGR_CD_START		0	//!< Clock Divisor
#define USART_BRGR_CD_SIZE		16	//!< Clock Divisor
#define USART_BRGR_FP_START		16	//!< Fractional Part
#define USART_BRGR_FP_SIZE		3	//!< Fractional Part
//@}

//! \name Bitfields in RTOR
//@{
#define USART_RTOR_TO_START		0	//!< Time-out Value
#define USART_RTOR_TO_SIZE		17	//!< Time-out Value
//@}

//! \name Bitfields in TTGR
//@{
#define USART_TTGR_TG_START		0	//!< Timeguard Value
#define USART_TTGR_TG_SIZE		8	//!< Timeguard Value
//@}

//! \name Bitfields in FIDI
//@{
#define USART_FIDI_FI_DI_RATIO_START	0	//!< FI Over DI Ratio Value
#define USART_FIDI_FI_DI_RATIO_SIZE	11	//!< FI Over DI Ratio Value
//@}

//! \name Bitfields in NER
//@{
#define USART_NER_NB_ERRORS_START	0	//!< Number of ISO7816 errors
#define USART_NER_NB_ERRORS_SIZE	8	//!< Number of ISO7816 errors
//@}

//! \name Bitfields in XXR
//@{
#define USART_XXR_XOFF_START		0	//!< XOFF Character
#define USART_XXR_XOFF_SIZE		8	//!< XOFF Character
#define USART_XXR_XON_START		8	//!< XON Character
#define USART_XXR_XON_SIZE		8	//!< XON Character
//@}

//! \name Bitfields in IFR
//@{
#define USART_IFR_IRDA_FILTER_START	0	//!< Irda filter
#define USART_IFR_IRDA_FILTER_SIZE	8	//!< Irda filter
//@}

//! \name Bitfields in MAN
//@{
#define USART_MAN_TX_PL_START		0	//!< TX Preamble Length
#define USART_MAN_TX_PL_SIZE		4	//!< TX Preamble Length
#define USART_MAN_TX_PP_START		8	//!< TX Preamble Pattern
#define USART_MAN_TX_PP_SIZE		2	//!< TX Preamble Pattern
#define USART_MAN_TX_MPOL_BIT		12	//!< TX Manchester Polarity
#define USART_MAN_RX_PL_START		16	//!< RX Preamble Length
#define USART_MAN_RX_PL_SIZE		4	//!< RX Preamble Length
#define USART_MAN_RX_PP_START		24	//!< RX Preamble Pattern
#define USART_MAN_RX_PP_SIZE		2	//!< RX Preamble Pattern
#define USART_MAN_RX_MPOL_BIT		28	//!< RX Manchester Polarity
#define USART_MAN_DRIFT_BIT		30	//!< Drift compensation
//@}

//! \name Bitfields in LINMR
//@{
#define USART_LINMR_NACT_START		0	//!< LIN Node Action
#define USART_LINMR_NACT_SIZE		2	//!< LIN Node Action
#define USART_LINMR_PARDIS_BIT		2	//!< Parity Disable
#define USART_LINMR_CHKDIS_BIT		3	//!< Checksum Disable
#define USART_LINMR_CHKTYP_BIT		4	//!< Checksum Type
#define USART_LINMR_DLM_BIT		5	//!< Data Length Mode
#define USART_LINMR_FSDIS_BIT		6	//!< Frame Slot Mode Disable
#define USART_LINMR_WKUPTYP_BIT		7	//!< Wakeup Signal Type
#define USART_LINMR_DLC_START		8	//!< Data Length Control
#define USART_LINMR_DLC_SIZE		8	//!< Data Length Control
#define USART_LINMR_PDCM_BIT		16	//!< PDC Mode
//@}

//! \name Bitfields in LINIR
//@{
#define USART_LINIR_IDCHR_START		0	//!< Identifier Character
#define USART_LINIR_IDCHR_SIZE		8	//!< Identifier Character
//@}

//! \name Bitfields in WPMR
//@{
#define USART_WPMR_WPEN_BIT		0	//!< Write Protect Enable
#define USART_WPMR_WPKEY_START		8	//!< Write Protect Key
#define USART_WPMR_WPKEY_SIZE		24	//!< Write Protect Key
//@}

//! \name Bitfields in WPSR
//@{
#define USART_WPSR_WPV_BIT		0	//!< WP Violation Status
#define USART_WPSR_WPVSRC_START		8	//!< WP Violation Source
#define USART_WPSR_WPVSRC_SIZE		16	//!< WP Violation Source
//@}

//! \name Bitfields in VERSION
//@{
#define USART_VERSION_VERSION_START	0
#define USART_VERSION_VERSION_SIZE	12
#define USART_VERSION_MFN_START		16
#define USART_VERSION_MFN_SIZE		3
//@}

//! \name MR_MODE Bitfield Values
//@{
#define USART_MODE_NORMAL		0	//!< Normal
#define USART_MODE_RS485		1	//!< RS485
#define USART_MODE_LIN_MASTER		10	//!< LIN Master
#define USART_MODE_LIN_SLAVE		11	//!< LIN Slave
#define USART_MODE_SPI_MASTER		14	//!< SPI Master
#define USART_MODE_SPI_SLAVE		15	//!< SPI Slave
#define USART_MODE_HARDWARE		2	//!< Hardware Handshaking
#define USART_MODE_MODEM		3	//!< Modem
#define USART_MODE_ISO7816_T0		4	//!< IS07816 Protocol: T = 0
#define USART_MODE_ISO7816_T1		6	//!< IS07816 Protocol: T = 1
#define USART_MODE_IRDA			8	//!< IrDA
//@}

//! \name MR_USCLKS Bitfield Values
//@{
#define USART_USCLKS_MCK		0	//!< MCK
#define USART_USCLKS_MCK_DIV		1	//!< MCK / DIV
#define USART_USCLKS_SCK		3	//!< SCK
//@}

//! \name MR_CHRL Bitfield Values
//@{
#define USART_CHRL_5			0	//!< 5 bits
#define USART_CHRL_6			1	//!< 6 bits
#define USART_CHRL_7			2	//!< 7 bits
#define USART_CHRL_8			3	//!< 8 bits
//@}

//! \name MR_PAR Bitfield Values
//@{
#define USART_PAR_EVEN			0	//!< Even parity
#define USART_PAR_ODD			1	//!< Odd parity
#define USART_PAR_SPACE			2	//!< Parity forced to 0 (Space)
#define USART_PAR_MARK			3	//!< Parity forced to 1 (Mark)
#define USART_PAR_NONE			4	//!< No Parity
#define USART_PAR_MULTI			6	//!< Multi-drop mode
//@}

//! \name MR_NBSTOP Bitfield Values
//@{
#define USART_NBSTOP_1			0	//!< 1 stop bit
#define USART_NBSTOP_1_5		1	//!< 1.5 stop bits
#define USART_NBSTOP_2			2	//!< 2 stop bits
//@}

//! \name MR_CHMODE Bitfield Values
//@{
#define USART_CHMODE_NORMAL		0	//!< Normal Mode
#define USART_CHMODE_ECHO		1	//!< Automatic Echo
#define USART_CHMODE_LOCAL_LOOP		2	//!< Local Loopback
#define USART_CHMODE_REMOTE_LOOP	3	//!< Remote Loopback
//@}

//! \name LINMR_NACT Bitfield Values
//@{
//! The LIN Controller transmits the response
#define USART_NACT_PUBLISH		0
//! The LIN Controller receives the response
#define USART_NACT_SUBSCRIBE		1
//! The LIN Controller doesn't transmit and doesn't receive the response
#define USART_NACT_IGNORE		2
//@}

//! \name Bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set.
#define USART_BIT(name)							\
	(1U << USART_##name##_BIT)
//! \brief Create a mask with bitfield \a name set to \a value.
#define USART_BF(name, value)						\
	((value) << USART_##name##_START)
//! \brief Create a mask of the bitfield \a name.
#define USART_BFMASK(name)						\
	(((1U << %s_##name##_SIZE) - 1) << %s_##name##_START)
//! \brief Extract the value of bitfield \a name from \a regval.
#define USART_BFEXT(name, regval)					\
	(((regval) >> USART_##name##_START)				\
		& ((1U << USART_##name##_SIZE) - 1))
//! \brief Return \a regval with bitfield \a name set to \a value.
#define USART_BFINS(name, value, regval)				\
	(((regval) & ~(((1U << USART_##name##_SIZE) - 1)		\
			<< USART_##name##_START))			\
		| USART_BF(name, value))
//@}

//! \name Register access macros
//@{
//! \brief Read the value of USART register \a reg.
#define usart_read_reg(base, reg)					\
	mmio_read32((void *)((uintptr_t)(base) + USART_##reg))
//! \brief Write \a value to USART register \a reg.
#define usart_write_reg(base, reg, value)				\
	mmio_write32((void *)((uintptr_t)(base) + USART_##reg), (value))
//@}

//! @}

#endif /* REGS_AVR32_USART_H_INCLUDED */

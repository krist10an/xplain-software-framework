/**
 * \file
 *
 * \brief XMEGA Power Reduction register definitions
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
#ifndef REGS_XMEGA_PR_H_INCLUDED
#define REGS_XMEGA_PR_H_INCLUDED

#include <chip/memory-map.h>
#include <io.h>

/**
 * \ingroup regs_group
 * \defgroup xmega_pr_regs_group XMEGA Power Reduction Register Definitions
 *
 * These are the XMEGA Power Reduction registers taken from the XMEGA A
 * manual.
 *
 * @{
 */

//! \name Register Offsets
//@{
#define XMEGA_PR_PRGEN          0x00    //!< General Power Reduction Register
#define XMEGA_PR_PRPA           0x01    //!< Power Reduction Port A
#define XMEGA_PR_PRPB           0x02    //!< Power Reduction Port B
#define XMEGA_PR_PRPC           0x03    //!< Power Reduction Port C
#define XMEGA_PR_PRPD           0x04    //!< Power Reduction Port D
#define XMEGA_PR_PRPE           0x05    //!< Power Reduction Port E
#define XMEGA_PR_PRPF           0x06    //!< Power Reduction Port F
//@}

//! \name Bitfields in PRGEN
//@{
#define PR_DMA_BIT              0       //!< DMA Controller
#define PR_EVSYS_BIT            1       //!< Event System
#define PR_RTC_BIT              2       //!< Real-Time Counter
#define PR_EBI_BIT              3       //!< External Bus Interface
#define PR_AES_BIT              4       //!< AES Module
#define PR_USB_BIT              6       //!< USB Module
//@}

//! \name Bitfields in PRPA and PRPB
//@{
#define PR_AC_BIT               0       //!< Analog Comparator
#define PR_ADC_BIT              1       //!< Analog/Digital Converter
#define PR_DAC_BIT              2       //!< Digital/Analog Converter
//@}

//! \name Bitfields in PRPC, PRPD, PRPE and PRPF
//@{
#define PR_TC0_BIT              0       //!< Timer/Counter 0
#define PR_TC1_BIT              1       //!< Timer/Counter 1
#define PR_HIRES_BIT            2       //!< Hi-Resolution Extension
#define PR_SPI_BIT              3       //!< Serial Peripheral Interface
#define PR_USART0_BIT           4       //!< USART 0
#define PR_USART1_BIT           5       //!< USART 1
#define PR_TWI_BIT              6       //!< Two-Wire Interface
//@}

//! \name Bit manipulation macros
//@{
//! \brief Create a mask with bit \a name set.
#define PR_BIT(name)            (1U << PR_##name##_BIT)
//@}

//! \name Register access macros
//@{
//! \brief Read the value of PR register \a reg.
#define pr_read_reg(reg)                                        \
	_pr_read_reg(reg)
#define _pr_read_reg(reg)                                       \
	mmio_read8((void *)(PR_BASE + XMEGA_PR_##reg))
//! \brief Write \a value to PR register \a reg.
#define pr_write_reg(reg, value)                                \
	_pr_write_reg(reg, value)
#define _pr_write_reg(reg, value)                               \
	mmio_write8((void *)(PR_BASE + XMEGA_PR_##reg), (value))
//@}

//! @}

#endif /* REGS_XMEGA_PR_H_INCLUDED */

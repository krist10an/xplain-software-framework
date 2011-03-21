/**
 * \file
 *
 * \brief PMIC (Programmable Multi-Level Interrupt Controller) driver.
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

#ifndef PMIC_H_INCLUDED
#define PMIC_H_INCLUDED

#include <pmic_regs.h>
#include <chip/pmic.h>

/**
 * \defgroup pmic_group Programmable Multilevel Interrupt Controller
 * @{
 *
 * The Programmable Multilevel Interrupt Controller (PMIC) is available on
 * the XMEGA devices. All peripherals can define three different priority
 * levels for interrupts; high, medium or low. Medium level interrupts
 * may interrupt low level interrupt service routines. High level interrupts
 * may interrupt both low and medium level interrupt service routines. Low
 * level interrupts have an optional round robin scheme to make sure all
 * interrupts are serviced within a certain amount of time.
 *
 * An interrupt level (low, medium or high) must be enabled in the PIMC, in
 * addition to the global interrupt enable and the interrupt enable in the
 * peripheral itself, before an interrupt of that level is able to trigger the
 * according interrupt service routine.
 *
 * To enable the interrupt in the peripheral itself, the desired interrupt
 * level must be configured in the peripheral. According defines for the
 * different levels are also available in this driver (e.g. for low level
 * interrupt use PMIC_INTLVL_LOW).
 *
 */

/* Interrupt level definitions */
#define PMIC_INTLVL_OFF		0x0
#define PMIC_INTLVL_LOW		0x1
#define PMIC_INTLVL_MEDIUM	0x2
#define PMIC_INTLVL_HIGH	0x3

/** \brief Enable low level interrupts. */
#define pmic_enable_low_lvl_int() \
	pmic_write_reg8(CTRL, pmic_read_reg8(CTRL) | PMIC_CTRL_LOLVLEN)

/** \brief Enable medium level interrupts. */
#define pmic_enable_medium_lvl_int() \
	pmic_write_reg8(CTRL, pmic_read_reg8(CTRL) | PMIC_CTRL_MEDLVLEN)

/** \brief Enable high level interrupts. */
#define pmic_enable_high_lvl_int() \
	pmic_write_reg8(CTRL, pmic_read_reg8(CTRL) | PMIC_CTRL_HILVLEN)

/** \brief Disable low level interrupts. */
#define pmic_disable_low_lvl_int() \
	pmic_write_reg8(CTRL, pmic_read_reg8(CTRL) & ~PMIC_CTRL_LOLVLEN)

/** \brief Disable medium level interrupts. */
#define pmic_disable_medium_lvl_int() \
	pmic_write_reg8(CTRL, pmic_read_reg8(CTRL) & ~PMIC_CTRL_MEDLVLEN)

/** \brief Disable high level interrupts. */
#define pmic_disable_high_lvl_int() \
	pmic_write_reg8(CTRL, pmic_read_reg8(CTRL) & ~PMIC_CTRL_HILVLEN)

//! @}
#endif /* PMIC_H_INCLUDED */

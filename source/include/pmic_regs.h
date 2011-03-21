/**
 * \file
 *
 * \brief PMIC (Programmable Multi-Level Interrupt Controller) register
 * definitions.
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

#ifndef PMIC_REGS_H_INCLUDED
#define PMIC_REGS_H_INCLUDED

#include <chip/memory-map.h>
#include <io.h>

#define PMIC_STATUS	0x00
#define PMIC_INTPRI	0x01
#define PMIC_CTRL	0x02

#define PMIC_STATUS_LOLVLEX	(1 << 0)
#define PMIC_STATUS_MEDLVLEX	(1 << 1)
#define PMIC_STATUS_HILVLEX	(1 << 2)
#define PMIC_STATUS_NMIEX	(1 << 7)

#define PMIC_CTRL_LOLVLEN	(1 << 0)
#define PMIC_CTRL_MEDLVLEN	(1 << 1)
#define PMIC_CTRL_HILVLEN	(1 << 2)
#define PMIC_CTRL_IVSEL		(1 << 6)
#define PMIC_CTRL_RREN		(1 << 7)

#define pmic_write_reg8(reg, value) \
	mmio_write8((void *) (PMIC_BASE + PMIC_##reg), value)

#define pmic_read_reg8(reg) \
	mmio_read8((void *) (PMIC_BASE + PMIC_##reg))

#endif /* PMIC_REGS_H_INCLUDED */

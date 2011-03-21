/**
 * \file
 *
 * \brief Oscillator startup code for AVR32 devices with PM v2+
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
#include <assembler.h>
#include <arch/sysreg.h>
#include <chip/memory-map.h>
#include <clk/osc.h>
#include <clk/sys.h>

/* These registers have a relatively long lifetime */
#define zero	r0
#define pm	r1
#define mcctrl	r2

#define OSCCTRL0_VALUE	(PM_BF(OSCCTRL_STARTUP, OSC0_STARTUP_VALUE)	\
				| PM_BF(OSCCTRL_MODE, OSC0_MODE_VALUE))

	PUBLIC_FUNCTION(osc_setup)
	EXTERN_SYMBOL(avr32_runtime_init)

	mov	pm, lo(PM_BASE)
	orh	pm, hi(PM_BASE)

	/* Initialize and enable OSC0 */
	ld.w	mcctrl, pm[PM_MCCTRL]
	mov	r9, OSCCTRL0_VALUE
	st.w	pm[PM_OSC0CTRL], r9
	sbr	mcctrl, PM_MCCTRL_OSC0EN_BIT
	st.w	pm[PM_MCCTRL], mcctrl

	/* OSC0 startup timeout begins here */
	mov	zero, 0
	mtsr	SYSREG_COMPARE, zero
	mtsr	SYSREG_COUNT, zero

	/* Wait for OSC0 to become ready */
L(osc0_loop):
	ld.w	r8, pm[PM_POSCSR]
	bld	r8, PM_OSC0RDY_BIT
	brcs	L(osc0_ready)
	mfsr	r9, SYSREG_COUNT
	cp.w	r9, OSC0_STARTUP_TIMEOUT
	brlt	L(osc0_loop)

	/* Timed out -- try to run from slow clock instead */
	rjmp	L(mcctrl_ready)

L(osc0_ready):
	/* Switch to OSC0 as the main clock source */
	mov	r8, SYSCLK_SRC_OSC0
	bfins	mcctrl, r8, PM_MCCTRL_MCSEL_START, PM_MCCTRL_MCSEL_SIZE
	st.w	pm[PM_MCCTRL], mcctrl

L(mcctrl_ready):
	/* Continue with the rest of the startup code */
	rjmp	avr32_runtime_init

	END_FUNC(osc_setup)
	END_FILE()

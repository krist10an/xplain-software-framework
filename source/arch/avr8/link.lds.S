/**
 * \file
 *
 * \brief Linker script for AVR using the GNU toolchain
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
OUTPUT_FORMAT("elf32-avr", "elf32-avr", "elf32-avr")
OUTPUT_ARCH(avr:CONFIG_AVR_ARCH)
ENTRY(_start)

#include <chip/memory-map.h>

SECTIONS
{
	. = 0;
	.text : {
		_text = .;
		*(.vectors)
		*(.avr.text.avr_runtime_init)
		*(.text.intc_priv_unhandled_irq)
		*(.text)
		*(.text.*)
		*(.progmem.gcc_sw_table)
		*(.progmem.data)
		. = ALIGN(2);
		_etext = .;
	}

	. = 0x800000 | CONFIG_SRAM_BASE;
	.data : AT(LOADADDR(.text) + SIZEOF(.text)) {
		_data = .;
		*(.rodata)
		*(.rodata.*)
		*(.data)
		*(.data.*)
		. = ALIGN(2);
		_edata = .;
	}

	.bss (NOLOAD) : AT(LOADADDR(.data) + SIZEOF(.data)) {
		*(.bss)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(2);
		_end = .;
	}

	.heap (NOLOAD) : {
		heap_area = .;
	}

	. = 0x800000 + CPU_SRAM_BASE + CPU_SRAM_SIZE - CONFIG_CSTACK_SIZE \
		- CONFIG_RSTACK_SIZE;
	.stack (NOLOAD) : {
		stack_area = .;
		. = . + CONFIG_CSTACK_SIZE + CONFIG_RSTACK_SIZE;
		. = ALIGN(2);
		_estack = .;
	}
}

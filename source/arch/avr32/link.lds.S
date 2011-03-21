/**
 * \file
 *
 * \brief Linker script for AVR32 using the GNU toolchain
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
#include <chip/memory-map.h>
#include <cpu/addrspace.h>
#include <linker.h>

OUTPUT_FORMAT("elf32-avr32", "elf32-avr32", "elf32-avr32")
OUTPUT_ARCH(avr32)
ENTRY(_start)

SECTIONS
{
	. = TEXT_VMA;
	.text : AT(TEXT_LMA) {
		_text = .;
		*(.avr.text.exception)
		*(.avr.text.exception.*)
		*(.asf.text.intc_priv*);
		*(.asf.text.softirq);
		*(.text)
		*(.text.*)
		. = ALIGN(32);
	}

	.rodata : AT(LOADADDR(.text) + SIZEOF(.text)) {
		*(.rodata)
		*(.rodata.*)
		*(.rela.dyn)
		*(.rela.got)
		. = ALIGN(32);
		_etext = .;
	}

	. = CACHEABLE_VMA(DATA_VMA);
	.data : AT(LOADADDR(.text) + _etext - _text) {
		_data = .;
		*(.got)
		*(.data)
		*(.data.*)
		. = ALIGN(32);
		_edata = .;
	}


	.bss (NOLOAD) : AT(LOADADDR(.data) + SIZEOF(.data)) {
		*(.asf.bss.intc_priv*)
		*(.bss)
		*(.bss.*)
		*(COMMON)
		KEEP(*(.nanotrace));
		. = ALIGN(32);
		_ebss = .;
	}

	.heap (NOLOAD) : {
		heap_area = .;
	}

	. = CPU_SRAM_END + 1 - CONFIG_STACK_SIZE;
	.stack (NOLOAD) : {
		stack_area = .;
		. = . + CONFIG_STACK_SIZE;
		. = ALIGN(8);
		_estack = .;
	}
}

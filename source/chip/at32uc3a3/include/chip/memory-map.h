/**
 * \file
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
#ifndef CHIP_MEMORY_MAP_H_INCLUDED
#define CHIP_MEMORY_MAP_H_INCLUDED

/* Internal memories */
#define CPU_SRAM_BASE			0x00000000
#define CPU_SRAM_END			0x0000ffff
#define FLASH_BASE			0x80000000
#define FLASH_END			0x8003ffff
#define FLASH_USER_PAGE_BASE		0x80800000
#define FLASH_USER_PAGE_END		0x808001ff
#define HSB_SRAM_BASE			0xff000000
#define HSB_SRAM_END			0xff00ffff

#define CPU_SRAM_SIZE			(CPU_SRAM_END - CPU_SRAM_BASE + 1)
#define FLASH_SIZE			(FLASH_END - FLASH_BASE + 1)
#define FLASH_USER_PAGE_SIZE					\
	(FLASH_USER_PAGE_END - FLASH_USER_PAGE_BASE + 1)
#define HSB_SRAM_SIZE			(HSB_SRAM_END - HSB_SRAM_BASE + 1)

#define CHIP_ENTRY_VMA			FLASH_BASE
#define CHIP_ENTRY_LMA			FLASH_BASE

/* Used by the linker script to place the writeable data sections */
#define DATA_SRAM_BASE			CPU_SRAM_BASE
#define DATA_SRAM_END			CPU_SRAM_END

/* External Bus Interface (EBI) */
#define EBI_SRAM_CS0_BASE               0xc0000000
#define EBI_SRAM_CS2_BASE               0xc8000000
#define EBI_SRAM_CS3_BASE               0xcc000000
#define EBI_SRAM_CS1_BASE               0xd0000000
#define EBI_SRAM_CS4_BASE               0xd8000000
#define EBI_SRAM_CS5_BASE               0xdc000000
#define EBI_SDRAM_BASE                  EBI_SRAM_CS1_BASE

/* Devices on the High Speed Bus (HSB) */
#define USBB_FIFO_BASE			0xe0000000
#define DMACA_BASE			0xff100000
#define AES_BASE			0xfffd0000

/* Devices on Peripheral Bus B (PBB) */
#define USBB_REGS_BASE			0xfffe0000
#define HMATRIX_BASE			0xfffe1000
#define FLASHC_BASE			0xfffe1400
#define SMC_BASE			0xfffe1c00
#define SDRAMC_BASE			0xfffe2000
#define ECCHRS_BASE			0xfffe2400
#define BUSMON_BASE			0xfffe2800
#define MCI_BASE			0xfffe4000
#define MSI_BASE			0xfffe8000

/* Devices on Peripheral Bus A (PBA) */
#define PDCA_BASE			0xffff0000
#define INTC_BASE			0xffff0800
#define PM_BASE				0xffff0c00
#define RTC_BASE			0xffff0d00
#define WDT_BASE			0xffff0d30
#define FREQM_BASE			0xffff0d50
#define EIC_BASE			0xffff0d80
#define GPIO_BASE			0xffff1000
#define USART0_BASE			0xffff1400
#define USART1_BASE			0xffff1800
#define USART2_BASE			0xffff1c00
#define USART3_BASE			0xffff2000
#define SPI0_BASE			0xffff2400
#define SPI1_BASE			0xffff2800
#define TWIM0_BASE			0xffff2c00
#define TWIM1_BASE			0xffff3000
#define SSC_BASE			0xffff3400
#define TC0_BASE			0xffff3800
#define ADC_BASE			0xffff3c00
#define DAC_BASE			0xffff4000
#define TC1_BASE			0xffff4400
#define TWIS0_BASE			0xffff5000
#define TWIS1_BASE			0xffff5400

#endif /* CHIP_MEMORY_MAP_H_INCLUDED */

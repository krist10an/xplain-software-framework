/**
 * \file
 *
 * \brief NVM register definitions.
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
#ifndef CHIP_MEMORY_H_INCLUDED

#ifndef __ASSEMBLY__

#define CHIP_MEMORY_H_INCLUDED

#include <memory/flash/flash_xmega.h>
#include <memory/eeprom/eeprom_xmega.h>
#include <memory/fuses/fuses_xmega.h>
#include <memory/sram/sram_xmega.h>
#include <memory/lockbits/lockbits_xmega.h>
#include <memory/user-signature/user-signature_xmega.h>
#include <memory/common/memory_common_xmega.h>
#include <io.h>
#include <types.h>

#endif /* __ASSEMBLY__ */

#include <chip/memory-map.h>


/* NVM Controller Offsets */
#define NVM_ADDR0			0x00
#define NVM_ADDR1			0x01
#define NVM_ADDR2			0x02
#define NVM_DATA0			0x04
#define NVM_DATA1			0x05
#define NVM_DATA2			0x06
#define NVM_CMD 			0x0A
#define NVM_CTRLA			0x0B
#define NVM_CTRLB			0x0C
#define NVM_INTCTRL			0x0D
#define NVM_STATUS			0x0F
#define NVM_LOCKBITS			0x10

/* NVM Commands */
#define NVM_CMD_NO_OPERATION		0x00
#define NVM_CMD_LOAD_FLASH_BUFFER	0x23
#define NVM_CMD_ERASE_FLASH_PAGE	0x2B
#define NVM_CMD_WRITE_FLASH_PAGE	0x2E
#define NVM_CMD_ERASE_APP_PAGE		0x22
#define NVM_CMD_WRITE_APP_PAGE		0x24
#define NVM_CMD_ERASE_WRITE_APP_PAGE	0x25
#define NVM_CMD_ERASE_APP		0x20

#define NVM_CMD_READ_EEPROM		0x06
#define NVM_CMD_ERASE_EEPROM		0x30
#define NVM_CMD_WRITE_EEPROM_PAGE	0x34
#define NVM_CMD_ERASE_EEPROM_PAGE	0x32
#define NVM_CMD_LOAD_EEPROM_BUFFER	0x33
#define NVM_CMD_ERASE_EEPROM_BUFFER	0x36
#define NVM_CMD_ERASE_WRITE_EEPROM_PAGE	0x35

#define NVM_CMD_READ_FUSES		0x07
#define NVM_CMD_WRITE_LOCK_BITS		0x08

#define NVM_CMD_READ_USER_SIG_ROW	0x01
#define NVM_CMD_WRITE_USER_SIG_ROW	0x1A
#define NVM_CMD_ERASE_USER_SIG_ROW	0x18

/* CTRLA register */
#define NVM_CTRLA_CMDEX			0x01

/* CTRLB register */
#define NVM_CTRLB_EEMAPEN		0x08

/* STATUS register */
#define NVM_STATUS_NVMBUSY		0x80
#define NVM_STATUS_EELOAD		0x02

/**
 * \brief Write \a value to the 8-bit NVM register \a reg
 */
#define nvm_write_reg(reg, value) \
	mmio_write8((void *) (NVM_CONTROLLER_BASE + NVM_##reg), value)
/**
 * \brief Read the 8-bit NVM register \a reg
 */
#define nvm_read_reg(reg) \
	mmio_read8((void *)(NVM_CONTROLLER_BASE + NVM_##reg))

#endif /* CHIP_MEMORY_H_INCLUDED */

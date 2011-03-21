/**
 * \file
 *
 * \brief High-speed Bus (HSB): Chip-specific definitions
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
#ifndef CHIP_HSB_H_INCLUDED
#define CHIP_HSB_H_INCLUDED

/**
 * \weakgroup hsb_group
 * @{
 */

//! High Speed Bus (HSB) Slave IDs
enum hsb_slave_id {
	HSB_SLAVE_FLASH		= 0,	//!< Internal Flash
	HSB_SLAVE_PBA		= 1,	//!< HSB-PB Bridge A
	HSB_SLAVE_PBB		= 2,	//!< HSB-PB Bridge B
	HSB_SLAVE_AES		= 3,	//!< AES module
	HSB_SLAVE_CPU_RAM	= 4,	//!< Embedded CPU SRAM
	HSB_SLAVE_USBB		= 5,	//!< USBB FIFO interface
	HSB_SLAVE_EBI		= 6,	//!< External Bus Interface
	HSB_SLAVE_DMACA		= 7,	//!< DMACA register interface
	HSB_SLAVE_HSB_SRAM0	= 8,	//!< Embedded System SRAM 0
	HSB_SLAVE_HSB_SRAM1	= 9,	//!< Embedded System SRAM 1
	HSB_NR_SLAVES           = 10,   //!< Total number of HSB slaves
};

//! High Speed Bus (HSB) Master IDs
enum hsb_master_id {
	HSB_MASTER_CPU_DATA	= 0,	//!< CPU load/store access
	HSB_MASTER_CPU_INSN	= 1,	//!< CPU instruction fetch
	HSB_MASTER_CPU_SAB	= 2,	//!< CPU SAB access
	HSB_MASTER_PDCA		= 3,	//!< Peripheral DMA
	HSB_MASTER_DMACA0	= 4,	//!< DMACA master 0
	HSB_MASTER_DMACA1	= 5,	//!< DMACA master 1
	HSB_MASTER_USBB		= 6,	//!< USBB DMA engine
	HSB_NR_MASTERS          = 7,    //!< Total number of HSB masters
};

/**
 * \brief Masks indicating writeable priority bits for each slave's PRAS
 * register.
 *
 * Not all HSB masters can access every HSB slave in the system. For all
 * missing master-slave links in the HMATRIX, the corresponding priority
 * bits in the PRAS and PRBS registers are stuck at 0.
 *
 * Each entry in this array contains a 32-bit value with a '1' bit
 * for each bit in the corresponding PRAS register which can be written.
 * This is primarily useful for test purposes, but it can also be used
 * to determine which HSB masters and slaves are connected.
 */
static const uint32_t hsb_pras_mask[HSB_NR_SLAVES] = {
	0x03333333,
	0x00003303,
	0x00333303,
	0x00330303,
	0x03333330,
	0x00330303,
	0x03333333,
	0x00000003,
	0x03333333,
	0x03333333,
};

/**
 * \brief Masks indicating writeable priority bits for each slave's PRBS
 * register.
 *
 * Since this chip only has 7 HSB masters, all bits in all of the PRBS
 * registers are stuck at 0.
 */
static const uint32_t hsb_prbs_mask[HSB_NR_SLAVES];

//! @}

#endif /* CHIP_HSB_H_INCLUDED */

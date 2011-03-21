/**
 * \file
 *
 * \brief SCSI Block Commands
 *
 * This file contains definitions of some of the commands found in the
 * SCSI SBC-2 standard.
 *
 * Note that the SBC specification depends on several commands defined
 * by the SCSI Primary Commands (SPC) standard. Each version of the SBC
 * standard is meant to be used in conjunction with a specific version
 * of the SPC standard, as follows:
 *   - SBC   depends on SPC
 *   - SBC-2 depends on SPC-3
 *   - SBC-3 depends on SPC-4
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
#ifndef SCSI_SBC_PROTOCOL_H_INCLUDED
#define SCSI_SBC_PROTOCOL_H_INCLUDED

//! \name SCSI commands defined by SBC-2
//@{
#define SCSI_CMD_FORMAT_UNIT		0x04
#define SCSI_CMD_READ6			0x08
#define SCSI_CMD_WRITE6			0x0a
#define SCSI_CMD_START_STOP_UNIT	0x1b
#define SCSI_CMD_READ_CAPACITY10	0x25
#define SCSI_CMD_READ10			0x28
#define SCSI_CMD_WRITE10		0x2a
#define SCSI_CMD_VERIFY10		0x2f
//@}

//! \name SBC-2 Mode page definitions
//@{

enum scsi_sbc_mode_page_code {
	SCSI_MS_PAGE_CACHING		= 0x08,
};

//! \name SBC-2 Device-Specific Parameter
//@{
#define SCSI_MS_SBC_WP			0x80	//!< Write Protected
#define SCSI_MS_SBC_DPOFUA		0x10	//!< DPO and FUA supported
//@}

/**
 * \brief SBC-2 Short LBA mode parameter block descriptor
 */
struct sbc_slba_block_desc {
	be32_t	nr_blocks;			//!< Number of Blocks
	be32_t	block_len;			//!< Block Length
#define SBC_SLBA_BLOCK_LEN_MASK	0x00ffffffU	//!< Mask reserved bits
};

/**
 * \brief SBC-2 Caching mode page
 */
struct sbc_caching_mode_page {
	uint8_t	page_code;
	uint8_t	page_length;
	uint8_t	flags2;
#define SBC_MP_CACHE_IC		(1 << 7)	//!< Initiator Control
#define SBC_MP_CACHE_ABPF	(1 << 6)	//!< Abort Pre-Fetch
#define SBC_MP_CACHE_CAP	(1 << 5)	//!< Caching Analysis Permitted
#define SBC_MP_CACHE_DISC	(1 << 4)	//!< Discontinuity
#define SBC_MP_CACHE_SIZE	(1 << 3)	//!< Size enable
#define SBC_MP_CACHE_WCE	(1 << 2)	//!< Writeback Cache Enable
#define SBC_MP_CACHE_MF		(1 << 1)	//!< Multiplication Factor
#define SBC_MP_CACHE_RCD	(1 << 0)	//!< Read Cache Disable
	uint8_t	retention;
	be16_t	dis_pf_transfer_len;
	be16_t	min_prefetch;
	be16_t	max_prefetch;
	be16_t	max_prefetch_ceil;
	uint8_t	flags12;
#define SBC_MP_CACHE_FSW	(1 << 7)	//!< Force Sequential Write
#define SBC_MP_CACHE_LBCSS	(1 << 6)	//!< Logical Blk Cache Seg Sz
#define SBC_MP_CACHE_DRA	(1 << 5)	//!< Disable Read-Ahead
#define SBC_MP_CACHE_NV_DIS	(1 << 0)	//!< Non-Volatile Cache Disable
	uint8_t	nr_cache_segments;
	be16_t	cache_segment_size;
	uint8_t	reserved[4];
};

//@}

/**
 * \brief SBC-2 READ CAPACITY (10) parameter data
 */
struct sbc_read_capacity10_data {
	be32_t	max_lba;	//!< LBA of last logical block
	be32_t	block_len;	//!< Number of bytes in the last logical block
};

#endif /* SCSI_SBC_PROTOCOL_H_INCLUDED */

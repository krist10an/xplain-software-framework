/**
 * \file
 *
 * \brief USB Mass Storage Class (MSC) protocol definitions.
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
#ifndef USB_MSC_PROTOCOL_H_INCLUDED
#define USB_MSC_PROTOCOL_H_INCLUDED

#include <types.h>

/**
 * \ingroup usb_protocol_group
 * \defgroup msc_protocol_group Mass Storage Class Definitions
 *
 * This module contains definitions and data structures taken from the
 * USB Mass Storage Class Specification Overview 1.2 and the USB Mass
 * Storage Class Bulk-Only Transport 1.0 specifications.
 *
 * @{
 */

/**
 * \brief Possible bInterfaceSubClass values for a mass-storage
 * interface.
 *
 * \note In practice, most devices should use
 * #USB_MSC_SUBCLASS_TRANSPARENT and specify the actual command set in
 * the standard INQUIRY data block, even if the MSC spec indicates
 * otherwise. In particular, RBC is not supported by certain major
 * operating systems like Windows XP.
 */
enum usb_msc_interface_subclass {
	USB_MSC_SUBCLASS_RBC		= 0x01,	//!< Reduced Block Commands
	USB_MSC_SUBCLASS_ATAPI		= 0x02,	//!< CD/DVD devices
	USB_MSC_SUBCLASS_QIC_157	= 0x03,	//!< Tape devices
	USB_MSC_SUBCLASS_UFI		= 0x04,	//!< Floppy disk drives
	USB_MSC_SUBCLASS_SFF_8070I	= 0x05,	//!< Floppy disk drives
	USB_MSC_SUBCLASS_TRANSPARENT	= 0x06,	//!< Determined by INQUIRY
};

/**
 * \brief Possible bInterfaceProtocol values for a mass-storage
 * interface.
 *
 * \note The CBI protocols should not be used in new designs.
 */
enum usb_msc_interface_protocol {
	USB_MSC_PROTOCOL_CBI		= 0x00,	//!< Command/Bulk/Interrupt
	USB_MSC_PROTOCOL_CBI_ALT	= 0x01,	//!< W/o command completion
	USB_MSC_PROTOCOL_BULK		= 0x50,	//!< Bulk-only
};

/**
 * \brief Mass Storage Bulk-only class specific requests.
 */
enum usb_msc_reqid {
	USB_MSC_REQ_BULK_RESET		= 0xff,	//!< Mass Storage Reset
	USB_MSC_REQ_GET_MAX_LUN		= 0xfe,	//!< Get Max LUN
};

COMPILER_PACK_SET(1);

/**
 * \brief A Command Block Wrapper (CBW).
 */
struct usb_msc_cbw {
	le32_t	dCBWSignature;		//!< Must contain 'USBC'
	le32_t	dCBWTag;		//!< Unique command ID
	le32_t	dCBWDataTransferLength;	//!< Number of bytes to transfer
	uint8_t	bmCBWFlags;		//!< Direction in bit 7
	uint8_t	bCBWLUN;		//!< Logical Unit Number
	uint8_t	bCBWCBLength;		//!< Number of valid CDB bytes
	uint8_t	CDB[16];		//!< SCSI Command Descriptor Block
};

#define USB_CBW_SIGNATURE	0x43425355	//!< dCBWSignature value
#define USB_CBW_DIRECTION_IN	(1 << 7)	//!< Data from device to host
#define USB_CBW_LUN_MASK	0x0f		//!< Valid bits in bCBWLUN
#define USB_CBW_LEN_MASK	0x1f		//!< Valid bits in bCBWCBLength

/**
 * \brief A Command Status Wrapper (CSW).
 */
struct usb_msc_csw {
	le32_t	dCSWSignature;		//!< Must contain 'USBS'
	le32_t	dCSWTag;		//!< Same as dCBWTag
	le32_t	dCSWDataResidue;	//!< Number of bytes not transfered
	uint8_t	bCSWStatus;		//!< Status code
};

#define USB_CSW_SIGNATURE	0x53425355	//!< dCSWSignature value
#define USB_CSW_STATUS_PASS	0x00		//!< Command Passed
#define USB_CSW_STATUS_FAIL	0x01		//!< Command Failed
#define USB_CSW_STATUS_PE	0x02		//!< Phase Error

COMPILER_PACK_RESET();

//! @}

#endif /* USB_MSC_PROTOCOL_H_INCLUDED */

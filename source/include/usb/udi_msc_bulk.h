/**
 * \file
 *
 * \brief USB Mass Storage Class Bulk-only device interface driver
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
#ifndef USB_UDI_MSC_BULK_H_INCLUDED
#define USB_UDI_MSC_BULK_H_INCLUDED

/**
 * \ingroup usb_device_group
 * \defgroup udi_msc_bulk_group USB MSC BBB Device Interface driver
 *
 * This driver implements a USB Mass Storage Class Bulk-only interface.
 * Command, data and status transfers are all sent over two Bulk
 * endpoints (one in and one out); also known as Bulk-Bulk-Bulk (BBB).
 * This is the approach which has proven to give the best performance
 * while using the least amount of resources (e.g. endpoints); other
 * approaches like Command-Bulk-Interrupt (CBI) are not recommended for
 * new devices according to the MSC Specification Overview.
 *
 * This driver aims to conform with the following standards:
 * - Universal Serial Bus Specification Revision 2.0
 * - USB Mass Storage Class Specification Overview Revision 1.2
 * - USB Mass Storage Class Bulk-Only Transport Revision 1.0
 * - SCSI Architecture Model - 3 (SAM-3)
 * - SCSI Primary Commands - 3 (SPC-3)
 * - SCSI Block Commands - 2 (SBC-2)
 *
 * @{
 */

struct block_device;

/**
 * \brief Maximum number of characters in the device serial number.
 *
 * Note that there's usually no point in having more than 12 characters.
 */
#define MSC_MAX_SERIAL_LEN	20

extern struct udm_interface *udi_msc_create_iface(struct block_device *bdev);

status_t udi_msc_enable(struct udc *udc, struct udm_interface *iface,
		uint16_t setting);
void udi_msc_disable(struct udc *udc, struct udm_interface *iface);
status_t udi_msc_setup(struct udc *udc, struct udm_interface *iface,
		struct usb_setup_req *req);
status_t udi_msc_get_iface_descriptor(struct udm_interface *iface,
		struct usb_request *req, enum usb_device_speed speed,
		uint16_t len);
void udi_msc_free_descriptor(struct udm_interface *iface,
		struct usb_request *req);

void udi_msc_set_ready(struct udm_interface *iface);
void udi_msc_set_busy(struct udm_interface *iface, uint16_t asc,
		void (*queue_empty)(void *data), void *data);

//! @}

#endif /* USB_UDI_MSC_BULK_H_INCLUDED */

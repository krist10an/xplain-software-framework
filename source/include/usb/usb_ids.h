/**
 * \file
 *
 * \brief USB Vendor, Product and Class IDs
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
#ifndef USB_USB_IDS_H_INCLUDED
#define USB_USB_IDS_H_INCLUDED

/**
 * \ingroup usb_protocol_group
 * \defgroup usb_ids_group USB Vendor, Product and Class Identifiers
 *
 * These IDs are found in various Device and Interface descriptors. Used
 * when defining such descriptors in a USB device application, and
 * when enumerating devices in a USB host application.
 *
 * @{
 */

//! \name Vendor IDs
//@{
#define USB_VID_ATMEL		0x03eb	//!< Atmel Corporation
//@}

//! \name Product IDs
//@{
//! AVR32786 High Speed USB Mass Storage Device SDHC
#define USB_PID_AVR32786	0x2112
#define USB_PID_ASF_TEST	0x21fe	//!< For internal testing only
//@}

//! \name USB Device and Interface Class IDs
//@{
#define USB_CLASS_NONE		0x00	//!< No class
#define USB_CLASS_AUDIO		0x01	//!< USB Audio Class
#define USB_CLASS_CDC_DEVICE	0x02	//!< USB Communication Device Class
#define USB_CLASS_CDC_COMM	0x02	//!< CDC Communication Class Interface
#define USB_CLASS_CDC_DATA	0x0a	//!< CDC Data Class Interface
#define USB_CLASS_HID		0x03	//!< USB Human Interface Device Class
#define USB_CLASS_MSC		0x08	//!< USB Mass Storage Class
#define USB_CLASS_APP		0xfe	//!< Application-specific Class
#define USB_CLASS_VENDOR	0xff	//!< Vendor-specific class
//@}

//! \name USB Device and Interface Subclass IDs
//@{
#define USB_SUBCLASS_NONE	0x00	//!< No subclass
//@}

//! \name USB Device and Interface Protocol IDs
//@{
#define USB_PROTOCOL_NONE	0x00	//!< No protocol
#define USB_PROTOCOL_VENDOR	0xff	//!< Vendor-specific protocol
//@}

//! @}

#endif /* USB_USB_IDS_H_INCLUDED */

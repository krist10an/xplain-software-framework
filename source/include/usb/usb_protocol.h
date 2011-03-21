/**
 * \file
 *
 * \brief USB protocol definitions.
 *
 * This file contains definitions and data structures taken from the
 * USB 2.0 specification.
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
#ifndef USB_PROTOCOL_H_INCLUDED
#define USB_PROTOCOL_H_INCLUDED

#include <types.h>

/**
 * \ingroup usb_group
 * \defgroup usb_protocol_group USB Protocol Definitions
 *
 * This module defines constants and data structures taken from the USB
 * 2.0 specification.
 *
 * @{
 */

/**
 * \brief USB device state
 *
 * See the device state diagram in the USB 2.0 specification for more
 * information.
 */
enum usb_device_state {
	//! Device is attached to host, but Vbus is not present
	USB_STATE_ATTACHED = 0,
	//! Vbus is present, but the device has not been reset yet
	USB_STATE_POWERED,
	//! The device has been reset, and is using the default address
	USB_STATE_DEFAULT,
	/**
	 * \brief The device has received its own address, but no
	 * configuration has been selected
	 */
	USB_STATE_ADDRESS,
	//! A valid configuration has been selected
	USB_STATE_CONFIGURED,
	//! The device is suspended
	USB_STATE_SUSPENDED = 0x80,
	USB_STATE_POWERED_SUSPENDED = USB_STATE_POWERED | USB_STATE_SUSPENDED,
	USB_STATE_DEFAULT_SUSPENDED = USB_STATE_DEFAULT | USB_STATE_SUSPENDED,
	USB_STATE_ADDRESS_SUSPENDED = USB_STATE_ADDRESS | USB_STATE_SUSPENDED,
	USB_STATE_CONFIGURED_SUSPENDED = (USB_STATE_CONFIGURED
					  | USB_STATE_SUSPENDED),
};

/**
 * \brief USB device speed
 *
 * #USB_SPEED_UNKNOWN indicates that the device has not yet seen a reset
 * (i.e. it is in the \ref USB_STATE_POWERED "POWERED" or the \ref
 * USB_STATE_ATTACHED "ATTACHED" state, possibly suspended.)
 */
enum usb_device_speed {
	USB_SPEED_UNKNOWN = 0,	//!< Speed not known yet
	USB_SPEED_LOW,		//!< Low speed (1.5 Mb/s)
	USB_SPEED_FULL,		//!< Full speed (12 Mb/s)
	USB_SPEED_HIGH,		//!< High speed (480 Mb/s)
};

/**
 * \brief USB request types (bmRequestType)
 */
enum usb_reqtype {
	USB_REQTYPE_STANDARD	= 0, //!< Request defined by USB specification
	USB_REQTYPE_CLASS	= 1, //!< Class-specific request
	USB_REQTYPE_VENDOR	= 2, //!< Vendor-specific request
};

#define USB_REQTYPE_STANDARD_MASK	(USB_REQTYPE_STANDARD << 5)
#define USB_REQTYPE_CLASS_MASK		(USB_REQTYPE_CLASS << 5)
#define USB_REQTYPE_VENDOR_MASK		(USB_REQTYPE_VENDOR << 5)

/**
 * \brief USB recipient codes (bmRequestType)
 */
enum usb_recipient {
	USB_RECIP_DEVICE	= 0,
	USB_RECIP_INTERFACE	= 1,
	USB_RECIP_ENDPOINT	= 2,
	USB_RECIP_OTHER		= 3,
};

#define USB_RECIP_DEVICE_MASK		(USB_RECIP_DEVICE << 0)
#define USB_RECIP_INTERFACE_MASK	(USB_RECIP_INTERFACE << 0)
#define USB_RECIP_ENDPOINT_MASK		(USB_RECIP_ENDPOINT << 0)
#define USB_RECIP_OTHER_MASK		(USB_RECIP_OTHER << 0)

/**
 * \brief Standard USB requests (bRequest)
 */
enum usb_reqid {
	USB_REQ_GET_STATUS		=  0,
	USB_REQ_CLEAR_FEATURE		=  1,
	USB_REQ_SET_FEATURE		=  3,
	USB_REQ_SET_ADDRESS		=  5,
	USB_REQ_GET_DESCRIPTOR		=  6,
	USB_REQ_SET_DESCRIPTOR		=  7,
	USB_REQ_GET_CONFIGURATION	=  8,
	USB_REQ_SET_CONFIGURATION	=  9,
	USB_REQ_GET_INTERFACE		= 10,
	USB_REQ_SET_INTERFACE		= 11,
	USB_REQ_SYNCH_FRAME		= 12,
};

/**
 * \brief Standard USB device feature/status flags
 *
 * \note #USB_DEV_STATUS_SELF_POWERED is only valid for GetStatus, not
 * SetFeature and ClearFeature.
 *
 * \note #USB_DEV_FEATURE_TEST_MODE is only valid for SetFeature. After
 * this feature has been set, the device will not respond to any further
 * requests.
 */
enum usb_device_feature {
	USB_DEV_STATUS_SELF_POWERED	= 0, //!< Does not draw power from bus
	USB_DEV_FEATURE_REMOTE_WAKEUP	= 1, //!< Remote wakeup enabled
	USB_DEV_FEATURE_TEST_MODE	= 2, //!< USB test mode
};

/**
 * \brief Standard USB endpoint feature/status flags
 */
enum usb_endpoint_feature {
	USB_EP_FEATURE_HALT		= 0,
};

/**
 * \brief Standard USB Test Mode Selectors
 */
enum usb_test_mode_selector {
	USB_TEST_J			= 0x01,
	USB_TEST_K			= 0x02,
	USB_TEST_SE0_NAK		= 0x03,
	USB_TEST_PACKET			= 0x04,
	USB_TEST_FORCE_ENABLE		= 0x05,
};

/**
 * \brief Standard USB descriptor types
 */
enum usb_descriptor_type {
	USB_DT_DEVICE			= 1,
	USB_DT_CONFIGURATION		= 2,
	USB_DT_STRING			= 3,
	USB_DT_INTERFACE		= 4,
	USB_DT_ENDPOINT			= 5,
	USB_DT_DEVICE_QUALIFIER		= 6,
	USB_DT_OTHER_SPEED_CONFIGURATION = 7,
	USB_DT_INTERFACE_POWER		= 8,
};

/**
 * \brief Standard USB endpoint transfer types
 */
enum usb_ep_xfer_type {
	USB_EP_XFER_CONTROL		= 0x00,
	USB_EP_XFER_ISOCHRONOUS		= 0x01,
	USB_EP_XFER_BULK		= 0x02,
	USB_EP_XFER_INTERRUPT		= 0x03,
};

/**
 * \brief Standard USB language IDs for string descriptors
 */
enum usb_langid {
	USB_LANGID_EN_US		= 0x0406, //!< English (United States)
};

/**
 * \brief Mask selecting the index part of an endpoint address
 */
#define USB_EP_ADDR_MASK		0x0f
/**
 * \brief Endpoint transfer direction is IN
 */
#define USB_DIR_IN			0x80
/**
 * \brief Endpoint transfer direction is OUT
 */
#define USB_DIR_OUT			0x00

/**
 * \brief Maximum length in bytes of a USB descriptor
 *
 * The maximum length of a USB descriptor is limited by the 8-bit
 * bLength field.
 */
#define USB_MAX_DESC_LEN		255

/*
 * 2-byte alignment is OK for setup requests and device (qualifier)
 * descriptors.
 */
COMPILER_PACK_SET(2);

/**
 * \brief A USB Device SETUP request
 *
 * The data payload of SETUP packets always follows this structure.
 */
struct usb_setup_req {
	uint8_t bmRequestType;
	uint8_t bRequest;
	le16_t wValue;
	le16_t wIndex;
	le16_t wLength;
};

//! Return true if the setup request \a req indicates IN data transfer
#define usb_setup_is_in(req)		(!!((req)->bmRequestType & USB_DIR_IN))
//! Return true if the setup request \a req indicates OUT data transfer
#define usb_setup_is_out(req)		(!((req)->bmRequestType & USB_DIR_IN))
//! Return the type of the SETUP request \a req. \see usb_reqtype.
#define usb_setup_type(req)		(((req)->bmRequestType >> 5) & 3)
//! Return the recipient of the SETUP request \a req. \see usb_recipient
#define usb_setup_recipient(req)	((req)->bmRequestType & 0x1f)

/**
 * \brief Standard USB device descriptor
 */
struct usb_device_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	le16_t bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize0;
	le16_t idVendor;
	le16_t idProduct;
	le16_t bcdDevice;
	uint8_t iManufacturer;
	uint8_t iProduct;
	uint8_t iSerialNumber;
	uint8_t bNumConfigurations;
};

/**
 * \brief Standard USB device qualifier descriptor
 *
 * This descriptor contains information about the device when running at
 * the "other" speed (i.e. if the device is currently operating at high
 * speed, this descriptor can be used to determine what would change if
 * the device was operating at full speed.)
 */
struct usb_device_qualifier_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	le16_t bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize0;
	uint8_t bNumConfigurations;
	uint8_t bReserved;
};

/* Configuration and associated descriptors must be packed tightly */
COMPILER_PACK_SET(1);

/**
 * \brief Standard USB configuration descriptor
 */
struct usb_configuration_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	le16_t wTotalLength;
	uint8_t bNumInterfaces;
	uint8_t bConfigurationValue;
	uint8_t iConfiguration;
	uint8_t bmAttributes;
	uint8_t bMaxPower;
};

#define USB_CONFIG_ATTR_MUST_SET	(1U << 7) //!< Must always be set
#define USB_CONFIG_ATTR_SELF_POWERED	(1U << 6) //!< Self-powered
#define USB_CONFIG_ATTR_REMOTE_WAKEUP	(1U << 5) //!< Supports remote wakeup
#define USB_CONFIG_MAX_POWER(ma)	(((ma) + 1) / 2) //!< Max power in mA

/**
 * \brief Standard USB interface descriptor
 */
struct usb_interface_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bInterfaceNumber;
	uint8_t bAlternateSetting;
	uint8_t bNumEndpoints;
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t iInterface;
};

/**
 * \brief Standard USB endpoint descriptor
 */
struct usb_endpoint_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bEndpointAddress;
	uint8_t bmAttributes;
	le16_t wMaxPacketSize;
	uint8_t bInterval;
};

//! Return the index of the endpoint represented by \a desc
#define usb_ep_index(desc)	((desc)->bEndpointAddress & USB_EP_ADDR_MASK)
//! Return true if \a desc represents an IN endpoint
#define usb_ep_is_in(desc)	(!!((desc)->bEndpointAddress & USB_DIR_IN))
//! Return true if \a desc represents an OUT endpoint
#define usb_ep_is_out(desc)	(!((desc)->bEndpointAddress & USB_DIR_IN))
//! Return the transfer type of the endpoint represented by \a desc
#define usb_ep_xfer(desc)	((desc)->bmAttributes & 0x03)

/* String descriptors may be 2-byte aligned */
COMPILER_PACK_SET(2);

/**
 * \brief A standard USB string descriptor
 */
struct usb_string_descriptor {
	uint8_t	bLength;
	uint8_t	bDescriptorType;
	le16_t	bString[0];
};

COMPILER_PACK_RESET();

//! @}

#endif /* USB_PROTOCOL_H_INCLUDED */

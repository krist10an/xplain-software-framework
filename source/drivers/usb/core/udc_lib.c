/**
 * \file
 *
 * \brief USB Device Controller library functions.
 *
 * This file contains various optional utility functions for use by UDC
 * drivers.
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
#include <byteorder.h>
#include <debug.h>
#include <usb/dev.h>
#include <usb/udc.h>
#include <usb/udc_lib.h>
#include <usb/usb_protocol.h>

/**
 * \weakgroup udc_group
 * @{
 */

/**
 * \internal
 * \brief Handle a standard USB SETUP request
 *
 * \todo Need a way for the application and/or drivers to switch between
 * self-powered and bus-powered mode.
 *
 * \todo Implement remote wakeup support
 */
static status_t udc_lib_standard_request(struct udc *udc,
		struct usb_setup_req *req)
{
	uint16_t	value = le16_to_cpu(req->wValue);
	uint16_t	index = le16_to_cpu(req->wIndex);
	uint16_t	len = le16_to_cpu(req->wLength);

	switch (req->bRequest) {
	case USB_REQ_GET_STATUS: {
		le16_t status = LE16(0);
		status_t ret;

		if (len != sizeof(status) || usb_setup_is_out(req))
			return -1;

		switch (usb_setup_recipient(req)) {
		case USB_RECIP_DEVICE:
			status = LE16(1U << USB_DEV_STATUS_SELF_POWERED);
			break;
		case USB_RECIP_ENDPOINT:
			ret = udc_ep_is_halted(udc, index & USB_EP_ADDR_MASK);
			if (ret < 0)
				return -1;

			status = cpu_to_le16(ret);
			break;
		default:
			break;
		}

		udc_ep0_write_sync(udc, &status, sizeof(status));
		udc_ep0_expect_status(udc);
		break;
	}

	case USB_REQ_CLEAR_FEATURE:
		if (len != 0 || usb_setup_is_in(req))
			return -1;

		if (usb_setup_recipient(req) == USB_RECIP_DEVICE
				&& value == USB_DEV_FEATURE_REMOTE_WAKEUP) {
			/* do nothing */
		} else if (usb_setup_recipient(req) == USB_RECIP_ENDPOINT
				&& value == USB_EP_FEATURE_HALT) {
			if (udc_ep_clear_halt(udc, index & USB_EP_ADDR_MASK))
				return -1;
		} else {
			return -1;
		}

		udc_ep0_send_status(udc);
		break;

	case USB_REQ_SET_FEATURE:
		if (len != 0 || usb_setup_is_in(req))
			return -1;
		if (udc_is_high_speed(udc)
				&& usb_setup_recipient(req) == USB_RECIP_DEVICE
				&& value == USB_DEV_FEATURE_TEST_MODE) {
			if (index & 0xff)
				return -1;

			/*
			 * Unconfigure the device, terminating all
			 * ongoing requests.
			 */
			usb_dev_set_configuration(udc, 0);

			if (udc_enter_test_mode(udc, index >> 8))
				return -1;
		} else if (usb_setup_recipient(req) == USB_RECIP_ENDPOINT
				&& value == USB_EP_FEATURE_HALT) {
			if (udc_ep_set_halt(udc, index & USB_EP_ADDR_MASK))
				return -1;
		} else {
			return -1;
		}

		udc_ep0_send_status(udc);
		break;

	case USB_REQ_SET_ADDRESS:
		if (len != 0 || usb_setup_is_in(req) || value > 127)
			return -1;

		/*
		 * The address isn't actually changed until the status
		 * stage is complete. Make sure we don't handle any
		 * SETUP packets until then.
		 */
		udc_set_address(udc, value);
		udc_ep0_send_status(udc);
		break;

	case USB_REQ_GET_DESCRIPTOR:
		if (usb_setup_is_out(req))
			return -1;

		if (usb_dev_get_descriptor(udc, value, index, len) < 0)
			return -1;
		break;

	case USB_REQ_SET_DESCRIPTOR:
		/* Not supported (defined as optional by the USB 2.0 spec) */
		return -1;

	case USB_REQ_GET_CONFIGURATION:
		build_assert(sizeof(udc->config) == 1);

		if (len != 1 || usb_setup_is_out(req))
			return -1;

		udc_ep0_write_sync(udc, &udc->config, 1);
		udc_ep0_expect_status(udc);
		break;

	case USB_REQ_SET_CONFIGURATION:
		if (len != 0 || usb_setup_is_in(req) || !udc->address)
			return -1;

		if (usb_dev_set_configuration(udc, value) < 0)
			return -1;

		udc_ep0_send_status(udc);
		break;

	case USB_REQ_GET_INTERFACE:
		if (len != 1 || usb_setup_is_out(req) || !udc->config)
			return -1;

		if (usb_dev_get_interface(udc, index) < 0)
			return -1;
		break;

	case USB_REQ_SET_INTERFACE:
		if (len != 0 || usb_setup_is_in(req) || !udc->config)
			return -1;

		if (usb_dev_set_interface(udc, index, value) < 0)
			return -1;

		udc_ep0_send_status(udc);
		break;

	default:
		return -1;
	}

	return 0;
}

/**
 * \brief Handle a USB SETUP request.
 *
 * This function parses a USB SETUP request and submits an appropriate
 * response back to the host or, in the case of SETUP OUT requests
 * with data, sets up a buffer for receiving the data payload.
 *
 * Standard requests defined by the USB 2.0 standard are handled
 * internally, while class- and vendor-specific requests are passed on
 * to a higher-level driver.
 *
 * \param udc USB Device Controller instance.
 * \param req The raw USB SETUP request.
 *
 * \retval #STATUS_OK The request was handled successfully
 * \retval negative The request failed, so ep0 should be STALLed.
 */
status_t udc_lib_process_setup_request(struct udc *udc,
		struct usb_setup_req *req)
{
	dbg_verbose("req %02x: t%02x v%04x i%04x l%04x\n",
		   req->bRequest, req->bmRequestType,
		   le16_to_cpu(req->wValue), le16_to_cpu(req->wIndex),
		   le16_to_cpu(req->wLength));

	if (usb_setup_type(req) == USB_REQTYPE_STANDARD)
		return udc_lib_standard_request(udc, req);
	else
		return usb_dev_process_setup_request(udc, req);
}

//! @}

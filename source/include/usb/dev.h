/**
 * \file
 *
 * \brief USB hardware-independent device layer.
 *
 * This file defines the interface between the UDC driver and the
 * hardware-independent device core layer.
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
#ifndef USB_FUNCTION_H_INCLUDED
#define USB_FUNCTION_H_INCLUDED

#include <types.h>

struct udc;
struct usb_setup_req;

/**
 * \ingroup usb_device_group
 * \defgroup usb_dev_core_group USB Device Core Interface
 *
 * The functions defined by this module are called by the USB Device
 * Controller driver or the UDC library in response to bus events and
 * control requests from the host. The USB Device Multiplexing layer
 * will, when enabled, implement these, so applications utilizing the
 * UDM should not have to care about these.
 *
 * @{
 */

/**
 * \brief Reset the USB device.
 *
 * This is called when a USB bus reset is detected, and when the
 * device is disconnected from the host.
 *
 * \param udc USB Device Controller instance
 */
void usb_dev_reset(struct udc *udc);

/**
 * \brief Handle a standard GET_DESCRIPTOR request.
 *
 * This is called when a GET_DESCRIPTOR request is received from the
 * host. The higher-level driver may queue one or more IN buffers with
 * the requested descriptor data if available.
 *
 * After the descriptor data has been successfully sent, which may or
 * may not happen after this function returns, udc_ep0_expect_status()
 * must be called.
 *
 * \param udc USB Device Controller instance
 * \param value The wValue of the request in native byte order
 * \param index The wIndex of the request in native byte order
 * \param len The wLength of the request in native byte order. The
 *	higher-level driver must never queue more than this number of
 *	bytes even if the requested descriptor is larger.
 *
 * \retval #STATUS_OK The request was handled successfully.
 * \retval negative The request failed, so ep0 should be stalled.
 */
status_t usb_dev_get_descriptor(struct udc *udc, uint16_t value,
		uint16_t index, uint16_t len);

/**
 * \brief Set the active configuration.
 *
 * This function will deselect the currently active configuration, and
 * optionally select a new one. If \a config_id is zero, no new
 * configuration will be selected, but the function will still report
 * success. If \a config_id is higher than the number of supported
 * configurations, the function will report failure.
 *
 * \param udc USB Device Controller instance
 * \param config_id The bConfigurationValue of the new configuration,
 *	or zero if the device is to change to the Address state.
 *
 * \retval #STATUS_OK The request was handled successfully.
 * \retval negative The request failed, so ep0 should be stalled.
 */
status_t usb_dev_set_configuration(struct udc *udc, uint16_t config_id);

/**
 * \brief Get the currently active alternate setting of an interface.
 *
 * This function will queue a IN packet with a single byte indicating
 * the currently selected alternate setting of the specified interface.
 *
 * \param udc USB Device Controller instance
 * \param iface_id The bInterfaceNumber of the interface to query
 *
 * \retval #STATUS_OK The request was handled successfully, and the IN
 *	packet was queued.
 * \retval #ERR_INVALID_ARG \a iface_id does not match any interface in
 *	the current configuration, so ep0 should be stalled.
 */
status_t usb_dev_get_interface(struct udc *udc, uint16_t iface_id);

/**
 * \brief Select an alternate setting for an interface.
 *
 * Select alternate setting \a alt_setting for interface \a iface_id.
 *
 * \param udc USB Device Controller instance
 * \param iface_id The bInterfaceNumber of the interface
 * \param alt_setting The bAlternateSetting to be selected
 *
 * \retval #STATUS_OK The request was handled successfully.
 * \retval negative The request failed, so ep0 should be stalled.
 */
status_t usb_dev_set_interface(struct udc *udc, uint16_t iface_id,
		uint16_t alt_setting);

/**
 * \brief Handle a class- or vendor-defined setup request
 *
 * This function is called when a SETUP request whose Type field in
 * bmRequestType does not indicate a standard request. It is
 * completely up to the higher-level driver to decide which requests to
 * support; it may choose not to support any.
 *
 * This function may queue one or more data or status packets
 * asynchronously, and call udc_ep0_expect_status() when any queued IN
 * data has been sent.
 *
 * Note that the fields in \a setup are in USB (little-endian) byte
 * order.
 *
 * \param udc USB Device Controller instance
 * \param setup USB SETUP request
 *
 * \retval #STATUS_OK The request was handled successfully.
 * \retval negative The request failed, so ep0 should be stalled.
 */
status_t usb_dev_process_setup_request(struct udc *udc,
		struct usb_setup_req *setup);

//! @}

#endif /* USB_FUNCTION_H_INCLUDED */

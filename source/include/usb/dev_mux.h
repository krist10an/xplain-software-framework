/**
 * \file
 *
 * \brief USB device multiplexing layer
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
#ifndef USB_DEV_MUX_H_INCLUDED
#define USB_DEV_MUX_H_INCLUDED

#include <buffer.h>
#include <usb/usb_protocol.h>

struct usb_request;
struct udc;

/**
 * \addtogroup usb_dev_mux
 * @{
 */

/**
 * \brief A USB device interface.
 *
 * The interface driver may assume that the callbacks within this
 * structure are never called concurrently with each other. However,
 * they may be called concurrently with callbacks from other USB
 * requests.
 */
struct udm_interface {
	/**
	 * \brief Enable the interface.
	 *
	 * This function is called when the host selects a configuration
	 * to which this interface belongs through a Set Configuration
	 * request, and when the host selects an alternate setting of
	 * this interface through a Set Interface request.
	 *
	 * If \a setting is not valid for this interface, this function
	 * must return #ERR_INVALID_ARG without causing any changes to
	 * the internal state.
	 *
	 * If the interface is already enabled, the interface must
	 * disable and re-enable itself, possibly with a new alternate
	 * setting.
	 *
	 * This function must also update usb_dev_iface::cur_setting
	 * upon success.
	 *
	 * \param udc The USB Device Controller.
	 * \param iface The interface to be enabled.
	 * \param setting The alternate setting to be enabled.
	 *
	 * \retval #STATUS_OK The interface was successfully enabled.
	 * \retval negative The interface could not be enabled.
	 */
	status_t (*enable)(struct udc *udc, struct udm_interface *iface,
			uint16_t setting);
	/**
	 * \brief Disable the interface.
	 *
	 * This function is called when this interface is currently
	 * active, and
	 * - the host selects any configuration through a Set
	 *   Configuration request, or
	 * - the host issues a USB reset, or
	 * - the device is detached from the host (i.e. Vbus is no
	 *   longer present)
	 *
	 * \param udc The USB Device Controller.
	 * \param iface The interface to be disabled.
	 */
	void	(*disable)(struct udc *udc, struct udm_interface *iface);
	/**
	 * \brief Handle a control request directed at an interface.
	 *
	 * This function is called when this interface is currently
	 * active and the host sends a vendor- or class-specific SETUP
	 * request with this interface as the recipient.
	 *
	 * \param udc The USB Device Controller.
	 * \param iface The interface which the request is for.
	 * \param req The SETUP packet.
	 *
	 * \retval #STATUS_OK The control request was successfully handled.
	 * \retval negative The control request was not handled, and the
	 *	default control endpoint should be halted.
	 */
	status_t (*setup)(struct udc *udc, struct udm_interface *iface,
			struct usb_setup_req *req);
	/**
	 * \brief Get the interface descriptor
	 *
	 * This function must queue interface descriptors for all
	 * alternate settings, along with any associated endpoint
	 * descriptors and other interface-specific descriptors, as long
	 * as the total number of bytes queued doesn't exceed \a len.
	 *
	 * If some descriptors, or parts of them, cannot be queued
	 * because the total size of the buffers would exceed \a len,
	 * this does not affect the return value.
	 *
	 * \param iface The interface for which to retrieve the
	 *	descriptor.
	 * \param req The USB request used for sending the descriptors
	 * \param speed The current USB bus speed
	 * \param len The maximum number of bytes to be queued,
	 *	excluding everything that has been already queued to \a
	 *	req. Note that \a len may be zero.
	 *
	 * \return The total size of the descriptors in bytes, not
	 *	limited by \a len, or a negative error code on failure.
	 */
	status_t (*get_iface_descriptor)(struct udm_interface *iface,
			struct usb_request *req,
			enum usb_device_speed speed,
			uint16_t len);
	/**
	 * \brief Free any data allocated by get_iface_descriptor()
	 *
	 * When all descriptors have been successfully sent to the host,
	 * the device multiplexer will call free_descriptor() on each
	 * interface in the same order that get_descriptor() was called
	 * when preparing the request. Each interface must remove the
	 * same number of buffers that it queued during get_descriptor()
	 * from the head of the request's buffer list and free the
	 * buffer and any data associated with it.
	 *
	 * If the request's buffer list becomes empty, this function
	 * won't be called, as it indicates that the interface didn't
	 * queue any buffers.
	 *
	 * \param iface The interface which queued the data.
	 * \param req The USB request used for sending the descriptors
	 */
	void	(*free_descriptor)(struct udm_interface *iface,
			struct usb_request *req);

	/** \brief The interface number identifying this interface. */
	uint8_t iface_number;
	/** \brief The current setting of this interface. */
	uint8_t	cur_setting;
};

/**
 * \brief A USB device configuration.
 *
 * A configuration is a set of interfaces that can be selected using
 * the SET_CONFIGURATION standard control request.
 */
struct udm_config {
	/** \brief The configuration descriptor for this configuration. */
	struct usb_configuration_descriptor	desc;
	/**
	 * \brief The interfaces available when this configuration is
	 * active.
	 */
	struct udm_interface		*interface[];
};

extern struct udm_config *udm_create_config(uint8_t value,
		uint8_t nr_interfaces);
extern void udm_config_add_interface(struct udm_config *config,
		struct udm_interface *iface);
extern void udm_config_set_max_power(struct udm_config *config,
		unsigned int milliamps);
extern void udm_config_set_self_powered(struct udm_config *config);
extern void udm_config_set_bus_powered(struct udm_config *config);

extern int udm_submit_ascii_string_desc(struct udc *udc,
		struct usb_request *req, const char *str,
		uint16_t max_len);
extern int udm_submit_utf8_string_desc(struct udc *udc,
		struct usb_request *req, const char *str,
		uint16_t max_len);
extern int udm_submit_utf16le_string_desc(struct udc *udc,
		struct usb_request *req, const le16_t *str,
		uint16_t max_len);

//! @}

#endif /* USB_DEV_MUX_H_INCLUDED */

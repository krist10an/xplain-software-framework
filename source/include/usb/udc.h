/**
 * \file
 *
 * \brief USB Device Controller interface
 *
 * This file contains functions and data structures common to all USB
 * Device Controller implementations.
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
#ifndef USB_UDC_H_INCLUDED
#define USB_UDC_H_INCLUDED

#include <bitops.h>
#include <usb/usb_protocol.h>

struct usb_request;

/**
 * \ingroup usb_device_group
 * \defgroup udc_group USB Device Controller (UDC)
 *
 * The UDC driver provides a low-level abstraction of the device
 * controller hardware.  Most events originate from the hardware in the
 * form of interrupts, which may cause the UDC driver to call into the
 * UDC library or the device core. These may in turn call back into
 * the driver in order to send an appropriate response to the host.
 *
 * @{
 */

/**
 * \brief Endpoint identifier
 *
 * Used to identify endpoints on a USB device.
 */
typedef int8_t	usb_ep_id_t;

/**
 * \brief Bit definitions for udc::flags
 */
enum udc_flag {
	UDC_IS_ENABLED,		//!< Device Controller enabled
	UDC_AUTOATTACH,		//!< Attach when possible
	UDC_HAS_POWER,		//!< Vbus detected
	UDC_IS_SUSPENDED,	//!< Bus is suspended
};

/**
 * \brief A USB Device Controller
 *
 * This structure contains data about a USB Device Controller usable by
 * the rest of the system. The UDC driver will normally wrap this inside
 * its own structure, containing additional controller-specific data.
 */
struct udc {
	//! The speed that we're currently operating at.
	enum usb_device_speed	speed;
	/**
	 * \brief Current device configuration value
	 *
	 * If the device has not reached the \ref USB_STATE_CONFIGURED
	 * "CONFIGURED" state, this field contains 0.
	 */
	uint8_t			config;
	//! Bitmask of flags indicating the state of the device
	uint8_t			flags;
	/**
	 * \brief Our current device address
	 *
	 * If the device has not reached the \ref USB_STATE_ADDRESS
	 * "ADDRESS" state, this field contains 0.
	 */
	uint8_t			address;
};

#if defined(CONFIG_USBB_UDC)
# include <usb/udc_usbb.h>
#elif defined(CONFIG_AT90USB_UDC)
# include <usb/udc_at90usb.h>
#else
/* Needed for the testsuite */
extern void udc_set_address(struct udc *udc, unsigned int addr);
#endif

//! \name Initialization
//@{

/**
 * \brief Initialize the USB Device Controller
 *
 * This will initialize the USB Device Controller, making it ready to
 * attach to the bus. The controller won't attach automatically until
 * udc_attach() has been called as well.
 *
 * If the UDC is part of a dual-role (OTG) controller, and dual-role
 * configuration is enabled, this function will initialize the whole
 * controller if it hasn't been initialized before.
 *
 * \return A USB Device Controller object
 */
extern struct udc *udc_init(void);

//@}

/**
 * \name Device State
 *
 * The following functions are used to query and modify the state of the
 * USB device as a whole.
 */
//@{

/**
 * \brief Test whether the USB Device Controller is running at high
 * speed.
 *
 * If the UDC doesn't support high-speed operation, this function will
 * always return false. This may be used by the compiler to eliminate
 * high-speed only code when the UDC doesn't support it.
 *
 * \retval true The UDC is running at high speed
 * \retval false The UDC is not running at high speed
 */
static inline bool udc_is_high_speed(struct udc *udc)
{
#ifdef CONFIG_UDC_HIGH_SPEED
	return udc->speed == USB_SPEED_HIGH;
#else
	return false;
#endif
}

/**
 * \brief Test whether the USB Device Controller is running at full
 * speed.
 *
 * If the UDC doesn't support full-speed operation, this function will
 * always return false. This may be used by the compiler to eliminate
 * full-speed only code when the UDC doesn't support it, e.g. when it is
 * configured to run in low-speed mode only.
 *
 * \retval true The UDC is running at full speed
 * \retval false The UDC is not running at full speed
 */
static inline bool udc_is_full_speed(struct udc *udc)
{
#ifdef CONFIG_UDC_FULL_SPEED
	return udc->speed == USB_SPEED_FULL;
#else
	return false;
#endif
}

/**
 * \brief Test whether the USB Device Controller is running at low
 * speed.
 *
 * If the UDC doesn't support low-speed operation, this function will
 * always return false. This may be used by the compiler to eliminate
 * low-speed only code when the UDC doesn't support it. Note that
 * low-speed mode is incompatible with all other modes, so this function
 * will always return false for full- and/or high-speed capable devices.
 *
 * \retval true The UDC is running at low speed
 * \retval false The UDC is not running at low speed
 */
static inline bool udc_is_low_speed(struct udc *udc)
{
#ifdef CONFIG_UDC_LOW_SPEED
	return udc->speed == USB_SPEED_LOW;
#else
	return false;
#endif
}

/**
 * \brief Figure out which #usb_device_state the device is in
 *
 * This function is probably overly expensive for most purposes, but
 * might useful for debugging. It's also useful for documentation
 * purposes.
 */
static inline enum usb_device_state udc_state(struct udc *udc)
{
	int state;

	if (!test_bit(UDC_HAS_POWER, &udc->flags))
		state = USB_STATE_ATTACHED;
	else if (udc->speed == USB_SPEED_UNKNOWN)
		state = USB_STATE_POWERED;
	else if (!udc->address)
		state = USB_STATE_DEFAULT;
	else if (!udc->config)
		state = USB_STATE_ADDRESS;
	else
		state = USB_STATE_CONFIGURED;

	if (test_bit(UDC_IS_SUSPENDED, &udc->flags))
		state |= USB_STATE_SUSPENDED;

	return state;
}

/**
 * \brief Attach \a %udc to the bus when possible
 *
 * Call this function to signal that the application is ready for the
 * UDC to attach to the bus. This will cause the UDC to attach whenever
 * the following conditions are present:
 * - The ID pin indicates Device operation. When the driver operates in
 *   device-only mode, this condition is assumed to always be true.
 * - An acceptable Vbus level from the host is detected.
 */
extern void udc_attach(struct udc *udc);

/**
 * \brief Detach \a %udc from the bus
 *
 * Call this function to forcibly detach the UDC from the bus. The UDC
 * will detach immediately and won't reattach until udc_attach() is
 * called, subject to the conditions listed for that function.
 */
extern void udc_detach(struct udc *udc);

//@}

/**
 * \name Endpoint Management
 *
 * The following functions allow drivers to create and destroy
 * endpoints, as well as set, clear and query their "halted" and
 * "wedged" states.
 *
 * Wedging an endpoint is useful when encountering errors that need some
 * sort of error recovery procedure to be performed by the host. A wedged
 * endpoint behaves like one that is halted, except that the host is
 * prevented from clearing the halted state through the standard
 * ClearFeature(HALT) request. This behaviour is required by the USB
 * Mass Storage Bulk-Only Transport specification.
 */
//@{

/**
 * \brief Create a new Bulk endpoint
 *
 * Create a new endpoint with bulk transfer type. If bit 7 of \a addr is
 * set, the endpoint is configured as a bulk-IN endpoint; otherwise,
 * it's configured as a bulk-OUT endpoint.
 *
 * \param udc USB Device Controller instance
 * \param addr USB endpoint address, including direction. Bits 6..4 are
 *	reserved and must be set to 0. Same as the bEndpointAddress
 *	field of the endpoint descriptor.
 * \param max_packet_size The maximum packet size to allow on this
 *	endpoint. Same as the wMaxPacket field in the endpoint
 *	descriptor converted to native endian.
 *
 * \return A cookie identifying the new endpoint, or a negative error
 *	code
 */
extern usb_ep_id_t udc_ep_create_bulk(struct udc *udc, uint8_t addr,
		uint16_t max_packet_size);

/**
 * \brief Create a new Interrupt endpoint
 *
 * Create a new endpoint with interrupt transfer type. If bit 7 of \a
 * addr is set, the endpoint is configured as an interrupt-IN endpoint;
 * otherwise, it's configured as an interrupt-OUT endpoint.
 *
 * \param udc USB Device Controller instance
 * \param addr USB endpoint address, including direction. Bits 6..4 are
 *	reserved and must be set to 0. Same as the bEndpointAddress
 *	field of the endpoint descriptor.
 * \param max_packet_size The maximum packet size to allow on this
 *	endpoint. Same as the wMaxPacket field in the endpoint
 *	descriptor converted to native endian.
 *
 * \return A cookie identifying the new endpoint, or a negative error
 *	code
 */
extern usb_ep_id_t udc_ep_create_interrupt(struct udc *udc, uint8_t addr,
		uint16_t max_packet_size);

/**
 * \brief Destroy a previously created endpoint
 *
 * This function will disable the specified endpoint, terminating all
 * queued buffers.
 *
 * \param udc USB Device Controller instance
 * \param ep USB Endpoint ID previously returned by udc_ep_create()
 */
extern void udc_ep_destroy(struct udc *udc, usb_ep_id_t ep);

/**
 * \brief Clear the halted state of the endpoint \a ep
 *
 * After this function is called, any transaction on \a ep will
 * be handled normally, i.e. a STALL hanshake will not be sent, and
 * the data toggle sequence will start at DATA0.
 *
 * \param udc USB Device Controller instance
 * \param ep The ID of the endpoint to be un-halted
 *
 * \retval STATUS_OK The endpoint was successfully halted
 * \retval negative The endpoint could not be halted
 */
extern status_t udc_ep_clear_halt(struct udc *udc, usb_ep_id_t ep);
/**
 * \brief Set the halted state of the endpoint \a ep
 *
 * After this function is called, any transaction on \a ep will result
 * in a STALL hanshake being sent. Any pending transactions will be
 * performed first, however.
 *
 * \param udc USB Device Controller instance
 * \param ep The ID of the endpoint to be halted
 *
 * \retval STATUS_OK The endpoint was successfully halted
 * \retval negative The endpoint could not be halted
 */
extern status_t udc_ep_set_halt(struct udc *udc, usb_ep_id_t ep);
/**
 * \brief Check if the endpoint \a ep is halted.
 *
 * \param udc USB Device Controller instance.
 * \param ep The ID of the endpoint to check.
 *
 * \retval 1 \a ep is halted, i.e. the controller will respond
 *	with a STALL handshake to any transaction other than SETUP.
 * \retval 0 \a ep is not halted.
 * \retval negative The halted state could not be determined (i.e. \a ep
 *	does not identify a valid endpoint.)
 */
extern status_t udc_ep_is_halted(struct udc *udc, usb_ep_id_t ep);

/**
 * \brief Clear the wedged state of an endpoint
 *
 * After this function is called, the endpoint halt condition may be
 * cleared by calling udc_ep_clear_halt(). In particular, the host is
 * allowed to clear the halt condition using the ClearFeature(HALT)
 * control request.
 *
 * This function may be called even if the endpoint isn't wedged, but
 * if it is wedged, it must be halted too.
 *
 * \param udc USB Device Controller instance
 * \param ep The ID of the endpoint to be un-wedged
 *
 * \pre ep < #APP_UDC_NR_ENDPOINTS
 */
extern void udc_ep_clear_wedge(struct udc *udc, usb_ep_id_t ep);
/**
 * \brief Set the wedged state of an endpoint
 *
 * After this function is called, any transaction on \a ep will result
 * in a STALL hanshake being sent, and all requests to clear the halt
 * condition will be ignored. Any pending transactions will be
 * performed first, however.
 *
 * \param udc USB Device Controller instance
 * \param ep The ID of the endpoint to be wedged
 *
 * \pre ep < #APP_UDC_NR_ENDPOINTS
 */
extern void udc_ep_set_wedge(struct udc *udc, usb_ep_id_t ep);
/**
 * \brief Check if a given endpoint is wedged
 *
 * A wedged endpoint is a halted endpoint where udc_ep_clear_halt()
 * requests are ignored. To un-halt an wedged endpoint, first call
 * udc_ep_clear_wedge(), then call udc_ep_clear_halt().
 *
 * \param udc USB Device Controller instance
 * \param ep The ID of the endpoint to check
 *
 * \retval true \a ep is wedged
 * \retval false \a ep is not wedged.
 *
 * \pre ep < #APP_UDC_NR_ENDPOINTS
 */
extern bool udc_ep_is_wedged(struct udc *udc, usb_ep_id_t ep);

/**
 * \brief Terminate all pending requests on an endpoint.
 *
 * This function will flush an endpoint, terminating all queued
 * requests with an error status. After this function returns, the
 * endpoint request queue will be empty. The data toggle sequence and
 * the halted and wedged states of the endpoints will not be affected.
 *
 * \param udc USB Device Controller instance.
 * \param ep USB Endpoint ID previously returned by udc_ep_create().
 */
extern void udc_ep_flush(struct udc *udc, usb_ep_id_t ep);

//@}

/**
 * \name Control Transfer
 *
 * The following functions are used to send and receive packets over the
 * default control endpoint, ep0. Most functions are asynchronous and
 * will call a callback function after the transfer is done. The only
 * exception is udc_ep0_write_sync(), which will write the data to the
 * FIFO immediately and busy-wait until it has been sent.
 */
//@{

/**
 * \brief Submit an OUT request on the default control endpoint
 *
 * This function queues a USB request for receiving OUT data on the
 * default control endpoint (ep0).
 *
 * \param udc The USB Device Controller instance
 * \param req Request to use for receiving the OUT data
 *
 * \pre No other requests are queued on ep0
 * \post ep0 has entered the data OUT phase
 */
extern void udc_ep0_submit_out_req(struct udc *udc, struct usb_request *req);
/**
 * \brief Submit an IN request on the default control endpoint
 *
 * This function queues a USB request for transmitting IN data on the
 * default control endpoint (ep0).
 *
 * \param udc The USB Device Controller instance
 * \param req Request containing IN data for transmission
 *
 * \pre No other requests are queued on ep0
 * \post ep0 has entered the data IN phase
 */
extern void udc_ep0_submit_in_req(struct udc *udc, struct usb_request *req);

/**
 * \brief Transmit IN data on the default control endpoint synchronously
 *
 * This function will submit IN data on the default control endpoint
 * (ep0) and busy-wait until it has been sent.
 *
 * \note This function must only be used for small quantities of data
 * when it is impractical to submit a buffer asynchronously.
 *
 * \param udc The USB Device Controller instance
 * \param data The data to be transmitted on ep0
 * \param len The number of bytes to be transmitted
 *
 * \return The number of bytes actually transmitted. This may be less
 *	than the requested number of bytes. When sending 8 bytes or
 *	less, it is safe to assume that everything will be sent.
 *
 * \pre ep0 is ready to transmit data (no other buffers are queued)
 * \post ep0 is ready to transmit data
 */
extern status_t udc_ep0_write_sync(struct udc *udc,
		const void *data, size_t len);
/**
 * \brief Send a status IN packet on the default control endpoint
 *
 * This function will send a zero-length status IN packet on ep0. It
 * does not wait for completion, as a status packet marks the end of a
 * control transaction so no further action by the higher layers is
 * necessary.
 *
 * \param udc The USB Device Controller instance
 *
 * \pre ep0 is ready to transmit data (no other buffers are queued)
 * \post ep0 has entered the status IN phase
 */
extern void udc_ep0_send_status(struct udc *udc);
/**
 * \brief Signal that a status OUT packet is expected on the default
 *	control endpoint
 *
 * This function marks the end of the data IN phase, and signals that
 * a status OUT packet is expected.
 *
 * \param udc The USB Device Controller instance
 *
 * \pre ep0 is ready to transmit data (no other buffers are queued)
 * \post ep0 has entered the status OUT phase
 */
extern void udc_ep0_expect_status(struct udc *udc);

//@}

/**
 * \name Data Transfer
 *
 * The following functions are used to do non-control transfers, i.e.
 * bulk, interrupt and isochronous transfers. Note that no synchronous
 * functions are available -- the caller must always provide a callback
 * if needed.
 */
//@{

/**
 * \brief Submit an OUT request on a non-control endpoint
 *
 * This function queues a USB request for receiving OUT data on a
 * non-control endpoint.
 *
 * \param udc The USB Device Controller instance
 * \param ep The endpoint ID on which to queue the buffer
 * \param req Request containing OUT data for reception
 *
 * \pre Interrupts not masked
 * \pre ep > 0 && ep < #APP_UDC_NR_ENDPOINTS
 */
extern void udc_ep_submit_out_req(struct udc *udc, usb_ep_id_t ep,
		struct usb_request *req);
/**
 * \brief Submit an IN request on a non-control endpoint
 *
 * This function queues a USB request for transmitting IN data on a
 * non-control endpoint.
 *
 * \param udc The USB Device Controller instance
 * \param ep The endpoint ID on which to queue the buffer
 * \param req Request containing IN data for transmission
 *
 * \pre Interrupts not masked
 * \pre ep > 0 && ep < #APP_UDC_NR_ENDPOINTS
 */
extern void udc_ep_submit_in_req(struct udc *udc, usb_ep_id_t ep,
		struct usb_request *req);

//@}

//@}
//! @}

#endif /* USB_UDC_H_INCLUDED */

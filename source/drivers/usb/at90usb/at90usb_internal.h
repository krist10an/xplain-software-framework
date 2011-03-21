/**
 * \file
 *
 * \brief AT90USB driver: Internal functions and definitions
 *
 * This header is for internal use only. There is lots of ugly preprocessor
 * stuff in here so that the rest of the code can be reasonably clean and
 * efficient.
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
#ifndef AT90USB_INTERNAL_H_INCLUDED
#define AT90USB_INTERNAL_H_INCLUDED

#include <slist.h>
#include <bitops.h>
#include <stdint.h>
#include <workqueue.h>

#include <usb/udc.h>
#include <usb/request.h>

#include <chip/regs.h>

#include "at90usb_regs.h"

/* USB Device Controller part */

/**
 * Control endpoint state.
 */
enum ep0_state {
	EP0_STATE_SETUP = 0,  /**< Waiting for SETUP */
	EP0_STATE_DATA_IN,    /**< IN data being sent */
	EP0_STATE_DATA_ZLP,   /**< Zero-Length Packet being sent */
	EP0_STATE_DATA_OUT,   /**< Waiting for OUT data */
	EP0_STATE_STATUS_IN,  /**< IN status sent */
	EP0_STATE_STATUS_OUT, /**< Waiting for OUT status */
};

/**
 * Endpoint states.
 */
enum at90usb_ep_flag {
	AT90USB_EP_ALLOCATED,   /**< Endpoint is allocated */
	AT90USB_EP_ACTIVE_XFER, /**< Endpoint is active with a transfer */
	AT90USB_EP_ENABLED,     /**< Endpoint is enabled */
	AT90USB_EP_IS_IN,       /**< Endpoint is in IN mode */
	AT90USB_EP_WEDGE,       /**< Endpoint is in wedge state */
};

/**
 * Endpoint (EP) struct.
 */
struct at90usb_udc_ep {
	/** \brief Shadow variable of the EP UEIENX register */
	uint8_t               ueienx;
	/** \brief Endpoint number */
	usb_ep_id_t           id;
	/** \brief Pointer to the UDC controller */
	struct at90usb_udc    *udc90;
	/** \brief EP0 buffer offset */
	unsigned int          buf_offset;
	/** \brief EP work queue */
	struct workqueue_task task;
	/** \brief EP USB requests */
	struct slist          req_queue;
	/** \brief EP0 buffers for USB requests */
	struct slist          buf_queue;
	/** \brief EP state flags */
	bit_word_t            flags;
	/** \brief EP maximum packet size */
	uint16_t              maxpacket;
	/** \brief EP0 bytes written for an USB request */
	uint16_t              bytes_written;
};

/**
 * USB device class (UDC) struct.
 */
struct at90usb_udc {
	/** \brief The UDC controller */
	struct udc                udc;
	/** \brief EP0 state */
	enum ep0_state            ctrl_state;
	/** \brief USB setup request data */
	struct usb_setup_req      setup_req;
	/** \brief Pointer to the USB controller */
	struct at90usb_controller *at90usb;
	/** \brief UDC work queue */
	struct workqueue_task     task;
	/** \brief Array of EPs */
	struct at90usb_udc_ep     ep[APP_UDC_NR_ENDPOINTS];
};


/**
 * \internal
 * \brief Check if the AT90USB UDC controller is enabled.
 *
 * \param udc90 The UDC controller.
 *
 * \return True if UDC mode is enabled, false otherwise.
 */
static inline bool at90usb_udc_is_enabled(struct at90usb_udc *udc90)
{
#if defined(CONFIG_AT90USB_OTG)
	return test_bit(UDC_IS_ENABLED, &udc90->udc.flags);
#elif defined(CONFIG_UDC)
	return true;
#else
	return false;
#endif
}

/**
 * \internal
 * \brief Enable the AT90USB UDC controller.
 *
 * \param udc90 The UDC controller.
 */
static inline void at90usb_udc_enable(struct at90usb_udc *udc90)
{
	/*
	 * Keep the autoattach flag as the application left it, set the
	 * enabled flag and clear everything else. The AT90USB core driver
	 * will check the Vbus state right after enabling the
	 * controller.
	 */
	udc90->udc.flags &= (1 << UDC_IS_ENABLED) | (1 << UDC_AUTOATTACH);
	udc90->udc.flags |= (1 << UDC_IS_ENABLED);
}

/**
 * \internal
 * \brief Disable the AT90USB UDC controller.
 *
 * \param udc90 The UDC controller.
 */
static inline void at90usb_udc_disable(struct at90usb_udc *udc90)
{
	/*
	 * Clear all flags except the autoattach flag set by the
	 * application
	 */
	udc90->udc.flags &= (1 << UDC_AUTOATTACH);
}

extern void at90usb_udc_vbus_off(struct at90usb_udc *udc90);
extern void at90usb_udc_vbus_on(struct at90usb_udc *udc90);
extern struct at90usb_udc *at90usb_udc_init(void);
extern void at90usb_udc_shutdown(struct at90usb_udc *udc90);

/* USB Host part */

/**
 * USB host struct.
 */
struct at90usb_host {
	/* Empty structs are not allowed in general. */
	uint8_t placeholder;
};

/**
 * \internal
 * \brief Check if the AT90USB host controller is enabled.
 *
 * \param host90 The host controller.
 *
 * \return True if host mode is enabled, false otherwise.
 */
static inline bool at90usb_host_is_enabled(struct at90usb_host *host90)
{
#if defined(CONFIG_AT90USB_OTG)
	return test_bit(USB_HOST_IS_ENABLED, &host90->hcd.flags);
#else
	return false;
#endif
}

static inline void at90usb_host_enable(struct at90usb_host *host90)
{
	// TODO
}

static inline void at90usb_host_disable(struct at90usb_host *host90)
{
	// TODO
}
extern struct at90usb_host *at90usb_host_init(void);
extern void at90usb_host_shutdown(struct at90usb_host *host90);

/* USB Master and OTG part */

/**
 * USB controller struct.
 */
struct at90usb_controller {
#ifdef CONFIG_UDC
	struct at90usb_udc *udc;
#endif
};

#endif /* AT90USB_INTERNAL_H_INCLUDED */

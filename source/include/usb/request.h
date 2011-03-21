/**
 * \file
 *
 * \brief USB request structure and associated helper functions.
 *
 * A USB request contains one or more data buffers which are used to
 * send and receive data over USB pipes and endpoints.
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
#ifndef USB_REQUEST_H_INCLUDED
#define USB_REQUEST_H_INCLUDED

#include <bitops.h>
#include <buffer.h>
#include <types.h>
#include <status_codes.h>

struct udc;

/**
 * \addtogroup usb_request_group
 * @{
 */

/**
 * \brief Flag bits for indicating that a request needs special
 * treatment.
 */
enum usb_request_flag {
	/**
	 * \brief Last packet must be short.
	 *
	 * Setting this flag indicates that the last packet sent from
	 * this buffer must be short. If this buffer ends on an
	 * endpoint boundary, the UDC driver will send a zero-length
	 * packet to terminate the transfer.
	 */
	USB_REQ_SHORT_PKT,
};

/**
 * \brief A USB request
 *
 * This structure keeps track of the data associated with a USB
 * request for use in asynchronous transfers.
 */
struct usb_request {
	//! List of buffers associated with this request
	struct slist		buf_list;
	//! List node on the controller queue
	struct slist_node	node;
	/**
	 * \brief A bitwise combination of the bits defined by enum
	 * #usb_request_flag
	 */
	uint8_t			flags;
	/**
	 * \brief Function to be called when this request is completed
	 * \param udc The USB Device Controller that did the request
	 * \param req The request that was completed.
	 */
	void (*req_done)(struct udc *udc, struct usb_request *req);
	/**
	 * \brief Arbitrary data pointer associated with this request,
	 * for use by the submitter
	 */
	void			*context;
	/**
	 * \brief Status code set by the UDC driver upon completion of
	 * this request
	 */
	enum status_code	status;
	/**
	 * \brief The number of bytes actually transfered
	 *
	 * May be less than len if an error occurred, or if the UDC
	 * received a short packet.
	 */
	size_t			bytes_xfered;
};

extern void usb_req_init(struct usb_request *req);

/**
 * \brief Add a buffer to a USB request
 *
 * This will add the buffer \a buf to the end of the buffer list
 * associated with the USB request \a req.
 *
 * \param req The USB request
 * \param buf The buffer to be associated with \a req
 */
static inline void usb_req_add_buffer(struct usb_request *req,
		struct buffer *buf)
{
	slist_insert_tail(&req->buf_list, &buf->node);
}

extern struct usb_request *usb_req_alloc(void);
extern void usb_req_free(struct usb_request *req);
extern void usb_init(void);

extern struct usb_request *usb_req_alloc_single_tx(
		const void *data, size_t len);
extern struct usb_request *usb_req_alloc_single_rx(void *data, size_t len);
extern void usb_req_free_all(struct usb_request *req);

/**
 * \brief Return the first buffer associated with \a req
 */
static inline struct buffer *usb_req_get_first_buffer(
		struct usb_request *req)
{
	return slist_peek_head(&req->buf_list, struct buffer, node);
}

//! @}

#endif /* USB_REQUEST_H_INCLUDED */

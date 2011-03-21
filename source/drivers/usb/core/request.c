/**
 * \file
 *
 * \brief USB request helpers
 *
 * This file contains helper functions for dealing with USB request
 * structures.
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
#include <assert.h>
#include <mempool.h>
#include <physmem.h>
#include <slist.h>
#include <usb/request.h>

#include <app/usb.h>

static struct mem_pool	usb_request_pool;

/**
 * \brief Initialize a USB request.
 *
 * This initializes a USB request and makes sure that it doesn't have
 * any special flags set, and that it doesn't have any other buffers
 * associated with it.
 *
 * \param req The USB request to be initialized.
 */
void usb_req_init(struct usb_request *req)
{
	slist_init(&req->buf_list);
	req->flags = 0;
	req->bytes_xfered = 0;
}

/**
 * \brief Allocate and initialize a USB request structure.
 *
 * \return A pointer to the newly allocated USB request, or NULL if
 * the pool is exhausted.
 */
struct usb_request *usb_req_alloc(void)
{
	struct usb_request	*req;

	req = mem_pool_alloc(&usb_request_pool);
	if (req)
		usb_req_init(req);

	return req;
}

/**
 * \brief Free a USB request structure.
 *
 * \param req The request to be released back to the pool.
 */
void usb_req_free(struct usb_request *req)
{
	mem_pool_free(&usb_request_pool, req);
}

/**
 * \brief Initialize the USB core subsystem.
 *
 * This function will initialize the USB request pool.
 */
void usb_init(void)
{
	mem_pool_init_physmem(&usb_request_pool, &cpu_sram_pool,
			APP_USB_NR_REQUESTS,
			sizeof(struct usb_request), 2);
}

/**
 * \file
 *
 * \brief AT90USB driver: Device part
 *
 * This file implements a USB Device Controller (UDC) driver utilizing
 * the AT90USB controller hardware.
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
#include <byteorder.h>
#include <bitops.h>
#include <debug.h>
#include <interrupt.h>
#include <io.h>
#include <status_codes.h>
#include <string.h>
#include <util.h>
#include <workqueue.h>

#include <usb/request.h>
#include <usb/dev.h>
#include <usb/udc.h>
#include <usb/udc_lib.h>

#include <chip/regs.h>
#include <chip/memory-map.h>
#include <chip/at90usb.h>

#include <app/usb.h>

#include "at90usb_internal.h"
#include "at90usb_regs.h"

/* Configuration sanity-checks */
#if !defined(CONFIG_UDC_LOW_SPEED) && !defined(CONFIG_UDC_FULL_SPEED)
# error No valid USB speed has been configured
#endif
#if defined(CONFIG_UDC_HIGH_SPEED)
# error High-speed configuration not supported on this chip
#endif
#if defined(CONFIG_UDC_FULL_SPEED) && !defined(CHIP_AT90USB_UDC_FULL_SPEED)
# error Full-speed configuration not supported on this chip
#endif
#if defined(CONFIG_UDC_LOW_SPEED)
# if !defined(CHIP_AT90USB_UDC_LOW_SPEED)
#  error Low-speed configuration not supported on this chip
# endif
# if defined(CONFIG_UDC_FULL_SPEED)
#  error Low-speed configuration must be selected exclusively
# endif
#endif

/**
 * \internal
 * \brief Helper function to read data from the USB fifo.
 *
 * \param dest The destination to copy to.
 * \param len Number of bytes to read from the FIFO.
 */
static void copy_from_fifo(uint8_t *dest, const unsigned int len)
{
	unsigned int i;

	for (i = 0; i < len; i++)
		dest[i] = avr_read_reg8(UEDATX);
}

/**
 * \internal
 * \brief Helper function to write data to the USB fifo.
 *
 * \param source The destination to copy from.
 * \param len Number of bytes to write to the FIFO.
 */
static void copy_to_fifo(const uint8_t *source, const unsigned int len)
{
	unsigned int i;

	for (i = 0; i < len; i++)
		avr_write_reg8(UEDATX, source[i]);
}

/**
 * \addtogroup udc_group
 * @{
 */

/**
 * \internal
 * \brief Helper function get the AT90USB UDC instance from the UDC instance.
 *
 * \param udc An UDC instance.
 *
 * \return The AT90USB UDC instance.
 */
static inline struct at90usb_udc *at90usb_udc_of(struct udc *udc)
{
	return container_of(udc, struct at90usb_udc, udc);
}

/**
 * \internal
 * \brief Helper function get the AT90USB UDC instance from the work
 *        queue task.
 *
 * \param task A work queue task.
 *
 * \return The AT90USB UDC instance.
 */
static inline
struct at90usb_udc *at90usb_udc_task_of(struct workqueue_task *task)
{
        return container_of(task, struct at90usb_udc, task);
}

/**
 * \internal
 * \brief Helper function get the AT90USB UDC endpoint instance from the work
 *        queue task.
 *
 * \param task A work queue task.
 *
 * \return The AT90USB UDC endpoint instance.
 */
static inline
struct at90usb_udc_ep *at90usb_ep_task_of(struct workqueue_task *task)
{
	return container_of(task, struct at90usb_udc_ep, task);
}

/**
 * \internal
 * \brief Helper function to complete USB requests.
 *
 * This helper function will set the USB request status flag and call the
 * request done callback if needed.
 *
 * \param udc The UDC instance.
 * \param req A completed USB request.
 * \param status Status code for the completed USB request.
 */
static void at90usb_udc_req_done(struct udc *udc, struct usb_request *req,
		enum status_code status)
{
	req->status = status;

	if (req->req_done)
		req->req_done(udc, req);
}

/**
 * \internal
 * \brief Kill last bank for an IN endpoint.
 *
 * \pre In interrupt handler and/or interrupts disabled and with the
 *      appropriate endpoint selected through the \ref AVR_REG_UENUM UENUM
 *      register.
 */
static void at90usb_udc_kill_last_in_bank(void)
{
	avr_write_reg8(UEINTX, avr_read_reg8(UEINTX) | AT90USB_UEINTX_KILLBK);
	while (avr_read_reg8(UEINTX) & AT90USB_UEINTX_KILLBK)
		barrier();
}

/**
 * \internal
 * \brief Kill first bank for an OUT endpoint.
 *
 * \pre In interrupt handler and/or interrupts disabled and with the
 *      appropriate endpoint selected through the \ref AVR_REG_UENUM UENUM
 *      register.
 */
static void at90usb_udc_kill_first_out_bank(void)
{
	avr_write_reg8(UEINTX, avr_read_reg8(UEINTX)
			& ~(AT90USB_UEINTX_RXOUTI | AT90USB_UEINTX_FIFOCON));
}

/**
 * \internal
 * \brief Kill all banks for an endpoint.
 *
 * \pre In interrupt handler and/or interrupts disabled and with the
 *      appropriate endpoint selected through the \ref AVR_REG_UENUM UENUM
 *      register.
 *
 * \param udc The UDC instance.
 * \param ep_id Endpoint number.
 */
static void at90usb_udc_kill_all_banks(struct udc *udc, usb_ep_id_t ep_id)
{
	bool is_in = avr_read_reg8(UECFG0X) & AT90USB_UECFG0X_EPDIR_IN;

	while (AT90USB_UESTA0X_GET_NBUSYBK(avr_read_reg8(UESTA0X))) {
		if (is_in)
			at90usb_udc_kill_last_in_bank();
		else
			at90usb_udc_kill_first_out_bank();
	}
}

void udc_ep0_submit_out_req(struct udc *udc, struct usb_request *req)
{
	struct at90usb_udc	*udc90 = at90usb_udc_of(udc);
	struct at90usb_udc_ep	*ep = &udc90->ep[0];
	irqflags_t		iflags;

	assert(!test_bit(AT90USB_EP_ACTIVE_XFER, &ep->flags));
	assert(slist_is_empty(&ep->buf_queue));
	assert(slist_is_empty(&ep->req_queue));
	assert(ep->buf_offset == 0);

	udc90->ctrl_state = EP0_STATE_DATA_OUT;
	set_bit(AT90USB_EP_ACTIVE_XFER, &ep->flags);
	slist_borrow_to_tail(&ep->buf_queue, &req->buf_list);
	slist_insert_tail(&ep->req_queue, &req->node);
	barrier();

	iflags = cpu_irq_save();

	avr_write_reg8(UENUM, 0);
	ep->ueienx |= AT90USB_UEIENX_RXOUTE;
	avr_write_reg8(UEIENX, ep->ueienx);

	cpu_irq_restore(iflags);
}

void udc_ep0_submit_in_req(struct udc *udc, struct usb_request *req)
{
	struct at90usb_udc	*udc90 = at90usb_udc_of(udc);
	struct at90usb_udc_ep	*ep = &udc90->ep[0];
	irqflags_t		iflags;

	assert(!test_bit(AT90USB_EP_ACTIVE_XFER, &ep->flags));
	assert(slist_is_empty(&ep->buf_queue));
	assert(slist_is_empty(&ep->req_queue));
	assert(ep->buf_offset == 0);

	udc90->ctrl_state = EP0_STATE_DATA_IN;
	set_bit(AT90USB_EP_ACTIVE_XFER, &ep->flags);
	slist_borrow_to_tail(&ep->buf_queue, &req->buf_list);
	slist_insert_tail(&ep->req_queue, &req->node);
	barrier();

	iflags = cpu_irq_save();

	avr_write_reg8(UENUM, 0);
	ep->ueienx |= AT90USB_UEIENX_TXINE;
	avr_write_reg8(UEIENX, ep->ueienx);

	cpu_irq_restore(iflags);
}

status_t udc_ep0_write_sync(struct udc *udc, const void *data, size_t len)
{
	irqflags_t		iflags;
	const uint8_t		*data_p = data;

	assert(len > 0);
	assert(!test_bit(AT90USB_EP_ACTIVE_XFER,
				&at90usb_udc_of(udc)->ep[0].flags));
	avr_write_reg8(UENUM, 0);
	assert(avr_read_reg8(UEINTX) & AT90USB_UEINTX_TXINI);

	len = min_u(len, APP_UDC_MAXPACKETSIZE0);

	copy_to_fifo(data_p, len);

	iflags = cpu_irq_save();
	avr_write_reg8(UEINTX, avr_read_reg8(UEINTX) & ~AT90USB_UEINTX_TXINI);
	cpu_irq_restore(iflags);

	while (!(avr_read_reg8(UEINTX) & AT90USB_UEINTX_TXINI))
		barrier();

	return len;
}

void udc_ep0_send_status(struct udc *udc)
{
	struct at90usb_udc	*udc90 = at90usb_udc_of(udc);
	struct at90usb_udc_ep	*ep = &udc90->ep[0];
	irqflags_t		iflags;

	dbg_verbose("at90usb-udc: send status\n");

	assert(!test_bit(AT90USB_EP_ACTIVE_XFER, &udc90->ep[0].flags));
	avr_write_reg8(UENUM, 0);
	assert(avr_read_reg8(UEINTX) & AT90USB_UEINTX_TXINI);

	iflags = cpu_irq_save();

	ep->ueienx &= ~AT90USB_UEIENX_RXOUTE;
	ep->ueienx |= AT90USB_UEIENX_TXINE;
	avr_write_reg8(UEINTX, avr_read_reg8(UEINTX) & ~AT90USB_UEINTX_TXINI);
	avr_write_reg8(UEIENX, ep->ueienx);

	cpu_irq_restore(iflags);

	udc90->ctrl_state = EP0_STATE_STATUS_IN;
}

void udc_ep0_expect_status(struct udc *udc)
{
	struct at90usb_udc *udc90 = at90usb_udc_of(udc);

	// Here, the TXINI flag must be set for endpoint 0

	assert(!test_bit(AT90USB_EP_ACTIVE_XFER, &udc90->ep[0].flags));

	/*
	 * Don't bother waiting for STATUS OUT. The RX interrupt will
	 * get cleared when we receive the next SETUP packet.
	 */
	udc90->ctrl_state = EP0_STATE_STATUS_OUT;
}

/**
 * \internal
 * \brief Submit queued buffers on a non-control OUT endpoint.
 *
 * \note This function will disable and enable interrupts while processing the
 * queue, but will return with interrupt flag as when called. It will process
 * an entire USB request before calling the callback (if any) and exit,
 * potentially doing a reentry shortly after to process any additional USB
 * requests.
 *
 * \param task Work queue task.
 *
 * \pre ep->active == NULL
 * \pre Called from workqueue, interrupts enabled.
 */
static void at90usb_udc_submit_out_queue(struct workqueue_task *task)
{
	struct at90usb_udc_ep	*ep = at90usb_ep_task_of(task);
	struct at90usb_udc	*udc90 = ep->udc90;
	struct buffer		*buf;
	struct usb_request	*req;
	uint16_t		buf_offset = 0;
	uint16_t		fifo_size;

	if (slist_is_empty(&ep->req_queue))
		goto no_transfer;

	avr_write_reg8(UENUM, ep->id);

	cpu_irq_disable();

	dbg_verbose("ep%u-out: submit req_queue %p status=%02x\n",
			ep->id, slist_peek_head_node(&ep->req_queue),
			avr_read_reg8(UEINTX));

	assert(!test_bit(AT90USB_EP_ACTIVE_XFER, &ep->flags));

	set_bit(AT90USB_EP_ACTIVE_XFER, &ep->flags);

	cpu_irq_enable();

	if (!test_bit(AT90USB_EP_ENABLED, &ep->flags))
		goto no_transfer;

	cpu_irq_disable();
	req = slist_pop_head(&ep->req_queue, struct usb_request, node);
	cpu_irq_enable();
	if (slist_is_empty(&req->buf_list)) {
		assert(slist_is_empty(&ep->req_queue));
		goto no_transfer;
	}

	buf = slist_peek_head(&req->buf_list, struct buffer, node);
	while (slist_node_is_valid(&req->buf_list, &buf->node)) {
		uint16_t nbytes;

		cpu_irq_disable();
		/* Clear RXOUTI control bit to ack the ready bank. */
		avr_write_reg8(UEINTX, avr_read_reg8(UEINTX)
				& ~AT90USB_UEINTX_RXOUTI);
		cpu_irq_enable();

		fifo_size = (avr_read_reg8(UEBCHX) << 8) | avr_read_reg8(UEBCLX);
		nbytes = min_u(fifo_size, buf->len - buf_offset);

		copy_from_fifo((uint8_t *)((uintptr_t)buf->addr.phys
					+ buf_offset), nbytes);

		buf_offset += nbytes;

		cpu_irq_disable();
		/* Clear FIFO bit to ack that the contents are read. */
		avr_write_reg8(UEINTX, avr_read_reg8(UEINTX)
				& ~AT90USB_UEINTX_FIFOCON);
		cpu_irq_enable();

		/*
		 * We got a shorter package than expected. Store number of
		 * transferred bytes and break the while loop to call the
		 * at90usb_udc_req_done.
		 */
		if (fifo_size < ep->maxpacket) {
			req->bytes_xfered += buf_offset;
			break;
		}

		if (buf_offset == buf->len) {
			req->bytes_xfered += buf_offset;

			/*
			 * If this is the last buffer in the request we need an
			 * interrupt to advance to any potential next requests.
			 */
			if (req->buf_list.last == &buf->node)
				break;

			buf = slist_peek_next(&buf->node, struct buffer, node);
			buf_offset = 0;
		}

		/* Wait for a bank to be ready before reading the FIFO. */
		while (!(avr_read_reg8(UEINTX) & AT90USB_UEINTX_RXOUTI)) {
			if (!test_bit(AT90USB_EP_ENABLED, &ep->flags))
				goto no_transfer;
		}
	}

	/* The compiler must not move any stores beyond this point. */
	barrier();

	at90usb_udc_req_done(&udc90->udc, req, 0);

	cpu_irq_disable();
	/* The at90usb_udc_req_done call might change the endpoint. */
	avr_write_reg8(UENUM, ep->id);
	ep->ueienx |= AT90USB_UEIENX_RXOUTE;
	avr_write_reg8(UEIENX, ep->ueienx);
	cpu_irq_enable();

	goto out;

no_transfer:
	cpu_irq_disable();
	ep->ueienx &= ~AT90USB_UEIENX_RXOUTE;
	avr_write_reg8(UEIENX, ep->ueienx);
	cpu_irq_enable();
out:
	clear_bit(AT90USB_EP_ACTIVE_XFER, &ep->flags);
}

/**
 * \internal
 * \brief Submit queued buffers on a non-control IN endpoint.
 *
 * \note This function will disable and enable interrupts while processing the
 * queue, but will return with interrupt flag as when called. It will process
 * an entire USB request before calling the callback (if any) and exit,
 * potentially doing a reentry shortly after to process any additional USB
 * requests.
 *
 * \param task Work queue task.
 *
 * \pre ep->active == NULL
 * \pre Called from workqueue, interrupts enabled.
 */
static void at90usb_udc_submit_in_queue(struct workqueue_task *task)
{
	struct at90usb_udc_ep	*ep = at90usb_ep_task_of(task);
	struct at90usb_udc	*udc90 = ep->udc90;
	struct buffer		*buf;
	struct usb_request	*req;
	uint16_t		buf_offset = 0;
	uint16_t		fifo_len = ep->maxpacket;
	uint16_t		fifo_offset;

	if (slist_is_empty(&ep->req_queue))
		goto no_transfer;

	avr_write_reg8(UENUM, ep->id);

	cpu_irq_disable();

	dbg_verbose("ep%u-in: submit req_queue %p status=%02x\n",
			ep->id, slist_peek_head_node(&ep->req_queue),
			avr_read_reg8(UEINTX));

	assert(!test_bit(AT90USB_EP_ACTIVE_XFER, &ep->flags));

	set_bit(AT90USB_EP_ACTIVE_XFER, &ep->flags);

	cpu_irq_enable();

	if (!test_bit(AT90USB_EP_ENABLED, &ep->flags))
		goto no_transfer;

	cpu_irq_disable();
	req = slist_pop_head(&ep->req_queue, struct usb_request, node);
	cpu_irq_enable();
	if (slist_is_empty(&req->buf_list)) {
		assert(slist_is_empty(&ep->req_queue));
		goto no_transfer;
	}

	buf = slist_peek_head(&req->buf_list, struct buffer, node);
	fifo_offset = (avr_read_reg8(UEBCHX) << 8) | avr_read_reg8(UEBCLX);

	while (slist_node_is_valid(&req->buf_list, &buf->node)) {
		unsigned int nbytes;

		nbytes = min_u(fifo_len - fifo_offset, buf->len - buf_offset);

		/* Wait for a bank to be ready before writing the FIFO. */
		while (!(avr_read_reg8(UEINTX) & AT90USB_UEINTX_TXINI))
			barrier();

		cpu_irq_disable();
		/* Clear TXINI control bit to ack the ready bank. */
		avr_write_reg8(UEINTX, avr_read_reg8(UEINTX)
				& ~AT90USB_UEINTX_TXINI);
		cpu_irq_enable();

		copy_to_fifo((uint8_t *)((uintptr_t)buf->addr.phys
					+ buf_offset), nbytes);

		buf_offset += nbytes;
		fifo_offset += nbytes;

		if (fifo_offset == fifo_len || buf_offset == buf->len) {
			/* Clear FIFO control bit to send the contents. */
			cpu_irq_disable();
			avr_write_reg8(UEINTX, avr_read_reg8(UEINTX)
					& ~AT90USB_UEINTX_FIFOCON);
			cpu_irq_enable();
			fifo_offset = 0;
		}

		if (buf_offset == buf->len) {
			req->bytes_xfered += buf_offset;

			/* If this is the last buffer in the request we break
			 * before advancing to any potential next requests.
			 */
			if (req->buf_list.last == &buf->node)
				break;

			buf = slist_peek_next(&buf->node, struct buffer, node);
			buf_offset = 0;
		}

		if (!test_bit(AT90USB_EP_ENABLED, &ep->flags))
			goto no_transfer;
	}

	/* The compiler must not move any stores beyond this point. */
	barrier();

	at90usb_udc_req_done(&udc90->udc, req, 0);
	workqueue_add_task(&main_workqueue, &ep->task);
no_transfer:
	clear_bit(AT90USB_EP_ACTIVE_XFER, &ep->flags);
}

void udc_ep_submit_out_req(struct udc *udc, usb_ep_id_t ep_id,
		struct usb_request *req)
{
	struct at90usb_udc	*udc90 = at90usb_udc_of(udc);
	struct at90usb_udc_ep	*ep = &udc90->ep[ep_id];
	bool			queued = true;

	assert(cpu_irq_is_enabled());
	assert(ep_id > 0 && ep_id < APP_UDC_NR_ENDPOINTS);

	req->bytes_xfered = 0;
	req->status = OPERATION_IN_PROGRESS;

	cpu_irq_disable();
	if (test_bit(AT90USB_EP_ENABLED, &ep->flags)) {
		slist_insert_tail(&ep->req_queue, &req->node);
		if (!test_bit(AT90USB_EP_ACTIVE_XFER, &ep->flags)) {
			avr_write_reg8(UENUM, ep->id);
			ep->ueienx |= AT90USB_UEIENX_RXOUTE;
			avr_write_reg8(UEIENX, ep->ueienx);
		}
	} else {
		queued = false;
	}
	cpu_irq_enable();

	if (!queued)
		at90usb_udc_req_done(udc, req, ERR_FLUSHED);
}

void udc_ep_submit_in_req(struct udc *udc, usb_ep_id_t ep_id,
		struct usb_request *req)
{
	struct at90usb_udc	*udc90 = at90usb_udc_of(udc);
	struct at90usb_udc_ep	*ep = &udc90->ep[ep_id];
	bool			queued = true;

	assert(cpu_irq_is_enabled());
	assert(ep_id > 0 && ep_id < APP_UDC_NR_ENDPOINTS);

	req->bytes_xfered = 0;
	req->status = OPERATION_IN_PROGRESS;

	cpu_irq_disable();
	if (test_bit(AT90USB_EP_ENABLED, &ep->flags)) {
		slist_insert_tail(&ep->req_queue, &req->node);
		if (!test_bit(AT90USB_EP_ACTIVE_XFER, &ep->flags))
			workqueue_add_task(&main_workqueue, &ep->task);
	} else {
		queued = false;
	}
	cpu_irq_enable();

	if (!queued)
		at90usb_udc_req_done(udc, req, ERR_FLUSHED);
}

status_t udc_ep_is_halted(struct udc *udc, usb_ep_id_t ep)
{
	irqflags_t iflags;
	status_t retval;

	if (ep >= APP_UDC_NR_ENDPOINTS)
		return -1;

	iflags = cpu_irq_save();

	avr_write_reg8(UENUM, ep);
	retval = !!(avr_read_reg8(UECONX) & AT90USB_UECONX_STALLRQ);

	cpu_irq_restore(iflags);

	return retval;
}

status_t udc_ep_set_halt(struct udc *udc, usb_ep_id_t ep)
{
	irqflags_t	iflags;
	uint8_t		uecfg;

	dbg_verbose("at90usb-udc: ep%d: set halt\n", ep);

	if (ep >= APP_UDC_NR_ENDPOINTS)
		return -1;

	avr_write_reg8(UENUM, ep);

	/*
	 * Even though the function driver takes care not to request stall
	 * until it has received a callback for the data transfer, there's
	 * still a chance that the data transfer may get STALLed.
	 *
	 * This is because the callback happens when the transfer is complete,
	 * not when the transfers is actually done. At this point, there may
	 * still be IN data stuck in a bank waiting for the host to request it,
	 * and if so, the host will see a STALL instead of the data it asked
	 * for.
	 *
	 * Work around this by waiting for all IN banks to become empty before
	 * requesting a STALL. Ideally, an interrupt should be used, but that
	 * may cause us to send additional data before setting the STALL
	 * request, which would be just as bad.
	 *
	 * One way to improve this would be to set a flag indicating that the
	 * endpoint is really stalled and refusing to submit any more requests
	 * until the endpoint has been un-stalled. When NBUSYBK becomes 0, we
	 * can set STALLRQ and restart the queue when the stall is cleared by
	 * the host or some driver.
	 */
	uecfg = avr_read_reg8(UECFG0X);
	if (uecfg & AT90USB_UECFG0X_EPDIR_IN) {
		uint8_t uesta;

		/* Request stall as soon as the FIFO is empty */
		do {
			uesta = avr_read_reg8(UESTA0X);
		} while (AT90USB_UESTA0X_GET_NBUSYBK(uesta) != 0);
	}


	iflags = cpu_irq_save();
	avr_write_reg8(UECONX, avr_read_reg8(UECONX)
			| AT90USB_UECONX_STALLRQ);
	cpu_irq_restore(iflags);

	if (ep != 0 && !(uecfg & AT90USB_UECFG0X_EPDIR_IN)) {
		/*
		 * Flush the FIFO for OUT endpoints. The caller may be
		 * stalling because it doesn't want any more data, but
		 * the controller may have already received some.
		 */
		at90usb_udc_kill_all_banks(udc, ep);
	}

	return 0;
}

status_t udc_ep_clear_halt(struct udc *udc, usb_ep_id_t ep)
{
	struct at90usb_udc *udc90 = at90usb_udc_of(udc);
	irqflags_t iflags;

	dbg_verbose("at90usb-udc: ep%d: clear halt (%swedged)\n", ep,
			test_bit(AT90USB_EP_WEDGE, &udc90->ep[ep].flags)
			? "" : "not ");

	if (ep >= APP_UDC_NR_ENDPOINTS)
		return -1;

	iflags = cpu_irq_save();

	avr_write_reg8(UENUM, ep);

	/* Always reset data toggle sequence */
	avr_write_reg8(UECONX, avr_read_reg8(UECONX) | AT90USB_UECONX_RSTDT);

	if (!test_bit(AT90USB_EP_WEDGE, &udc90->ep[ep].flags)) {
		/* Clear the STALL request */
		avr_write_reg8(UECONX, avr_read_reg8(UECONX)
				| AT90USB_UECONX_STALLRQC);
	}

	cpu_irq_restore(iflags);

	return 0;
}

bool udc_ep_is_wedged(struct udc *udc, usb_ep_id_t ep)
{
	struct at90usb_udc *udc90 = at90usb_udc_of(udc);

	assert(ep < APP_UDC_NR_ENDPOINTS);

	return test_bit(AT90USB_EP_WEDGE, &udc90->ep[ep].flags);
}

void udc_ep_set_wedge(struct udc *udc, usb_ep_id_t ep)
{
	struct at90usb_udc	*udc90 = at90usb_udc_of(udc);
	irqflags_t		iflags;

	dbg_verbose("at90usb-udc: ep%d: set wedge\n", ep);

	iflags = cpu_irq_save();
	set_bit(AT90USB_EP_WEDGE, &udc90->ep[ep].flags);

	/*
	 * This function isn't called in response to host control
	 * requests, so it's always a bug when ep isn't valid.
	 */
	assert(ep < APP_UDC_NR_ENDPOINTS);
	udc_ep_set_halt(udc, ep);

	cpu_irq_restore(iflags);
}

void udc_ep_clear_wedge(struct udc *udc, usb_ep_id_t ep)
{
	struct at90usb_udc	*udc90 = at90usb_udc_of(udc);
	irqflags_t		iflags;

	dbg_verbose("at90usb-udc: ep%d: clear wedge\n", ep);

	assert(ep < APP_UDC_NR_ENDPOINTS);

	iflags = cpu_irq_save();

	assert(!test_bit(AT90USB_EP_WEDGE, &udc90->ep[ep].flags)
			|| udc_ep_is_halted(udc, ep) > 0);

	clear_bit(AT90USB_EP_WEDGE, &udc90->ep[ep].flags);
	cpu_irq_restore(iflags);
}

/**
 * \internal
 * Configure an endpoint at the hardware level.
 *
 * \param id Endpoint number.
 * \param size Maximum packet size.
 * \param type Endpoint transfer type.
 * \param is_in true if the endpoint is an IN endpoint. Must be false
 *              for control endpoints.
 * \param nr_banks Number of FIFO banks (1 or 2).
 *
 * \retval STATUS_OK The endpoint was configured successfully
 * \retval ERR_INVALID_ARG The endpoint configuration is invalid
 */
static status_t at90usb_udc_configure_ep(uint8_t id, unsigned int size,
		enum usb_ep_xfer_type type, bool is_in, uint8_t nr_banks)
{
	irqflags_t	iflags;
	uint8_t		config;

	/** \todo Only endpoint 1 can be 256 bytes, max 64 for the rest. */
	assert(nr_banks >= 1 && nr_banks <= 2 && size >= 8 && size <= 256);

	iflags = cpu_irq_save();

	avr_write_reg8(UENUM, id);
	avr_write_reg8(UECONX, AT90USB_UECONX_EPEN);

	config = AT90USB_UECFG0X_EPTYPE(type);
	if (is_in)
		config |= AT90USB_UECFG0X_EPDIR_IN;

	avr_write_reg8(UECFG0X, config);

	config = (AT90USB_UECFG1X_ALLOC
			| AT90USB_UECFG1X_EPBK(nr_banks - 1));

	switch(size) {
	case 8:
		config |= AT90USB_UECFG1X_EPSIZE_8;
		break;
	case 16:
		config |= AT90USB_UECFG1X_EPSIZE_16;
		break;
	case 32:
		config |= AT90USB_UECFG1X_EPSIZE_32;
		break;
	case 64:
		config |= AT90USB_UECFG1X_EPSIZE_64;
		break;
	case 128:
		config |= AT90USB_UECFG1X_EPSIZE_128;
		break;
	case 256:
		config |= AT90USB_UECFG1X_EPSIZE_256;
		break;
	}

	avr_write_reg8(UECFG1X, config);
	if (!(avr_read_reg8(UESTA0X) & AT90USB_UESTA0X_CFGOK)) {
		avr_write_reg8(UECONX, 0);
		dbg_error("ep%u: configuration %02x invalid\n", id, config);
		cpu_irq_restore(iflags);
		return ERR_INVALID_ARG;
	}

	avr_write_reg8(UERST, AT90USB_UERST_EPRST(id));
	avr_write_reg8(UERST, 0);

	cpu_irq_restore(iflags);

	return STATUS_OK;
}

/**
 * \internal
 * \brief Flush a non-control endpoint.
 *
 * \param udc90 The AT90USB UDC instance.
 * \param ep An UDC endpoint instance.
 */
static void
at90usb_udc_ep_flush(struct at90usb_udc *udc90, struct at90usb_udc_ep *ep)
{
	struct udc		*udc = &udc90->udc;
	struct buffer		*buf;
	struct buffer		*next_buf;
	struct usb_request	*req;
	irqflags_t		iflags;

	assert(ep->id != 0 && ep->id < APP_UDC_NR_ENDPOINTS);
	assert(test_bit(AT90USB_EP_ENABLED, &ep->flags));

	/*
	 * First, reset the hardware state, but don't disable the
	 * endpoint or reset any data toggles.
	 */
	iflags = cpu_irq_save();

	/* Prevent queueing new requests */
	clear_bit(AT90USB_EP_ENABLED, &ep->flags);
	clear_bit(AT90USB_EP_ACTIVE_XFER, &ep->flags);

	ep->ueienx = 0;
	avr_write_reg8(UENUM, ep->id);
	avr_write_reg8(UEIENX, ep->ueienx);

	/* Flush all data from the FIFO */
	at90usb_udc_kill_all_banks(udc, ep->id);
	cpu_irq_restore(iflags);

	/* Then, terminate all queued requests */
	if (!slist_is_empty(&ep->req_queue)) {
		req = slist_pop_head(&ep->req_queue, struct usb_request, node);
		buf = slist_peek_head(&req->buf_list, struct buffer, node);
		while (slist_node_is_valid(&req->buf_list, &buf->node)) {
			next_buf = slist_peek_next(&buf->node,
					struct buffer, node);
			dbg_verbose("buf %p req [%p %p]\n",
					&buf->node, req->buf_list.first.next,
					req->buf_list.last);
			if (req->buf_list.last == &buf->node) {
				at90usb_udc_req_done(udc, req, ERR_FLUSHED);
				if (!slist_is_empty(&ep->req_queue))
					req = slist_pop_head(&ep->req_queue,
							struct usb_request,
							node);
			}

			buf = next_buf;
		}

		assert(slist_is_empty(&ep->req_queue));
	}
}

void udc_ep_flush(struct udc *udc, usb_ep_id_t ep_id)
{
	struct at90usb_udc	*udc90 = at90usb_udc_of(udc);
	struct at90usb_udc_ep	*ep;
	irqflags_t		iflags;

	dbg_verbose("at90usb-udc: flush ep%u\n", ep_id);

	ep = &udc90->ep[ep_id];
	at90usb_udc_ep_flush(udc90, ep);

	/* Allow queueing new requests */
	iflags = cpu_irq_save();
	set_bit(AT90USB_EP_ENABLED, &ep->flags);
	cpu_irq_restore(iflags);
}

/**
 * \internal
 * \brief Allocate a non-control endpoint.
 *
 * Allocate and initialize the basics variables for a non-control endpoint
 * instance. Also initialize the work queue handler.
 *
 * \param udc The UDC instance.
 * \param addr Endpoint address.
 * \param max_packet_size Endpoint maximum packet size.
 *
 * \return Address of the allocated endpoint instance, NULL on error.
 */
static struct at90usb_udc_ep *
at90usb_ep_alloc(struct udc *udc, uint8_t addr, uint16_t max_packet_size)
{
	struct at90usb_udc	*udc90 = at90usb_udc_of(udc);
	struct at90usb_udc_ep	*ep;
	uint8_t			index;

	index = addr & USB_EP_ADDR_MASK;
	assert(index > 0 && index < APP_UDC_NR_ENDPOINTS);

	ep = &udc90->ep[index];
	if (atomic_test_and_set_bit(AT90USB_EP_ALLOCATED, &ep->flags))
		return NULL;

	/*
	 * No need for atomics since we have marked the endpoint as
	 * allocated but not enabled.
	 */
	if (addr & USB_DIR_IN)
		set_bit(AT90USB_EP_IS_IN, &ep->flags);

	ep->id = index;
	ep->udc90 = udc90;
	ep->ueienx = 0;
	ep->maxpacket = max_packet_size;

	slist_init(&ep->req_queue);

	if (test_bit(AT90USB_EP_IS_IN, &ep->flags))
		workqueue_task_init(&ep->task, at90usb_udc_submit_in_queue);
	else
		workqueue_task_init(&ep->task, at90usb_udc_submit_out_queue);

	return ep;
}

/**
 * \internal
 * \brief Enable a non-control endpoint.
 *
 * Configures and enables a non-control endpoint for transfers.
 *
 * \param ep An AT90USB UDC endpoint instance.
 * \param addr Endpoint address.
 * \param max_packet_size Endpoint maximum packet size.
 * \param type Endpoint transfer type.
 * \param nr_banks Number of endpoint banks.
 *
 * \return An allocated endpoint instance, or NULL on error.
 */
static usb_ep_id_t at90usb_ep_enable(struct at90usb_udc_ep *ep, uint8_t addr,
		uint16_t max_packet_size, enum usb_ep_xfer_type type,
		unsigned int nr_banks)
{
	status_t        ret;
	uint8_t         index;

	index = addr & USB_EP_ADDR_MASK;
	ret = at90usb_udc_configure_ep(index, max_packet_size, type,
			addr & USB_DIR_IN, nr_banks);
	if (ret)
		return ret;

	/*
	 * Make sure everything else has been updated before marking the
	 * endpoint as enabled.
	 */
	barrier();
	set_bit(AT90USB_EP_ENABLED, &ep->flags);

	return index;
}

usb_ep_id_t udc_ep_create_bulk(struct udc *udc, uint8_t addr,
		uint16_t max_packet_size)
{
	struct at90usb_udc_ep	*ep;
	usb_ep_id_t		ret;

	dbg_verbose("at90usb-udc: create BULK ep addr: %02x size: %u\n",
			addr, max_packet_size);

	ep = at90usb_ep_alloc(udc, addr, max_packet_size);
	if (!ep)
		return ERR_BUSY;

	ret = at90usb_ep_enable(ep, addr, max_packet_size,
			USB_EP_XFER_BULK, 2);
	if (ret < 0)
		atomic_clear_bit(AT90USB_EP_ALLOCATED, &ep->flags);

	return ret;
}

usb_ep_id_t udc_ep_create_interrupt(struct udc *udc, uint8_t addr,
		uint16_t max_packet_size)
{
	struct at90usb_udc_ep	*ep;
	status_t		ret;

	dbg_verbose("at90usb-udc: create INTERRUPT ep addr: %02x size: %u\n",
			addr, max_packet_size);

	ep = at90usb_ep_alloc(udc, addr, max_packet_size);
	if (!ep)
		return ERR_BUSY;

	ret = at90usb_ep_enable(ep, addr, max_packet_size,
			USB_EP_XFER_INTERRUPT, 2);
	if (ret < 0)
		atomic_clear_bit(AT90USB_EP_ALLOCATED, &ep->flags);

	return ret;
}

void udc_ep_destroy(struct udc *udc, usb_ep_id_t ep_id)
{
	struct at90usb_udc	*udc90 = at90usb_udc_of(udc);
	struct at90usb_udc_ep	*ep;
	irqflags_t		iflags;

	dbg_verbose("at90usb-udc: destroy ep%u\n", ep_id);

	assert(ep_id != 0 && ep_id < APP_UDC_NR_ENDPOINTS);

	ep = &udc90->ep[ep_id];
	at90usb_udc_ep_flush(udc90, ep);

	iflags = cpu_irq_save();

	avr_write_reg8(UENUM, ep_id);

	/* Now that we've flushed the queue, disable the endpoint */
	avr_write_reg8(UERST, AT90USB_UERST_EPRST(ep_id));
	avr_write_reg8(UERST, 0);

	avr_write_reg8(UECONX, 0);
	avr_write_reg8(UESTA0X, 0);

	/* Allow re-use after all the cleanup has been done */
	ep->flags = 0;
	cpu_irq_restore(iflags);
}

status_t udc_enter_test_mode(struct udc *udc, unsigned int mode)
{
	return -1;
}

/**
 * \internal
 * \brief Helper function to complete control endpoint USB requests.
 *
 * This helper function will set the USB request status flag and call the
 * request done callback if needed for requests on the control endpoint.
 *
 * \param udc The UDC instance.
 * \param ep The control endpoint instance.
 */
static void at90usb_ep0_tx_complete(struct udc *udc, struct at90usb_udc_ep *ep)
{
	struct usb_request	*req;
	struct buffer		*buf;
	unsigned int		buf_offset;
	unsigned int		bytes_written;

	if (slist_is_empty(&ep->req_queue)) {
		assert(slist_is_empty(&ep->buf_queue));
		return;
	}

	assert(!slist_is_empty(&ep->buf_queue));

	req = slist_peek_head(&ep->req_queue, struct usb_request, node);
	buf = slist_peek_head(&ep->buf_queue, struct buffer, node);

	buf_offset = ep->buf_offset;
	bytes_written = ep->bytes_written;

	req->bytes_xfered += bytes_written;

	while (bytes_written && buf_offset + bytes_written >= buf->len) {
		bytes_written -= buf->len - buf_offset;
		buf_offset = 0;
		slist_pop_head_node(&ep->buf_queue);
		if (bytes_written)
			assert(!slist_is_empty(&ep->buf_queue));
		buf = slist_peek_head(&ep->buf_queue, struct buffer, node);
	}

	if (slist_is_empty(&ep->buf_queue)) {
		slist_init(&ep->req_queue);
		/* The queue is now empty */
		slist_give_back_head(&req->buf_list, &ep->buf_queue);
		clear_bit(AT90USB_EP_ACTIVE_XFER, &ep->flags);
		at90usb_udc_req_done(udc, req, 0);
	}

	ep->buf_offset = bytes_written;
	ep->bytes_written = 0;
}

/**
 * \internal
 * \brief Flush a control endpoint.
 *
 * \param udc The UDC instance.
 * \param ep The control endpoint instance.
 */
static void at90usb_ep0_tx_flush(struct udc *udc, struct at90usb_udc_ep *ep)
{
	/* First, complete all buffers that have been submitted */
	at90usb_ep0_tx_complete(udc, ep);

	/* Then, if the request is still not done, terminate it */
	if (!slist_is_empty(&ep->req_queue)) {
		struct usb_request	*req;

		req = slist_pop_head(&ep->req_queue, struct usb_request, node);
		assert(slist_is_empty(&ep->req_queue));
		slist_init(&ep->buf_queue);
		ep->buf_offset = 0;
		clear_bit(AT90USB_EP_ACTIVE_XFER, &ep->flags);
		at90usb_udc_req_done(udc, req, ERR_PROTOCOL);
	}
}

/**
 * \internal
 * \brief The AT90USB UDC endpoint 0 work task handler.
 *
 * \note This function will disable and enable interrupts while processing the
 * queue, but will return with interrupt flag as when called.
 *
 * \param task Work queue task.
 *
 * \pre Called from workqueue, interrupts enabled.
 */
static void at90usb_udc_ep0_worker(struct workqueue_task *task)
{
	struct at90usb_udc_ep	*ep = at90usb_ep_task_of(task);
	struct at90usb_udc	*udc90 = ep->udc90;
	uint8_t			status;
	uint8_t			enabled;
	uint8_t			pending;

	avr_write_reg8(UENUM, 0);

	status  = avr_read_reg8(UEINTX);
	enabled = ep->ueienx;
	pending = enabled & status;

	if (pending & AT90USB_UEINTX_RXOUTI) {
		struct usb_request	*req;
		struct buffer		*buf;
		uint16_t		fifo_len;
		uint16_t		buf_offset;
		uint16_t		fifo_offset = 0;

		cpu_irq_disable();
		if (slist_is_empty(&ep->req_queue)) {
			assert(slist_is_empty(&ep->buf_queue));
			/* Unexpected data -- stall the endpoint */
			cpu_irq_enable();
			goto stall;
		}

		req = slist_peek_head(&ep->req_queue, struct usb_request, node);
		cpu_irq_enable();

		buf_offset = ep->buf_offset;
		fifo_len = (avr_read_reg8(UEBCHX) << 8) | avr_read_reg8(UEBCLX);

		cpu_irq_disable();
		while (!slist_is_empty(&ep->buf_queue)) {
			unsigned int nbytes;

			cpu_irq_disable();
			buf = slist_peek_head(&ep->buf_queue,
					struct buffer, node);
			cpu_irq_enable();

			nbytes = min_u(fifo_len - fifo_offset,
					buf->len - buf_offset);

			copy_from_fifo((uint8_t *)((uintptr_t)buf->addr.phys
						+ buf_offset), nbytes);

			buf_offset += nbytes;
			fifo_offset += nbytes;

			if (buf_offset == buf->len) {
				req->bytes_xfered += buf_offset;
				cpu_irq_disable();
				slist_pop_head_node(&ep->buf_queue);
				cpu_irq_enable();
				buf_offset = 0;
			}
		}

		cpu_irq_disable();
		avr_write_reg8(UEINTX, avr_read_reg8(UEINTX)
				& ~AT90USB_UEINTX_RXOUTI);
		cpu_irq_enable();
		ep->buf_offset = buf_offset;

		cpu_irq_disable();
		if (slist_is_empty(&ep->buf_queue)) {
			/* Request is done */
			assert(buf_offset == 0);
			slist_init(&ep->req_queue);
			ep->ueienx &= ~AT90USB_UEIENX_RXOUTE;
			udc90->ctrl_state = EP0_STATE_SETUP;
			clear_bit(AT90USB_EP_ACTIVE_XFER, &ep->flags);
			cpu_irq_enable();
			at90usb_udc_req_done(&udc90->udc, req, 0);
		}
		cpu_irq_enable();
	}
	if (pending & AT90USB_UEINTX_TXINI) {
		struct usb_request	*req;
		struct buffer		*buf;
		unsigned int		buf_offset;
		unsigned int		fifo;
		unsigned int		fifo_end;

		if (udc90->ctrl_state == EP0_STATE_STATUS_IN) {
			struct usb_setup_req *setup;

			/* Status IN complete */
			setup = &udc90->setup_req;
			if (setup->bRequest == USB_REQ_SET_ADDRESS) {
				uint8_t addr = le16_to_cpu(setup->wValue)
					& AT90USB_UDADDR_UADD_MASK;
				/* Address and enable address must be written
				 * in two stages according to datasheet. */
				avr_write_reg8(UDADDR, addr);
				avr_write_reg8(UDADDR,
						AT90USB_UDADDR_ADDEN | addr);
				udc90->udc.address = addr;
			}
			cpu_irq_disable();
			ep->ueienx &= ~AT90USB_UEIENX_TXINE;
			cpu_irq_enable();
			udc90->ctrl_state = EP0_STATE_SETUP;
			goto out;
		}

		/* Send ZLP to terminate the current transfer if required */
		if (udc90->ctrl_state == EP0_STATE_DATA_ZLP) {
			udc90->ctrl_state = EP0_STATE_DATA_IN;
			cpu_irq_disable();
			avr_write_reg8(UEINTX, avr_read_reg8(UEINTX)
					& ~AT90USB_UEINTX_TXINI);
			cpu_irq_enable();
			goto out;
		}

		/* Terminate any completed buffers */
		at90usb_ep0_tx_complete(&udc90->udc, ep);

		/* Submit any remaining buffers */
		if (slist_is_empty(&ep->buf_queue)) {
			cpu_irq_disable();
			ep->ueienx &= ~AT90USB_UEIENX_TXINE;
			cpu_irq_enable();
			udc90->ctrl_state = EP0_STATE_SETUP;
			goto out;
		}

		buf_offset = ep->buf_offset;
		cpu_irq_disable();
		buf = slist_peek_head(&ep->buf_queue, struct buffer, node);
		cpu_irq_enable();

		fifo = 0;
		fifo_end = APP_UDC_MAXPACKETSIZE0;
		while (fifo < APP_UDC_MAXPACKETSIZE0) {
			unsigned int	nbytes;
			unsigned int	buf_len;

			buf_len = buf->len;

			nbytes = min_u(fifo_end - fifo, buf_len - buf_offset);
			dbg_verbose("fifo %u data %p off %u nbytes %u\n",
					fifo, buf->addr.ptr,
					buf_offset, nbytes);

			copy_to_fifo((uint8_t *)((uintptr_t)buf->addr.phys
						+ buf_offset), nbytes);

			buf_offset += nbytes;
			fifo += nbytes;

			if (buf_offset == buf_len) {
				cpu_irq_disable();
				if (slist_node_is_last(&ep->buf_queue,
							&buf->node))
					break;
				buf = slist_peek_next(&buf->node,
						struct buffer, node);
				cpu_irq_enable();
				buf_offset = 0;
			}
		}

		cpu_irq_disable();
		avr_write_reg8(UEINTX, avr_read_reg8(UEINTX)
					& ~AT90USB_UEINTX_TXINI);
		cpu_irq_enable();

		ep->bytes_written = (unsigned long)fifo;

		cpu_irq_disable();
		req = slist_peek_head(&ep->req_queue, struct usb_request, node);
		if (&buf->node == req->buf_list.last
				&& buf_offset == buf->len
				&& test_bit(USB_REQ_SHORT_PKT, &req->flags)
				&& fifo == fifo_end)
			udc90->ctrl_state = EP0_STATE_DATA_ZLP;
		cpu_irq_enable();
	}
	if (pending & AT90USB_UEINTX_RXSTPI) {
		struct usb_setup_req	*setup;
		uint16_t		byte_count;

		setup = &udc90->setup_req;

		/*
		 * A SETUP packet clears the TX complete interrupt, so
		 * complete all submitted requests now.
		 */
		if (enabled & AT90USB_UEINTX_TXINI) {
			cpu_irq_disable();
			avr_write_reg8(UEINTX, avr_read_reg8(UEINTX)
					& ~AT90USB_UEINTX_TXINI);
			cpu_irq_enable();
			at90usb_ep0_tx_flush(&udc90->udc, ep);
		}

		byte_count = (avr_read_reg8(UEBCHX) << 8) |
			avr_read_reg8(UEBCLX);

		if (byte_count != sizeof(*setup)) {
			/* Invalid SETUP packet length -- stall */
			cpu_irq_disable();
			avr_write_reg8(UEINTX, avr_read_reg8(UEINTX)
					& ~AT90USB_UEINTX_RXSTPI);
			cpu_irq_enable();
			goto stall;
		}

		copy_from_fifo((uint8_t *)setup, sizeof(*setup));

		cpu_irq_disable();
		avr_write_reg8(UEINTX, avr_read_reg8(UEINTX)
				& ~AT90USB_UEINTX_RXSTPI);
		cpu_irq_enable();

		if (udc_lib_process_setup_request(&udc90->udc, setup) < 0)
			goto stall;
	}

out:
	avr_write_reg8(UEIENX, ep->ueienx);
	return;

stall:
	cpu_irq_disable();
	ep->ueienx &= ~(AT90USB_UEIENX_RXOUTE | AT90USB_UEIENX_TXINE);
	cpu_irq_enable();
	avr_write_reg8(UEIENX, ep->ueienx);
	udc_ep_set_halt(&udc90->udc, 0);
	goto out;
}

static struct at90usb_udc the_at90usb_udc;

/**
 * \internal
 * \brief The AT90USB device-mode device work task handler.
 *
 * \pre Called from workqueue, interrupts enabled.
 */
static void at90usb_udc_device_worker(struct workqueue_task *task)
{
	if (avr_read_reg8(UDINT) & AT90USB_UDINT_EORSTI) {
		struct at90usb_udc	*udc90 = at90usb_udc_task_of(task);
		struct at90usb_udc_ep	*ep = &udc90->ep[0];
		struct udc		*udc = &udc90->udc;
		irqflags_t		iflags;

		avr_write_reg8(UDINT, avr_read_reg8(UDINT)
				& ~AT90USB_UDINT_EORSTI);

		/* Reset the device state */
		udc->address = 0;
		clear_bit(UDC_IS_SUSPENDED, &udc->flags);

		/* Figure out what speed we're running at */
#ifdef CONFIG_UDC_FULL_SPEED
		udc->speed = USB_SPEED_FULL;
#else
		udc->speed = USB_SPEED_LOW;
#endif
		dbg_verbose("at90usb-udc: reset speed %u\n", udc->speed);

		at90usb_ep0_tx_flush(udc, &udc90->ep[0]);
		usb_dev_reset(udc);

		/* Set up ep0 for control transfers */
		if (at90usb_udc_configure_ep(0, APP_UDC_MAXPACKETSIZE0,
					USB_EP_XFER_CONTROL, false, 1)) {
			udc->speed = USB_SPEED_UNKNOWN;
			goto out;
		}

		/* Get ready to be enumerated */
		udc90->ctrl_state = EP0_STATE_SETUP;

		iflags = cpu_irq_save();

		avr_write_reg8(UENUM, 0);
		ep->ueienx |= AT90USB_UEIENX_RXSTPE;
		avr_write_reg8(UEIENX, ep->ueienx);

		cpu_irq_restore(iflags);
	}
out:
	avr_write_reg8(UDIEN, AT90USB_UDIEN_EORSTE);
}

/**
 * \internal
 * \brief Helper function to let the UDC maybe attach to the USB bus.
 *
 * \param udc90 The AT90USB UDC instance.
 */
static void at90usb_udc_maybe_attach(struct at90usb_udc *udc90)
{
	struct udc	*udc = &udc90->udc;

	dbg_verbose("at90usb_udc maybe attach: flags=0x%x\n", udc->flags);
	if (at90usb_udc_is_enabled(udc90)
			&& test_bit(UDC_HAS_POWER, &udc->flags)
			&& test_bit(UDC_AUTOATTACH, &udc->flags)) {
		dbg_verbose("at90usb_udc: attaching...\n");
#ifdef CONFIG_AT90USB_ENABLE_UVREG
		avr_write_reg8(UHWCON, avr_read_reg8(UHWCON)
				| AT90USB_UHWCON_UVREGE);
#endif
		avr_write_reg8(UDCON, avr_read_reg8(UDCON)
				& ~AT90USB_UDCON_DETACH);
		avr_write_reg8(UDIEN, AT90USB_UDIEN_EORSTE);
	}
}

/**
 * \internal
 * \brief Helper function to let the UDC detach from the USB bus.
 *
 * \param udc90 The AT90USB UDC instance.
 */
static void at90usb_udc_detach(struct at90usb_udc *udc90)
{
	struct udc	*udc = &udc90->udc;
	uint8_t		udcon;

	dbg_verbose("at90usb_udc detach: flags=0x%x\n", udc->flags);

	udc->speed = USB_SPEED_UNKNOWN;
	udc->address = 0;
	udc->flags &= (1 << UDC_IS_ENABLED) | (1 << UDC_HAS_POWER)
			| (1 << UDC_AUTOATTACH);

	udcon = avr_read_reg8(UDCON);

	if (!(udcon & AT90USB_UDCON_DETACH)) {
		usb_dev_reset(&udc90->udc);

		avr_write_reg8(UDCON, udcon | AT90USB_UDCON_DETACH);
		avr_write_reg8(UDIEN, 0);
	}
}

/**
 * \internal
 * \brief Signal that a high Vbus level has been detected.
 *
 * This function is called by the AT90USB bus interface driver when Vbus
 * power is provided by the host.
 *
 * \param udc90 The AT90USB UDC instance.
 */
void at90usb_udc_vbus_on(struct at90usb_udc *udc90)
{
	struct udc *udc = &udc90->udc;

	if (!test_bit(UDC_HAS_POWER, &udc->flags)) {
		dbg_verbose("at90usb_udc: Vbus ON\n");
		set_bit(UDC_HAS_POWER, &udc->flags);
		at90usb_udc_maybe_attach(udc90);
	}
}

/**
 * \internal
 * \brief Signal that a low Vbus level has been detected.
 *
 * This function is called by the AT90USB bus interface driver when Vbus
 * power is no longer provided by the host.
 *
 * \param udc90 The AT90USB UDC instance.
 */
void at90usb_udc_vbus_off(struct at90usb_udc *udc90)
{
	struct udc *udc = &udc90->udc;

	if (test_bit(UDC_HAS_POWER, &udc->flags)) {
		dbg_verbose("at90usb_udc: Vbus OFF\n");
		clear_bit(UDC_HAS_POWER, &udc->flags);
		at90usb_udc_detach(udc90);
	}
}

void udc_attach(struct udc *udc)
{
	struct at90usb_udc *udc90 = at90usb_udc_of(udc);
	irqflags_t	iflags;

	iflags = cpu_irq_save();
	if (!test_bit(UDC_AUTOATTACH, &udc->flags)) {
		set_bit(UDC_AUTOATTACH, &udc->flags);
		at90usb_udc_maybe_attach(udc90);
	}
	cpu_irq_restore(iflags);
}

void udc_detach(struct udc *udc)
{
	struct at90usb_udc *udc90 = at90usb_udc_of(udc);
	irqflags_t	iflags;

	iflags = cpu_irq_save();
	if (test_bit(UDC_AUTOATTACH, &udc->flags)) {
		clear_bit(UDC_AUTOATTACH, &udc->flags);
		at90usb_udc_detach(udc90);
	}
	cpu_irq_restore(iflags);
}

/**
 * \internal
 * \brief Initialize the device part of the AT90USB controller.
 *
 * This function does any device-side initialization necessary when
 * the AT90USB controller as a whole is being initialized. It does not
 * enable any device-side functionality.
 *
 * \return The AT90USB device controller (UDC) instance.
 */
struct at90usb_udc *at90usb_udc_init(void)
{
	struct at90usb_udc *udc90 = &the_at90usb_udc;

	slist_init(&udc90->ep[0].req_queue);
	slist_init(&udc90->ep[0].buf_queue);

	udc90->udc.flags = 0;

	workqueue_task_init(&udc90->task, at90usb_udc_device_worker);

	udc90->ep[0].id = 0;
	udc90->ep[0].udc90 = udc90;

	workqueue_task_init(&udc90->ep[0].task, at90usb_udc_ep0_worker);

#ifdef CONFIG_UDC_LOW_SPEED
	avr_write_reg8(UDCON, avr_read_reg8(UDCON) | AT90USB_UDCON_LSM);
#else
	avr_write_reg8(UDCON, avr_read_reg8(UDCON) & ~AT90USB_UDCON_LSM);
#endif

	return udc90;
}

/**
 * \internal
 * \brief Shut down the device part of the AT90USB controller.
 *
 * This function does any device-side cleanups necessary when the AT90USB
 * controller as a whole is being shut down.
 *
 * \param udc90 The AT90USB UDC instance.
 */
void at90usb_udc_shutdown(struct at90usb_udc *udc90)
{
	udc90->udc.flags = 0;
}

//! @}

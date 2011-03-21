/**
 * \file
 *
 * \brief USB Device Controller library functions
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
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef USB_UDC_LIB_H_INCLUDED
#define USB_UDC_LIB_H_INCLUDED

#include <slist.h>
#include <string.h>
#include <types.h>
#include <usb/request.h>

struct udc;
struct usb_setup_req;

/**
 * \addtogroup udc_group
 * @{
 */

/**
 * \name UDC Library Interface
 *
 * The following functions make up the interface between the UDC driver
 * and the optional UDC library. The UDC library contains common helper
 * functions for UDC drivers.
 */
//@{

/**
 * \brief Enter Test Mode
 *
 * This function is implemented by the UDC driver and called by the UDC
 * library to request the device to enter test mode after the status IN
 * stage is complete.
 *
 * \param udc The USB Device Controller instance
 * \param mode The index of the test mode to be entered
 *
 * \retval STATUS_OK The specified test mode will be entered
 * \retval negative The device does not support the specified test mode
 */
extern status_t udc_enter_test_mode(struct udc *udc, unsigned int mode);

/**
 * \fn void udc_set_address(struct udc *udc, unsigned int addr)
 * \brief Signal that the UDC is to change its USB address after the
 *	status IN phase is complete
 *
 * This function is implemented by the UDC driver and called by the UDC
 * library when a Set Address request is received from the host.
 *
 * \param udc The USB Device Controller instance
 * \param addr The new USB address to be used starting from the next
 *	control transaction
 */

extern status_t udc_lib_process_setup_request(struct udc *udc,
		struct usb_setup_req *req);

//@}

/**
 * \name FIFO Access Helpers
 *
 * The following functions may be used by the UDC driver to copy data
 * into and out of a FIFO aperture. It is assumed that the FIFO aperture
 * has a fixed size equal to the max packet size of the endpoint, and
 * that the bytes within it must be accessed sequentially from offset 0
 * up to the number of bytes available, i.e. no random access is
 * possible.
 */
//@{

/**
 * \brief Internal state variables used by the FIFO accessors
 */
struct udc_fifo {
	size_t		buf_offset;     //!< Offset into the head buffer
	size_t		size;           //!< Size of the FIFO aperture
	/**
	 * \brief True if a Zero-Length Packet is to be sent by the UDC
	 * driver at the next opportunity.
	 *
	 * \todo This should be a request flag instead.
	 */
	bool		send_zlp;
};

/**
 * \brief Initialize the internal state of the FIFO
 *
 * \param fifo The FIFO to be initialized
 * \param aperture_size The size of the FIFO aperture in bytes
 */
static inline void udc_fifo_init(struct udc_fifo *fifo, size_t aperture_size)
{
	fifo->buf_offset = 0;
	fifo->size = aperture_size;
	fifo->send_zlp = false;
}

/**
 * \brief Return true if the UDC driver is to send a Zero-Length Packet
 * once the controller is ready to send a new packet
 *
 * \param fifo Internal state for use by the FIFO accessors
 * \param req The USB request currently at the head of the queue
 *
 * \retval true The UDC driver must send a ZLP after the currently
 * pending packets have been sent.
 * \retval false The UDC driver must not send a ZLP at this point.
 */
static inline bool udc_fifo_zlp_is_pending(struct udc_fifo *fifo,
		struct usb_request *req)
{
	return fifo->send_zlp;
}

/**
 * \brief Notify the FIFO accessors that a ZLP has been queued, so it is
 * not pending anymore.
 *
 * \param fifo Internal state for use by the FIFO accessors
 * \param req The USB request currently at the head of the queue
 */
static inline void udc_fifo_zlp_clear_pending(struct udc_fifo *fifo,
		struct usb_request *req)
{
	fifo->send_zlp = false;
}

/**
 * \brief Copy data from one or more buffers into the FIFO
 *
 * This function will consume one or more buffers from \a req and copy
 * the data contained within them into the FIFO aperture at \a dest. If
 * the buffer contains more data than can fit into the FIFO, the current
 * offset will be recorded and the next call to this function will
 * resume where it left off.
 *
 * It is the caller's responsibility to ensure that the FIFO bank at \a
 * dest is available and has room for a whole USB packet.
 *
 * \param dest The virtual address of the FIFO aperture
 * \param fifo Internal state for use by the FIFO accessors
 * \param done_list Buffers which have been fully consumed are placed on
 * this list
 * \param req The USB request from which to consume buffers
 *
 * \return The number of bytes written to the FIFO, or a negative error
 * code on failure.
 */
static inline int udc_fifo_write_data(void *dest, struct udc_fifo *fifo,
		struct slist *done_list, struct usb_request *req)
{
	struct buffer	*buf;
	unsigned int	bytes_written;
	unsigned int	buf_offset;
	unsigned int	fifo_size;

	buf = buf_list_pop_head(&req->buf_list);
	buf_offset = fifo->buf_offset;
	fifo_size = fifo->size;

	for (bytes_written = 0; bytes_written < fifo_size; ) {
		unsigned int	nbytes;
		unsigned int	buf_len;

		buf_len = buf->len;
		nbytes = min_u(fifo_size - bytes_written, buf_len - buf_offset);
		dbg_verbose("udc write: dst %p src %p off %u nbytes %u\n",
				(char *)dest + bytes_written,
				buf->addr.ptr, buf_offset, nbytes);
		memcpy((char *)dest + bytes_written,
				(const char *)buf->addr.ptr + buf_offset,
				nbytes);

		buf_offset += nbytes;
		bytes_written += nbytes;

		if (buf_offset == buf_len) {
			buf_offset = 0;
			slist_insert_tail(done_list, &buf->node);
			if (slist_is_empty(&req->buf_list)) {
				if (bytes_written == fifo_size
						&& test_bit(USB_REQ_SHORT_PKT,
							&req->flags))
					fifo->send_zlp = true;
				goto done;
			}
			buf = buf_list_pop_head(&req->buf_list);
		}
	}

	/* Put the last unfinished buffer back on the queue */
	slist_insert_head(&req->buf_list, &buf->node);

done:
	fifo->buf_offset = buf_offset;

	return bytes_written;
}

/**
 * \brief Copy data from the FIFO into one or more buffers
 *
 * This function will consume one or more buffers from \a req and copy
 * data from the FIFO into the memory areas they represent. If the
 * buffer has room for more data than the FIFO can provide, the current
 * offset will be recorded and the next call to this function will
 * resume where it left off.
 *
 * It is the caller's responsibility to ensure that the FIFO bank at \a
 * src contains valid data.
 *
 * \param src The virtual address of the FIFO aperture
 * \param fifo Internal state for use by the FIFO accessors
 * \param fifo_nbytes The number of valid bytes in the FIFO, as reported
 * by the UDC hardware
 * \param done_bufs Buffers which have been fully consumed are placed on
 * this list
 * \param req The USB request from which to consume buffers
 *
 * \return The number of bytes read from the FIFO, or a negative error
 * code on failure.
 */
static inline unsigned int udc_fifo_read_data(const void *src,
		struct udc_fifo *fifo, unsigned int fifo_nbytes,
		struct slist *done_bufs, struct usb_request *req)
{
	struct buffer	*buf;
	unsigned int	bytes_read;
	unsigned int	buf_offset;

	assert(fifo_nbytes <= fifo->size);

	buf = buf_list_pop_head(&req->buf_list);
	buf_offset = fifo->buf_offset;

	for (bytes_read = 0; bytes_read < fifo_nbytes; ) {
		unsigned int	nbytes;
		unsigned int	buf_len;

		buf_len = buf->len;
		nbytes = min_u(fifo_nbytes - bytes_read, buf_len - buf_offset);
		dbg_verbose("udc read: dst %p src %p off %u nbytes %u\n",
				buf->addr.ptr, (const char *)src + bytes_read,
				buf_offset, nbytes);
		memcpy((char *)buf->addr.ptr + buf_offset,
				(const char *)src + bytes_read, nbytes);

		buf_offset += nbytes;
		bytes_read += nbytes;

		if (buf_offset == buf_len) {
			buf_offset = 0;
			slist_insert_tail(done_bufs, &buf->node);
			if (slist_is_empty(&req->buf_list))
				goto done;
			buf = buf_list_pop_head(&req->buf_list);
		}
	}

	/* Put the last unfinished buffer back on the queue */
	slist_insert_head(&req->buf_list, &buf->node);

done:
	fifo->buf_offset = buf_offset;

	return bytes_read;
}

//@}
//! @}

#endif /* USB_UDC_LIB_H_INCLUDED */

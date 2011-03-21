/**
 * \file
 *
 * \brief USB Mass Storage Class Bulk-only device interface driver
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
#include <atomic.h>
#include <byteorder.h>
#include <dmapool.h>
#include <interrupt.h>
#include <physmem.h>
#include <status_codes.h>
#include <string.h>
#include <types.h>
#include <util.h>
#include <block/device.h>
#include <scsi/cdb.h>
#include <scsi/spc_protocol.h>
#include <scsi/sbc_protocol.h>
#include <usb/dev_mux.h>
#include <usb/udi_msc_bulk.h>
#include <usb/msc_protocol.h>
#include <usb/request.h>
#include <usb/usb_protocol.h>
#include <usb/udc.h>
#include <app/usb.h>

/**
 * \weakgroup udi_msc_bulk_group
 * @{
 */

#ifndef APP_UDI_MSC_FS_BULK_EP_SIZE
# define APP_UDI_MSC_FS_BULK_EP_SIZE	64
#endif

#define MSC_DATA_BUFFER_SIZE	CONFIG_DMAPOOL_LARGE_OBJ_SIZE
#define MSC_MAX_NR_BUFFERS	(CONFIG_DMAPOOL_NR_LARGE_OBJS / 2)
#define MSC_MAX_DATA_LEN	(MSC_MAX_NR_BUFFERS * MSC_DATA_BUFFER_SIZE)

/* Convert endpoint indexes to endpoint addresses */
#define MSC_BULK_IN_EP_ADDR		(APP_UDI_MSC_BULK_IN_EP | USB_DIR_IN)
#define MSC_BULK_OUT_EP_ADDR		(APP_UDI_MSC_BULK_OUT_EP | USB_DIR_OUT)

/// Maximum number of pending block buffer segments
#define MSC_MAX_NR_SEGS	(2)

/* The serial number may be at most 28 characters */
#define MSC_VPD_SERIAL_BUF_SIZE	(MSC_MAX_SERIAL_LEN + SCSI_VPD_HEADER_SIZE)

/*
 * Size of the Device Identification VPD:
 *  - 4 bytes for the header
 *  - 4 bytes for the ID descriptor header
 *  - 8 bytes for the T10 vendor ID
 *  - 16 bytes for the product ID
 *  - MSC_MAX_SERIAL_LEN bytes for the serial number
 */
#define MSC_VPD_ID_BUF_SIZE	(SCSI_VPD_HEADER_SIZE			\
					+ SCSI_VPD_ID_HEADER_SIZE	\
					+ 8 + 16			\
					+ MSC_MAX_SERIAL_LEN)

/**
 * \internal
 * \brief A Mass Storage Class interface instance
 */
struct msc_interface {
	//! Interface object registered with the UDM layer
	struct udm_interface	iface;
	//! The low-level USB Device Controller
	struct udc		*udc;
	//! The block device used for storage
	struct block_device	*bdev;
	//! Bulk-IN endpoint used for data transfer
	usb_ep_id_t		bulk_in_ep;
	//! Bulk-OUT endpoint used for data transfer
	usb_ep_id_t		bulk_out_ep;
	//! Buffer holding the current sense information
	dma_addr_t		sense_data;
	//! Number of blocks queued for transfer via bdev
	uint32_t		blocks_queued;
	//! The total number of blocks to be transfered
	uint32_t		blocks_total;
	//! The first Logical Block Address to be transfered
	uint32_t		first_lba;
	//! Number of pending block buffer blocks
	atomic_t		blk_blocks_pending;
	//! Number of pending USB data requests
	atomic_t		usb_reqs_pending;
	//! The Command Block Wrapper
	dma_addr_t		cbw;
	//! The Command Status Wrapper
	dma_addr_t		csw;
	//! USB request used for transfering the CBW and CSW
	struct usb_request	*cbw_csw_req;
	//! Block request used for doing block data transfers via bdev
	struct block_request	*block_req;
	//! Additional Sense Code (Qualifier) set by usb_msc_set_busy()
	uint16_t		busy_asc;
	//! Function to be called when the MSC interface has been blocked
	void			(*busy_cb)(void *);
	//! Arbitrary data to be passed to busy_cb()
	void			*busy_cb_data;
	//! True while queuing new buffers for a request
	bool			queue_locked;
	//! True if all block data transfer requests are to be blocked
	bool			not_ready;
	//! True if there's currently a block data transfer in progress
	bool			xfer_in_progress;
};

static inline struct msc_interface *msc_interface_of(
		struct udm_interface *iface)
{
	return container_of(iface, struct msc_interface, iface);
}

union msc_mode_param_header {
	struct scsi_mode_param_header6	h6;
	struct scsi_mode_param_header10	h10;
};
#define MSC_MODE_PARAM_HDR_BUF_LEN			\
	(sizeof(union msc_mode_param_header)		\
		+ sizeof(struct sbc_slba_block_desc))

COMPILER_PACK_SET(1);
struct msc_iface_desc {
	struct usb_interface_descriptor	iface;
	struct usb_endpoint_descriptor	ep[2];
};
COMPILER_PACK_RESET();

static const struct msc_iface_desc msc_desc_template = {
	.iface.bLength		= sizeof(struct usb_interface_descriptor),
	.iface.bDescriptorType		= USB_DT_INTERFACE,
	.iface.bInterfaceNumber		= APP_UDI_MSC_INTERFACE_ID,
	.iface.bAlternateSetting	= 0,
	.iface.bNumEndpoints		= 2,
	.iface.bInterfaceClass		= USB_CLASS_MSC,
	/* Use INQUIRY to determine command set */
	.iface.bInterfaceSubClass	= USB_MSC_SUBCLASS_TRANSPARENT,
	/* BBB is the only protocol recommended for new designs */
	.iface.bInterfaceProtocol	= USB_MSC_PROTOCOL_BULK,
	.ep[0].bLength		= sizeof(struct usb_endpoint_descriptor),
	.ep[0].bDescriptorType		= USB_DT_ENDPOINT,
	.ep[0].bEndpointAddress		= MSC_BULK_IN_EP_ADDR,
	.ep[0].bmAttributes		= USB_EP_XFER_BULK,
	.ep[0].wMaxPacketSize		= LE16(APP_UDI_MSC_FS_BULK_EP_SIZE),
	.ep[1].bLength		= sizeof(struct usb_endpoint_descriptor),
	.ep[1].bDescriptorType		= USB_DT_ENDPOINT,
	.ep[1].bEndpointAddress		= MSC_BULK_OUT_EP_ADDR,
	.ep[1].bmAttributes		= USB_EP_XFER_BULK,
	.ep[1].wMaxPacketSize		= LE16(APP_UDI_MSC_FS_BULK_EP_SIZE),
};

static const struct scsi_inquiry_data msc_inquiry_data = {
	.pq_pdt		= SCSI_INQ_PQ_CONNECTED | SCSI_INQ_DT_DIR_ACCESS,
#ifdef CONFIG_UDI_MSC_REMOVABLE
	.flags1		= SCSI_INQ_RMB,
#else
	.flags1		= 0,
#endif
	.version	= SCSI_INQ_VER_SPC3,
	.flags3		= SCSI_INQ_HISUP | SCSI_INQ_RSP_SPC2,
	.addl_len	= SCSI_INQ_ADDL_LEN(sizeof(msc_inquiry_data)),
	.vendor_id	= { APP_UDI_MSC_INQ_VENDOR_ID },
	.product_id	= { APP_UDI_MSC_INQ_PRODUCT_ID },
	.product_rev	= { APP_UDI_MSC_INQ_PRODUCT_VERSION },
};

static struct usb_msc_cbw *msc_get_cbw(struct msc_interface *msc)
{
	return msc->cbw.ptr;
}

static struct usb_msc_csw *msc_get_csw(struct msc_interface *msc)
{
	return msc->csw.ptr;
}

static void msc_free_dma_buf_list(struct slist *buf_list)
{
	struct buffer	*buf;

	while (!slist_is_empty(buf_list)) {
		buf = slist_pop_head(buf_list, struct buffer, node);
		buffer_dma_free(buf, MSC_DATA_BUFFER_SIZE);
	}
}

/**
 * \internal
 * \brief Current transfer is done; run any registered busy callback
 * functions and clear it.
 */
static void msc_queue_empty(struct msc_interface *msc)
{
	irqflags_t      iflags;

	iflags = cpu_irq_save();

	dbg_verbose("msc: queue empty, not_ready=%d busy_cb=%p\n",
			msc->not_ready, msc->busy_cb);

	msc->xfer_in_progress = false;
	if (msc->not_ready && msc->busy_cb) {
		void (*busy_cb)(void *) = msc->busy_cb;
		void *busy_data = msc->busy_cb_data;

		msc->busy_cb = NULL;
		cpu_irq_restore(iflags);
		busy_cb(busy_data);
	} else {
		cpu_irq_restore(iflags);
	}
}

static void msc_init_sense(struct msc_interface *msc, uint8_t sense_key,
		uint16_t add_sense, uint32_t lba)
{
	uint8_t		*data = msc->sense_data.ptr;

	memset(data, 0, 18);
	data[0] = SCSI_SENSE_VALID | SCSI_SENSE_CURRENT;
	data[2] = sense_key;
	data[3] = lba >> 24;
	data[4] = lba >> 16;
	data[5] = lba >> 8;
	data[6] = lba;
	data[7] = SCSI_SENSE_ADDL_LEN(18);
	data[12] = add_sense >> 8;
	data[13] = add_sense;
}

static unsigned int msc_sense_len(struct msc_interface *msc)
{
	const uint8_t	*data = msc->sense_data.ptr;
	return data[7] + 8;
}

static void msc_cbw_received(struct udc *udc, struct usb_request *req);
static void msc_csw_sent(struct udc *udc, struct usb_request *req);

static void msc_prepare_csw(struct msc_interface *msc, uint32_t residue,
		uint8_t status)
{
	struct usb_msc_csw	*csw;
	struct usb_msc_cbw	*cbw;
	struct usb_request	*req;
	struct buffer		*buf;

	csw = msc->csw.ptr;
	cbw = msc->cbw.ptr;
	csw->dCSWTag = cbw->dCBWTag;
	csw->dCSWDataResidue = cpu_to_le32(residue);
	csw->bCSWStatus = status;

	req = msc->cbw_csw_req;
	req->bytes_xfered = 0;
	req->req_done = msc_csw_sent;
	buf = usb_req_get_first_buffer(req);
	buffer_init_tx(buf, csw, sizeof(struct usb_msc_csw));

	dbg_verbose("msc: CSW t%08lx r%lu s%u\n", le32_to_cpu(csw->dCSWTag),
			residue, status);
}

static void msc_request_data_done(struct udc *udc, struct msc_interface *msc)
{
	struct usb_msc_csw	*csw;

	csw = msc_get_csw(msc);
	if (csw->dCSWDataResidue != LE32(0) && msc->bulk_in_ep) {
		struct usb_msc_cbw	*cbw = msc_get_cbw(msc);

		if (cbw->bmCBWFlags & USB_CBW_DIRECTION_IN)
			udc_ep_set_halt(udc, msc->bulk_in_ep);
		else
			udc_ep_set_halt(udc, msc->bulk_out_ep);
		udc_ep_submit_in_req(udc, msc->bulk_in_ep, msc->cbw_csw_req);
	}

	dbg_verbose("msc data done: t%08lx r%lu s%u %s\n",
			le32_to_cpu(csw->dCSWTag),
			le32_to_cpu(csw->dCSWDataResidue),
			csw->bCSWStatus,
			(msc_get_cbw(msc)->bmCBWFlags & USB_CBW_DIRECTION_IN)
			? "IN" : "OUT");
}

static void msc_request_done(struct udc *udc, struct msc_interface *msc,
		uint32_t residue)
{
	/*
	 * If we're transferring exactly the amount of data that the
	 * host requested, queue the CSW immediately. Otherwise, we
	 * need to stall after we've sent all the data we have, and
	 * send the CSW after that.
	 */
	if (!residue)
		udc_ep_submit_in_req(udc, msc->bulk_in_ep, msc->cbw_csw_req);

	dbg_verbose("msc req done\n");
}

static void msc_request_done_nodata(struct udc *udc, struct msc_interface *msc,
		uint32_t residue)
{
	if (residue) {
		struct usb_msc_cbw *cbw = msc_get_cbw(msc);
		if (cbw->bmCBWFlags & USB_CBW_DIRECTION_IN)
			udc_ep_set_halt(udc, msc->bulk_in_ep);
		else
			udc_ep_set_halt(udc, msc->bulk_out_ep);
	}

	udc_ep_submit_in_req(udc, msc->bulk_in_ep, msc->cbw_csw_req);
}

static void msc_request_failed(struct msc_interface *msc, uint32_t residue,
		uint8_t csw_status, uint8_t sense_key, uint16_t add_sense)
{
	dbg_warning("msc: req %02x failed: SK %02x ASC(Q) %04x\n",
			scsi_cdb_get_opcode(msc_get_cbw(msc)->CDB),
			sense_key, add_sense);

	msc_init_sense(msc, sense_key, add_sense, 0);
	msc_prepare_csw(msc, residue, csw_status);
	msc_request_done_nodata(msc->udc, msc, residue);
}

static void msc_phase_error(struct msc_interface *msc)
{
	struct usb_msc_cbw	*cbw = msc_get_cbw(msc);
	uint32_t		cbw_xfer_len;

	cbw_xfer_len = le32_to_cpu(cbw->dCBWDataTransferLength);

	dbg_error("msc: Phase Error (opcode %02x)\n",
			scsi_cdb_get_opcode(cbw->CDB));
	dbg_verbose("msc:   CBW bmCBWFlags = 0x%02x\n", cbw->bmCBWFlags);
	dbg_verbose("msc:   CBW dCBWDataTransferLength = 0x%lx\n", cbw_xfer_len);

	msc_prepare_csw(msc, cbw_xfer_len, USB_CSW_STATUS_PE);
	msc_request_done_nodata(msc->udc, msc, cbw_xfer_len);
}

static void msc_out_of_memory(struct msc_interface *msc)
{
	dbg_error("msc: Out of memory!\n");
	msc_phase_error(msc);
}

static void msc_csw_sent(struct udc *udc, struct usb_request *req)
{
	struct msc_interface	*msc = req->context;
	struct buffer		*buf;

	buf = usb_req_get_first_buffer(req);
	assert(buf->addr.ptr == msc->csw.ptr);

	dbg_verbose("msc: CSW sent, status=%d\n", req->status);

	msc_queue_empty(msc);

	/* Don't submit any buffers when the endpoint is unavailable */
	if (req->status)
		return;

	/* Re-use the USB request and buffer for the CBW */
	buffer_init_rx_mapped(buf, msc->cbw, sizeof(struct usb_msc_cbw));
	req->bytes_xfered = 0;
	req->req_done = msc_cbw_received;

	udc_ep_submit_out_req(udc, msc->bulk_out_ep, req);
}

static void msc_data_sent(struct udc *udc, struct usb_request *req)
{
	struct msc_interface	*msc = req->context;
	enum status_code	status = req->status;

	usb_req_free(req);

	if (!status)
		msc_request_data_done(udc, msc);
}

static void msc_buf_sent(struct udc *udc, struct usb_request *req)
{
	struct buffer		*buf;

	buf = usb_req_get_first_buffer(req);
	buffer_free(buf);
	msc_data_sent(udc, req);
}

static void msc_capacity_sent(struct udc *udc, struct usb_request *req)
{
	struct buffer		*buf;

	buf = usb_req_get_first_buffer(req);
	buffer_dma_free(buf, sizeof(struct sbc_read_capacity10_data));
	msc_data_sent(udc, req);
}

static void msc_vpd_serial_sent(struct udc *udc, struct usb_request *req)
{
	struct buffer		*buf;

	buf = usb_req_get_first_buffer(req);
	buffer_dma_free(buf, MSC_VPD_SERIAL_BUF_SIZE);
	msc_data_sent(udc, req);
}

static void msc_vpd_id_sent(struct udc *udc, struct usb_request *req)
{
	struct buffer		*buf;

	buf = usb_req_get_first_buffer(req);
	buffer_dma_free(buf, MSC_VPD_ID_BUF_SIZE);
	msc_data_sent(udc, req);
}

static void msc_sense_data_sent(struct udc *udc, struct usb_request *req)
{
	struct msc_interface	*msc = req->context;
	struct buffer		*buf;
	enum status_code	status = req->status;

	dbg_verbose("msc sense data sent: %zu bytes, status %d\n",
			req->bytes_xfered, req->status);

	msc_init_sense(msc, SCSI_SK_NO_SENSE,
			SCSI_ASC_NO_ADDITIONAL_SENSE_INFO, 0);
	buf = usb_req_get_first_buffer(req);
	buffer_free(buf);
	usb_req_free(req);

	if (!status)
		msc_request_data_done(udc, msc);
}

static void msc_mode_params_sent(struct udc *udc, struct usb_request *req)
{
	struct buffer	*buf;

	buf = usb_req_get_first_buffer(req);
	buffer_dma_free(buf, MSC_MODE_PARAM_HDR_BUF_LEN);
	msc_data_sent(udc, req);
}

static long msc_validate_req(struct msc_interface *msc, struct usb_msc_cbw *cbw,
		uint32_t alloc_len, uint8_t dir_flag)
{
	uint32_t	cbw_len;

	cbw_len = le32_to_cpu(cbw->dCBWDataTransferLength);

	/*
	 * The following cases should result in a phase error:
	 *  - Case  2: Hn < Di
	 *  - Case  3: Hn < Do
	 *  - Case  7: Hi < Di
	 *  - Case  8: Hi <> Do
	 *  - Case 10: Ho <> Di
	 *  - Case 13: Ho < Do
	 */
	if ((cbw->bmCBWFlags ^ dir_flag) & USB_CBW_DIRECTION_IN
			|| cbw_len < alloc_len) {
		msc_phase_error(msc);
		return -1;
	}

	/*
	 * The following cases should result in a stall and nonzero
	 * residue:
	 *  - Case  4: Hi > Dn
	 *  - Case  5: Hi > Di
	 *  - Case  9: Ho > Dn
	 *  - Case 11: Ho > Do
	 */
	return cbw_len - alloc_len;
}

static void msc_test_unit_ready(struct msc_interface *msc, struct udc *udc,
		uint32_t cbw_data_len)
{
	dbg_verbose("msc TEST UNIT READY len %lu\n", cbw_data_len);

	if (msc->not_ready) {
		msc_request_failed(msc, cbw_data_len, USB_CSW_STATUS_FAIL,
				SCSI_SK_NOT_READY,
				msc->busy_asc);
	} else if (test_bit(BDEV_PRESENT, &msc->bdev->flags)) {
		msc_prepare_csw(msc, cbw_data_len, USB_CSW_STATUS_PASS);
		msc_request_done_nodata(udc, msc, cbw_data_len);
	} else {
		msc_request_failed(msc, cbw_data_len, USB_CSW_STATUS_FAIL,
				SCSI_SK_NOT_READY,
				SCSI_ASC_MEDIUM_NOT_PRESENT);
	}
}

static void msc_request_sense(struct msc_interface *msc, struct udc *udc,
		struct usb_msc_cbw *cbw)
{
	struct usb_request	*req;
	struct buffer		*buf;
	const uint8_t		*cdb = cbw->CDB;
	long			residue;
	uint8_t			alloc_len;

	alloc_len = scsi_cdb6_get_alloc_len(cdb);

	dbg_verbose("msc: REQUEST SENSE len %u (sense len %u)\n",
			alloc_len, msc_sense_len(msc));

	residue = msc_validate_req(msc, cbw, alloc_len, USB_CBW_DIRECTION_IN);
	if (residue < 0)
		return;

	if (alloc_len > 0) {
		/*
		 * Just send the sense buffer. It should be
		 * initialized by the last command that failed; after
		 * it has been successfully sent, it gets reset to
		 * NO_SENSE.
		 */
		req = usb_req_alloc();
		if (!req)
			goto err_req_alloc;
		buf = buffer_alloc();
		if (!buf)
			goto err_buf_alloc;

		buffer_init_tx_mapped(buf, msc->sense_data,
				min_u(alloc_len, msc_sense_len(msc)));
		usb_req_add_buffer(req, buf);
		req->context = msc;
		req->req_done = msc_sense_data_sent;

		residue += alloc_len - buf->len;
		msc_prepare_csw(msc, residue, USB_CSW_STATUS_PASS);
		udc_ep_submit_in_req(udc, msc->bulk_in_ep, req);

		msc_request_done(udc, msc, residue);
	} else {
		/*
		 * REQUEST SENSE always resets the sense data, even if
		 * no data was actually transfered.
		 */
		msc_init_sense(msc, SCSI_SK_NO_SENSE,
				SCSI_ASC_NO_ADDITIONAL_SENSE_INFO, 0);
		msc_prepare_csw(msc, residue, USB_CSW_STATUS_PASS);
		msc_request_done_nodata(udc, msc, residue);
	}

	return;

err_buf_alloc:
	usb_req_free(req);
err_req_alloc:
	msc_out_of_memory(msc);
}

static int msc_vpd_supported_pages(struct msc_interface *msc,
		struct usb_request *req, size_t alloc_len)
{
	static const uint8_t vpd_page[] = {
		SCSI_INQ_PQ_CONNECTED | SCSI_INQ_DT_DIR_ACCESS,
		SCSI_VPD_SUPPORTED_PAGES,
		0x00,
		3,
		SCSI_VPD_SUPPORTED_PAGES,
		SCSI_VPD_UNIT_SERIAL_NUMBER,
		SCSI_VPD_DEVICE_IDENTIFICATION,
	};
	struct buffer	*buf;
	size_t		total_len = 0;

	if (alloc_len) {
		buf = buffer_alloc();
		if (!buf)
			return -1;

		total_len = min_u(alloc_len, sizeof(vpd_page));
		buffer_init_tx(buf, &vpd_page, total_len);
		usb_req_add_buffer(req, buf);
		req->req_done = msc_buf_sent;
	}

	return total_len;
}

static int msc_vpd_serial_number(struct msc_interface *msc,
		struct usb_request *req, size_t alloc_len)
{
	struct buffer	*buf;
	uint8_t		*vpd_page;
	const char	*serial;
	size_t		serial_len;
	size_t		total_len = 0;

	if (alloc_len) {
		serial = app_get_serial_number();
		serial_len = strlen(serial);

		/*
		 * The application must verify that the serial number
		 * is valid by calling msc_serial_number_is_valid() at
		 * startup. An invalid serial number is considered a
		 * serious configuration error.
		 */
		assert(serial_len <= MSC_MAX_SERIAL_LEN);

		buf = buffer_dma_alloc(MSC_VPD_SERIAL_BUF_SIZE);
		if (!buf)
			return -1;

		vpd_page = buf->addr.ptr;
		vpd_page[0] = SCSI_INQ_PQ_CONNECTED | SCSI_INQ_DT_DIR_ACCESS;
		vpd_page[1] = SCSI_VPD_UNIT_SERIAL_NUMBER;
		vpd_page[2] = 0;
		vpd_page[3] = MSC_MAX_SERIAL_LEN;
		memset(vpd_page + SCSI_VPD_HEADER_SIZE,
				' ', MSC_MAX_SERIAL_LEN - serial_len);
		memcpy(vpd_page + MSC_VPD_SERIAL_BUF_SIZE - serial_len,
				serial, serial_len);

		total_len = MSC_VPD_SERIAL_BUF_SIZE;
		if (alloc_len < total_len) {
			total_len = alloc_len;
			buffer_resize(buf, total_len);
		}

		usb_req_add_buffer(req, buf);
		req->req_done = msc_vpd_serial_sent;
	}

	return total_len;
}

static int msc_vpd_device_id(struct msc_interface *msc,
		struct usb_request *req, size_t alloc_len)
{
	struct buffer	*buf;
	const char	*serial;
	size_t		serial_len;
	uint8_t		*vpd_page;
	uint8_t		*p;
	size_t		total_len = 0;

	if (alloc_len) {
		serial = app_get_serial_number();
		serial_len = strlen(serial);

		/*
		 * The application must verify that the serial number
		 * is valid by calling msc_serial_number_is_valid() at
		 * startup. An invalid serial number is considered a
		 * serious configuration error.
		 */
		assert(serial_len <= MSC_MAX_SERIAL_LEN);

		buf = buffer_dma_alloc(MSC_VPD_ID_BUF_SIZE);
		if (!buf)
			return -1;

		vpd_page = buf->addr.ptr;
		vpd_page[0] = SCSI_INQ_PQ_CONNECTED | SCSI_INQ_DT_DIR_ACCESS;
		vpd_page[1] = SCSI_VPD_DEVICE_IDENTIFICATION;
		vpd_page[2] = 0;
		vpd_page[3] = MSC_VPD_ID_BUF_SIZE - SCSI_VPD_HEADER_SIZE;
		vpd_page[4] = SCSI_VPD_CODE_SET_ASCII;
		vpd_page[5] = SCSI_VPD_ID_TYPE_T10;
		vpd_page[6] = 0;
		vpd_page[7] = MSC_VPD_ID_BUF_SIZE - SCSI_VPD_HEADER_SIZE
				- SCSI_VPD_ID_HEADER_SIZE;

		/* Copy Vendor ID and Product ID in one go */
		p = vpd_page + SCSI_VPD_HEADER_SIZE + SCSI_VPD_ID_HEADER_SIZE;
		memcpy(p, msc_inquiry_data.vendor_id, 8 + 16);

		/* And finally, the right-aligned serial number */
		p += 8 + 16;
		memset(p, ' ', MSC_MAX_SERIAL_LEN - serial_len);
		p += MSC_MAX_SERIAL_LEN - serial_len;
		memcpy(p, serial, serial_len);

		total_len = MSC_VPD_ID_BUF_SIZE;
		if (alloc_len < total_len) {
			total_len = alloc_len;
			buffer_resize(buf, total_len);
		}

		usb_req_add_buffer(req, buf);
		req->req_done = msc_vpd_id_sent;
	}

	return total_len;
}

static void msc_inquiry(struct msc_interface *msc, struct udc *udc,
		struct usb_msc_cbw *cbw)
{
	struct usb_request	*req;
	struct buffer		*buf;
	long			residue;
	long			ret;
	uint16_t		alloc_len;
	const uint8_t		*cdb = cbw->CDB;
	size_t			total_len;
	uint8_t			page_code;

	alloc_len = scsi_cdb_get_u16(cdb, 3);

	dbg_verbose("msc: INQUIRY %u (inq len %zu)\n",
			alloc_len, sizeof(msc_inquiry_data));

	residue = msc_validate_req(msc, cbw, alloc_len, USB_CBW_DIRECTION_IN);
	if (residue < 0)
		return;

	req = usb_req_alloc();
	if (!req)
		goto err_req_alloc;
	req->context = msc;

	total_len = 0;
	page_code = cdb[2];

	if (cdb[1] & SCSI_INQ_REQ_EVPD) {
		/* Vital Product Data page requested */
		switch (page_code) {
		case SCSI_VPD_SUPPORTED_PAGES:
			ret = msc_vpd_supported_pages(msc, req, alloc_len);
			break;
		case SCSI_VPD_UNIT_SERIAL_NUMBER:
			ret = msc_vpd_serial_number(msc, req, alloc_len);
			break;
		case SCSI_VPD_DEVICE_IDENTIFICATION:
			ret = msc_vpd_device_id(msc, req, alloc_len);
			break;
		default:
			dbg_info("msc: unsupported VPD page %02x requested\n",
					page_code);
			goto illegal_request;
		}

		if (ret < 0)
			goto err_buf_alloc;

		total_len = ret;
	} else if (page_code != 0) {
		/* Nonzero PC is illegal when EVPD is not set */
		dbg_info("msc: INQUIRY PC=%02x but EVPD not set\n", page_code);
		goto illegal_request;
	} else if (alloc_len) {
		/* Send the normal INQUIRY data */
		buf = buffer_alloc();
		if (!buf)
			goto err_buf_alloc;

		total_len = min_u(alloc_len, sizeof(msc_inquiry_data));
		buffer_init_tx(buf, &msc_inquiry_data, total_len);
		usb_req_add_buffer(req, buf);
		req->req_done = msc_buf_sent;
	}

	residue += alloc_len - total_len;
	msc_prepare_csw(msc, residue, USB_CSW_STATUS_PASS);

	if (total_len) {
		udc_ep_submit_in_req(udc, msc->bulk_in_ep, req);
		msc_request_done(udc, msc, residue);
	} else {
		usb_req_free(req);
		msc_request_done_nodata(udc, msc, residue);
	}

	return;

illegal_request:
	msc_request_failed(msc, alloc_len + residue,
			USB_CSW_STATUS_FAIL,
			SCSI_SK_ILLEGAL_REQUEST,
			SCSI_ASC_INVALID_FIELD_IN_CDB);
	usb_req_free(req);
	return;

err_buf_alloc:
	usb_req_free(req);
err_req_alloc:
	msc_out_of_memory(msc);
}

static long msc_add_mode_pages(struct msc_interface *msc,
		struct usb_request *req, uint32_t avail_len,
		const uint8_t *cdb, uint32_t cbw_data_len)
{
	/* Changeable values are not supported */
	if (scsi_mode_sense_get_pc(cdb) == SCSI_MS_PC_CHANGEABLE) {
		dbg_info("msc: changeable mode pages not supported\n");
		goto invalid_request;
	}

	/* Saved values are not supported either, but we're allowed to
	 * return current values instead. Also, since we don't support
	 * changeable values, current values equal default values */

	/* Now, send any mode pages that were requested */
	switch (scsi_mode_sense_get_page_code(cdb)) {
	case 0:
		/* Just return the header and block descriptors */
		break;
	case SCSI_MS_PAGE_ALL:
		/* We don't support any actual mode pages yet */
		break;
	default:
		dbg_info("msc: unsupported mode page 0x%x\n",
				scsi_mode_sense_get_page_code(cdb));
		goto invalid_request;
	}

	return 0;

invalid_request:
	msc_request_failed(msc, cbw_data_len, USB_CSW_STATUS_FAIL,
			SCSI_SK_ILLEGAL_REQUEST,
			SCSI_ASC_INVALID_FIELD_IN_CDB);
	return -1;
}

static void msc_mode_sense(struct msc_interface *msc, struct udc *udc,
		struct usb_msc_cbw *cbw, uint32_t alloc_len)
{
	union msc_mode_param_header *header;
	struct usb_request	*req;
	struct buffer		*buf;
	const uint8_t		*cdb = cbw->CDB;
	long			residue;
	long			ret;
	size_t			total_size;

	dbg_verbose("msc MODE SENSE(N) page %u PC%u len %lu\n",
			scsi_mode_sense_get_page_code(cdb),
			scsi_mode_sense_get_pc(cdb),
			alloc_len);

	residue = msc_validate_req(msc, cbw, alloc_len, USB_CBW_DIRECTION_IN);
	if (residue < 0)
		return;

	req = usb_req_alloc();
	if (!req)
		goto err_req_alloc;
	req->context = msc;
	req->req_done = msc_mode_params_sent;

	/*
	 * Always send a header. Also allocate room for a block
	 * descriptor while we're at it.
	 */
	buf = buffer_dma_alloc(MSC_MODE_PARAM_HDR_BUF_LEN);
	if (!buf)
		goto err_buf_alloc;
	usb_req_add_buffer(req, buf);

	header = buf->addr.ptr;
	memset(header, 0, MSC_MODE_PARAM_HDR_BUF_LEN);

	/* The block device determines WP status */
	if (scsi_cdb_get_opcode(cdb) == SCSI_CMD_MODE_SENSE6) {
		if (!test_bit(BDEV_WRITEABLE, &msc->bdev->flags))
			header->h6.device_specific_parameter = SCSI_MS_SBC_WP;
		total_size = sizeof(header->h6);
	} else {
		if (!test_bit(BDEV_WRITEABLE, &msc->bdev->flags))
			header->h10.device_specific_parameter = SCSI_MS_SBC_WP;
		total_size = sizeof(header->h10);
	}

	/*
	 * If the Disable Block Descriptors bit isn't set, add a
	 * block descriptor.
	 */
	if (!scsi_mode_sense_dbd_is_set(cdb)) {
		struct sbc_slba_block_desc	*desc;

		total_size += sizeof(*desc);
		if (scsi_cdb_get_opcode(cdb) == SCSI_CMD_MODE_SENSE6) {
			header->h6.block_descriptor_length = sizeof(*desc);
			desc = (struct sbc_slba_block_desc *)(&header->h6 + 1);
		} else {
			header->h10.block_descriptor_length
				= cpu_to_be16(sizeof(*desc));
			desc = (struct sbc_slba_block_desc *)(&header->h10 + 1);
		}
		desc->nr_blocks = cpu_to_be32(msc->bdev->nr_blocks);
		desc->block_len = cpu_to_be32(blkdev_get_block_size(msc->bdev));
		assert(!(be32_to_cpu(desc->block_len)
					& ~SBC_SLBA_BLOCK_LEN_MASK));
	}

	buffer_resize(buf, total_size);

	/* Add the mode pages to the response */
	ret = msc_add_mode_pages(msc, req, alloc_len - total_size, cdb,
			alloc_len + residue);
	if (ret < 0) {
		usb_req_free(req);
		buffer_dma_free(buf, MSC_MODE_PARAM_HDR_BUF_LEN);
		return;
	}

	total_size += ret;
	if (scsi_cdb_get_opcode(cdb) == SCSI_CMD_MODE_SENSE6)
		header->h6.mode_data_length = total_size - 1;
	else
		header->h10.mode_data_length = cpu_to_be16(total_size - 2);

	/*
	 * Finish up the USB request and send it if it ends up
	 * containing any data.
	 */
	residue += alloc_len - min_u(alloc_len, total_size);
	msc_prepare_csw(msc, residue, USB_CSW_STATUS_PASS);
	if (alloc_len > 0) {
		udc_ep_submit_in_req(udc, msc->bulk_in_ep, req);
		msc_request_done(udc, msc, residue);
	} else {
		usb_req_free(req);
		buffer_dma_free(buf, MSC_MODE_PARAM_HDR_BUF_LEN);
		msc_request_done_nodata(udc, msc, residue);
	}

	return;

err_buf_alloc:
	usb_req_free(req);
err_req_alloc:
	msc_out_of_memory(msc);
}

static void msc_read_capacity(struct msc_interface *msc, struct udc *udc,
		struct usb_msc_cbw *cbw)
{
	struct sbc_read_capacity10_data	*response;
	struct usb_request	*req;
	struct buffer		*buf;
	long                    residue;

	build_assert(sizeof(*response) == 8);

	dbg_verbose("msc READ CAPACITY LBA %lx blklen %u\n",
			msc->bdev->nr_blocks - 1,
			blkdev_get_block_size(msc->bdev));

	residue = msc_validate_req(msc, cbw, 8, USB_CBW_DIRECTION_IN);
	if (residue < 0)
		return;

	req = usb_req_alloc();
	if (!req)
		goto err_req_alloc;
	req->req_done = msc_capacity_sent;
	req->context = msc;

	buf = buffer_dma_alloc(sizeof(*response));
	if (!buf)
		goto err_buf_alloc;
	usb_req_add_buffer(req, buf);

	response = buf->addr.ptr;
	response->max_lba = cpu_to_be32(msc->bdev->nr_blocks - 1);
	response->block_len = cpu_to_be32(blkdev_get_block_size(msc->bdev));

	msc_prepare_csw(msc, residue, USB_CSW_STATUS_PASS);
	udc_ep_submit_in_req(udc, msc->bulk_in_ep, req);
	msc_request_done(udc, msc, residue);

	return;

err_buf_alloc:
	usb_req_free(req);
err_req_alloc:
	msc_out_of_memory(msc);
}

static uint32_t msc_fill_buffer_list(struct slist *buf_list,
		unsigned int block_size, uint32_t nr_blocks)
{
	uint32_t		blocks_remaining;
	uint32_t		blocks_per_buf;
	uint_fast8_t		i;

	blocks_remaining = nr_blocks;
	blocks_per_buf = MSC_DATA_BUFFER_SIZE / block_size;

	for (i = 0; i < MSC_MAX_NR_BUFFERS; i++) {
		struct buffer		*buf;

		buf = buffer_dma_alloc(MSC_DATA_BUFFER_SIZE);
		if (!buf)
			break;

		if (blocks_per_buf > blocks_remaining) {
			blocks_per_buf = blocks_remaining;
			buffer_resize(buf, blocks_per_buf * block_size);
		}

		slist_insert_tail(buf_list, &buf->node);
		blocks_remaining -= blocks_per_buf;
		if (!blocks_remaining)
			break;
	}

	return nr_blocks - blocks_remaining;
}

/**
 * \internal
 *
 * Submit a list of buffers for storing data read from the block device.
 * We will stop submitting buffers when
 *   - we have submitted enough to store all the data we intend to read, or
 *   - we have reached the maximum amount of data for a single buffer list, or
 *   - there's no more buffer memory available, or
 *   - the request was ended prematurely
 *
 * \return The number of blocks covered by the submitted buffers
 */
static int msc_submit_read_buffers(struct msc_interface *msc,
		struct block_device *bdev, struct block_request *breq,
		uint32_t nr_blocks)
{
	struct slist		buf_list;
	uint32_t		blocks_queued;

	slist_init(&buf_list);
	blocks_queued = msc_fill_buffer_list(&buf_list,
				blkdev_get_block_size(bdev), nr_blocks);

	dbg_verbose("msc: blocks %lu/%lu queued for read\n", blocks_queued,
			nr_blocks);

	if (unlikely(!blocks_queued))
		return 0;

	/*
	 * If the request failed before we got around to submitting the
	 * buffer list, abort without submitting anything.
	 */
	atomic_add(&msc->blk_blocks_pending, blocks_queued);
	if (block_submit_buf_list(bdev, breq, &buf_list)) {
		atomic_sub(&msc->blk_blocks_pending, blocks_queued);
		msc_free_dma_buf_list(&buf_list);
		return 0;
	}

	msc->blocks_queued += blocks_queued;

	return blocks_queued;
}

/**
 * \internal
 *
 * This function is called each time one of the following happens:
 *   - A USB read data request is done
 *   - The block device has started processing our request
 *
 * The function will then try to keep both the block device and the USB
 * controller as busy as possible by submitting new requests and buffer
 * lists.
 */
static void msc_read_worker(struct msc_interface *msc)
{
	struct block_device	*bdev = msc->bdev;
	struct block_request	*breq = msc->block_req;
	uint32_t		blocks_remaining;
	uint32_t		submitted;
	uint32_t		blocks_per_seg;

	cpu_irq_disable();
	dbg_verbose("msc: blk pending %u locked %d\n",
			atomic_read(&msc->blk_blocks_pending),
			msc->queue_locked);
	blocks_per_seg = MSC_DATA_BUFFER_SIZE / blkdev_get_block_size(bdev);
	while ((atomic_read(&msc->blk_blocks_pending) * blocks_per_seg)
			< MSC_MAX_NR_SEGS
			&& !msc->queue_locked) {
		dbg_verbose("msc: read worker: q%lu <= t%lu s %d\n",
				msc->blocks_queued, msc->blocks_total,
				breq->status);
		assert(msc->blocks_queued <= msc->blocks_total);
		blocks_remaining = msc->blocks_total - msc->blocks_queued;
		if (!blocks_remaining)
			break;

		msc->queue_locked = true;
		cpu_irq_enable();

		submitted = msc_submit_read_buffers(msc, bdev, breq,
				blocks_remaining);

		cpu_irq_disable();
		msc->queue_locked = false;

		if (!submitted)
			break;
	}
	cpu_irq_enable();

	dbg_verbose("msc read worker done\n");
}

static void msc_read_data_sent(struct udc *udc, struct usb_request *req)
{
	struct msc_interface	*msc = req->context;
	uint32_t		blocks_remaining;
	enum status_code	status;

	dbg_verbose("msc: data sent: first=%p last=%p\n",
			slist_peek_head_node(&req->buf_list),
			slist_peek_tail_node(&req->buf_list));

	msc_free_dma_buf_list(&req->buf_list);
	status = req->status;
	usb_req_free(req);

	assert(atomic_read(&msc->usb_reqs_pending) > 0);
	atomic_dec(&msc->usb_reqs_pending);

	/*
	 * If the USB transfer failed, we were probably disconnected
	 * or reset. Abort the operation.
	 */
	if (status) {
		block_abort_req(msc->bdev, msc->block_req);
		return;
	}

	blocks_remaining = msc->blocks_total - msc->blocks_queued;
	if (!blocks_remaining)
		msc_request_data_done(udc, msc);
	else
		msc_read_worker(msc);
}

/*
 * This function is called when the block request has moved to the head
 * of the queue and the first batch of buffers have been submitted for
 * DMA. If we start submitting buffers before this, they will be
 * combined with the first batch, and we won't get a separate completion
 * callback for them.
 */
static void msc_block_read_started(struct block_device *bdev,
		struct block_request *breq)
{
	struct msc_interface	*msc = breq->context;

	if (msc->blocks_queued < msc->blocks_total)
		msc_read_worker(msc);
}

static void msc_block_read_done(struct block_device *bdev,
		struct block_request *breq)
{
	struct msc_interface	*msc = breq->context;
	struct usb_msc_csw	*csw = msc_get_csw(msc);
	uint32_t		residue;

	assert(breq == msc->block_req);

	residue = le32_to_cpu(csw->dCSWDataResidue);

	if (breq->status) {
		uint32_t		blocks_xfered;
		struct usb_msc_cbw	*cbw;

		blocks_xfered = blk_req_get_blocks_xfered(bdev, breq);

		dbg_warning("msc: block read failed: %d (after %lu blocks)\n",
				breq->status, blocks_xfered);

		cbw = msc_get_cbw(msc);
		residue = le32_to_cpu(cbw->dCBWDataTransferLength);
		residue -= blkdev_get_block_size(bdev) * blocks_xfered;
		csw->dCSWDataResidue = cpu_to_le32(residue);
		csw->bCSWStatus = USB_CSW_STATUS_FAIL;

		msc_init_sense(msc, SCSI_SK_MEDIUM_ERROR,
				SCSI_ASC_UNRECOVERED_READ_ERROR,
				msc->first_lba + blocks_xfered);
	}

	msc_request_done(msc->udc, msc, residue);
}

static void msc_block_read_buffers_done(struct block_device *bdev,
		struct block_request *breq, struct slist *buf_list)
{
	struct msc_interface 	*msc = breq->context;
	struct usb_request	*req;
	struct buffer		*buf;
	uint32_t		nr_blocks;

	assert(!slist_is_empty(buf_list));

	dbg_verbose("msc: read bufs done: status %d\n", breq->status);

	if (breq->status != OPERATION_IN_PROGRESS || !msc->bulk_in_ep) {
		dbg_verbose("  request terminated, discarding buffers\n");
		msc_free_dma_buf_list(buf_list);
		return;
	}

	req = usb_req_alloc();
	if (!req) {
		block_abort_req(bdev, breq);
		msc_free_dma_buf_list(buf_list);
		msc_out_of_memory(msc);
		return;
	}

	assert(atomic_read(&msc->blk_blocks_pending) > 0);
	atomic_inc(&msc->usb_reqs_pending);

	for (nr_blocks = 0, buf = buf_list_peek_head(buf_list);
			slist_node_is_valid(buf_list, &buf->node);
			buf = buf_list_peek_next(buf)) {
		nr_blocks += buf->len / blkdev_get_block_size(bdev);
	}
	atomic_sub(&msc->blk_blocks_pending, nr_blocks);

	slist_move_to_tail(&req->buf_list, buf_list);
	req->req_done = msc_read_data_sent;
	req->context = msc;
	dbg_verbose("  submitting IN request...\n");
	udc_ep_submit_in_req(msc->udc, msc->bulk_in_ep, req);
}

/**
 * \internal
 *
 * This function handles the READ(6) and READ(10) commands. It returns
 * after submitting zero or more buffers; if no buffers is submitted, a
 * CSW packet is sent to the host immediately, possibly after stalling
 * one or more endpoints.
 */
static void msc_do_read(struct msc_interface *msc, struct udc *udc,
		struct usb_msc_cbw *cbw, uint32_t lba, uint32_t nr_blocks)
{
	struct block_device	*bdev = msc->bdev;
	struct block_request	*breq;
	long			residue;
	uint32_t		cdb_data_len;
	uint32_t		blocks_queued;
	irqflags_t		iflags;

	dbg_verbose("msc READ(x) %lu blocks, LBA %lu\n", nr_blocks, lba);

	assert(!msc->xfer_in_progress);

	/*
	 * READ(10) allows max 65535 blocks. We allow max 65535 for
	 * block_size, so this cannot overflow.
	 */
	cdb_data_len = nr_blocks * blkdev_get_block_size(bdev);

	residue = msc_validate_req(msc, cbw, cdb_data_len,
			USB_CBW_DIRECTION_IN);
	if (unlikely(residue < 0))
		return;

	iflags = cpu_irq_save();
	if (msc->not_ready) {
		cpu_irq_restore(iflags);
		msc_request_failed(msc,
				le32_to_cpu(cbw->dCBWDataTransferLength),
				USB_CSW_STATUS_FAIL,
				SCSI_SK_NOT_READY, msc->busy_asc);
		return;
	}

	msc->xfer_in_progress = true;
	cpu_irq_restore(iflags);

	msc_prepare_csw(msc, residue, USB_CSW_STATUS_PASS);

	/* If no data was requested, we're done processing the command */
	if (unlikely(nr_blocks == 0)) {
		msc_request_done_nodata(udc, msc, residue);
		return;
	}

	msc->blocks_total = nr_blocks;
	msc->blocks_queued = 0;
	msc->queue_locked = true;
	atomic_write(&msc->blk_blocks_pending, 0);
	atomic_write(&msc->usb_reqs_pending, 0);

	breq = msc->block_req;
	breq->req_started = msc_block_read_started;
	breq->req_done = msc_block_read_done;
	breq->buf_list_done = msc_block_read_buffers_done;
	breq->context = msc;
	block_queue_req(bdev, breq, lba, nr_blocks, BLK_OP_READ);

	blocks_queued = msc_submit_read_buffers(msc, bdev, breq, nr_blocks);
	if (blocks_queued == 0) {
		block_abort_req(bdev, breq);
		msc_out_of_memory(msc);
	}
	msc->queue_locked = false;
}

static void msc_write_data_received(struct udc *udc, struct usb_request *req);

/**
 * \internal
 *
 * Submit a USB OUT request for receiving data to be written to the
 * block device. We will add buffers to the request until
 *   - we have submitted enough to receive all the data we intend to write, or
 *   - we have reached the maximum amount of data for a single buffer list, or
 *   - there's no more buffer memory available
 *
 * \return The number of blocks covered by the submitted request
 */
static int msc_submit_write_data_req(struct msc_interface *msc,
		struct block_device *bdev, uint32_t nr_blocks)
{
	struct usb_request	*req;
	uint32_t		blocks_queued;

	req = usb_req_alloc();
	if (!req)
		return 0;

	req->context = msc;
	req->req_done = msc_write_data_received;

	blocks_queued = msc_fill_buffer_list(&req->buf_list,
				blkdev_get_block_size(bdev), nr_blocks);

	dbg_verbose("msc: blocks %lu/%lu queued for write\n", blocks_queued,
			nr_blocks);

	if (unlikely(!blocks_queued)) {
		usb_req_free(req);
		return 0;
	}

	msc->blocks_queued += blocks_queued;

	atomic_inc(&msc->usb_reqs_pending);
	udc_ep_submit_out_req(msc->udc, msc->bulk_out_ep, req);

	return blocks_queued;
}

/**
 * \internal
 *
 * This function is called each time one of the following happens:
 *   - The block device has started processing our write request
 *   - The block device is done processing one buffer list
 *
 * The function will then try to keep both the block device and the USB
 * controller as busy as possible by submitting new requests and buffer
 * lists.
 */
static void msc_write_worker(void *data)
{
	struct msc_interface	*msc = data;
	struct block_device	*bdev = msc->bdev;
	uint32_t		blocks_remaining;
	uint32_t		submitted;

	while (atomic_read(&msc->usb_reqs_pending) < MSC_MAX_NR_SEGS
			&& !msc->queue_locked) {
		dbg_verbose("msc: write worker: q%lu <= 5%lu s %d\n",
				msc->blocks_queued, msc->blocks_total,
				msc->block_req->status);
		assert(msc->blocks_queued <= msc->blocks_total);
		blocks_remaining = msc->blocks_total - msc->blocks_queued;
		if (!blocks_remaining)
			break;

		msc->queue_locked = true;
		cpu_irq_enable();

		submitted = msc_submit_write_data_req(msc, bdev,
				blocks_remaining);

		cpu_irq_disable();
		msc->queue_locked = false;

		if (!submitted)
			break;
	}
}

static void msc_block_write_started(struct block_device *bdev,
		struct block_request *breq)
{
	struct msc_interface	*msc = breq->context;

	if (msc->blocks_queued < msc->blocks_total)
		msc_write_worker(msc);
}

static void msc_block_write_done(struct block_device *bdev,
		struct block_request *breq)
{
	struct msc_interface	*msc = breq->context;
	struct usb_msc_csw	*csw = msc_get_csw(msc);
	uint32_t		residue;

	assert(breq == msc->block_req);

	residue = le32_to_cpu(csw->dCSWDataResidue);

	if (breq->status) {
		uint32_t		blocks_xfered;
		struct usb_msc_cbw	*cbw;

		blocks_xfered = blk_req_get_blocks_xfered(bdev, breq);

		dbg_warning("msc: block write failed: %d (after %lu blocks)\n",
				breq->status, blocks_xfered);

		cbw = msc_get_cbw(msc);
		residue = le32_to_cpu(cbw->dCBWDataTransferLength);
		residue -= blkdev_get_block_size(bdev) * blocks_xfered;
		csw->dCSWDataResidue = cpu_to_le32(residue);
		csw->bCSWStatus = USB_CSW_STATUS_FAIL;

		msc_init_sense(msc, SCSI_SK_MEDIUM_ERROR,
				SCSI_ASC_WRITE_ERROR,
				msc->first_lba + blocks_xfered);
	}

	msc_request_done(msc->udc, msc, residue);
	msc_request_data_done(msc->udc, msc);
}

static void msc_block_write_buffers_done(struct block_device *bdev,
		struct block_request *breq, struct slist *buf_list)
{
	struct msc_interface 	*msc = breq->context;
	struct buffer		*buf;
	uint32_t		nr_blocks;

	msc_free_dma_buf_list(buf_list);

	assert(atomic_read(&msc->blk_blocks_pending) > 0);

	for (nr_blocks = 0, buf = buf_list_peek_head(buf_list);
			slist_node_is_valid(buf_list, &buf->node);
			buf = buf_list_peek_next(buf)) {
		nr_blocks += buf->len / blkdev_get_block_size(bdev);
	}
	atomic_sub(&msc->blk_blocks_pending, nr_blocks);

	assert(msc->blocks_queued <= msc->blocks_total);
	if (msc->blocks_queued < msc->blocks_total)
		msc_write_worker(msc);
}

static void msc_write_data_received(struct udc *udc, struct usb_request *req)
{
	struct slist		buf_list;
	struct msc_interface 	*msc = req->context;
	struct block_device	*bdev;
	struct block_request	*breq;
	enum status_code	status;
	struct buffer		*buf;
	uint32_t		nr_blocks;

	status = req->status;
	slist_init(&buf_list);
	slist_move_to_tail(&buf_list, &req->buf_list);
	usb_req_free(req);

	bdev = msc->bdev;
	breq = msc->block_req;

	for (nr_blocks = 0, buf = buf_list_peek_head(&buf_list);
			slist_node_is_valid(&buf_list, &buf->node);
			buf = buf_list_peek_next(buf)) {
		nr_blocks += buf->len / blkdev_get_block_size(bdev);
	}

	if (!status) {
		atomic_add(&msc->blk_blocks_pending, nr_blocks);
		atomic_dec(&msc->usb_reqs_pending);

		if (block_submit_buf_list(bdev, breq, &buf_list)) {
			atomic_sub(&msc->blk_blocks_pending, nr_blocks);
			msc_free_dma_buf_list(&buf_list);
		}
	} else {
		block_abort_req(bdev, breq);
	}
}

static void msc_do_write(struct msc_interface *msc, struct udc *udc,
		struct usb_msc_cbw *cbw, uint32_t lba, uint32_t nr_blocks)
{
	struct block_device	*bdev = msc->bdev;
	struct block_request	*breq;
	long			residue;
	uint32_t		cdb_data_len;
	uint32_t		blocks_queued;
	irqflags_t		iflags;

	dbg_verbose("msc WRITE(x) %lu blocks, LBA %lu\n", nr_blocks, lba);

	assert(!msc->xfer_in_progress);

	/*
	 * WRITE(10) allows max 65535 blocks. We allow max 65535 for
	 * block_size, so this cannot overflow.
	 */
	cdb_data_len = nr_blocks * blkdev_get_block_size(bdev);

	residue = msc_validate_req(msc, cbw, cdb_data_len, 0);
	if (unlikely(residue < 0))
		return;

	iflags = cpu_irq_save();
	if (msc->not_ready) {
		cpu_irq_restore(iflags);
		msc_request_failed(msc,
				le32_to_cpu(cbw->dCBWDataTransferLength),
				USB_CSW_STATUS_FAIL,
				SCSI_SK_NOT_READY, msc->busy_asc);
		return;
	}

	msc->xfer_in_progress = true;
	cpu_irq_restore(iflags);

	msc_prepare_csw(msc, residue, USB_CSW_STATUS_PASS);

	/* If no data was requested, we're done processing the command */
	if (unlikely(nr_blocks == 0)) {
		msc_request_done_nodata(udc, msc, residue);
		return;
	}

	msc->blocks_total = nr_blocks;
	msc->blocks_queued = 0;
	msc->queue_locked = true;
	atomic_write(&msc->blk_blocks_pending, 0);
	atomic_write(&msc->usb_reqs_pending, 0);

	breq = msc->block_req;
	breq->req_started = msc_block_write_started;
	breq->req_done = msc_block_write_done;
	breq->buf_list_done = msc_block_write_buffers_done;
	breq->context = msc;
	block_queue_req(bdev, breq, lba, nr_blocks, BLK_OP_WRITE);

	blocks_queued = msc_submit_write_data_req(msc, bdev, nr_blocks);
	if (blocks_queued == 0) {
		block_abort_req(bdev, breq);
		msc_out_of_memory(msc);
	}
	msc->queue_locked = false;
}

static void msc_verify_read(struct msc_interface *msc,
		struct block_device *bdev, uint32_t first_lba,
		uint32_t nr_blocks);
static void msc_verify_bytchk(struct msc_interface *msc,
		struct block_device *bdev, uint32_t first_lba,
		uint32_t nr_blocks);

static void msc_verify_bytchk(struct msc_interface *msc,
		struct block_device *bdev,
		uint32_t first_lba, uint32_t nr_blocks)
{
	msc_request_failed(msc, le32_to_cpu(msc_get_csw(msc)->dCSWDataResidue),
			USB_CSW_STATUS_FAIL, SCSI_SK_ILLEGAL_REQUEST,
			SCSI_ASC_INVALID_FIELD_IN_CDB);
}

static void msc_verify_read_done(struct block_device *bdev,
		struct block_request *breq)
{
	struct msc_interface	*msc = breq->context;
	struct usb_msc_csw	*csw = msc_get_csw(msc);

	assert(breq == msc->block_req);

	if (breq->status) {
		uint32_t	blocks_xfered;

		blocks_xfered = blk_req_get_blocks_xfered(bdev, breq);

		csw->bCSWStatus = USB_CSW_STATUS_FAIL;
		msc_init_sense(msc, SCSI_SK_MEDIUM_ERROR,
				SCSI_ASC_UNRECOVERED_READ_ERROR,
				msc->first_lba + blocks_xfered);
	}

	msc_request_done_nodata(msc->udc, msc,
			le32_to_cpu(csw->dCSWDataResidue));
}

static void msc_verify_read_buffers_done(struct block_device *bdev,
		struct block_request *breq, struct slist *buf_list)
{
	struct slist		new_buf_list;
	struct msc_interface	*msc = breq->context;
	unsigned int		block_size;
	uint32_t		blocks_queued;
	uint32_t		blocks_total;
	unsigned int		blocks_per_buf;

	blocks_total = msc->blocks_total;
	blocks_queued = msc->blocks_queued;
	block_size = blkdev_get_block_size(bdev);
	blocks_per_buf = MSC_DATA_BUFFER_SIZE / block_size;
	slist_init(&new_buf_list);

	while (!slist_is_empty(buf_list)) {
		struct buffer	*buf;

		assert(blocks_queued <= blocks_total);
		if (blocks_queued == blocks_total)
			break;

		buf = slist_pop_head(buf_list, struct buffer, node);
		blocks_queued += blocks_per_buf;

		if (blocks_queued > blocks_total) {
			blocks_per_buf -= blocks_queued - blocks_total;
			buffer_resize(buf, block_size * blocks_per_buf);
			blocks_queued = blocks_total;
		}
		slist_insert_tail(&new_buf_list, &buf->node);
	}

	if (!slist_is_empty(&new_buf_list)) {
		if (block_submit_buf_list(bdev, breq, &new_buf_list))
			msc_free_dma_buf_list(&new_buf_list);
		else
			msc->blocks_queued = blocks_queued;
	}

	/* Free whatever is left over from the loop above */
	msc_free_dma_buf_list(buf_list);
}

static void msc_verify_read(struct msc_interface *msc, struct block_device *bdev,
		uint32_t first_lba, uint32_t nr_blocks)
{
	struct slist		buf_list;
	struct block_request	*breq;
	uint32_t		blocks_queued;

	/*
	 * VERIFY isn't as performance critical as READ and WRITE, so
	 * we're not doing double-buffering here.
	 */
	msc->blocks_total = nr_blocks;
	breq = msc->block_req;
	breq->req_started = NULL;
	breq->req_done = msc_verify_read_done;
	breq->buf_list_done = msc_verify_read_buffers_done;
	breq->context = msc;
	block_queue_req(bdev, breq, first_lba, nr_blocks, BLK_OP_READ);

	slist_init(&buf_list);

	blocks_queued = msc_fill_buffer_list(&buf_list,
				blkdev_get_block_size(bdev), nr_blocks);

	if (unlikely(blocks_queued == 0)) {
		block_abort_req(bdev, breq);
		msc_out_of_memory(msc);
		return;
	}

	msc->blocks_queued = blocks_queued;
	if (block_submit_buf_list(bdev, breq, &buf_list)) {
		block_abort_req(bdev, breq);
		msc_free_dma_buf_list(&buf_list);
		msc_out_of_memory(msc);
	}
}

static void msc_do_verify(struct msc_interface *msc, struct udc *udc,
		struct usb_msc_cbw *cbw, uint32_t lba, uint32_t nr_blocks,
		bool bytchk)
{
	struct block_device	*bdev = msc->bdev;
	long			residue;
	uint32_t		cdb_data_len = 0;
	irqflags_t		iflags;

	dbg_verbose("msc VERIFY(x) %lu blocks, LBA %lu\n", nr_blocks, lba);

	/* Only expect to transfer data when doing byte checking */
	if (bytchk)
		cdb_data_len = nr_blocks * blkdev_get_block_size(bdev);

	residue = msc_validate_req(msc, cbw, cdb_data_len, 0);
	if (unlikely(residue < 0))
		return;

	iflags = cpu_irq_save();
	if (msc->not_ready) {
		cpu_irq_restore(iflags);
		msc_request_failed(msc,
				le32_to_cpu(cbw->dCBWDataTransferLength),
				USB_CSW_STATUS_FAIL,
				SCSI_SK_NOT_READY, msc->busy_asc);
		return;
	}

	msc->xfer_in_progress = true;
	cpu_irq_restore(iflags);

	msc_prepare_csw(msc, residue, USB_CSW_STATUS_PASS);

	if (unlikely(nr_blocks == 0)) {
		/* Nothing to verify; not an error */
		msc_request_done_nodata(udc, msc, residue);
		return;
	}

	if (bytchk)
		msc_verify_bytchk(msc, bdev, lba, nr_blocks);
	else
		msc_verify_read(msc, bdev, lba, nr_blocks);
}

static void msc_cbw_received(struct udc *udc, struct usb_request *req)
{
	struct msc_interface	*msc = req->context;
	struct usb_msc_cbw	*cbw;
	uint8_t			opcode;

	dbg_verbose("cbw received: status %d len %zu\n",
			req->status, req->bytes_xfered);

	cbw = msc_get_cbw(msc);
	assert(req == msc->cbw_csw_req);
	assert(cbw == usb_req_get_first_buffer(req)->addr.ptr);

	/* Don't submit any buffers when the USB transfer failed */
	if (req->status)
		return;

	/* Is this a valid CBW? */
	if (cbw->dCBWSignature != LE32(USB_CBW_SIGNATURE)
			|| req->bytes_xfered != 31) {
		/*
		 * No. Wedge both endpoints -- the host must do a
		 * Bulk-Only Mass Storage Reset to recover.
		 */
		udc_ep_set_wedge(udc, msc->bulk_in_ep);
		udc_ep_set_wedge(udc, msc->bulk_out_ep);
		return;
	}

	opcode = scsi_cdb_get_opcode(cbw->CDB);

	/* Try to handle the request */
	switch (opcode) {
	case SCSI_CMD_TEST_UNIT_READY:
		msc_test_unit_ready(msc, udc,
				le32_to_cpu(cbw->dCBWDataTransferLength));
		break;

	case SCSI_CMD_REQUEST_SENSE:
		msc_request_sense(msc, udc, cbw);
		break;

	case SCSI_CMD_READ6:
		msc_do_read(msc, udc, cbw, scsi_cdb6_get_lba(cbw->CDB),
				scsi_cdb6_get_xfer_len(cbw->CDB));
		break;

	case SCSI_CMD_WRITE6:
		msc_do_write(msc, udc, cbw, scsi_cdb6_get_lba(cbw->CDB),
				scsi_cdb6_get_xfer_len(cbw->CDB));
		break;

	case SCSI_CMD_INQUIRY:
		msc_inquiry(msc, udc, cbw);
		break;

	case SCSI_CMD_MODE_SENSE6:
		msc_mode_sense(msc, udc, cbw,
			scsi_cdb6_get_alloc_len(cbw->CDB));
		break;

	case SCSI_CMD_READ_CAPACITY10:
		msc_read_capacity(msc, udc, cbw);
		break;

	case SCSI_CMD_READ10:
		msc_do_read(msc, udc, cbw, scsi_cdb10_get_lba(cbw->CDB),
				scsi_cdb10_get_xfer_len(cbw->CDB));
		break;

	case SCSI_CMD_WRITE10:
		msc_do_write(msc, udc, cbw, scsi_cdb10_get_lba(cbw->CDB),
				scsi_cdb10_get_xfer_len(cbw->CDB));
		break;

	case SCSI_CMD_VERIFY10:
		msc_do_verify(msc, udc, cbw, scsi_cdb10_get_lba(cbw->CDB),
				scsi_cdb10_get_xfer_len(cbw->CDB),
				scsi_cdb10_bytchk_is_set(cbw->CDB));
		break;

	case SCSI_CMD_MODE_SENSE10:
		msc_mode_sense(msc, udc, cbw,
				scsi_cdb10_get_alloc_len(cbw->CDB));
		break;

	default:
		dbg_verbose("MSC: Unhandled opcode %02x\n", opcode);

		msc_request_failed(msc,
				le32_to_cpu(cbw->dCBWDataTransferLength),
				USB_CSW_STATUS_FAIL,
				SCSI_SK_ILLEGAL_REQUEST,
				SCSI_ASC_INVALID_COMMAND_OPERATION_CODE);
		break;
	}
}

/**
 * \brief Enable the MSC interface
 *
 * This function is normally called by the UDM layer, but the
 * application may call it directly if the UDM layer isn't used.
 *
 * \param udc The USB Device Controller which received the Set
 * Configuration or Set Interface request.
 * \param iface The UDM interface object representing this interface
 * \param setting The alternate setting to be enabled. If a Set
 * Interface request was received, pass wValue converted to native
 * endian; otherwise, pass 0.
 *
 * \retval STATUS_OK The interface was successfully enabled
 * \retval STATUS_INVALID_ARG \a setting is not supported
 * \retval negative Failed to enable the interface
 */
status_t udi_msc_enable(struct udc *udc, struct udm_interface *iface,
		uint16_t setting)
{
	struct msc_interface	*msc = msc_interface_of(iface);
	struct usb_msc_csw	*csw;
	dma_addr_t		addr;
	struct usb_request	*req;
	struct buffer		*buf;
	uint16_t		ep_size;
	usb_ep_id_t		ep_id;

	dbg_verbose("msc: enabling interface setting %u...\n", setting);

	if (setting != 0)
		return ERR_INVALID_ARG;

	msc->udc = udc;
	msc_queue_empty(msc);

	ep_size = APP_UDI_MSC_FS_BULK_EP_SIZE;
	if (udc_is_high_speed(udc))
		ep_size = 512;

	dbg_verbose("    creating bulk-in ep%02x: %u bytes\n",
			MSC_BULK_IN_EP_ADDR, ep_size);
	ep_id = udc_ep_create_bulk(udc, MSC_BULK_IN_EP_ADDR, ep_size);
	if (ep_id < 0)
		goto err_create_in_ep;
	msc->bulk_in_ep = ep_id;
	dbg_verbose("    creating bulk-out ep%02x: %u bytes\n",
			MSC_BULK_OUT_EP_ADDR, ep_size);
	ep_id = udc_ep_create_bulk(udc, MSC_BULK_OUT_EP_ADDR, ep_size);
	if (ep_id < 0)
		goto err_create_out_ep;
	msc->bulk_out_ep = ep_id;

	dbg_verbose("    allocating block device request\n");
	msc->block_req = block_alloc_request(msc->bdev);
	if (!msc->block_req)
		goto err_alloc_breq;

	dbg_verbose("    allocating DMA memory for CSW\n");
	addr = dma_alloc(sizeof(struct usb_msc_csw));
	if (!addr.ptr)
		goto err_alloc_csw;
	csw = addr.ptr;
	csw->dCSWSignature = LE32(USB_CSW_SIGNATURE);
	msc->csw = addr;

	dbg_verbose("    allocating DMA memory for CBW\n");
	addr = dma_alloc(sizeof(struct usb_msc_cbw));
	if (!addr.ptr)
		goto err_alloc_cbw;
	msc->cbw = addr;

	dbg_verbose("    allocating USB request for CBW and CSW\n");
	req = usb_req_alloc();
	if (!req)
		goto err_alloc_cbw_req;
	req->req_done = msc_cbw_received;
	req->context = msc;
	msc->cbw_csw_req = req;

	dbg_verbose("    allocating buffer to hold CBW and CSW\n");
	buf = buffer_alloc();
	if (!buf)
		goto err_alloc_buf;
	buffer_init_rx_mapped(buf, addr, sizeof(struct usb_msc_cbw));
	usb_req_add_buffer(req, buf);

	dbg_verbose("    submitting CBW - done!\n");
	udc_ep_submit_out_req(udc, msc->bulk_out_ep, req);

	return 0;

err_alloc_buf:
	usb_req_free(req);
err_alloc_cbw_req:
	dma_free(msc->cbw, sizeof(struct usb_msc_cbw));
err_alloc_cbw:
	dma_free(msc->csw, sizeof(struct usb_msc_csw));
err_alloc_csw:
	block_free_request(msc->bdev, msc->block_req);
err_alloc_breq:
	ep_id = msc->bulk_out_ep;
	msc->bulk_out_ep = 0;
	udc_ep_destroy(udc, ep_id);
err_create_out_ep:
	ep_id = msc->bulk_in_ep;
	msc->bulk_in_ep = 0;
	udc_ep_destroy(udc, ep_id);
err_create_in_ep:
	return -1;
}

/**
 * \brief Disable the MSC interface
 *
 * This function is normally called by the UDM layer, but the
 * application may call it directly if the UDM layer isn't used.
 *
 * \param udc The USB Device Controller which received the request
 * \param iface The UDM interface object representing this interface
 */
void udi_msc_disable(struct udc *udc, struct udm_interface *iface)
{
	struct msc_interface	*msc = msc_interface_of(iface);
	usb_ep_id_t		in, out;

	msc_queue_empty(msc);

	in = msc->bulk_in_ep;
	msc->bulk_in_ep = 0;
	out = msc->bulk_out_ep;
	msc->bulk_out_ep = 0;

	if (in > 0)
		udc_ep_destroy(udc, in);
	if (out > 0)
		udc_ep_destroy(udc, out);

	dma_free(msc->cbw, sizeof(struct usb_msc_cbw));
	dma_free(msc->csw, sizeof(struct usb_msc_csw));
	buffer_free(usb_req_get_first_buffer(msc->cbw_csw_req));
	usb_req_free(msc->cbw_csw_req);
	block_free_request(msc->bdev, msc->block_req);
}

static int msc_bulk_reset(struct udc *udc, struct msc_interface *msc)
{
	struct usb_request	*req;
	struct buffer		*buf;

	dbg_info("MSC Bulk Reset\n");

	/*
	 * Flush and un-wedge the endpoints. They will still be
	 * halted, so the host must still do ClearFeature(HALT) on
	 * both endpoints as per section 5.3.4 in the MSC Bulk-Only
	 * spec.
	 */
	if (msc->bulk_in_ep > 0) {
		udc_ep_flush(udc, msc->bulk_in_ep);
		udc_ep_clear_wedge(udc, msc->bulk_in_ep);
	}
	if (msc->bulk_out_ep > 0) {
		udc_ep_flush(udc, msc->bulk_out_ep);
		udc_ep_clear_wedge(udc, msc->bulk_out_ep);
	}

	msc_queue_empty(msc);

	/* Now, submit a new CBW and keep going */
	req = msc->cbw_csw_req;
	buf = usb_req_get_first_buffer(req);
	usb_req_init(req);

	buffer_init_rx_mapped(buf, msc->cbw, sizeof(struct usb_msc_cbw));
	usb_req_add_buffer(req, buf);
	req->req_done = msc_cbw_received;

	udc_ep_submit_out_req(udc, msc->bulk_out_ep, req);

	return 0;
}

/**
 * \brief Handle a setup request directed at the MSC interface
 *
 * This function is normally called by the UDM layer, but the
 * application may call it directly if the UDM layer isn't used.
 *
 * \param udc The USB Device Controller which received the request
 * \param iface The UDM interface object representing this interface
 * \param setup The unparsed SETUP request to be handled
 *
 * \retval STATUS_OK The request was handled successfully
 * \retval negative The request failed, so ep0 should be STALLed.
 */
status_t udi_msc_setup(struct udc *udc, struct udm_interface *iface,
		struct usb_setup_req *req)
{
	uint16_t	value = le16_to_cpu(req->wValue);
	uint16_t	len = le16_to_cpu(req->wLength);
	uint8_t		byte;

	if (usb_setup_type(req) != USB_REQTYPE_CLASS)
		return -1;

	switch (req->bRequest) {
	case USB_MSC_REQ_BULK_RESET:
		if (len || value || usb_setup_is_in(req))
			return -1;

		if (msc_bulk_reset(udc, msc_interface_of(iface)))
			return -1;

		udc_ep0_send_status(udc);
		break;

	case USB_MSC_REQ_GET_MAX_LUN:
		if (len != 1 || value || usb_setup_is_out(req))
			return -1;

		/* TODO: Support multiple LUNs */
		byte = 0;
		udc_ep0_write_sync(udc, &byte, sizeof(byte));
		udc_ep0_expect_status(udc);
		break;

	default:
		return -1;
	}

	return 0;
}

/**
 * \brief Get the interface descriptor for the MSC interface
 *
 * This function is normally called by the UDM layer, but the
 * application may call it directly if the UDM layer isn't used.
 *
 * \param iface The UDM interface object representing this interface
 * \param req The USB request being used to send the configuration
 * descriptor. Zero or more buffers containing the interface descriptor
 * data will be added to this request.
 * \param speed Get interface descriptors matching this speed. May be
 * different from the current speed if the "other speed" configuration
 * descriptor was requested.
 * \param len The number of bytes available for the descriptor data
 *
 * \return The number of bytes to be sent assuming \a len is large
 * enough. May be more than the number of bytes actually queued for
 * transfer if it was limited by \a len. Use this value to calculate
 * the wTotalLength field of the configuration descriptor.
 */
status_t udi_msc_get_iface_descriptor(struct udm_interface *iface,
		struct usb_request *req, enum usb_device_speed speed,
		uint16_t len)
{
	struct msc_iface_desc	*desc;
	struct buffer		*buf;

	if (!len)
		/* Just report the total length */
		goto out;

	buf = buffer_dma_alloc(sizeof(struct msc_iface_desc));
	desc = buf->addr.ptr;

	/* Copy the template as a starting point */
	memcpy(desc, &msc_desc_template, sizeof(struct msc_iface_desc));

	/* Then, fix up a few values known only at run-time */
	desc->iface.bInterfaceNumber = iface->iface_number;
#ifdef CONFIG_UDC_HIGH_SPEED
	if (speed == USB_SPEED_HIGH) {
		/* Only allowed value for high-speed bulk endpoints */
		desc->ep[0].wMaxPacketSize = LE16(512);
		desc->ep[1].wMaxPacketSize = LE16(512);
	}
#endif

	if (len < sizeof(struct msc_iface_desc))
		buffer_resize(buf, len);

	usb_req_add_buffer(req, buf);

out:
	return sizeof(struct msc_iface_desc);
}

/**
 * \brief Free the memory allocated by msc_get_iface_descriptor()
 *
 * This function is normally called by the UDM layer, but the
 * application may call it directly if the UDM layer isn't used.
 *
 * After the configuration descriptor has been successfully sent to the
 * host, this function should be called for each interface in the same
 * order that get_iface_descriptor() was called earlier as long as there
 * are buffers left in \a req's buffer list. The buffers used by the CDC
 * Data Class Interface are removed from \a req's buffer list.
 *
 * \param iface The UDM interface object representing this interface
 * \param req The USB request which was used to send the configuration
 * descriptor
 */
void udi_msc_free_descriptor(struct udm_interface *iface,
		struct usb_request *req)
{
	struct buffer	*buf;

	buf = slist_pop_head(&req->buf_list, struct buffer, node);
	assert(buf->len <= sizeof(struct msc_iface_desc));
	buffer_dma_free(buf, sizeof(struct msc_iface_desc));
}

/**
 * \brief Reject data transfer requests
 *
 * After the \a queue_empty callback has been called, all data transfer
 * requests from the host are rejected with sense key NOT_READY and
 * Additional Sense Code (Qualifier) as specified by \a asc.
 *
 * \param iface The MSC interface
 * \param asc ASC/ASCQ pair to return in the sense data
 * \param queue_empty Callback to be called after any ongoing transfers
 * have completed
 * \param data Arbitrary data pointer to be passed to \a queue_empty
 */
void udi_msc_set_busy(struct udm_interface *iface, uint16_t asc,
		void (*queue_empty)(void *data), void *data)
{
	struct msc_interface	*msc = msc_interface_of(iface);
	irqflags_t		iflags;

	iflags = cpu_irq_save();
	msc->not_ready = true;
	msc->busy_asc = asc;
	dbg_verbose("msc_set_busy: ASC(Q) %04x in_progress: %d\n",
			asc, msc->xfer_in_progress);
	if (msc->xfer_in_progress) {
		msc->busy_cb = queue_empty;
		msc->busy_cb_data = data;
		cpu_irq_restore(iflags);
	} else {
		cpu_irq_restore(iflags);
		queue_empty(data);
	}
}

/**
 * \brief Allow data transfer requests
 *
 * After this function returns, the MSC interface \a iface will no
 * longer reject data transfers from the host.
 */
void udi_msc_set_ready(struct udm_interface *iface)
{
	struct msc_interface	*msc = msc_interface_of(iface);

	msc->not_ready = false;
}

static struct msc_interface msc_interface = {
	.iface.iface_number		= APP_UDI_MSC_INTERFACE_ID,
	.iface.enable			= udi_msc_enable,
	.iface.disable			= udi_msc_disable,
	.iface.setup			= udi_msc_setup,
	.iface.get_iface_descriptor	= udi_msc_get_iface_descriptor,
	.iface.free_descriptor		= udi_msc_free_descriptor,
};

struct udm_interface *udi_msc_create_iface(struct block_device *bdev)
{
	struct msc_interface	*msc = &msc_interface;

	msc->bdev = bdev;

	build_assert(CONFIG_DMAPOOL_SMALL_OBJ_SIZE % 4 == 0);
	build_assert(MSC_DATA_BUFFER_SIZE % 512 == 0);

	msc->sense_data = dma_alloc(32);
	msc_init_sense(msc, SCSI_SK_NO_SENSE,
			SCSI_ASC_NO_ADDITIONAL_SENSE_INFO, 0);

	return &msc->iface;
}

//! @}

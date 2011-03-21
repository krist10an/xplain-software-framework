/**
 * \file
 *
 * \brief USB device multiplexing layer.
 *
 * This file implements a glue layer for easier device interface driver
 * implementation, and for supporting multiple configurations and/or
 * interfaces provided by separate drivers.
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
#include <debug.h>
#include <dmapool.h>
#include <interrupt.h>
#include <malloc.h>
#include <status_codes.h>
#include <string.h>
#include <util.h>
#include <usb/dev.h>
#include <usb/dev_mux.h>
#include <usb/request.h>
#include <usb/udc.h>

#include <app/usb.h>

/**
 * \ingroup usb_device_group
 * \defgroup usb_dev_mux USB Device Multiplexer (UDM)
 *
 * The USB Device Multiplexer (UDM) keeps track of configurations,
 * interfaces and settings, and switches between them based on requests
 * received from the host.
 *
 * A USB device can have one or more configurations, which the host can
 * choose between using the standard Set Configuration request.  The
 * host can also choose not to enable any configuration by selecting the
 * special configuration ID 0. Each configuration can have one or more
 * interfaces, which are all active at the same time.  Each interface
 * can have one or more alternate settings which the host can choose
 * between using the standard Set Interface request.
 *
 * \section udm-device-descriptor The Device Descriptor
 *
 * The USB Device Multiplexer provides a Device Descriptor for the
 * device. The contents of this can be specified by defining the
 * following macros in app/usb.h. Please see the USB Specification for
 * more information about the meaning of the fields.
 * - #APP_USB_DEVICE_CLASS: bDeviceClass
 * - #APP_USB_DEVICE_SUBCLASS: bDeviceSubClass
 * - #APP_USB_DEVICE_PROTOCOL: bDeviceProtocol
 * - #APP_UDC_MAXPACKETSIZE0: bMaxPacketSize0 (also used by UDC driver)
 * - #APP_USB_DEVICE_VENDOR_ID: idVendor
 * - #APP_USB_DEVICE_PRODUCT_ID: idProduct
 * - #APP_USB_DEVICE_MAJOR_VERSION /
 *   #APP_USB_DEVICE_MINOR_VERSION: bcdDevice
 * - #APP_USB_DEVICE_NR_CONFIGS: bNumConfigurations
 *
 * \subsection udm-device-desc-strings Strings in the Device Descriptor
 *
 * The application may define the following string IDs (and add support
 * for string descriptors as explained below) to allow the host to
 * display user-friendly strings whenever the device is connected:
 * - #USB_STRING_DEV_MANUFACTURER: iManufacturer
 * - #USB_STRING_DEV_PRODUCT: iProduct
 * - #USB_STRING_DEV_SERIAL: iSerialNumber
 *
 * \section udm-string-desc USB String Descriptor Support
 *
 * The following example illustrates how to implement support for string
 * descriptors in an application. Note that the value passed in the \a
 * langid parameter is in native byte order, not little endian.
 *
 * \par apps/foo/main.c (or somewhere else)
 * \code
 * static const le16_t langid_str[] = {
 *	LE16(USB_LANGID_EN_US),
 *	LE16(0),	// Terminate the list
 * };
 * static const char *string_table[] = {
 *	NULL,		// LANGID
 *	"This is my device",
 * };
 *
 * int app_usb_get_string_descriptor(struct udc *udc,
 *		struct usb_request *req, uint8_t index,
 *		uint16_t langid, uint16_t len)
 * {
 *	if (index >= ARRAY_LEN(string_table))
 *		return ERR_INVALID_ARG;
 *	if (index != 0 && langid != USB_LANGID_EN_US)
 *		return ERR_INVALID_ARG;
 *
 *	if (index == 0)
 *		return udm_submit_utf16le_string_desc(udc, req,
 *			langid_str, len);
 *
 *	return udm_submit_ascii_string_desc(udc, req,
 *		string_table[index], len);
 * }
 * \endcode
 *
 * \par apps/foo/include/app/usb.h
 * \code
 * extern int app_usb_get_string_descriptor(struct udc *udc,
 *		struct usb_request *req, uint8_t index,
 *		uint16_t langid, uint16_t len);
 * #define HAVE_APP_USB_GET_STRING_DESCRIPTOR
 * \endcode
 *
 * \section udi-driver USB Device Interface (UDI) drivers
 *
 * An interface is represented by an instance of struct #udm_interface,
 * implemented by a USB Device Interface (UDI) driver. The UDM layer
 * dispatches various requests received from the host to the UDI driver
 * through the function pointers in that structure.
 *
 * When an interface setting has been enabled, the driver may interact
 * directly with the UDC driver, managing endpoints and submitting
 * requests on them. The UDI driver may only submit control transfers
 * when it is processing a SETUP request directed at its interface.
 *
 * Note that the function driver must never keep any endpoints active
 * after udm_interface::disable() has been called. When the host
 * re-selects the currently active interface setting using Set
 * Interface, the UDM will call udm_interface::enable(). The UDI driver
 * is responsible for disabling and re-enabling itself when
 * udm_interface::enable() is called and the interface is already
 * active.
 *
 * @{
 */

//! \name USB Device Strings
//@{

/**
 * \def USB_STRING_DEV_MANUFACTURER
 * \brief String ID representing the name of the manufacturer
 *
 * The value of this define is stored in the iManufacturer field of the
 * device and device qualifier descriptors.
 */
/**
 * \def USB_STRING_DEV_PRODUCT
 * \brief String ID representing the name of the product
 *
 * The value of this define is stored in the iProduct field of the
 * device and device qualifier descriptors.
 */
/**
 * \def USB_STRING_DEV_SERIAL
 * \brief String ID representing the serial number of the device
 *
 * The value of this define is stored in the iSerialNumber field of the
 * device and device qualifier descriptors.
 */

#ifndef USB_STRING_DEV_MANUFACTURER
# define USB_STRING_DEV_MANUFACTURER	0
#endif
#ifndef USB_STRING_DEV_PRODUCT
# define USB_STRING_DEV_PRODUCT		0
#endif
#ifndef USB_STRING_DEV_SERIAL
# define USB_STRING_DEV_SERIAL		0
#endif

//@}

static const struct usb_device_descriptor udm_device_desc = {
	.bLength		= sizeof(struct usb_device_descriptor),
	.bDescriptorType	= USB_DT_DEVICE,
	.bcdUSB			= LE16(0x0200),
	.bDeviceClass		= APP_USB_DEVICE_CLASS,
	.bDeviceSubClass	= APP_USB_DEVICE_SUBCLASS,
	.bDeviceProtocol	= APP_USB_DEVICE_PROTOCOL,
	.bMaxPacketSize0	= APP_UDC_MAXPACKETSIZE0,
	.idVendor		= LE16(APP_USB_DEVICE_VENDOR_ID),
	.idProduct		= LE16(APP_USB_DEVICE_PRODUCT_ID),
	.bcdDevice		= LE16((APP_USB_DEVICE_MAJOR_VERSION << 8)
					| APP_USB_DEVICE_MINOR_VERSION),
	.iManufacturer		= USB_STRING_DEV_MANUFACTURER,
	.iProduct		= USB_STRING_DEV_PRODUCT,
	.iSerialNumber		= USB_STRING_DEV_SERIAL,
	.bNumConfigurations	= APP_USB_DEVICE_NR_CONFIGS,
};

#ifdef CONFIG_UDC_HIGH_SPEED
static const struct usb_device_qualifier_descriptor udm_device_qual = {
	.bLength	= sizeof(struct usb_device_qualifier_descriptor),
	.bDescriptorType	= USB_DT_DEVICE_QUALIFIER,
	.bcdUSB			= LE16(0x0200),
	.bDeviceClass		= APP_USB_DEVICE_CLASS,
	.bDeviceSubClass	= APP_USB_DEVICE_SUBCLASS,
	.bDeviceProtocol	= APP_USB_DEVICE_PROTOCOL,
	.bMaxPacketSize0	= APP_UDC_MAXPACKETSIZE0,
	.bNumConfigurations	= APP_USB_DEVICE_NR_CONFIGS,
};
#endif

static const struct usb_configuration_descriptor udm_config_desc_template = {
	.bLength		= sizeof(struct usb_configuration_descriptor),
	.bDescriptorType	= USB_DT_CONFIGURATION,
	.bmAttributes		= USB_CONFIG_ATTR_MUST_SET
					| USB_CONFIG_ATTR_SELF_POWERED,
	.bMaxPower		= USB_CONFIG_MAX_POWER(4),
};

static struct udm_config *udm_config[APP_USB_DEVICE_NR_CONFIGS];
static struct usb_request udm_control_req;
static struct buffer udm_desc_buf;

static struct udm_config *udm_get_config(unsigned int id)
{
	return udm_config[id - 1];
}

static struct udm_config *udm_get_current_config(struct udc *udc)
{
	if (!udc->config)
		return NULL;

	return udm_get_config(udc->config);
}

static unsigned int udm_nr_interfaces(struct udm_config *config)
{
	return config->desc.bNumInterfaces;
}

/**
 * \name Configuration Management
 *
 * The following functions may be called by the application to create
 * configurations and associate interfaces with them. The interface
 * objects are typically returned by the initialization function of a
 * USB Device Interface Driver.
 *
 * udm_create_config() returns a configuration object which can be
 * passed as a parameter to the other configuration management
 * functions. The configuration descriptor is managed by the UDM layer,
 * but the application may alter certain fields by calling
 * udm_config_set_self_powered(), udm_config_set_bus_powered(), and
 * udm_config_set_max_power(). The application must not create more than
 * one configuration with a given value, and it must not attempt to
 * create more configurations than specified by
 * #APP_USB_DEVICE_NR_CONFIGS.
 */
//@{

/**
 * \brief Add an interface to a configuration.
 *
 * This function will associate an interface with an existing
 * configuration so that it will be automatically enabled when the
 * configuration is selected, and the interface descriptor(s) will be
 * included in the configuration descriptor for this configuration.
 *
 * If no high-speed descriptors are provided, they will be assumed to
 * be the same as the full-speed descriptors.
 *
 * \param config The USB function configuration.
 * \param iface The interface to be added to \a config.
 *
 * \pre
 * - \a iface must have at least one alternate setting.
 * - \a iface must have a bInterfaceNumber less than bNumInterfaces
 *	of \a config.
 * - An interface with the same bInterfaceNumber must not have been
 *	registered before to the same configuration.
 * - All settings must have the same bInterfaceNumber.
 * - Each setting's high-speed descriptor, if present, must have
 *	the same bInterfaceNumber as the full-speed descriptor.
 */
void udm_config_add_interface(struct udm_config *config,
		struct udm_interface *iface)
{
	unsigned int	id = iface->iface_number;

	assert(id < udm_nr_interfaces(config));
	assert(!config->interface[id]);

	config->interface[id] = iface;

	dbg_info("udm: config %u: new interface %u\n",
			config->desc.bConfigurationValue, id);
}

/**
 * \brief Create a new USB device configuration.
 *
 * This function will create a new configuration and add it to the array
 * of possible configurations of the device.
 *
 * Initially, the configuration will be marked as self-powered and
 * drawing 4 mA from the bus.
 *
 * \param value The value used to select this configuration
 * \param nr_interfaces The number of interfaces in this configuration
 *
 * \return An object representing the new configuration.
 *
 * \pre
 * - \a value is not zero (this configuration value is reserved for
 *   representing no active configuration.)
 * - \a value is not higher than #APP_USB_DEVICE_NR_CONFIGS
 * - No configuration with the same \a value has been registered before.
 */
struct udm_config *udm_create_config(uint8_t value,
		uint8_t nr_interfaces)
{
	struct udm_config	*config;

	assert(value > 0 && value <= APP_USB_DEVICE_NR_CONFIGS);
	assert(!udm_get_config(value));

	config = zalloc(sizeof(struct udm_config)
			+ nr_interfaces * sizeof(config->interface[0]));
	if (unlikely(!config))
		return NULL;

	memcpy(&config->desc, &udm_config_desc_template, sizeof(config->desc));
	config->desc.bConfigurationValue = value;
	config->desc.bNumInterfaces = nr_interfaces;

	udm_config[value - 1] = config;

	return config;
}

/**
 * \brief Set the maximum power consumption of a configuration
 *
 * \param config A USB device configuration
 * \param milliamps The new maximum power consumption in milliamperes
 */
void udm_config_set_max_power(struct udm_config *config,
		unsigned int milliamps)
{
	assert(USB_CONFIG_MAX_POWER(milliamps) < 256);
	config->desc.bMaxPower = USB_CONFIG_MAX_POWER(milliamps);
}

/**
 * \brief Mark a configuration as self-powered
 *
 * \param config A USB device configuration
 */
void udm_config_set_self_powered(struct udm_config *config)
{
	irqflags_t	iflags;

	/* Maybe we should add size-aware atomic ops...? */
	iflags = cpu_irq_save();
	config->desc.bmAttributes |= USB_CONFIG_ATTR_SELF_POWERED;
	cpu_irq_restore(iflags);
}

/**
 * \brief Mark a configuration as bus-powered
 *
 * \param config A USB device configuration
 */
void udm_config_set_bus_powered(struct udm_config *config)
{
	irqflags_t	iflags;

	/* Maybe we should add size-aware atomic ops...? */
	iflags = cpu_irq_save();
	config->desc.bmAttributes &= ~USB_CONFIG_ATTR_SELF_POWERED;
	cpu_irq_restore(iflags);
}

//@}

static void udm_ctrl_in_done(struct udc *udc, struct usb_request *req)
{
	dbg_verbose("udm: Control IN request done\n");

	udc_ep0_expect_status(udc);
}

static void udm_config_desc_sent(struct udc *udc, struct usb_request *req)
{
	struct udm_config	*config = req->context;
	unsigned int		i;

	/* Pop the configuration descriptor */
	slist_pop_head_node(&req->buf_list);

	/* Ask all interfaces to remove of their own buffers */
	for (i = 0; i < udm_nr_interfaces(config); i++) {
		struct udm_interface	*iface;

		if (slist_is_empty(&req->buf_list))
			break;

		iface = config->interface[i];
		iface->free_descriptor(iface, req);
	}

	udm_ctrl_in_done(udc, req);
}

static void udm_string_desc_in_done(struct udc *udc,
		struct usb_request *req)
{
	struct buffer	*buf;

	buf = slist_pop_head(&req->buf_list, struct buffer, node);
	buffer_dma_free(buf, USB_MAX_DESC_LEN);

	udm_ctrl_in_done(udc, req);
}

/**
 * \name USB String Descriptor Support
 *
 * The following functions may be used by the application to implement
 * support for USB string descriptors. Such descriptors are optional
 * according to the USB spec, so by default, all string descriptor
 * requests are rejected.
 *
 * To support string descriptors, the application must implement the
 * function app_usb_get_string_descriptor() and provide a preprocessor
 * define named \c HAVE_APP_USB_GET_STRING_DESCRIPTOR (which will
 * disable the default implementation).
 */
//@{

/**
 * \brief Add a string descriptor buffer initialized from a UTF-16LE
 * encoded string
 *
 * \param udc The USB Device Controller
 * \param req The USB request to which the buffer should be added
 * \param str A UTF-16 LE encoded NUL-terminated string
 * \param max_len Maximum number of bytes available for the descriptor
 *
 * \return The number of bytes actually queued, or a negative error code
 * on failure.
 */
int udm_submit_utf16le_string_desc(struct udc *udc, struct usb_request *req,
		const le16_t *str, uint16_t max_len)
{
	struct usb_string_descriptor	*desc;
	struct buffer			*buf;
	const le16_t			*src;
	le16_t				*dst;
	uint16_t			len;

	if (max_len < offsetof(struct usb_string_descriptor, bString))
		return ERR_INVALID_ARG;

	buf = buffer_dma_alloc(USB_MAX_DESC_LEN);
	if (!buf)
		return ERR_NO_MEMORY;

	desc = buf->addr.ptr;
	desc->bDescriptorType = USB_DT_STRING;

	src = str;
	dst = desc->bString;
	for (len = offsetof(struct usb_string_descriptor, bString);
			len < max_len; len += 2) {
		le16_t	c = *src++;

		if (c == LE16(0))
			break;
		*dst++ = c;
	}

	desc->bLength = len;
	buffer_resize(buf, len);
	usb_req_add_buffer(req, buf);
	req->req_done = udm_string_desc_in_done;

	return len;
}

/**
 * \brief Add a string descriptor buffer initialized from an 8-bit ASCII
 * encoded string
 *
 * \param udc The USB Device Controller
 * \param req The USB request to which the buffer should be added
 * \param str A ASCII encoded NUL-terminated string
 * \param max_len Maximum number of bytes available for the descriptor
 *
 * \return The number of bytes actually queued, or a negative error code
 * on failure.
 */
int udm_submit_ascii_string_desc(struct udc *udc, struct usb_request *req,
		const char *str, uint16_t max_len)
{
	struct usb_string_descriptor	*desc;
	struct buffer			*buf;
	const char			*src;
	le16_t				*dst;
	uint16_t			len;

	if (max_len < offsetof(struct usb_string_descriptor, bString))
		return ERR_INVALID_ARG;

	buf = buffer_dma_alloc(USB_MAX_DESC_LEN);
	if (!buf)
		return ERR_NO_MEMORY;

	desc = buf->addr.ptr;
	desc->bDescriptorType = USB_DT_STRING;

	src = str;
	dst = desc->bString;
	for (len = offsetof(struct usb_string_descriptor, bString);
			len < max_len; len += 2) {
		char	c = *src++;

		if (c == '\0')
			break;
		*dst++ = cpu_to_le16(c);
	}

	desc->bLength = len;
	buffer_resize(buf, len);
	usb_req_add_buffer(req, buf);
	req->req_done = udm_string_desc_in_done;

	return len;
}

/**
 * \fn int app_usb_get_string_descriptor(struct udc *udc,
 *		struct usb_request *req, uint8_t index,
 *		uint16_t langid, uint16_t len)
 *
 * \brief Application-specific hook for retrieving a USB String
 * descriptor
 *
 * The default implementation of this hook will reject all string
 * descriptor requests. If an application needs to support string
 * descriptors, it must implement this function and define the
 * preprocessor symbol \c HAVE_APP_USB_GET_STRING_DESCRIPTOR.
 *
 * \param udc The USB Device Controller
 * \param req The USB request which will be used to send the descriptor
 * \param index The index of the requested string
 * \param langid The requested language ID
 * \param len The maximum number of bytes in the response
 *
 * \return The number of bytes added to \a req or a negative error code
 * on failure.
 */

#ifndef HAVE_APP_USB_GET_STRING_DESCRIPTOR
static int app_usb_get_string_descriptor(struct udc *udc,
		struct usb_request *req, uint8_t index,
		uint16_t langid, uint16_t len)
{
	/* String descriptors need application support */
	return -1;
}
#endif

//@}

static int udm_prep_config_desc(struct udm_config *config,
		struct usb_request *req,
		enum usb_device_speed speed, size_t max_len)
{
	struct udm_interface		*iface;
	struct buffer			*buffer;
	size_t				buf_len;
	size_t				total_len;
	size_t				len;
	unsigned int			i;

	total_len = sizeof(config->desc);
	len = min_u(max_len, total_len);

	buffer = &udm_desc_buf;
	buffer_init_tx(buffer, &config->desc, len);

	req->req_done = udm_config_desc_sent;
	req->context = config;
	usb_req_add_buffer(req, buffer);

	for (i = 0; i < udm_nr_interfaces(config); i++) {
		size_t	remaining_len;

		remaining_len = max_len - len;
		iface = config->interface[i];
		buf_len = iface->get_iface_descriptor(iface, req, speed,
				remaining_len);
		total_len += buf_len;
		len += min_u(buf_len, remaining_len);
	}

	config->desc.wTotalLength = cpu_to_le16(total_len);

	return len;
}

static int udm_enable_config(struct udc *udc, struct udm_config *config)
{
	unsigned int		nr_interfaces;
	unsigned int		i;

	dbg_verbose("udm: enabling configuration %u...\n",
			config->desc.bConfigurationValue);

	nr_interfaces = udm_nr_interfaces(config);
	for (i = 0; i < nr_interfaces; i++) {
		struct udm_interface	*iface;

		iface = config->interface[i];
		dbg_verbose("  - enabling interface %u...\n", i);
		if (iface->enable(udc, iface, 0))
			goto fail;
	}

	return 0;

fail:
	dbg_error("udm: failed to enable configuration %u, interface %u\n",
			config->desc.bConfigurationValue, i);
	while (i-- > 0) {
		struct udm_interface	*iface;

		iface = config->interface[i];
		iface->disable(udc, iface);
	}

	return -1;
}

static void udm_disable_config(struct udc *udc, struct udm_config *config)
{
	unsigned int		nr_interfaces;
	unsigned int		i;

	if (!config)
		return;

	dbg_verbose("udm: disabling configuration %u\n",
			config->desc.bConfigurationValue);

	nr_interfaces = udm_nr_interfaces(config);
	for (i = 0; i < nr_interfaces; i++) {
		struct udm_interface	*iface;

		iface = config->interface[i];
		dbg_verbose("  - disabling interface %u...\n", i);
		iface->disable(udc, iface);
	}
}

#if 0
static int udm_prep_string_desc(struct udc *udc,
		struct usb_request *req, uint8_t desc_index,
		uint16_t langid, uint16_t len)
{
	struct usb_string_descriptor	*desc;
	struct buffer			*buf;
	const char			*str;
	phys_addr_t			desc_phys;
	int				str_len;

	if (desc_index >= USB_NR_STRINGS)
		return -1;
	if (desc_index != USB_STRING_LANGID && langid != USB_DEV_LANGID)
		return -1;

	str = usb_dev_string_table[desc_index];
	if (!str)
		return -1;

	desc = dma_alloc(&desc_phys, USB_MAX_DESC_SIZE);
	if (!desc)
		return -1;

	desc->bDescriptorType = USB_DT_STRING;
	if (desc_index == USB_STRING_LANGID) {
		desc->bString[0] = LE16(USB_DEV_LANGID);
		desc->bLength = 4;
	} else {
		str_len = ascii_to_usb_str(str, desc->bString,
				USB_MAX_DESC_SIZE - 2);
		desc->bLength = str_len + 2;
	}

	buf = &udm_desc_buf;
	buffer_init_tx_mapped(buf, desc, desc_phys, min_u(desc->bLength, len));
	usb_req_add_buffer(req, buf);

	req->req_done = udm_string_desc_in_done;

	return buf->len;
}
#endif

//! @}

/**
 * \weakgroup usb_dev_core_group
 * @{
 */

status_t usb_dev_get_descriptor(struct udc *udc, uint16_t value,
		uint16_t index, uint16_t len)
{
	struct udm_config	*config;
	struct usb_request	*req;
	unsigned int		desc_index;
	unsigned int		desc_type;
	int			buf_len = -1;

	dbg_verbose("udm: get descriptor v%04x i%04x l%04x\n",
			value, index, len);

	req = &udm_control_req;
	usb_req_init(req);
	req->req_done = udm_ctrl_in_done;

	desc_type = value >> 8;
	desc_index = value & 0xff;

	switch (desc_type) {
	case USB_DT_DEVICE:
		buf_len = min_u(len, sizeof(udm_device_desc));
		buffer_init_tx(&udm_desc_buf, &udm_device_desc,
				buf_len);
		usb_req_add_buffer(req, &udm_desc_buf);
		break;

	case USB_DT_CONFIGURATION:
		if (desc_index >= APP_USB_DEVICE_NR_CONFIGS)
			return -1;

		config = udm_config[desc_index];
		config->desc.bDescriptorType = desc_type;
		buf_len = udm_prep_config_desc(config, req,
				udc->speed, len);
		break;

#ifdef CONFIG_UDC_HIGH_SPEED
	case USB_DT_DEVICE_QUALIFIER:
		buf_len = min_u(len, sizeof(udm_device_qual));
		buffer_init_tx(&udm_desc_buf, &udm_device_qual,
				buf_len);
		usb_req_add_buffer(req, &udm_desc_buf);
		break;

	case USB_DT_OTHER_SPEED_CONFIGURATION:
		if (desc_index >= APP_USB_DEVICE_NR_CONFIGS)
			return -1;

		config = udm_config[desc_index];
		config->desc.bDescriptorType = desc_type;

		if (udc->speed == USB_SPEED_HIGH)
			buf_len = udm_prep_config_desc(config, req,
					USB_SPEED_FULL, len);
		else
			buf_len = udm_prep_config_desc(config, req,
					USB_SPEED_HIGH, len);
		break;
#endif

	case USB_DT_STRING:
		buf_len = app_usb_get_string_descriptor(udc, req,
				desc_index, index, len);
		break;
	}

	if (buf_len < 0)
		return buf_len;

	if (buf_len < len)
		set_bit(USB_REQ_SHORT_PKT, &req->flags);

	udc_ep0_submit_in_req(udc, req);

	return STATUS_OK;
}

status_t usb_dev_set_configuration(struct udc *udc, uint16_t config_id)
{
	struct udm_config	*old;
	struct udm_config	*new;

	dbg_verbose("udm: set configuration %u\n", config_id);

	if (config_id > APP_USB_DEVICE_NR_CONFIGS)
		return -1;

	/* Disable the old configuration, if any */
	old = udm_get_current_config(udc);
	udc->config = 0;
	if (old)
		udm_disable_config(udc, old);

	if (config_id == 0)
		return STATUS_OK;

	new = udm_get_config(config_id);
	udc->config = config_id;
	if (udm_enable_config(udc, new)) {
		udc->config = 0;
		return -1;
	}

	return STATUS_OK;
}

status_t usb_dev_get_interface(struct udc *udc, uint16_t index)
{
	struct udm_config		*config;
	struct udm_interface		*iface;

	dbg_verbose("udm: get interface %u\n", index);

	config = udm_get_current_config(udc);
	if (!config || index >= udm_nr_interfaces(config))
		return -1;

	iface = config->interface[index];
	build_assert(sizeof(iface->cur_setting) == 1);

	udc_ep0_write_sync(udc, &iface->cur_setting, 1);
	udc_ep0_expect_status(udc);

	return 0;
}

status_t usb_dev_set_interface(struct udc *udc, uint16_t index,
		uint16_t altsetting)
{
	struct udm_config		*config;
	struct udm_interface		*iface;
	int				ret;

	dbg_verbose("udm: set interface %u altsetting %u\n",
			index, altsetting);

	config = udm_get_current_config(udc);
	if (!config || index >= udm_nr_interfaces(config))
		return -1;

	iface = config->interface[index];
	ret = iface->enable(udc, iface, altsetting);
	if (likely(!ret))
		iface->cur_setting = altsetting;

	return ret;
}

void usb_dev_reset(struct udc *udc)
{
	dbg_verbose("udm: reset, speed=%u\n", udc->speed);

	if (udc->config)
		usb_dev_set_configuration(udc, 0);
}

status_t usb_dev_process_setup_request(struct udc *udc,
		struct usb_setup_req *req)
{
	struct udm_config		*config;
	struct udm_interface		*iface;
	uint16_t			index = le16_to_cpu(req->wIndex);

	if (usb_setup_recipient(req) != USB_RECIP_INTERFACE) {
		dbg_warning("udm: bad request (bmRequestType: %u)\n",
				req->bmRequestType);
		return -1;
	}

	config = udm_get_current_config(udc);
	if (!config || index >= udm_nr_interfaces(config)) {
		dbg_warning("udm: bad interface %u\n", index);
		return -1;
	}

	iface = config->interface[index];
	if (!iface->setup)
		return -1;

	return iface->setup(udc, iface, req);
}

//! @}

/**
 * \file
 *
 * \brief Application-specific USB configuration.
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
#ifndef APP_USB_H_INCLUDED
#define APP_USB_H_INCLUDED

#include <app/version.h>
#include <usb/usb_ids.h>

#define APP_USB_DEVICE_CLASS		USB_CLASS_NONE
#define APP_USB_DEVICE_SUBCLASS		USB_SUBCLASS_NONE
#define APP_USB_DEVICE_PROTOCOL		USB_PROTOCOL_NONE
#define APP_USB_DEVICE_VENDOR_ID	USB_VID_ATMEL
/** \todo replace with configurable USB PID number. */
#define APP_USB_DEVICE_PRODUCT_ID	USB_PID_AVR32786
#define APP_USB_DEVICE_NR_CONFIGS	1

#define APP_USB_DEVICE_MAJOR_VERSION	MAJOR_VERSION
#define APP_USB_DEVICE_MINOR_VERSION	MINOR_VERSION

#define APP_USB_NR_REQUESTS		8
#define APP_UDC_NR_ENDPOINTS		3
#define APP_UDC_MAXPACKETSIZE0		64

#define APP_UDI_MSC_INTERFACE_ID	0

#define APP_UDI_MSC_BULK_IN_EP		1
#define APP_UDI_MSC_BULK_OUT_EP		2

#define APP_UDI_MSC_INQ_VENDOR_ID				\
	'A', 't', 'm', 'e', 'l', ' ', ' ', ' '
#define APP_UDI_MSC_INQ_PRODUCT_ID				\
	'U', 'S', 'B', ' ', 'M', 'a', 's', 's',			\
	' ', 'S', 't', 'o', 'r', 'a', 'g', 'e'
#define APP_UDI_MSC_INQ_PRODUCT_VERSION				\
	'0' + MAJOR_VERSION, '.', '0' + MINOR_VERSION, ' '

static inline const char *app_get_serial_number(void)
{
	return "0123456789AB";
}

#endif /* APP_USB_H_INCLUDED */

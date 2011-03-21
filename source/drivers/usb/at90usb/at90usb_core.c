/**
 * \file
 *
 * \brief AT90USB driver: Core/Transceiver part
 *
 * This file implements a driver for the transceiver logic in the AT90USB USB
 * hardware controller. It is responsible for enabling the host- and device
 * parts of the driver depending on configuration options, transceiver states
 * and USB On-The-Go events originating from HNP and SRP.
 *
 * \internal
 * @{
 * If the driver is configured as host-only or device-only, the OTG logic will
 * be disabled. This file is still responsible for detecting connection and
 * disconnection events, and generating and detecting Vbus.
 * @}
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
#include <debug.h>
#include <intc.h>
#include <physmem.h>
#include <stdbool.h>
#include <interrupt.h>

#include <clk/sys.h>

#include <chip/clk.h>
#include <chip/regs.h>
#include <chip/at90usb.h>
#include <chip/irq-map.h>

#include <cpu/io.h>

#include <usb/udc.h>

#include <app/usb.h>

#include "at90usb_internal.h"
#include "at90usb_regs.h"

/* Configuration sanity-checks. */
#if defined(CONFIG_UDC) && !defined(CHIP_AT90USB_HAS_DEVICE)
# error AT90USB: This chip does not have a Device Controller
#endif
#if defined(CONFIG_AT90USB_OTG) && !defined(CHIP_AT90USB_HAS_OTG)
# error AT90USB: This chip does not support USB On-The-Go
#endif

/**
 * \internal
 * \brief Get the AT90USB UDC struct if UDC mode is enabled.
 *
 * \param at90usb The AT90USB controller.
 *
 * \return The AT90USB UDC instance.
 */
static inline
struct at90usb_udc *at90usb_get_udc(struct at90usb_controller *at90usb)
{
#ifdef CONFIG_UDC
	return at90usb->udc;
#else
	return NULL;
#endif
}

/**
 * \internal
 * \brief Get the AT90USB host struct if USB host mode is enabled.
 *
 * \param at90usb The AT90USB controller.
 *
 * \return The AT90USB host instance.
 */
static inline
struct at90usb_host *at90usb_get_host(struct at90usb_controller *at90usb)
{
	return NULL;
}

/**
 * \internal
 * \brief Get the AT90USB OTG struct if USB OTG mode is enabled.
 *
 * \param at90usb The AT90USB controller.
 *
 * \return True if the controller supports OTG mode, false otherwise.
 */
static inline bool at90usb_is_otg(struct at90usb_controller *at90usb)
{
#ifdef CONFIG_AT90USB_OTG
	return true;
#else
	return false;
#endif
}

/**
 * \internal
 * \brief Check VBUS and notify the UDC about the status.
 *
 * Check the status of the VBUS line and notify the UDC driver about the
 * current status, either on or off.
 *
 * \param at90usb The AT90USB controller.
 */
static void at90usb_check_vbus(struct at90usb_controller *at90usb)
{
	struct at90usb_udc *udc = at90usb_get_udc(at90usb);

	if (avr_read_reg8(USBSTA) & AT90USB_USBSTA_VBUS)
		at90usb_udc_vbus_on(udc);
	else
		at90usb_udc_vbus_off(udc);
}

/**
 * \internal
 * \brief Enter USB device mode.
 *
 * Disable the host driver (if any), enable the UDC driver and start
 * watching the state of the Vbus line.
 *
 * \param at90usb The AT90USB controller.
 */
static void at90usb_enter_device_mode(struct at90usb_controller *at90usb)
{
	struct at90usb_host	*host = at90usb_get_host(at90usb);
	struct at90usb_udc	*udc = at90usb_get_udc(at90usb);

	dbg_verbose("AT90USB: Entering device mode...\n");

	if (at90usb_host_is_enabled(host))
		at90usb_host_disable(host);
	if (!at90usb_udc_is_enabled(udc))
		at90usb_udc_enable(udc);

	avr_write_reg8(USBINT, avr_read_reg8(USBINT) & ~AT90USB_USBINT_VBUSTI);

	at90usb_check_vbus(at90usb);

	avr_write_reg8(USBCON, avr_read_reg8(USBCON) | AT90USB_USBCON_VBUSTE);

	dbg_verbose("AT90USB: USBCON=%02x\n", avr_read_reg8(USBCON));
}

/**
 * \internal
 * \brief Enter USB host mode.
 *
 * Disable the UDC driver (if any), and then enable the host driver (if any).
 *
 * \param at90usb The AT90USB controller.
 */
static void at90usb_enter_host_mode(struct at90usb_controller *at90usb)
{
	struct at90usb_host	*host = at90usb_get_host(at90usb);
	struct at90usb_udc	*udc = at90usb_get_udc(at90usb);

	if (host) {
		if (at90usb_udc_is_enabled(udc))
			at90usb_udc_disable(udc);
		if (!at90usb_host_is_enabled(host))
			at90usb_host_enable(host);
	}
}

/**
 * \internal
 * \brief Check the state of the USB OTG ID pin.
 *
 * Check the state of the USB OTG ID pin and enable host/device
 * functionality as appropriate.
 *
 * \param at90usb The AT90USB controller.
 */
static void at90usb_check_id(struct at90usb_controller *at90usb)
{
	if (avr_read_reg8(USBSTA) & AT90USB_USBSTA_ID)
		at90usb_enter_device_mode(at90usb);
	else
		at90usb_enter_host_mode(at90usb);
}

static struct at90usb_controller the_at90usb_controller;

/**
 * \internal
 * \brief AT90USB general interrupt handler.
 *
 * This is the general interrupt handler for the AT90USB controller. It
 * handles OTG and Vbus events, and calls the host- or device-specific
 * interrupt handler, depending on the configuration and/or ID pin
 * state.
 *
 * \param data Data associated with this interrupt, the AT90USB controller.
 */
static void at90usb_generic_interrupt(void *data)
{
	struct at90usb_controller	*at90usb = data;
	struct at90usb_udc		*udc;
	struct at90usb_host		*hostb;
	uint8_t				reg;

	reg = avr_read_reg8(USBINT);

	if (at90usb_is_otg(at90usb)) {
		if (reg & AT90USB_USBINT_IDTI) {
			avr_write_reg8(USBINT, reg & ~AT90USB_USBINT_IDTI);
			at90usb_check_id(at90usb);
		}

		/* TODO: More OTG stuff */
	}

	udc = at90usb_get_udc(at90usb);
	if (at90usb_udc_is_enabled(udc)) {
		if (reg & AT90USB_USBINT_VBUSTI) {
			avr_write_reg8(USBINT, reg & ~AT90USB_USBINT_VBUSTI);
			at90usb_check_vbus(at90usb);
		}

		if (avr_read_reg8(UDINT) & avr_read_reg8(UDIEN)) {
			avr_write_reg8(UDIEN, avr_read_reg8(UDIEN)
					& ~avr_read_reg8(UDINT));
			workqueue_add_task(&main_workqueue, &udc->task);
		}
	}

	hostb = at90usb_get_host(at90usb);
	if (at90usb_host_is_enabled(hostb)) {
		/* TODO: More HOST stuff */
	}
}
INTC_DEFINE_HANDLER(USB_GEN_IRQ, at90usb_generic_interrupt, 0);

/**
 * \internal
 * \brief AT90USB endpoint/pipe interrupt handler.
 *
 * This is the endpoint/pipe interrupt handler for the AT90USB controller. It
 * handles OTG and Vbus events, and calls the host- or device-specific
 * interrupt handler, depending on the configuration and/or ID pin
 * state.
 *
 * \param data Data associated with this interrupt, the AT90USB controller.
 */
static void at90usb_ep_interrupt(void *data)
{
	struct at90usb_controller	*at90usb = data;
	struct at90usb_udc		*udc;
	struct at90usb_host		*hostb;
	/* Shadow current active endpoint. */
	uint8_t				uenum = avr_read_reg8(UENUM);

	if (at90usb_is_otg(at90usb)) {
		/* TODO: More OTG stuff? */
	}

	udc = at90usb_get_udc(at90usb);
	if (at90usb_udc_is_enabled(udc)) {
		usb_ep_id_t	ep_id;
		uint8_t		ueint = avr_read_reg8(UEINT);

		for (ep_id = 0; ep_id < APP_UDC_NR_ENDPOINTS; ep_id++) {
			if (ueint & AT90USB_UEINT_EP(ep_id)) {
				struct at90usb_udc_ep *ep = &udc->ep[ep_id];
				avr_write_reg8(UENUM, ep_id);
				ep->ueienx = avr_read_reg8(UEIENX);
				avr_write_reg8(UEIENX, ep->ueienx
						& ~avr_read_reg8(UEINTX));
				workqueue_add_task(&main_workqueue,
						&ep->task);
			}
		}
	}

	hostb = at90usb_get_host(at90usb);
	if (at90usb_host_is_enabled(hostb)) {
		/* TODO: More HOST stuff? */
	}

	/* Restore the endpoint to what it was before the interrupt. */
	avr_write_reg8(UENUM, uenum);
}
INTC_DEFINE_HANDLER(USB_EP_IRQ, at90usb_ep_interrupt, 0);

/**
 * \internal
 * \brief Initialize the AT90USB controller.
 *
 * This function will initialize and enable the AT90USB controller.
 * Depending on the configuration, this may involve one or more of the
 * following steps:
 *  - Initialize the host and device parts of the driver.
 *  - Start monitoring the ID pin.
 *  - Switch the controller into host or device mode.
 *  - Start monitoring the Vbus pin.
 *
 * \return The AT90USB controller instance.
 *
 * \todo Add support for host and OTG modes.
 */
static struct at90usb_controller *at90usb_init(void)
{
	struct at90usb_controller	*at90usb = &the_at90usb_controller;
	uint8_t				usbcon;

	/*
	 * Only do the initialization once. We might get called from
	 * udc_init() as well as other initialization functions.
	 */
	if (at90usb_get_host(at90usb) || at90usb_get_udc(at90usb))
		return at90usb;

	sysclk_enable_module(SYSCLK_USB);
	intc_setup_handler(USB_GEN_IRQ, 0, at90usb);
	intc_setup_handler(USB_EP_IRQ, 0, at90usb);

#ifdef CONFIG_UDC
	at90usb->udc = at90usb_udc_init();
	if (!at90usb->udc)
		goto err_udc;
#endif
#ifdef CONFIG_AT90USB_HOST
	at90usb->host = at90usb_host_init();
	if (!at90usb->host)
		goto err_host;
#endif

	clk_enable_at90usb();

	/* Keep the FRZCLK bit if set in USBCON. This bit must be held active
	 * while the USB peripheral is enabled. And then cleared on a following
	 * write to USBCON to enable the USB peripheral. */
	usbcon = (avr_read_reg8(USBCON) & AT90USB_USBCON_FRZCLK)
		| AT90USB_USBCON_USBE
		| AT90USB_USBCON_OTGPADE;

	if (at90usb_is_otg(at90usb)) {
		/* Full OTG */
		usbcon |= AT90USB_USBCON_IDTE;

		avr_write_reg8(UHWCON, AT90USB_UHWCON_UIDE);
		avr_write_reg8(USBINT, avr_read_reg8(USBINT)
				& ~AT90USB_USBINT_IDTI);
		avr_write_reg8(USBCON, usbcon);
		avr_write_reg8(USBCON, usbcon & ~AT90USB_USBCON_FRZCLK);

		at90usb_check_id(at90usb);

		/* TODO: initialize HNP, SRP handling, etc. */
	} else if (at90usb_get_udc(at90usb)) {
		/* Device only */
		avr_write_reg8(UHWCON, AT90USB_UHWCON_UIMOD);
		avr_write_reg8(USBCON, usbcon);
		avr_write_reg8(USBCON, usbcon & ~AT90USB_USBCON_FRZCLK);

		at90usb_enter_device_mode(at90usb);
	} else if (at90usb_get_host(at90usb)) {
		/* Host only */
		avr_write_reg8(UHWCON, AT90USB_UHWCON_UVREGE);
		avr_write_reg8(USBCON, usbcon);
		avr_write_reg8(USBCON, usbcon & ~AT90USB_USBCON_FRZCLK);

		at90usb_enter_host_mode(at90usb);
	}

	return at90usb;

#ifdef CONFIG_AT90USB_HOST
err_host:
	at90usb_udc_shutdown(at90usb_get_udc(at90usb));
#endif
#ifdef CONFIG_UDC
err_udc:
#endif
	return NULL;
}

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
 * \return The USB Device Controller object.
 */
struct udc *udc_init(void)
{
	struct at90usb_controller *at90usb;

	at90usb = at90usb_init();
	if (!at90usb)
		return NULL;

	return &at90usb_get_udc(at90usb)->udc;
}

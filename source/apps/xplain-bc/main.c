/**
 * \file
 *
 * \brief Main source file for Xplain series board controller.
 *
 * Copyright (C) 2009 - 2010 Atmel Corporation. All rights reserved.
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
#include <led.h>
#include <gpio.h>
#include <board.h>
#include <debug.h>
#include <dmapool.h>
#include <mainloop.h>
#include <clk/sys.h>
#include <usb/dev_mux.h>
#include <usb/request.h>
#include <usb/udc.h>

#include "xplain-bc.h"

static struct app_usb_mode_task *app_usb_mode_task_of_task(
		struct workqueue_task *task)
{
	return container_of(task, struct app_usb_mode_task, task);
}

/**
 * \brief Check USB mode switch input line and enable or disable the USB
 *        interface.
 *
 * This function will read the USB mode switch input line and disable or enable
 * the USB interface as needed. If MSC is enabled and input line is high, MSC
 * will be disabled and SPI lines set as input (slave mode). In addition the
 * red LED will be deactivated to indicate MSC is no longer available. Finally
 * the XMEGA chip is released from reset.
 *
 * When MSC is not enabled and the USB mode switch line is low the worker
 * function will set the SPI lines in output mode (master mode) and enable the
 * USB interface. In addition the red LED will be actived to indicate MSC is
 * available. Finally the XMEGA chip is held in reset to get synchronized
 * access to the flash memory.
 */
static void app_usb_mode_worker(struct workqueue_task *task)
{
	struct app_usb_mode_task        *usb_mode_task;
	bool                            is_usb_msc_mode;

	usb_mode_task = app_usb_mode_task_of_task(task);
	is_usb_msc_mode = board_gpio_is_usb_msc_mode();

	/* Check if MSC is enabled and mode pin is high (i.e. disable it). */
	if (usb_mode_task->msc_enabled && !is_usb_msc_mode) {
		/*
		 * If SPI is busy, wait for it to complete. \todo Correct usage
		 * master.base.status check for OPERATION_IN_PROGRESS?
		 */
		if (usb_mode_task->master.base.status == OPERATION_IN_PROGRESS)
			goto out_retry;

		/* Detach to the USB bus, disabling the MSC interface. */
		udc_detach(usb_mode_task->udc);
		usb_mode_task->msc_enabled = false;

		/* Set the SPI in slave mode. */
		board_gpio_set_spi_master_mode(false);

		/* Release the XMEGA chip from reset, SPI bus is now ready. */
		board_gpio_mcu_reset(false);
		led_deactivate(BOARD_LED_RED);
	}
	/* Check if MSC is disabled and mode pin is low (i.e. enable it). */
	else if (!usb_mode_task->msc_enabled && is_usb_msc_mode) {
		/* The XMEGA interferes on the SPI bus, keep it in reset. */
		board_gpio_mcu_reset(true);

		/* Set the SPI in master mode. */
		board_gpio_set_spi_master_mode(true);

		/* Attach to the USB bus, disabling the MSC interface. */
		udc_attach(usb_mode_task->udc);
		usb_mode_task->msc_enabled = true;
		led_activate(BOARD_LED_RED);
	}

	/**
	 * \todo Rewrite to use GPIO level change interrupt to trigger USB
	 *       mode switching task.
	 */
out_retry:
	workqueue_add_task(&main_workqueue, &usb_mode_task->task);
}

/**
 * \brief Set SPI lines as input (slave mode) and release XMEGA reset line.
 *
 * This worker function will set the SPI lines in input mode (slave mode) and
 * release the XMEGA reset line. Further it will switch to the main worker
 * function \ref app_usb_mode_worker and add that worker function to the queue.
 */
static void app_dataflash_ready(struct workqueue_task *task)
{
	struct app_usb_mode_task        *usb_mode_task;
	usb_mode_task = app_usb_mode_task_of_task(task);
	board_gpio_set_spi_master_mode(false);
	board_gpio_mcu_reset(false);
	workqueue_task_set_work_func(&usb_mode_task->task, app_usb_mode_worker);
	workqueue_add_task(&main_workqueue, &usb_mode_task->task);
}

/**
 * \brief Xplain board controller application.
 *
 * This application will enable or disable the USB mass-storage class (MSC)
 * interface given an input on the USB mode switch input line. The main
 * microcontroller on the Xplain board is held in reset while the board
 * controller has the MSC interface enabled to ensure synchronouse access.
 */
int main(void)
{
	static struct app_usb_mode_task  usb_mode_task;
	struct udc                       *udc;

	cpu_irq_enable();
	sysclk_init();
	dbg_init();
	board_init();
	workqueue_init(&main_workqueue);
	dma_pool_init();
	buffer_pool_init();
	usb_init();

	dbg_info("USB Mass Storage device initializing...\n");

	udc = udc_init();
	if (!udc) {
		dbg_panic("UDC initialization failed\n");
		return 1;
	}

	usb_mode_task.udc = udc;
	usb_mode_task.msc_enabled = false;

	workqueue_task_init(&(usb_mode_task.task), app_usb_mode_worker);

	/* Check USB mode pin to see if USB should be attached or detached at
	 * startup. If MSC interface should not be enabled set workqueue to
	 * app_dataflash_ready, which will set the SPI interface in slave mode
	 * and release the XMEGA reset line.
	 */
	if (!board_gpio_is_usb_msc_mode()) {
		workqueue_task_set_work_func(&usb_mode_task.task,
				app_dataflash_ready);
	}

	dataflash_init(&usb_mode_task);
	mainloop_run(&main_workqueue);
}

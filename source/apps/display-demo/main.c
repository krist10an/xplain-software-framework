/**
 * \file
 *
 * \brief Display demo application.
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

/**
 * \mainpage Display demo
 * \section dd_intro Introduction
 *
 * The Display demo application is an application that shows different modules
 * working together:
 *
 * - \ref gfx "Graphics display system"
 * - \ref gfx_gfx "Graphics drivers"
 * - \ref gfx_win "Window system"
 * - \ref gfx_wtk "Widget toolkit"
 * - \ref touch_driver_group "Touch drivers"
 * - \ref spi_group "SPI drivers"
 * - \ref block_device_group "Block device driver"
 * - \ref tsfs_group "Tiny Simple File system"
 *
 * For the individual applications available see \ref apps_group "Applications"
 */
#include <dma.h>
#include <led.h>
#include <board.h>
#include <mainloop.h>

#include <clk/sys.h>

#include <gfx/gfx.h>
#include <gfx/win.h>
#include <gfx/sysfont.h>

#ifdef CONFIG_TOUCH_RESISTIVE
#include <touch/touch.h>
#endif

#include "app_desktop.h"

#ifdef CONFIG_FS_TSFS
#include <spi.h>
#include <block/device.h>
#include <block/dataflash.h>
#include <fs/tsfs.h>

DECLARE_SPI_MASTER(CONFIG_APP_DATAFLASH_SPI_ID, my_master);
DECLARE_SPI_DEVICE(CONFIG_APP_DATAFLASH_SPI_ID, my_device);

static struct block_device     *bdev;
static struct workqueue_task   ready_task;
struct tsfs                    myfs;
struct spi_master              *master;
struct spi_device              *device;

static void tsfs_ready_callback(struct workqueue_task *task)
{
}

static void spi_ready_callback(struct workqueue_task *task)
{
	workqueue_task_set_work_func(&ready_task, tsfs_ready_callback);
	tsfs_init(&myfs, bdev, &ready_task);
}
#endif

int main(void)
{
	cpu_irq_enable();
	sysclk_init();
	dbg_init();
	board_init();
	led_activate(BOARD_LED0_ID);
	workqueue_init(&main_workqueue);
#ifdef CONFIG_TOUCH_RESISTIVE
	touch_init();
	touch_enable();
#endif
	gfx_init();
	membag_init(CPU_DMA_ALIGN);
	win_init();

#ifdef CONFIG_FS_TSFS
	master = spi_master_get_base(CONFIG_APP_DATAFLASH_SPI_ID, &my_master);
	device = spi_device_get_base(CONFIG_APP_DATAFLASH_SPI_ID, &my_device);

	spi_enable(CONFIG_APP_DATAFLASH_SPI_ID);

	spi_master_init(CONFIG_APP_DATAFLASH_SPI_ID, master);
	spi_master_setup_device(CONFIG_APP_DATAFLASH_SPI_ID, master, device,
			SPI_MODE_0, CONFIG_CPU_HZ, BOARD_DATAFLASH_SS);

	workqueue_task_init(&ready_task, spi_ready_callback);
	bdev = dataflash_blkdev_init(CONFIG_APP_DATAFLASH_SPI_ID, master, device,
			&ready_task);
#endif

	app_desktop_setup();

	mainloop_run(&main_workqueue);
}

/**
 * \file
 *
 * \brief DataFlash and USB configuration and initialization.
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
#include <usb/dev_mux.h>
#include <usb/udi_msc_bulk.h>
#include <block/dataflash.h>
#include "xplain-bc.h"

/**
 * \brief Initialize the USB interface and the DataFlash block driver.
 *
 * This function will initialize the SPI master, an SPI device, an USB
 * configuration and an USB interface. These instances are then used to setup
 * the DataFlash device, create a block device and eventually share the block
 * device through the USB mass-storage interface.
 */
void dataflash_init(struct app_usb_mode_task *usb_mode_task)
{
	struct udm_config               *config;
	struct udm_interface            *iface;
	struct block_device		*bdev;
	struct spi_master		*master =
			spi_master_get_base(SPI_ID, &usb_mode_task->master);
	struct spi_device		*device =
			spi_device_get_base(SPI_ID, &usb_mode_task->device);

	config = udm_create_config(1, 1);

	spi_enable(SPI_ID);
	spi_master_init(SPI_ID, master);
	spi_master_setup_device(SPI_ID, master, device, SPI_MODE_0,
			CONFIG_CPU_HZ, BOARD_DATAFLASH_SS);
	bdev = dataflash_blkdev_init(SPI_ID, master, device,
			&usb_mode_task->task);

	udm_config_set_bus_powered(config);
	udm_config_set_max_power(config, 42);

	iface = udi_msc_create_iface(bdev);
	udm_config_add_interface(config, iface);
}

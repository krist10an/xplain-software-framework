/**
 * \file
 *
 * \brief AT45 DataFlash(R) device driver
 *
 * Copyright (C) 2010 Atmel Corporation. All rights reserved.
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
#include <stdbool.h>
#include <stdint.h>
#include <spi.h>
#include <flash/at45_device.h>

static bool at45_wait_poll_status(struct at45_device *at45d);

static bool at45_wait_poll_done(struct at45_device *at45d)
{
	at45_deselect(at45d);
	if (at45_rsp_status_is_ready(at45d)) {
		at45d->next = NULL;
		return true;
	} else
		return at45_wait_poll_status(at45d);
}

static bool at45_wait_poll_cmd_done(struct at45_device *at45d)
{
	at45d->next = at45_wait_poll_done;
	at45_read_rsp(at45d, 1);
	return false;
}

static bool at45_wait_poll_status(struct at45_device *at45d)
{
	at45d->next = at45_wait_poll_cmd_done;
	at45_select(at45d);
	at45_cmd_read_status_reg(at45d);
	return false;
}

/**
 * \ingroup at45_device_group
 * \brief Actively wait for AT45 device to be ready
 *
 * This must be called over and overagain until it returns true.
 *
 * \param at45d AT45 device struct
 * \retval false Not done yet. The task supplied in at45_request will be
 *               resceduled when it's ready to be called again.
 * \retval true  Operation completed. Variables size, page_size and flags
 *               in \a at45 have now been populated.
 */
bool at45_wait_ready(struct at45_device *at45d)
{
	if (at45d->next)
		return at45d->next(at45d);
	else
		return at45_wait_poll_status(at45d);
}

/**
 * \ingroup at45_device_group
 * \defgroup at45_device_internals_group AT45 device driver internals
 *
 * \section at45_identify AT45 identification state machine
 *
 * \dot
	digraph at45_identify {
		size = "4, 7";

		node [shape = circle];
		INIT;
		FETCH_STATUS;
		GOT_STATUS;
		FETCH_ID;
		GOT_ID;
		DONE;

		INIT -> FETCH_STATUS [label="at45_cmd_read_status_reg"];
		FETCH_STATUS -> GOT_STATUS [label="at45_read_rsp"];
		GOT_STATUS -> FETCH_ID [label="at45_cmd_read_id"];
		GOT_STATUS -> GOT_STATUS [label="busy; at45_cmd_read_status_reg"];
		FETCH_ID -> GOT_ID [label="at45_read_rsp"];
		GOT_ID -> DONE [label=""];
	}
 * \enddot
 */

static bool at45_identify_got_id(struct at45_device *at45d)
{
	at45_deselect(at45d);
	spi_release_bus(at45d->spim);
	dbg_info("at45_device: DataFlash information:\n"
			"  Manufacturer ID   : 0x%02x\n"
			"  Device ID (part 1): 0x%02x\n"
			"  Device ID (part 2): 0x%02x\n"
			"  Extended info     : 0x%02x\n",
			at45d->cmdrsp[0], at45d->cmdrsp[1],
			at45d->cmdrsp[2], at45d->cmdrsp[3]);
	if (at45_is_valid_id(at45d->cmdrsp[0], at45d->cmdrsp[1])) {
		set_bit(AT45_FLAG_VALID, &at45d->flags);
		at45d->size = at45_get_size(at45d->cmdrsp[1]);
		if (at45d->size >= (1024L * 1024)) {
			dbg_info("  Flash size        : %ld MiB\n",
					at45d->size / (1024L * 1024));
		} else {
			dbg_info("  Flash size        : %ld KiB\n",
					at45d->size / 1024);
		}
		at45d->page_size = at45_get_page_size(at45d->cmdrsp[1]);
		dbg_info("  Page size         : %d\n", at45d->page_size);
	} else {
		dbg_warning("at45_device: No valid dataflash detected!\n");
	}
	at45d->next = NULL;
	return true;
}

static bool at45_identify_fetch_id(struct at45_device *at45d)
{
	at45d->next = at45_identify_got_id;
	at45_read_rsp(at45d, 4);
	return false;
}

static bool at45_identify_fetch_status(struct at45_device *at45d);

static bool at45_identify_got_status(struct at45_device *at45d)
{
	if (!at45_rsp_status_is_ready(at45d))
		return at45_identify_fetch_status(at45d);

	at45d->next = at45_identify_fetch_id;
	at45_deselect(at45d);
	dbg_info("at45_device: Status register: 0x%02x\n", at45d->cmdrsp[0]);
	if (at45_rsp_status_is_protected(at45d))
		set_bit(AT45_FLAG_PROTECTED, &at45d->flags);
	at45_select(at45d);
	at45_cmd_read_id(at45d);
	return false;
}

static bool at45_identify_fetch_status(struct at45_device *at45d)
{
	at45d->next = at45_identify_got_status;
	at45_read_rsp(at45d, 1);
	return false;
}

static bool at45_identify_start(struct at45_device *at45d)
{
	at45d->next = at45_identify_fetch_status;
	at45_select(at45d);
	at45_cmd_read_status_reg(at45d);
	return false;
}

/**
 * \ingroup at45_device_group
 * \brief Identify AT45 device
 *
 * Extract status and device ID from device and process it.
 *
 * \param at45d AT45 device struct
 * \retval false Not done yet. The task supplied in at45_request will be
 *               resceduled when it's ready to be called again.
 * \retval true  Operation completed. Variables at45d::size, at45d::page_size
 *               and at45d::flags have now been populated.
 */
bool at45_identify(struct at45_device *at45d)
{
	if (at45d->next)
		return at45d->next(at45d);
	else
		return at45_identify_start(at45d);
}

/**
 * \ingroup at45_device_group
 * \brief Initialize AT45 struct
 *
 * \param at45d  AT45 device struct
 * \param spi_id \ref spi_module_id
 * \param spim   SPI master struct
 * \param spid   SPI device struct
 */
void at45_device_init(struct at45_device *at45d, spi_id_t spi_id,
		struct spi_master *spim, struct spi_device *spid)
{
	at45d->spi_id = spi_id;
	at45d->spim = spim;
	at45d->spid = spid;
	at45d->next = NULL;
	at45d->flags = 0;
}


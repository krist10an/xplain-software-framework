/**
 * \file
 *
 * \brief AT45 DataFlash(R) Device Driver
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
#ifndef FLASH_AT45_DEVICE_H_INCLUDED
#define FLASH_AT45_DEVICE_H_INCLUDED

#include <flash/at45.h>

/**
 * \defgroup at45_device_group AT45 DataFlash Device Driver
 *
 * This is a driver for AT45 DataFlash devices.
 *
 * It's initalized by at45_device_init() with a pre-allocated at45_device and
 * SPI ID, master and device references.
 *
 * Exclusive access is requested with at45_request(), and on access granted the
 * supplied workqueue_task is scheduled to run and it's possible to call
 * different access methods. These access methods can initiate some driver
 * action which will re-schedule the task when ready.
 *
 * Exclusive access must be ended with at45_release(). And this will start any
 * other pending requests, or set it available.
 *
 * @{
 */

//! AT45 device flags
enum at45_device_flag {
	AT45_FLAG_VALID,     //!< Valid AT45 device detected
	AT45_FLAG_PROTECTED, //!< Device is protected from write operations
};

// Forward reference
struct at45_device;

/**
 * \internal
 * Next function definition for chained calls
 */
typedef bool (*at45_next_call_t)(struct at45_device *at45d);

//! AT45 DataFlash device
struct at45_device {
	//! SPI master struct
	struct spi_master     *spim;
	//! SPI device struct
	struct spi_device     *spid;
	//! \ref spi_module_id
	spi_id_t              spi_id;
	//! \internal Next call to be made for chained operations
	at45_next_call_t      next;
	//! Device size
	uint32_t              size;
	//! Device page size
	uint16_t              page_size;
	//! \ref at45_device_flag
	uint8_t               flags;
	/**
	 * \brief Temporary storage for command and response
	 *
	 * Storage to be used for command and response data
	 * Largest used command or response is 5 bytes
         */
	unsigned char         cmdrsp[5];
};

/**
 * \brief Request exclusive access to AT45 device
 *
 * Once exclusive access is granted the \a task is scheduled to run.
 *
 * Exclusive access must be ended with at45_release() in order for other
 * requests to be run.
 *
 * \param at45d AT45 device struct
 * \param task  Task to run when ready
 */
static inline void at45_request(struct at45_device *at45d,
		struct workqueue_task *task)
{
	spi_request_bus(at45d->spim, task);
}

/**
 * \brief Release exclusive access to AT45 device
 *
 * \param at45d AT45 device struct
 * \pre Can only be called when exclusive access have been gained with
 *      at45_request
 */
static inline void at45_release(struct at45_device *at45d)
{
	spi_release_bus(at45d->spim);
}

/**
 * \brief Select AT45 device (Chip select)
 *
 * \param at45d AT45 device struct
 * \pre Can only be called when exclusive access have been gained with
 *      at45_request
 */
static inline void at45_select(struct at45_device *at45d)
{
	spi_select_device(at45d->spi_id, at45d->spim, at45d->spid);
}

/**
 * \brief De-select AT45 device (Chip de-select)
 *
 * \param at45d AT45 device struct
 * \pre Can only be called when exclusive access have been gained with
 *      at45_request
 */
static inline void at45_deselect(struct at45_device *at45d)
{
	spi_deselect_device(at45d->spi_id, at45d->spim, at45d->spid);
}

/**
 * \brief Write command and other opcodes to AT45 device
 *
 * This will write the command from at45::cmdrsp to the AT45 device.
 *
 * When the command have been written the task will be rescheduled.
 *
 * \param at45d AT45 device struct
 * \param size  Size to be written
 * \pre Can only be called when exclusive access have been gained with
 *      at45_request
 */
static inline void at45_write_cmd(struct at45_device *at45d, uint8_t size)
{
	assert(size <= sizeof(at45d->cmdrsp));

	spi_write(at45d->spi_id, at45d->spim, at45d->cmdrsp, size);
}

/**
 * \brief Read response from AT45 device
 *
 * This will read the response from the AT45 device into at45_device::cmdrsp .
 *
 * When the response have been read the task will be rescheduled.
 *
 * \param at45d AT45 device struct
 * \param size  Size to be read
 * \pre Can only be called when exclusive access have been gained with
 *      at45_request
 */
static inline void at45_read_rsp(struct at45_device *at45d, uint8_t size)
{
	assert(size <= sizeof(at45d->cmdrsp));

	spi_read(at45d->spi_id, at45d->spim, at45d->cmdrsp, size);
}

/**
 * \brief Write AT45 device command: read status register
 *
 * \param at45d AT45 device struct
 * \pre Can only be called when exclusive access have been gained with
 *      at45_request
 */
static inline void at45_cmd_read_status_reg(struct at45_device *at45d)
{
	at45d->cmdrsp[0] = AT45_CMD_READ_STATUS_REG;
	at45_write_cmd(at45d, 1);
}

/**
 * \brief Write AT45 device command: read ID
 *
 * \param at45d AT45 device struct
 * \pre Can only be called when exclusive access have been gained with
 *      at45_request
 */
static inline void at45_cmd_read_id(struct at45_device *at45d)
{
	at45d->cmdrsp[0] = AT45_CMD_READ_ID;
	at45_write_cmd(at45d, 1);
}

/**
 * \brief Write AT45 device command: continous array read
 *
 * \param at45d AT45 device struct
 * \param page  Page address
 * \param pos   Position in page
 * \pre Can only be called when exclusive access have been gained with
 *      at45_request
 */
static inline void at45_cmd_cont_array_read(struct at45_device *at45d, uint16_t page,
		uint16_t pos)
{
	assert(!(page & ~AT45_PAGE_ADDR_MASK));
	assert(!(pos & ~AT45_PAGE_POS_MASK));

	at45d->cmdrsp[0] = AT45_CMD_CONTINOUS_ARRAY_READ;
	/* 24-bit address split between 13-bits page and 11-bits position in
	 * page.
	 */
	at45d->cmdrsp[1] = page >> 5;
	at45d->cmdrsp[2] = (page << 3) | (pos >> 8);
	at45d->cmdrsp[3] = pos;
	at45d->cmdrsp[4] = 0; // Dummy byte required for this command
	at45_write_cmd(at45d, 5);
}

/**
 * \brief Write AT45 device command: buffer 1 write
 *
 * \param at45d AT45 device struct
 * \param pos   Position in page
 * \pre Can only be called when exclusive access have been gained with
 *      at45_request
 */
static inline void at45_cmd_buffer_1_write(struct at45_device *at45d, uint16_t pos)
{
	assert(!(pos & ~AT45_PAGE_POS_MASK));

	at45d->cmdrsp[0] = AT45_CMD_BUFFER_1_WRITE;
	/* 24-bit address split between 13-bits don't care and 11-bits position
	 * in page.
	 */
	at45d->cmdrsp[2] = pos >> 8;
	at45d->cmdrsp[3] = pos;
	at45_write_cmd(at45d, 4);
}

/**
 * \brief Write AT45 device command: main memory to buffer 1 transfer
 *
 * \param at45d AT45 device struct
 * \param page  Page address
 * \pre Can only be called when exclusive access have been gained with
 *      at45_request
 */
static inline void at45_cmd_main_memory_to_buffer_1_transfer(struct at45_device *at45d,
		uint16_t page)
{
	assert(!(page & ~AT45_PAGE_ADDR_MASK));

	at45d->cmdrsp[0] = AT45_CMD_MAIN_MEMORY_TO_BUFFER_1_TRANSFER;
	// 24-bit address split between 13-bits page and 11-bits don't care
	at45d->cmdrsp[1] = page >> 5;
	at45d->cmdrsp[2] = page << 3;
	at45_write_cmd(at45d, 4);
}

/**
 * \brief Write AT45 device command: buffer 1 main memory program with erase
 *
 * \param at45d AT45 device struct
 * \param page  Page address
 * \pre Can only be called when exclusive access have been gained with
 *      at45_request
 */
static inline void at45_cmd_buffer_1_main_memory_program_with_erase(
		struct at45_device *at45d, uint16_t page)
{
	assert(!(page & ~AT45_PAGE_ADDR_MASK));

	at45d->cmdrsp[0] = AT45_CMD_BUFFER_1_MAIN_MEMORY_PROGRAM_WITH_ERASE;
	// 24-bit address split between 13-bits page and 11-bits don't care
	at45d->cmdrsp[1] = page >> 5;
	at45d->cmdrsp[2] = page << 3;
	at45_write_cmd(at45d, 4);
}

/**
 * \brief Read from AT45 device into buffer list
 *
 * \param at45d    AT45 device struct
 * \param buf_list List of buffer%s
 * \pre Can only be called when exclusive access have been gained with
 *      at45_request
 */
static inline void at45_read_buf_list(struct at45_device *at45d,
		struct slist *buf_list)
{
	spi_read_buf_list(at45d->spi_id, at45d->spim, buf_list);
}

/**
 * \brief Write from buffer list into AT45 device
 *
 * \param at45d    AT45 device struct
 * \param buf_list List of buffer%s
 * \pre Can only be called when exclusive access have been gained with
 *      at45_request
 */
static inline void at45_write_buf_list(struct at45_device *at45d,
		struct slist *buf_list)
{
	spi_write_buf_list(at45d->spi_id, at45d->spim, buf_list);
}

/**
 * \brief Test if AT45 device status register bit is set to ready
 *
 * \param at45d AT45 device struct
 * \pre Status register must have been read with at45_cmd_read_status_register
 *      first
 */
static inline bool at45_rsp_status_is_ready(struct at45_device *at45d)
{
	return at45d->cmdrsp[0] & (1 << AT45_STATUS_RDY);
}

/**
 * \brief Test if AT45 device status register bit is set to protected
 *
 * \param at45d AT45 device struct
 * \pre Status register must have been read with at45_cmd_read_status_register
 *      first
 */
static inline bool at45_rsp_status_is_protected(struct at45_device *at45d)
{
	return at45d->cmdrsp[0] & (1 << AT45_STATUS_PROTECT);
}

bool at45_wait_ready(struct at45_device *at45d);
bool at45_identify(struct at45_device *at45d);
void at45_device_init(struct at45_device *at45d, spi_id_t spi_id,
		struct spi_master *spim, struct spi_device *spid);

//! @}
#endif /* FLASH_AT45_DEVICE_H_INCLUDED */

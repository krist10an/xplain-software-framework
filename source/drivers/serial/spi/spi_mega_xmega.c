/**
 * \file
 *
 * \brief Common ATmega and ATxmega SPI driver parts.
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
#include <bitops.h>
#include <spi.h>
#include <board/spi.h>

/**
 * \addtogroup spi_group
 * \section spi_poll_loop_group SPI poll loops
 *
 * The internals of this SPI driver uses polling from a workqueue_task, and
 * this can be controlled with the configuration symbol
 * CONFIG_SPI_POLL_MAXLOOPS
 *
 * If unconfigured, the default value is 255
 */

/**
 * \addtogroup spi_mega_xmega_internal_group
 * @{
 */

/**
 * \brief Maximum number of polling loops
 *
 * This is the maximum number of loops run in the polling function before
 * it must reschedule itself.
 *
 */
#ifdef CONFIG_SPI_POLL_MAXLOOPS
# define MAXLOOPS CONFIG_SPI_POLL_MAXLOOPS
#else
# define MAXLOOPS 255
#endif

/**
 * \internal
 * \brief Defintion of poll loop counter variable
 *
 * This helps the compiler in optimizing the loop to be as fast as possible
 */
#if MAXLOOPS < 256
typedef uint8_t maxloops_t;
#else
typedef unsigned int maxloops_t;
#endif

/**
 * \internal
 * \brief Poll SPI registers for required actions
 *
 * \param task Polling task structure
 *
 * \note
 * This poll function flow is optimized for best performance with the SPI
 * hardware. There are two critical paths that should be as short as possible:
 *
 * 1. Time between interrupt flag is set to write of data register for next
 *    transfer should be as short as possible. This to keep the time between
 *    transfers as short as possible.
 *
 * 2. Time between writing and checking the interrupt flag should be minimum
 *    16 cycles long. Maximum speed of spi is half of cpu clock, and with
 *    a 8-bit transfer this ideally should happen within 16 cpu cycles.
 */
static void spi_poll(struct workqueue_task *task)
{
	struct spi_master_polled *spim_poll =
		container_of(task, struct spi_master_polled, poll);
	struct spi_master        *spim = &spim_poll->base;
	uint8_t                  *read;
	const uint8_t            *write;
	uint8_t                  rx_byte;
	uint8_t                  tx_byte;
	maxloops_t               i;
	size_t                   residue;
	bool                     read_op;
	bool                     write_op;

	/* Make sure that a lot of the variables are located in
	 * registers in the loop.
	 */
	residue = spim->residue;
	read = spim_poll->read_data;
	write = spim_poll->write_data;
	read_op = test_bit(SPI_OP_READ, &spim_poll->op);
	write_op = test_bit(SPI_OP_WRITE, &spim_poll->op);
	i = MAXLOOPS;
	tx_byte = 0;

	// First loop handles receiving and sending bytes.
	while (residue > 1) {
		if (write_op)
			tx_byte = *write;
		while (i--) {
			if (spi_priv_is_int_flag_set(spim)) {
				rx_byte = spi_priv_read_data(spim);
				spi_priv_write_data(spim, tx_byte);
				residue--;
				if (read_op)
					*read++ = rx_byte;
				if (write_op)
					write++;
				break;
			}
		}
		if (!i)
			goto yield;
	}

	// Second loop handles receiving of last byte.
	while (i--) {
		if (spi_priv_is_int_flag_set(spim)) {
			rx_byte = spi_priv_read_data(spim);
			if (read_op)
				*read = rx_byte;
			if (spi_polled_is_buffer_op(spim)) {
				spi_polled_sched_next_buffer(spim);
				return;
			}
			spim->residue = 0;
			spim->status = STATUS_OK;
			workqueue_add_task(&main_workqueue,
					spim->nwq.current);
			return;
		}
	}

yield:
	spim->residue = residue;
	spim_poll->read_data = read;
	spim_poll->write_data = write;
	spi_polled_sched_poll(spim);
}

static void spi_priv_start(struct spi_master *spim,
		uint8_t tx_byte)
{
	spi_priv_write_data(spim, tx_byte);
}

void spi_priv_master_setup_device(spi_id_t spi_id, struct spi_device *device,
		spi_flags_t flags, unsigned long baud_rate,
		board_spi_select_id_t sel_id)
{
	spi_priv_master_setup_device_regs(device, flags, baud_rate);
	board_spi_init_select(&device->sel, sel_id);
}

void spi_priv_select_device(struct spi_master *master,
		struct spi_device *device)
{
	spi_priv_select_device_regs(master, device);
	board_spi_select_device(master, &device->sel);
}

void spi_priv_deselect_device(struct spi_master *master,
		struct spi_device *device)
{
	board_spi_deselect_device(master, &device->sel);
	spi_priv_deselect_device_regs(master, device);
}

void spi_priv_master_init(spi_id_t spi_id, struct spi_master *spim)
{
	spi_polled_master_init(spim, spi_poll, spi_priv_start);
	spi_priv_master_init_regs(spi_id, spim);
}

//! @}

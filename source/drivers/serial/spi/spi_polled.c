/**
 * \file
 *
 * \brief Generic SPI abstraction for polled operation
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
 * \addtogroup spi_polled_internal_group
 * @{
 */

/**
 * \brief Start polled SPI transfer
 *
 * \param spim    SPI master struct
 * \param tx_byte Initial byte to send
 * \param residue Length of remaining bytes
 */
void spi_polled_start(struct spi_master *spim, uint8_t tx_byte, size_t residue)
{
	struct spi_master_polled *spim_poll = spi_master_polled_of(spim);

	spim->residue = residue;
	spim->status = OPERATION_IN_PROGRESS;
	spim_poll->start(spim, tx_byte);
	spi_polled_sched_poll(spim);
}

/**
 *  \internal
 *  \see spi_write
 */
void spi_polled_write(struct spi_master *spim, const uint8_t *data, size_t len)
{
	struct spi_master_polled *spim_poll = spi_master_polled_of(spim);

	spim_poll->op = 1 << SPI_OP_WRITE;
	spim_poll->write_data = data + 1;
	spi_polled_start(spim, *data, len);
}

/**
 *  \internal
 *  \see spi_read
 */
void spi_polled_read(struct spi_master *spim, uint8_t *data, size_t len)
{
	struct spi_master_polled *spim_poll = spi_master_polled_of(spim);

	spim_poll->op = 1 << SPI_OP_READ;
	spim_poll->read_data = data;
	spi_polled_start(spim, 0, len);
}

/**
 *  \internal
 *  \see spi_exchange
 */
void spi_polled_exchange(struct spi_master *spim, const uint8_t *write,
		uint8_t *read, size_t len)
{
	struct spi_master_polled *spim_poll = spi_master_polled_of(spim);

	spim_poll->op = (1 << SPI_OP_WRITE) | (1 << SPI_OP_READ);
	spim_poll->write_data = write + 1;
	spim_poll->read_data = read;
	spi_polled_start(spim, *write, len);
}

/**
 * \brief Initialize polled SPI master struct
 *
 * \param spim  SPI master struct
 * \param poll  Poll function
 * \param start Start function
 */
void spi_polled_master_init(struct spi_master *spim, workqueue_func_t poll,
		spi_start_func_t start)
{
	struct spi_master_polled *spim_poll = spi_master_polled_of(spim);

	nested_workqueue_init(&spim->nwq);
	workqueue_task_init(&spim_poll->poll, poll);
	spim_poll->start = start;
	spi_polled_buf_list_init(spim);
}

//! @}

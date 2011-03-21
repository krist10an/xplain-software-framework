/**
 * \file
 *
 * \brief Generic SPI abstraction for polled buf list operation
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
 * \brief Iterate to next buffer in SPI buffer list operation
 *
 * \param task Task struct
 */
void spi_polled_next_buffer(struct workqueue_task *task)
{
	struct spi_master_polled *spim_poll =
		container_of(task, struct spi_master_polled, poll_next_buffer);
	struct spi_master        *spim = &spim_poll->base;
	size_t                   len = 0;
	uint8_t                  tx_byte = 0;

	if (test_bit(SPI_OP_READ, &spim_poll->op)) {
		if (slist_node_is_last(spim_poll->read_buf_list,
					&spim_poll->read_buffer->node))
			goto done;
		spim_poll->read_buffer =
			buf_list_peek_next(spim_poll->read_buffer);
		spim_poll->read_data = spim_poll->read_buffer->addr.ptr;
		len = spim_poll->read_buffer->len;
	}
	if (test_bit(SPI_OP_WRITE, &spim_poll->op)) {
		if (slist_node_is_last(spim_poll->write_buf_list,
					&spim_poll->write_buffer->node))
			goto done;
		spim_poll->write_buffer =
			buf_list_peek_next(spim_poll->write_buffer);
		tx_byte = *(uint8_t *)spim_poll->write_buffer->addr.ptr;
		spim_poll->write_data =
			(uint8_t *)spim_poll->write_buffer->addr.ptr + 1;
		len = spim_poll->write_buffer->len;
	}
	spi_polled_start(spim, tx_byte, len);
	return;

done:
	spim->residue = 0;
	spim->status = STATUS_OK;
	workqueue_add_task(&main_workqueue, spim->nwq.current);
}

/**
 *  \internal
 *  \see spi_write_buf_list
 */
void spi_polled_write_buf_list(struct spi_master *spim, struct slist *buf_list)
{
	struct spi_master_polled *spim_poll = spi_master_polled_of(spim);
	struct buffer            *buffer;
	const uint8_t            *write;

	buffer = buf_list_peek_head(buf_list);
	write = buffer->addr.ptr;
	spim_poll->op = (1 << SPI_OP_WRITE) | (1 << SPI_OP_BUFFER);
	spim_poll->write_data = write + 1;
	spim_poll->write_buffer = buffer;
	spim_poll->write_buf_list = buf_list;
	spi_polled_start(spim, *write, buffer->len);
}

/**
 *  \internal
 *  \see spi_read_buf_list
 */
void spi_polled_read_buf_list(struct spi_master *spim, struct slist *buf_list)
{
	struct spi_master_polled *spim_poll = spi_master_polled_of(spim);
	struct buffer            *buffer;

	buffer = buf_list_peek_head(buf_list);
	spim_poll->op = (1 << SPI_OP_READ) | (1 << SPI_OP_BUFFER);
	spim_poll->read_data = buffer->addr.ptr;
	spim_poll->read_buffer = buffer;
	spim_poll->read_buf_list = buf_list;
	spi_polled_start(spim, 0, buffer->len);
}

/**
 *  \internal
 *  \see spi_exchange_buf_list
 */
void spi_polled_exchange_buf_list(struct spi_master *spim,
		struct slist *write_buf_list,
		struct slist *read_buf_list)
{
	struct spi_master_polled *spim_poll = spi_master_polled_of(spim);
	struct buffer            *write_buffer;
	struct buffer            *read_buffer;
	const uint8_t            *write;

	assert(!slist_is_empty(write_buf_list));
	assert(!slist_is_empty(read_buf_list));

	write_buffer = buf_list_peek_head(write_buf_list);
	read_buffer = buf_list_peek_head(read_buf_list);

	assert(write_buffer->len == read_buffer->len);

	write = write_buffer->addr.ptr;
	spim_poll->op = (1 << SPI_OP_WRITE) | (1 << SPI_OP_READ) |
		(1 << SPI_OP_BUFFER);
	spim_poll->write_data = write + 1;
	spim_poll->read_data = read_buffer->addr.ptr;
	spim_poll->write_buffer = write_buffer;
	spim_poll->write_buf_list = write_buf_list;
	spim_poll->read_buffer = write_buffer;
	spim_poll->read_buf_list = write_buf_list;
	spi_polled_start(spim, *write, write_buffer->len);
}

//! @}

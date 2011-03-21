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
#ifndef SPI_SPI_POLLED_H_INCLUDED
#define SPI_SPI_POLLED_H_INCLUDED

#include <bitops.h>
#include <board/spi.h>

/**
 * \ingroup spi_group
 * \defgroup spi_polled_internal_group Generic SPI polled internals
 *
 * @{
 */

//! Flags for SPI polled operation
enum spi_polled_op_flags {
	SPI_OP_WRITE,  //!< Write operation
	SPI_OP_READ,   //!< Read operation
	SPI_OP_BUFFER, //!< buffer list operation
};

/**
 * \brief SPI start transfer function
 *
 * \param spim    SPI master struct
 * \param tx_byte First byte to be written
 */
typedef void (*spi_start_func_t)(struct spi_master *spim, uint8_t tx_byte);

//! \brief Polled SPI master defintion
struct spi_master_polled {
	//! Base spi_master
	struct spi_master	base;
	//! Task to use for polling the spi hardware
	struct workqueue_task	poll;
	//! Virtual method for starting transfer
	spi_start_func_t	start;
	//! Ongoing operation bitfield (\ref spi_polled_op_flags)
	uint8_t			op;
	//! Current write data pointer
	const uint8_t		*write_data;
	//! Current read data pointer
	uint8_t			*read_data;
#ifdef CONFIG_SPI_BUF_LIST_API
	//! Current write buffer
	struct buffer		*write_buffer;
	//! Current read buffer
	struct buffer		*read_buffer;
	//! Write buffer list
	struct slist		*write_buf_list;
	//! Read buffer list
	struct slist		*read_buf_list;
	//! Task to use for iterating buffers in list
	struct workqueue_task	poll_next_buffer;
#endif
};

//! \brief Polled SPI device defintion
struct spi_device {
	//! Board specific select id
	struct board_spi_select	sel;
};

/**
 * \brief Return SPI master polled from SPI master
 *
 * \param base SPI master struct
 * \return SPI master polled struct
 */
static inline struct spi_master_polled *spi_master_polled_of(
		struct spi_master *base)
{
	return container_of(base, struct spi_master_polled, base);
}

void spi_polled_start(struct spi_master *spim, uint8_t tx_byte,
		size_t residue);

/**
 * \brief Test if polled SPI operation is using buffer list
 *
 * \param spim SPI master struct
 *
 * \retval true  Operation is buffer based
 * \retval false Operation is not buffer based
 */
static inline bool spi_polled_is_buffer_op(struct spi_master *spim)
{
#ifdef CONFIG_SPI_BUF_LIST_API
	struct spi_master_polled *spim_poll = spi_master_polled_of(spim);

	return test_bit(SPI_OP_BUFFER, &spim_poll->op);
#else
	return false;
#endif
}

/**
 * \brief Schedule SPI poll operation
 *
 * \param spim SPI master struct
 */
static inline void spi_polled_sched_poll(struct spi_master *spim)
{
	struct spi_master_polled *spim_poll = spi_master_polled_of(spim);

	workqueue_add_task(&main_workqueue, &spim_poll->poll);
}

void spi_polled_next_buffer(struct workqueue_task *task);

/**
 * \brief Schedule buffer list handling
 *
 * \param spim SPI master struct
 */
static inline void spi_polled_sched_next_buffer(struct spi_master *spim)
{
#ifdef CONFIG_SPI_BUF_LIST_API
	struct spi_master_polled *spim_poll = spi_master_polled_of(spim);

	workqueue_add_task(&main_workqueue, &spim_poll->poll_next_buffer);
#endif
}

void spi_polled_write(struct spi_master *spim, const uint8_t *data,
		size_t len);

void spi_polled_read(struct spi_master *spim, uint8_t *data, size_t len);

void spi_polled_exchange(struct spi_master *spim, const uint8_t *write,
		uint8_t *read, size_t len);

void spi_polled_write_buf_list(struct spi_master *spim,
		struct slist *buf_list);

void spi_polled_read_buf_list(struct spi_master *spim, struct slist *buf_list);

void spi_polled_exchange_buf_list(struct spi_master *spim,
		struct slist *write_buf_list,
		struct slist *read_buf_list);

/**
 * \brief Initialize polled SPI master
 *
 * \param spim SPI master struct
 */
static inline void spi_polled_buf_list_init(struct spi_master *spim)
{
#ifdef CONFIG_SPI_BUF_LIST_API
	struct spi_master_polled *spim_poll = spi_master_polled_of(spim);

	workqueue_task_init(&spim_poll->poll_next_buffer,
			spi_polled_next_buffer);
#endif
}

void spi_polled_master_init(struct spi_master *spim, workqueue_func_t poll,
		spi_start_func_t start);

//! @}
#endif /* SPI_SPI_POLLED_H_INCLUDED */

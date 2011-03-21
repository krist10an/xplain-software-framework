/**
 * \file
 *
 * \brief Board-specific SPI control
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
#ifndef BOARD_SPI_H_INCLUDED
#define BOARD_SPI_H_INCLUDED

#include <gpio.h>

//! \brief Board SPI select identifiers
enum board_spi_select_id {
	BOARD_DATAFLASH_SS = 13, // PORTB pin 5
	BOARD_XMEGA_SS = 8,      // PORTB pin 0
};

//! \brief Board SPI select identifier type
typedef enum board_spi_select_id board_spi_select_id_t;

//! \brief Board SPI select struct
struct board_spi_select {
	//! gpio pin used for SPI select
	gpio_pin_t	pin;
};

/**
 * \brief Board SPI select given device
 *
 * \param master SPI master struct
 * \param sel    Board SPI select struct
 */
static inline void board_spi_select_device(struct spi_master *master,
		struct board_spi_select *sel)
{
	gpio_set_value(sel->pin, false);
}

/**
 * \brief Board SPI deselect given device
 *
 * \param master SPI master struct
 * \param sel    Board SPI select struct
 */
static inline void board_spi_deselect_device(struct spi_master *master,
		struct board_spi_select *sel)
{
	gpio_set_value(sel->pin, true);
}

/**
 * \brief Init Board SPI select
 *
 * \param sel    Board SPI select struct
 * \param sel_id Board SPI select identifier
 */
static inline void board_spi_init_select(struct board_spi_select *sel,
		board_spi_select_id_t sel_id)
{
	sel->pin = sel_id;
	port_select_gpio_pin(sel_id, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
}

#endif /* BOARD_SPI_H_INCLUDED */

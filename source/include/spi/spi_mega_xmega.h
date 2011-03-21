/**
 * \file
 *
 * \brief ATmega and ATxmega SPI API.
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
#ifndef SPI_SPI_MEGA_XMEGA_H_INCLUDED
#define SPI_SPI_MEGA_XMEGA_H_INCLUDED

#include <assert.h>
#include <stdbool.h>
#include <buffer.h>
#include <board/spi.h>

/**
 * \ingroup spi_polled_internal_group
 * \defgroup spi_mega_xmega_internal_group Common ATmega and ATxmega SPI \
 * internals
 * @{
 */

/**
 * \def SPI_MASTER_NATIVE_TYPE
 * \brief Type defintion for spi_master derivation
 */

/**
 * \def SPI_MASTER_NATIVE_GET_BASE(spim_p)
 * \brief Get base definition of spi_master
 *
 * \param spim_p Pointer to private derived spi_master
 */

/**
 * \def SPI_DEVICE_NATIVE_TYPE
 * \brief Type defintion for native spi_device derivation
 */

/**
 * \def SPI_DEVICE_NATIVE_GET_BASE(spid_p)
 * \brief Get base definition of native spi_device
 *
 * \param spid_p Pointer to spi_device_priv
 */

/**
 * \fn inline void spi_priv_enable(spi_id_t spi_id)
 *
 * \brief Private SPI enable
 *
 * \param spi_id \ref spi_module_id
 */

/**
 * \fn inline void spi_priv_disable(spi_id_t spi_id)
 *
 * \brief Private SPI disable
 *
 * \param spi_id \ref spi_module_id
 */

/**
 * \fn inline bool spi_priv_is_enabled(spi_id_t spi_id)
 *
 * \brief Private SPI is enabled
 *
 * \param spi_id \ref spi_module_id
 *
 * \retval true  SPI is enabled
 * \retval false SPI is disabled
 */

/**
 * \fn inline bool spi_priv_is_int_flag_set(struct spi_master *spim)
 *
 * \brief Test if SPI interrupt flag is set
 *
 * \param spim SPI master struct
 *
 * \retval true  Interrupt flag is set
 * \retval false Interrupt flag is not set
 */

/**
 * \fn inline uint8_t spi_priv_read_data(struct spi_master *spim)
 *
 * \brief SPI read data register
 *
 * \param spim SPI master struct
 *
 * \return Value from data register
 */

/**
 * \fn inline void spi_priv_write_data(struct spi_master *spim, uint8_t data)
 *
 * \brief SPI write data register
 *
 * \param spim SPI master struct
 * \param data Data byte to write
 */

/**
 * \fn inline void spi_priv_master_setup_device_regs(struct spi_device *device,
 *     spi_flags_t flags, unsigned long baud_rate)
 *
 * \brief Setup SPI device register specifics
 *
 * \param device    SPI device struct
 * \param flags     Flags
 * \param baud_rate Baud rate
 */

/**
 * \fn inline void spi_priv_select_device_regs(struct spi_master *spim,
 *     struct spi_device *device)
 *
 * \brief Select SPI device register specifics
 *
 * \param spim   SPI master struct
 * \param device SPI device struct
 */

/**
 * \fn inline void spi_priv_deselect_device_regs(struct spi_master *spim,
 *     struct spi_device *device)
 *
 * \brief Deselect SPI device register specifics
 *
 * \param spim   SPI master struct
 * \param device SPI device struct
 */

/**
 * \fn inline void spi_priv_master_init_regs(spi_id_t spi_id,
 *     struct spi_master *spim)
 *
 * \brief Init SPI master register specifics
 *
 * \param spi_id \ref spi_module_id
 * \param spim   SPI master struct
 */

/**
 * \brief Private init SPI master
 *
 * \param spi_id \ref spi_module_id
 * \param master SPI master struct
 */
void spi_priv_master_init(spi_id_t spi_id, struct spi_master *master);

/**
 * \brief Private setup SPI device
 *
 * \param spi_id    \ref spi_module_id
 * \param device    SPI device struct
 * \param flags     Flags
 * \param baud_rate Baud rate
 * \param sel_id    Board select id
 */
void spi_priv_master_setup_device(spi_id_t spi_id, struct spi_device *device,
		spi_flags_t flags, unsigned long baud_rate,
		board_spi_select_id_t sel_id);

/**
 * \brief Private select SPI device
 *
 * \param master SPI master struct
 * \param device SPI device struct
 */
void spi_priv_select_device(struct spi_master *master,
		struct spi_device *device);

/**
 * \brief Private deselect SPI device
 *
 * \param master SPI master struct
 * \param device SPI device struct
 */
void spi_priv_deselect_device(struct spi_master *master,
		struct spi_device *device);

/**
 * \def SPI_ID_LAST
 * \brief Last SPI ID
 */

/**
 * \def SPI_ID_UART_FIRST
 * \brief First SPI ID which is a uart
 */

/**
 * \brief Test if \ref spi_module_id is valid
 *
 * \param spi_id \ref spi_module_id
 *
 * \retval true Id is valid
 * \retval false Not a valid id
 */
#define spi_id_is_valid(spi_id) (spi_id <= SPI_ID_LAST)

/**
 * \def SPI_ID_NATIVE_IS_ENABLED
 * \brief true if native SPI is enabled
 */

/**
 * \def SPI_ID_UART_IS_ENABLED
 * \brief true if uart SPI is enabled
 */

/**
 * \brief Get spi_master derived type name from spi_id
 *
 * \param spi_id \ref spi_module_id
 */
#define SPI_MASTER_TYPE_NAME(spi_id)	spi_master_type##spi_id

/**
 * \brief Declare a spi_master derived struct for given \ref spi_module_id
 *
 * \param spi_id  \ref spi_module_id
 * \param varname Name to declare it as
 */
#define DECLARE_SPI_MASTER(spi_id, varname) \
	struct SPI_MASTER_TYPE_NAME(spi_id) varname

/**
 * \brief Multiplex statements based on \a spi_id
 *
 * This multiplexing definition is set up based on what SPI driver variants
 * have been enabled thru config. For a single variant any testing on spi_id is
 * bypassed, and for several spi_id is used for multiplexing.
 *
 * The statements in \a native and \a uart are a bit off in concern to
 * c-syntax, but contains the whole statement to be used for that variant.
 *
 * \param spi_id \ref spi_module_id
 * \param native Statement to run if it's native based
 * \param uart   Statement to run if it's uart based
 */
#if SPI_ID_NATIVE_IS_ENABLED && !SPI_ID_UART_IS_ENABLED
# define SPI_MUX(spi_id, native, uart) native
#elif SPI_ID_UART_IS_ENABLED && !SPI_ID_NATIVE_IS_ENABLED
# define SPI_MUX(spi_id, native, uart) uart
#elif SPI_ID_NATIVE_IS_ENABLED && SPI_ID_UART_IS_ENABLED
# define SPI_MUX(spi_id, native, uart)	\
	if (spi_id < SPI_ID_UART_FIRST)	\
		native;			\
	else				\
		uart
#else
# error No SPI ID enabled!
#endif

/**
 * \brief Get base defintion of spi_master
 *
 * \param spi_id \ref spi_module_id
 * \param priv   Pointer to private declared spi master
 */
static inline struct spi_master *spi_master_get_base(spi_id_t spi_id,
		void *priv)
{
	assert(spi_id_is_valid(spi_id));

	SPI_MUX(spi_id,
		return SPI_MASTER_NATIVE_GET_BASE(priv),
		return NULL);
}

/**
 * \brief Get spi_master derived struct size for the given \ref spi_module_id
 *
 * \param spi_id \ref spi_module_id
 */
static inline size_t spi_master_get_size(spi_id_t spi_id)
{
	assert(spi_id_is_valid(spi_id));

	SPI_MUX(spi_id,
		return sizeof(struct SPI_MASTER_NATIVE_TYPE),
		return 0);
}

/**
 * \brief Get spi_device derived type name from spi_id
 *
 * \param spi_id \ref spi_module_id
 */
#define SPI_DEVICE_TYPE_NAME(spi_id)	spi_device_type##spi_id

/**
 * \brief Declare a spi_device derived struct for given \ref spi_module_id
 *
 * \param spi_id  \ref spi_module_id
 * \param varname Name to declare it as
 */
#define DECLARE_SPI_DEVICE(spi_id, varname) \
	struct SPI_DEVICE_TYPE_NAME(spi_id) varname

/**
 * \brief Get base defintion of spi_device
 *
 * \param spi_id \ref spi_module_id
 * \param priv   Pointer to private declared SPI device struct
 */
static inline struct spi_device *spi_device_get_base(spi_id_t spi_id,
		void *priv)
{
	assert(spi_id_is_valid(spi_id));

	SPI_MUX(spi_id,
		return SPI_DEVICE_NATIVE_GET_BASE(priv),
		return NULL);
}

/**
 * \brief Get spi_device derived struct size for the given \ref spi_module_id
 *
 * \param spi_id \ref spi_module_id
 */
static inline size_t spi_device_get_size(spi_id_t spi_id)
{
	assert(spi_id_is_valid(spi_id));

	SPI_MUX(spi_id,
		return sizeof(struct SPI_DEVICE_NATIVE_TYPE),
		return 0);
}

static inline void spi_enable(spi_id_t spi_id)
{
	assert(spi_id_is_valid(spi_id));

	SPI_MUX(spi_id,
		spi_priv_enable(spi_id),
		do {} while (0));
}

static inline void spi_disable(spi_id_t spi_id)
{
	assert(spi_id_is_valid(spi_id));

	SPI_MUX(spi_id,
		spi_priv_disable(spi_id),
		do {} while (0));
}

static inline bool spi_is_enabled(spi_id_t spi_id)
{
	assert(spi_id_is_valid(spi_id));

	SPI_MUX(spi_id,
		return spi_priv_is_enabled(spi_id),
		return false);
}

static inline void spi_master_init(spi_id_t spi_id,
		struct spi_master *spi_master)
{
	SPI_MUX(spi_id,
		spi_priv_master_init(spi_id, spi_master),
		do {} while (0));
}

static inline void spi_master_setup_device(spi_id_t spi_id,
		struct spi_master *spi_master, struct spi_device *spi_device,
		spi_flags_t flags, unsigned long baud_rate,
		board_spi_select_id_t sel_id)
{
	SPI_MUX(spi_id,
		spi_priv_master_setup_device(spi_id, spi_device, flags,
				baud_rate, sel_id),
		do {} while (0));
}

static inline void spi_select_device(spi_id_t spi_id,
		struct spi_master *master, struct spi_device *device)
{
	SPI_MUX(spi_id,
		spi_priv_select_device(master, device),
		do {} while (0));
}

static inline void spi_deselect_device(spi_id_t spi_id,
		struct spi_master *master, struct spi_device *device)
{
	SPI_MUX(spi_id,
		spi_priv_deselect_device(master, device),
		do {} while (0));
}

static inline void spi_write(spi_id_t spi_id, struct spi_master *master,
		const uint8_t *data, size_t len)
{
	spi_polled_write(master, data, len);
}

static inline void spi_read(spi_id_t spi_id, struct spi_master *master,
		uint8_t *data, size_t len)
{
	spi_polled_read(master, data, len);
}

static inline void spi_exchange(spi_id_t spi_id, struct spi_master *master,
		const uint8_t *write, uint8_t *read, size_t len)
{
	spi_polled_exchange(master, write, read, len);
}

static inline void spi_write_buf_list(spi_id_t spi_id,
		struct spi_master *master, struct slist *buf_list)
{
	spi_polled_write_buf_list(master, buf_list);
}

static inline void spi_read_buf_list(spi_id_t spi_id,
		struct spi_master *master, struct slist *buf_list)
{
	spi_polled_read_buf_list(master, buf_list);
}

static inline void spi_exchange_buf_list(spi_id_t spi_id,
		struct spi_master *master, struct slist *write_buf_list,
		struct slist *read_buf_list)
{
	spi_polled_exchange_buf_list(master, write_buf_list, read_buf_list);
}

//! @}
#endif /* SPI_SPI_MEGA_XMEGA_H_INCLUDED */

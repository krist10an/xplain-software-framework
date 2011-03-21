/**
 * \file
 *
 * \brief Common SPI API.
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

#ifndef SPI_H_INCLUDED
#define SPI_H_INCLUDED

#include <status_codes.h>
#include <workqueue.h>
#include <interrupt.h>

/**
 * \defgroup spi_group Serial Peripheral Interface (SPI)
 * @{
 *
 * This is the common API for SPIs on AVRs. Additional features are available
 * in the documentation of the specific modules.
 *
 * An \ref spi_module_id starts with 0 which stands for the first available SPI
 * module on the device and the next SPI modules get increasing numbers. After
 * the "standard" SPI modules the emulated versions are listed (e.g. USART in
 * SPI mode or USI in SPI mode).
 *
 * To add the SPI master driver to the build, the configuration symbol
 * CONFIG_SPI_MASTER must be set.
 *
 * The symbols CONFIG_SPI_MASTER_INT or CONFIG_SPI_MASTER_DMA enable
 * interrupt or DMA driven communication for the SPI master, if available on
 * the device.
 *
 * \section spi_modes_section SPI modes
 * There are four combinations of serial clock phase and polarity with respect
 * to serial data. The table below shows the combinations and when data is
 * sampled or set up. The leading edge is the first clock edge in a clock
 * cycle. Trailing edge is the last clock edge in a clock cycle.
 *
 * <table>
 * 	<tr>
 * 		<th>SPI Mode</th>
 * 		<th>CPOL</th>
 * 		<th>CPHA</th>
 * 		<th>Leading Edge</th>
 * 		<th>Trailing Edge</th>
 * 	</tr>
 * 	<tr>
 * 		<td>0</td>
 * 		<td>0</td>
 * 		<td>0</td>
 * 		<td>Sample on rising edge</td>
 * 		<td>Setup on falling edge</td>
 * 	</tr>
 * 	<tr>
 * 		<td>1</td>
 * 		<td>0</td>
 * 		<td>1</td>
 * 		<td>Setup on rising edge</td>
 * 		<td>Sample on falling edge</td>
 * 	</tr>
 * 	<tr>
 * 		<td>2</td>
 * 		<td>1</td>
 * 		<td>0</td>
 * 		<td>Sample on falling edge</td>
 * 		<td>Setup on rising edge</td>
 * 	</tr>
 * 	<tr>
 * 		<td>3</td>
 * 		<td>1</td>
 * 		<td>1</td>
 * 		<td>Setup on falling edge</td>
 * 		<td>Sample on rising edge</td>
 * 	</tr>
 * </table>
 *
 */

/**
 * \brief Clock phase
 */
#define SPI_CPHA	(1 << 0)

/**
 * \brief Clock polarity
 */
#define SPI_CPOL	(1 << 1)

/**
 * \brief SPI mode 0
 */
#define SPI_MODE_0	0
/**
 * \brief SPI mode 1
 */
#define SPI_MODE_1	(SPI_CPHA)
/**
 * \brief SPI mode 2
 */
#define SPI_MODE_2	(SPI_CPOL)
/**
 * \brief SPI mode 3
 */
#define SPI_MODE_3	(SPI_CPOL | SPI_CPHA)

/**
 * \brief SPI master
 *
 * Not be used for declearing spi_master structure.
 * Use either DECLARE_SPI_MASTER or spi_master_get_size for allocation size.
 */
struct spi_master {
	//! Nested workqueue for pending work
	struct nested_workqueue	nwq;
	//! Status of last operation
	enum status_code 	status;
	//! Number of bytes remaining
	size_t			residue;
};

#include <chip/spi.h>

/**
 * \fn inline void spi_enable(spi_id_t spi_id)
 *
 * \brief Enable SPI module.
 *
 * The driver keeps a reference count that is increased each time this function
 * is called.
 *
 * \param spi_id \ref spi_module_id
 */

/**
 * \fn inline void spi_disable(spi_id_t spi_id)
 *
 * \brief Disable SPI module.
 *
 * The SPI is only disabled when it is not used any more. This is ensured by
 * an internal counter that is decreased each time this function is called.
 *
 * \param spi_id \ref spi_module_id
 */

/**
 * \fn inline bool spi_is_enabled(spi_id_t spi_id)
 *
 * \brief Check if SPI module is enabled.
 *
 * \param spi_id \ref spi_module_id
 */

/**
 * \fn void spi_master_init(spi_id_t spi_id, struct spi_master *master)
 *
 * \brief Initialize SPI module in master mode.
 *
 * \param spi_id \ref spi_module_id
 * \param master Pointer to SPI master struct.
 */

/**
 * \fn void spi_master_setup_device(spi_id_t spi_id, struct spi_master *master,
 * struct spi_device *device, spi_flags_t flags, unsigned long baud_rate,
 * board_spi_select_id_t sel_id)
 *
 * \brief Setup a SPI device.
 *
 * The returned device descriptor structure must be passed to the driver
 * whenever that device should be used as current slave device.
 *
 * \param spi_id    \ref spi_module_id
 * \param master    Pointer to SPI master structure.
 * \param device    Pointer to SPI device struct that should be initialized.
 * \param flags     SPI configuration flags. Common flags for all
 *                  implementations are the SPI modes SPI_MODE_0 ...
 *                  SPI_MODE_3.
 * \param baud_rate Baud rate for communication with slave device in Hz.
 * \param sel_id    Board specific seclet id
 */

/**
 * \brief Request use of SPI bus
 *
 * \param master SPI master
 * \param task   Task doing the SPI bus operations
 *
 * \retval true  The task was successfully queued
 * \retval false The task has already been queued, so nothing was done
 */
static inline bool spi_request_bus(struct spi_master *master,
		struct workqueue_task *task)
{
	return nested_workqueue_add_task(&master->nwq, task);
}

/**
 * \brief Release use of SPI bus
 *
 * \param master SPI master
 *
 * \pre This must only be used in workqueue task issued by driver
 */
static inline void spi_release_bus(struct spi_master *master)
{
	nested_workqueue_next_task(&master->nwq);
}

/**
 * \fn void spi_select_device(spi_id_t spi_id, struct spi_master *master,
 * struct spi_device *device)
 *
 * \brief Select given device on the SPI bus
 *
 * Set device specific setting and calls board chip select.
 *
 * \param spi_id \ref spi_module_id
 * \param master SPI master
 * \param device SPI device
 *
 * \pre This must only be used in workqueue task issued by driver
 */

/**
 * \fn void spi_deselect_device(spi_id_t spi_id, struct spi_master *master,
 * struct spi_device *device)
 *
 * \brief Deselect given device on the SPI bus
 *
 * Calls board chip deselect.
 *
 * \param spi_id \ref spi_module_id
 * \param master SPI master
 * \param device SPI device
 *
 * \pre This must only be used in workqueue task issued by driver
 * \pre SPI device must be selected with spi_select_device() first
 */

/**
 * \fn void spi_write(spi_id_t spi_id, struct spi_master *master,
 * const uint8_t *data, size_t len)
 *
 * \brief Send a sequence of bytes to a SPI device
 *
 * Received bytes on the SPI bus are discarded.
 *
 * \param spi_id \ref spi_module_id
 * \param master SPI master
 * \param data   data buffer to write
 * \param len    Length of data
 *
 * \pre This must only be used in workqueue task issued by driver
 * \pre SPI device must be selected with spi_select_device() first
 */

/**
 * \fn void spi_read(spi_id_t spi_id, struct spi_master *master,
 * uint8_t *data, size_t len)
 *
 * \brief Receive a sequence of bytes from a SPI device
 *
 * All bytes sent out on SPI bus are sent as value 0.
 *
 * \param spi_id \ref spi_module_id
 * \param master SPI master
 * \param data   data buffer to read
 * \param len    Length of data
 *
 * \pre This must only be used in workqueue task issued by driver
 * \pre SPI device must be selected with spi_select_device() first
 */

/**
 * \fn void spi_exchange(spi_id_t spi_id, struct spi_master *master,
 * const uint8_t *write, uint8_t *read, size_t len)
 *
 * \brief Send/Receive a sequence of bytes to/from a SPI device
 *
 * Send bytes from \a write buffer and receive bytes into \a read buffer.
 * Length field \a len are common for both.
 *
 * \param spi_id \ref spi_module_id
 * \param master SPI master
 * \param write  data buffer to write
 * \param read   data buffer to read
 * \param len    Length of data
 *
 * \pre This must only be used in workqueue task issued by driver
 * \pre SPI device must be selected with spi_select_device() first
 */

/**
 * \fn void spi_write_buf_list(spi_id_t spi_id, struct spi_master *master,
 * struct slist *buf_list)
 *
 * \brief Send bytes from a list of buffer%s to a SPI device
 *
 * Received bytes on the SPI bus are discarded.
 *
 * \param spi_id   \ref spi_module_id
 * \param master   SPI master
 * \param buf_list List of buffer%s send from
 *
 * \pre This must only be used in workqueue task issued by driver
 * \pre SPI device must be selected with spi_select_device() first
 */

/**
 * \fn void spi_read_buf_list(spi_id_t spi_id, struct spi_master *master,
 * struct slist *buf_list)
 *
 * \brief Receive bytes into a list of buffer%s from a SPI device.
 *
 * All bytes sent out on SPI bus are sent as value 0.
 *
 * \param spi_id   \ref spi_module_id
 * \param master   SPI master
 * \param buf_list List of buffer%s to receive into
 *
 * \pre This must only be used in workqueue task issued by driver
 * \pre SPI device must be selected with spi_select_device() first
 */

/**
 * \fn void spi_exchange_buf_list(spi_id_t spi_id, struct spi_master *master,
 * struct slist *write_buf_list, struct slist *read_buf_list)
 *
 * \brief Send/Receive a list of buffer%s to/from a SPI device
 *
 * Buffer lists must be symetrical in the sense of number of buffers
 * and buffer sizes.
 *
 * \param spi_id         \ref spi_module_id
 * \param master         SPI master
 * \param write_buf_list List of buffer%s to send from
 * \param read_buf_list  List of buffer%s to receive into
 *
 * \pre This must only be used in workqueue task issued by driver
 * \pre SPI device must be selected with spi_select_device() first
 */

/** @} */
#endif /* SPI_H_INCLUDED */

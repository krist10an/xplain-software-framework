/**
 * \file
 *
 * \brief AT90USB1287 specific SPI API
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
#ifndef CHIP_SPI_H_INCLUDED
#define CHIP_SPI_H_INCLUDED

#include <chip/regs.h>

/**
 * \addtogroup spi_group
 * \section spi_module_id SPI Module ID
 *
 * Overview of mapping between SPI module id (\ref spi_id_t) and peripheral:
 * - ID 0: SPI
 * - ID 1: UART 1 in SPI mode
 */

/**
 * \ingroup spi_mega_internal_group
 * \defgroup spi_at90usb1287_internal_group AT90USB1287 SPI internals
 *
 * @{
 */

#define SPI_ID_UART_FIRST		1
#define SPI_ID_LAST			1

#if defined(CONFIG_SPI0)
# define SPI_ID_NATIVE_IS_ENABLED	true
#else
# define SPI_ID_NATIVE_IS_ENABLED	false
#endif

#if defined(CONFIG_SPI1)
# define SPI_ID_UART_IS_ENABLED		true
#else
# define SPI_ID_UART_IS_ENABLED		false
#endif

#include <spi/spi_mega.h>
#include <spi/spi_mega_xmega.h>

//! \brief spi_master derived type connections
//! @{
#define spi_master_type0	SPI_MASTER_NATIVE_TYPE
//! @}

//! \brief spi_device derived type connections
//! @{
#define spi_device_type0	SPI_DEVICE_NATIVE_TYPE
//! @}

//! @}

#endif /* CHIP_SPI_H_INCLUDED */

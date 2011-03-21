/**
 * \file
 *
 * \brief ATxmega128A1 specific SPI API.
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

#include <regs/xmega_spi.h>
#include <chip/memory-map.h>
#include <clk/sys.h>

/**
 * \addtogroup spi_group
 * \section spi_module_id SPI Module ID
 *
 * Overview of mapping between SPI module id (\ref spi_id_t) and peripheral:
 * - ID 0: SPI on PORTC
 * - ID 1: SPI on PORTD
 * - ID 2: SPI on PORTE
 * - ID 3: SPI on PORTF
 * - ID 4: USART 0 on PORTC in SPI mode
 * - ID 5: USART 1 on PORTC in SPI mode
 * - ID 6: USART 0 on PORTD in SPI mode
 * - ID 7: USART 1 on PORTD in SPI mode
 * - ID 8: USART 0 on PORTE in SPI mode
 * - ID 9: USART 1 on PORTE in SPI mode
 * - ID 10: USART 0 on PORTF in SPI mode
 * - ID 11: USART 1 on PORTF in SPI mode
 */

/**
 * \ingroup spi_xmega_internal_group
 * \defgroup spi_atxmega128a1_internal_group ATxmega128A1 SPI internals
 * @{
 */

#define SPI_ID_UART_FIRST		4
#define SPI_ID_LAST			11

#if defined(CONFIG_SPI0) || defined(CONFIG_SPI1) || \
	defined(CONFIG_SPI2) || defined(CONFIG_SPI3)
# define SPI_ID_NATIVE_IS_ENABLED	true
#else
# define SPI_ID_NATIVE_IS_ENABLED	false
#endif

#if defined(CONFIG_SPI4) || defined(CONFIG_SPI5) || \
	defined(CONFIG_SPI6) || defined(CONFIG_SPI7) || \
	defined(CONFIG_SPI8) || defined(CONFIG_SPI9) || \
	defined(CONFIG_SPI10) || defined(CONFIG_SPI11)
# define SPI_ID_UART_IS_ENABLED		true
#else
# define SPI_ID_UART_IS_ENABLED		false
#endif

static inline void *spi_get_base(uint8_t spi_id)
{
	switch (spi_id) {
#ifdef CONFIG_SPI0
	case 0:
		return (void *)SPI0_BASE;
#endif
#ifdef CONFIG_SPI1
	case 1:
		return (void *)SPI1_BASE;
#endif
#ifdef CONFIG_SPI2
	case 2:
		return (void *)SPI2_BASE;
#endif
#ifdef CONFIG_SPI3
	case 3:
		return (void *)SPI3_BASE;
#endif
	default:
		unhandled_case(spi_id);
		return NULL;
	}
}

static inline uint8_t spi_get_sysclk_port(uint8_t spi_id)
{
	switch (spi_id) {
	case 0:
		return SYSCLK_PORT_C;
	case 1:
		return SYSCLK_PORT_D;
	case 2:
		return SYSCLK_PORT_E;
	case 3:
		return SYSCLK_PORT_F;
	default:
		unhandled_case(spi_id);
		return 0;
	}
}

#include <spi/spi_xmega.h>
#include <spi/spi_mega_xmega.h>

//! \name spi_master derived type connections
//! @{
#define spi_master_type0	SPI_MASTER_NATIVE_TYPE
#define spi_master_type1	SPI_MASTER_NATIVE_TYPE
#define spi_master_type2	SPI_MASTER_NATIVE_TYPE
#define spi_master_type3	SPI_MASTER_NATIVE_TYPE
//! @}

//! \name spi_device derived type connections
//! @{
#define spi_device_type0	SPI_DEVICE_NATIVE_TYPE
#define spi_device_type1	SPI_DEVICE_NATIVE_TYPE
#define spi_device_type2	SPI_DEVICE_NATIVE_TYPE
#define spi_device_type3	SPI_DEVICE_NATIVE_TYPE
//! @}

//! @}
#endif /* CHIP_SPI_H_INCLUDED */

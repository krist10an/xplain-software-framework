/**
 * \file
 *
 * \brief ATxmega SPI API.
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
#ifndef SPI_SPI_XMEGA_H_INCLUDED
#define SPI_SPI_XMEGA_H_INCLUDED

#include <clk/sys.h>
#include <spi/spi_polled.h>

/**
 * \ingroup spi_mega_xmega_internal_group
 * \defgroup spi_xmega_internal_group ATxmega SPI internals
 *
 * This is ATxmega specific low level handling for \ref
 * spi_mega_xmega_internal_group
 *
 * @{
 */

//! \brief Private SPI master defintion
struct spi_master_priv {
	//! Base spi_master
	struct spi_master_polled	base;
	//! SPI register base
	void				*regs;
};

#define SPI_MASTER_NATIVE_TYPE spi_master_priv

#define SPI_MASTER_NATIVE_GET_BASE(spim_p) \
	(&((struct spi_master_priv *)spim_p)->base.base)

//! Private SPI device definition
struct spi_device_priv {
	//! Base spi_device
	struct spi_device	base;
	//! Storage for precalculated SPI Control register
	uint8_t			ctrl;
};

#define SPI_DEVICE_NATIVE_TYPE spi_device_priv

#define SPI_DEVICE_NATIVE_GET_BASE(spid_p) \
	(&((struct spi_device_priv *)spid_p)->base)

//! \brief \ref spi_module_id
typedef uint8_t spi_id_t;

//! \brief SPI setup flags. \see spi_modes_section
typedef uint8_t spi_flags_t;

static inline struct spi_master_priv *spi_master_priv_of(
		struct spi_master *spim)
{
	return container_of(spim, struct spi_master_priv, base.base);
}

static inline struct spi_device_priv *spi_device_priv_of(
		struct spi_device *spid)
{
	return container_of(spid, struct spi_device_priv, base);
}

static inline void spi_priv_enable(spi_id_t spi_id)
{
	sysclk_enable_module(spi_get_sysclk_port(spi_id), SYSCLK_SPI);
	spi_write_reg(spi_get_base(spi_id), CTRL, SPI_BIT(CTRL_ENABLE));
}

static inline void spi_priv_disable(spi_id_t spi_id)
{
	spi_write_reg(spi_get_base(spi_id), CTRL, 0);
	sysclk_disable_module(spi_get_sysclk_port(spi_id), SYSCLK_SPI);
}

static inline bool spi_priv_is_enabled(spi_id_t spi_id)
{
	return spi_read_reg(spi_get_base(spi_id), CTRL) &
		SPI_BIT(CTRL_ENABLE);
}

static inline bool spi_priv_is_int_flag_set(struct spi_master *spim)
{
	struct spi_master_priv *spim_p = spi_master_priv_of(spim);

	return spi_read_reg(spim_p->regs, STATUS) & SPI_BIT(STATUS_IF);
}

static inline uint8_t spi_priv_read_data(struct spi_master *spim)
{
	struct spi_master_priv *spim_p = spi_master_priv_of(spim);

	return spi_read_reg(spim_p->regs, DATA);
}

static inline void spi_priv_write_data(struct spi_master *spim, uint8_t data)
{
	struct spi_master_priv *spim_p = spi_master_priv_of(spim);

	spi_write_reg(spim_p->regs, DATA, data);
}

static inline void spi_priv_master_setup_device_regs(struct spi_device *device,
		spi_flags_t flags, unsigned long baud_rate)
{
	struct spi_device_priv	*spid_p = spi_device_priv_of(device);
	uint8_t			i;
	uint32_t		prescaled_hz = CONFIG_CPU_HZ >> 1;
	uint8_t			ctrl;

	// Set enable bit, master bit and mode
	ctrl = SPI_BIT(CTRL_ENABLE) | SPI_BIT(CTRL_MASTER) |
		SPI_BF(CTRL_MODE, flags);

	for (i = 0; i < 7; i++) {
		if (prescaled_hz <= baud_rate)
			break;
		prescaled_hz >>= 1;
	}
	if (!(i & 1))
		ctrl |= SPI_BIT(CTRL_CLK2X);
	ctrl |= SPI_BF(CTRL_PRESCALER, i >> 1);

	spid_p->ctrl = ctrl;
}

static inline void spi_priv_select_device_regs(struct spi_master *spim,
		struct spi_device *device)
{
	struct spi_master_priv	*spim_p = spi_master_priv_of(spim);
	struct spi_device_priv	*spid_p = spi_device_priv_of(device);

	spi_write_reg(spim_p->regs, CTRL, spid_p->ctrl);
}

static inline void spi_priv_deselect_device_regs(struct spi_master *spim,
		struct spi_device *device)
{
}

static inline void spi_priv_master_init_regs(spi_id_t spi_id,
		struct spi_master *spim)
{
	struct spi_master_priv *spim_p = spi_master_priv_of(spim);

	spim_p->regs = spi_get_base(spi_id);

	spi_write_reg(spim_p->regs, CTRL,
		SPI_BIT(CTRL_ENABLE) | SPI_BIT(CTRL_MASTER));

	// Make sure any interrupt flag is cleared
	spi_read_reg(spim_p->regs, STATUS);
	spi_read_reg(spim_p->regs, DATA);
}

//! @}
#endif /* SPI_SPI_XMEGA_H_INCLUDED */

/**
 * \file
 *
 * \brief HX8347A driver backend using External Bus Interface
 *
 * Copyright (C) 2010 Atmel Corporation. All rights reserved.
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
#ifndef DRIVERS_GFX_HX8347A_HX8347A_EBI_H_INCLUDED
#define DRIVERS_GFX_HX8347A_HX8347A_EBI_H_INCLUDED

#include <board/hx8347a.h>
#include <bus/ebi/core.h>
#include <byteorder.h>
#include <hugemem.h>

#define HX_REG_INDEX    (GFX_HX8347A_BASE + 0)
#define HX_REG_CMD      (GFX_HX8347A_BASE + (1 << GFX_HX8347A_DNC_BIT))

static void hx_write_index(uint8_t address)
{
	mmio_write8((void *)HX_REG_INDEX, address);
}

static void hx_write_cmd8(uint8_t value)
{
	mmio_write8((void *)HX_REG_CMD, value);
}

static void hx_write_cmd16(uint16_t value)
{
	mmio_write16((void *)HX_REG_CMD, cpu_to_le16(value));
}

static uint8_t hx_read_cmd8(void)
{
	/* Always read 16 bits to ensure the correct byte lane is used */
	return mmio_read16((void *)HX_REG_CMD);
}

static void hx_dummy_read_cmd(void)
{
	hx_read_cmd8();
}

static void gfx_write_register(uint8_t address, uint8_t value)
{
	hx_write_index(address);
	hx_write_cmd8(value);
}

static uint8_t gfx_read_register(uint8_t address)
{
	hx_write_index(address);
	return hx_read_cmd8();
}

static void gfx_init_comms(void)
{
	ebi_enable_clock();
}

static void gfx_setup_interface(void)
{
	portmux_select_gpio_pin(GFX_TE_PIN, PORTMUX_DIR_INPUT);
	portmux_select_gpio_pin(GFX_BACKLIGHT_PIN,
			PORTMUX_DIR_OUTPUT | PORTMUX_INIT_HIGH);
	portmux_select_gpio_pin(GFX_RESET_PIN,
			PORTMUX_DIR_OUTPUT | PORTMUX_INIT_HIGH);
}

static gfx_color_t gfx_read_gram(void)
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;

	hx_write_index(HX8347A_SRAMWRITE);
	hx_dummy_read_cmd();

	red = hx_read_cmd8();
	green = hx_read_cmd8();
	blue = hx_read_cmd8();

	return gfx_color(red, green, blue);
}

static void gfx_write_gram(gfx_color_t color)
{
	hx_write_index(HX8347A_SRAMWRITE);
	hx_write_cmd16(color);
}

void gfx_duplicate_pixel(gfx_color_t color, uint32_t count)
{
	// Count should not exceed 24 bit, and not be zero.
	assert((count >> 24) == 0);
	assert(count > 0);

	hx_write_index(HX8347A_SRAMWRITE);
	while (count-- > 0)
		hx_write_cmd16(color);
}

void gfx_copy_pixels_to_screen(const gfx_color_t *pixels, uint32_t count)
{
	assert(pixels);
	assert(count > 0);

	hx_write_index(HX8347A_SRAMWRITE);
	while (count-- > 0)
		hx_write_cmd16(*pixels++);
}

void gfx_copy_progmem_pixels_to_screen(const gfx_color_t *pixels,
		uint32_t count)
{
	gfx_copy_pixels_to_screen(pixels, count);
}

void gfx_copy_hugemem_pixels_to_screen(const hugemem_ptr_t pixels,
		uint32_t count)
{
	assert(pixels);
	assert(count);

	hx_write_index(HX8347A_SRAMWRITE);

	while(count--) {
		uint16_t pixel = hugemem_read16(pixels);
		hx_write_cmd16(pixel);
	}
}

void gfx_copy_pixels_from_screen(gfx_color_t *pixels, uint32_t count)
{
	assert(pixels);
	assert(count > 0);

	hx_write_index(HX8347A_SRAMWRITE);
	hx_dummy_read_cmd();

	while (count-- > 0) {
		uint16_t red;
		uint16_t green;
		uint16_t blue;

		red = hx_read_cmd8();
		green = hx_read_cmd8();
		blue = hx_read_cmd8();

		*pixels++ = gfx_color(red, green, blue);
	}
}

#endif /* DRIVERS_GFX_HX8347A_HX8347A_EBI_H_INCLUDED */

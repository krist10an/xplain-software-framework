/**
 * \file
 *
 * \brief Implementation of graphics routines for HX8347A compatible displays
 *
 * Copyright (C) 2009 - 2010 Atmel Corporation. All rights reserved.
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
#include <clk/sys.h>
#include <gpio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <delay.h>
#include <assert.h>
#include <gfx/gfx.h>

#include "hx8347a_regs.h"

#if defined(CONFIG_CPU_XMEGA)
# include "hx8347a_xmega.h"
#elif defined(CONFIG_CPU_UC3)
# include "hx8347a_ebi.h"
#endif

#ifdef CONFIG_GFX_USE_CLIPPING
gfx_coord_t gfx_min_x;
gfx_coord_t gfx_min_y;
gfx_coord_t gfx_max_x;
gfx_coord_t gfx_max_y;
#endif

gfx_coord_t gfx_width;
gfx_coord_t gfx_height;


#define GFX_PANELWIDTH 240
#define GFX_PANELHEIGHT 320

//! \internal Read-modify-write shortcut to set bits in a register.
static void gfx_set_register(uint8_t address, uint8_t bitmask)
{
	uint8_t value;

	value = gfx_read_register(address);
	value |= bitmask;
	gfx_write_register(address, value);
}

//! \internal Read-modify-write shortcut to clear bits in a register.
static void gfx_clear_register(uint8_t address, uint8_t bitmask)
{
	uint8_t value;

	value = gfx_read_register(address);
	value &= ~bitmask;
	gfx_write_register(address, value);
}

//! \internal Reset display using digital control interface.
static void gfx_reset_display(void)
{
	gpio_set_value(GFX_RESET_PIN, false);
	udelay(50);
	gpio_set_value(GFX_RESET_PIN, true);
	udelay(5000);
}


//! \internal Send display commands to exit standby mode.
static void gfx_exit_standby(void)
{
	gfx_set_register(HX8347A_OSCCTRL1, 0x3A | (1 << HX8347A_OSC_EN));
	udelay(50000); // Actually 10ms at least.
	gfx_clear_register(HX8347A_POWERCTRL1, (1 << HX8347A_STB));
}


//! \internal Full display init function as described in EDT appnote
static void gfx_display_init_all(void)
{
	// Undocumented values, received from display manufacturer.
	gfx_write_register(HX8347A_GAMMACTRL1, 0x94);
	gfx_write_register(HX8347A_GAMMACTRL2, 0x41);
	gfx_write_register(HX8347A_GAMMACTRL3, 0x00);
	gfx_write_register(HX8347A_GAMMACTRL4, 0x33);
	gfx_write_register(HX8347A_GAMMACTRL5, 0x23);
	gfx_write_register(HX8347A_GAMMACTRL6, 0x45);
	gfx_write_register(HX8347A_GAMMACTRL7, 0x44);
	gfx_write_register(HX8347A_GAMMACTRL8, 0x77);
	gfx_write_register(HX8347A_GAMMACTRL9, 0x12);
	gfx_write_register(HX8347A_GAMMACTRL10, 0xCC);
	gfx_write_register(HX8347A_GAMMACTRL11, 0x46);
	gfx_write_register(HX8347A_GAMMACTRL12, 0x82);

	gfx_write_register(HX8347A_DISPMODECTRL, 0x06);

	// Not actually gamma, but driving timing,
	// required for noise removal for our LCD.
	gfx_write_register(HX8347A_CYCLECTRL1, 0x01);
	gfx_write_register(HX8347A_CYCLECTRL2, 0x01);
	gfx_write_register(HX8347A_CYCLECTRL3, 0xF0);
	gfx_write_register(HX8347A_CYCLECTRL4, 0x00);
	gfx_write_register(HX8347A_CYCLECTRL5, 0x38);
	gfx_write_register(HX8347A_CYCLECTRL6, 0x0F);
	gfx_write_register(HX8347A_CYCLECTRL7, 0xF0);

	gfx_write_register(HX8347A_DISPCTRL2, 0x02);
	gfx_write_register(HX8347A_DISPCTRL3, 0x02);
	gfx_write_register(HX8347A_DISPCTRL4, 0x02);
	gfx_write_register(HX8347A_DISPCTRL5, 0x02);
	gfx_write_register(HX8347A_DISPCTRL6, 0x02);
	gfx_write_register(HX8347A_DISPCTRL7, 0x02);

	gfx_write_register(HX8347A_INTERNAL28, 0x0E);
	gfx_write_register(HX8347A_OSCCTRL1,   0x49);

	gfx_write_register(HX8347A_COLSTARTHIGH,  0x00);
	gfx_write_register(HX8347A_COLSTARTLOW,   0x00);
	gfx_write_register(HX8347A_COLENDHIGH,    0x00);
	gfx_write_register(HX8347A_COLENDLOW,     0xEF);
	gfx_write_register(HX8347A_ROWSTARTHIGH,  0x00);
	gfx_write_register(HX8347A_ROWSTARTLOW,   0x00);
	gfx_write_register(HX8347A_ROWENDHIGH,    0x01);
	gfx_write_register(HX8347A_ROWENDLOW,     0x3F);

	gfx_write_register(HX8347A_MEMACCESSCTRL, 0x08);

	gfx_write_register(HX8347A_CYCLECTRL1_X, 0x95);
	gfx_write_register(HX8347A_CYCLECTRL2_X, 0x95);
	gfx_write_register(HX8347A_CYCLECTRL3_X, 0xFF);
	gfx_write_register(HX8347A_DISPCTRL8, 0x7F);
	gfx_write_register(HX8347A_DISPCTRL9, 0x38);
	gfx_write_register(HX8347A_DISPCTRL10, 0x78);

	gfx_write_register(HX8347A_POWERCTRL3, 0x07);
	gfx_write_register(HX8347A_POWERCTRL4, 0x00);
	gfx_write_register(HX8347A_POWERCTRL5, 0x04);
	gfx_write_register(HX8347A_POWERCTRL6, 0x40);
	gfx_write_register(HX8347A_VCOMCTRL2,  0x38);
	gfx_write_register(HX8347A_VCOMCTRL3,  0x12);

	udelay(10000);
	gfx_write_register(HX8347A_POWERCTRL2, 0x04);
	udelay(20000);
	gfx_write_register(HX8347A_VCOMCTRL1,  0x80);
	udelay(5000);
	gfx_write_register(HX8347A_POWERCTRL1, 0x08);
	udelay(40000);
	gfx_write_register(HX8347A_POWERCTRL1, 0x10);
	udelay(40000);
	gfx_write_register(HX8347A_DISPCTRL1, 0x04);
	udelay(40000);
	gfx_write_register(HX8347A_DISPCTRL1, 0x24);
	gfx_write_register(HX8347A_DISPCTRL1, 0x2C);
	udelay(40000);
	gfx_write_register(HX8347A_DISPCTRL1, 0x3C);
}


void gfx_init(void)
{
	// Initialize LCD driver communications interface.
	gfx_init_comms();

	// Startup sequence according to datasheet.
	gfx_setup_interface();
	gfx_reset_display();
	gfx_exit_standby();

	// Init display according to appnote from EDT
	gfx_display_init_all();

	// Start off with standard orientation..
	gfx_set_orientation(GFX_DEFAULT_ORIENTATION);
}

void gfx_sync(void)
{
	// Intentionally empty, since this driver implementation is synchronous.
}

void gfx_set_clipping(gfx_coord_t min_x, gfx_coord_t min_y,
		gfx_coord_t max_x, gfx_coord_t max_y)
{
#ifdef CONFIG_GFX_USE_CLIPPING
	// Limit clipping region to within display panel boundaries.
	if (min_x < 0)
		min_x = 0;
	if (min_y < 0)
		min_y = 0;
	if (max_x >= gfx_width)
		max_x = gfx_width - 1;
	if (max_y >= gfx_height)
		max_y = gfx_height - 1;

	gfx_min_x = min_x;
	gfx_min_y = min_y;
	gfx_max_x = max_x;
	gfx_max_y = max_y;
#endif
}

void gfx_set_orientation(uint8_t flags)
{
	uint8_t setting = 0;
	uint8_t regval;

	setting |= (flags & GFX_FLIP_X ? GFX_HX_FLIP_X : 0);
	setting |= (flags & GFX_FLIP_Y ? GFX_HX_FLIP_Y : 0);
	setting |= (flags & GFX_SWITCH_XY ? GFX_HX_SWITCH_XY : 0);

	// Read-modify-write HIMAX control register.
	regval = gfx_read_register(HX8347A_MEMACCESSCTRL);
	regval &= ~(GFX_HX_FLIP_X | GFX_HX_FLIP_Y | GFX_HX_SWITCH_XY);
	regval |= setting;
	gfx_write_register(HX8347A_MEMACCESSCTRL, regval);

	// Switch width and height if XY is switched.
	if ((setting & GFX_HX_SWITCH_XY) != 0x00) {
		gfx_width = GFX_PANELHEIGHT;
		gfx_height = GFX_PANELWIDTH;
	} else {
		gfx_width = GFX_PANELWIDTH;
		gfx_height = GFX_PANELHEIGHT;
	}

#ifdef CONFIG_GFX_USE_CLIPPING
	// Reset clipping region.
	gfx_set_clipping(0, 0, gfx_width - 1, gfx_height - 1);
#endif
}

gfx_coord_t gfx_get_width(void)
{
	return gfx_width;
}

gfx_coord_t gfx_get_height(void)
{
	return gfx_height;
}

gfx_color_t gfx_color(uint8_t r, uint8_t g, uint8_t b)
{
	gfx_color_t color;
	uint16_t red = r >> 3;
	uint16_t green = g >> 2;
	uint16_t blue = b >> 3;

	// Stuff into one 16-bit word.
	red <<= (5 + 6);
	green <<= 5;
	color = red | green | blue;

	// Convert to big endian, to fit the display data format.
	color = (color >> 8) | (color << 8);

	return color;
}

gfx_color_t gfx_get_pixel(gfx_coord_t x, gfx_coord_t y)
{
	gfx_color_t color;

#ifdef CONFIG_GFX_USE_CLIPPING
	if ((x < gfx_min_x) || (x > gfx_max_x)
			|| (y < gfx_min_y) || (y > gfx_max_y))
		return GFX_COLOR_INVALID;
#endif

	// Set up draw area and read the three bytes of pixel data.
	gfx_set_limits(x, y, x, y);
	color = gfx_read_gram();

	return color;
}

void gfx_draw_pixel(gfx_coord_t x, gfx_coord_t y, gfx_color_t color)
{
#ifdef CONFIG_GFX_USE_CLIPPING
	if ((x < gfx_min_x) || (x > gfx_max_x)
			|| (y < gfx_min_y) || (y > gfx_max_y))
		return;
#endif

	// Set up draw area and write the two bytes of pixel data.
	gfx_set_limits(x, y, x, y);
	gfx_write_gram(color);
}

void gfx_draw_line_pixel(gfx_coord_t x, gfx_coord_t y, gfx_color_t color)
{
#ifdef CONFIG_GFX_USE_CLIPPING
	if ((x < gfx_min_x) || (x > gfx_max_x)
			|| (y < gfx_min_y) || (y > gfx_max_y))
		return;
#endif

	// Set up top left corner of area and write the two bytes of
	// pixel data.  Bottom left corner is already set to max_x/y.
	gfx_set_top_left_limit(x, y);
	gfx_write_gram(color);
}

void gfx_set_top_left_limit(gfx_coord_t x, gfx_coord_t y)
{
	gfx_write_register(HX8347A_COLSTARTHIGH, (x >> 8));
	gfx_write_register(HX8347A_COLSTARTLOW, (x & 0xff));
	gfx_write_register(HX8347A_ROWSTARTHIGH, (y >> 8));
	gfx_write_register(HX8347A_ROWSTARTLOW, (y & 0xff));
}

void gfx_set_bottom_right_limit(gfx_coord_t x, gfx_coord_t y)
{
	gfx_write_register(HX8347A_COLENDHIGH, (x >> 8));
	gfx_write_register(HX8347A_COLENDLOW, (x & 0xff));
	gfx_write_register(HX8347A_ROWENDHIGH, (y >> 8));
	gfx_write_register(HX8347A_ROWENDLOW, (y & 0xff));
}

void gfx_set_limits(gfx_coord_t x1, gfx_coord_t y1,
		gfx_coord_t x2, gfx_coord_t y2)
{
	gfx_set_top_left_limit(x1, y1);
	gfx_set_bottom_right_limit(x2, y2);
}

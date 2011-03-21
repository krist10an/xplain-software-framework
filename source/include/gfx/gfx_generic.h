/**
 * \file
 *
 * \brief Generic graphics routines header file
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
#ifndef GFX_GENERIC_H_INCLUDED
#define GFX_GENERIC_H_INCLUDED

/**
 * \ingroup gfx_gfx
 * \defgroup gfx_generic Generic Implementation of Graphics Primitives
 *
 * This is a library of generic implementations of various graphics
 * primitives. They are meant to simplify the implementation of the
 * low-level graphics driver by providing generic implementations of
 * primitives that aren't directly supported by the hardware.
 *
 * @{
 */

//! Generic implementation of gfx_draw_horizontal_line().
void gfx_generic_draw_horizontal_line(gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t length, gfx_color_t color);

//! Generic implementation of gfx_draw_vertical_line().
void gfx_generic_draw_vertical_line(gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t length, gfx_color_t color);

//! Generic implementation of gfx_draw_line().
void gfx_generic_draw_line(gfx_coord_t x1, gfx_coord_t y1,
		gfx_coord_t x2, gfx_coord_t y2,
		gfx_color_t color);

//! Generic implementation of gfx_draw_rect().
void gfx_generic_draw_rect(gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t width, gfx_coord_t height,
		gfx_color_t color);

//! Generic implementation of gfx_draw_filled_rect().
void gfx_generic_draw_filled_rect(gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t width, gfx_coord_t height,
		gfx_color_t color);

//! Generic implementation of gfx_draw_circle().
void gfx_generic_draw_circle(gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t radius, gfx_color_t color,
		uint8_t octant_mask);

//! Generic implementation of gfx_draw_filled_circle().
void gfx_generic_draw_filled_circle(gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t radius, gfx_color_t color,
		uint8_t quadrant_mask);

//! Generic implementation of gfx_get_pixmap().
void gfx_generic_get_pixmap(gfx_color_t *pixmap, gfx_coord_t map_width,
		gfx_coord_t map_x, gfx_coord_t map_y,
		gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t width, gfx_coord_t height);

//! Generic implementation of gfx_put_pixmap().
void gfx_generic_put_pixmap(const gfx_color_t *pixmap, gfx_coord_t map_width,
		gfx_coord_t map_x, gfx_coord_t map_y,
		gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t width, gfx_coord_t height);

//! @}

#endif // GFX_GENERIC_H_INCLUDED

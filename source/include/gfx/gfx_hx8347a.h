/**
 * \file
 *
 * \brief Graphic driver for HX8347A compatible displays
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

#ifndef GFX_HX8347A_H_INCLUDED
#define GFX_HX8347A_H_INCLUDED

/**
 * \weakgroup gfx_gfx
 * @{
 */

/*
 * Use the generic drawing functions for this driver
 */

/**
 * HX8347A display driver uses generic gfx implementation for this function. See
 * \ref gfx_generic_draw_horizontal_line
 */
#define gfx_draw_horizontal_line(x, y, length, color) \
		gfx_generic_draw_horizontal_line(x, y, length, color)

/**
 * HX8347A display driver uses generic gfx implementation for this function. See
 * \ref gfx_generic_draw_vertical_line
 */
#define gfx_draw_vertical_line(x, y, length, color) \
		gfx_generic_draw_vertical_line(x, y, length, color)

/**
 * HX8347A display driver uses generic gfx implementation for this function. See
 * \ref gfx_generic_draw_line
 */
#define gfx_draw_line(x1, y1, x2, y2, color) \
		gfx_generic_draw_line(x1, y1, x2, y2, color)

/**
 * HX8347A display driver uses generic gfx implementation for this function. See
 * \ref gfx_generic_draw_rect
 */
#define gfx_draw_rect(x, y, width, height, color) \
		gfx_generic_draw_rect(x, y, width, height, color)

/**
 * HX8347A display driver uses generic gfx implementation for this function. See
 * \ref gfx_generic_draw_filled_rect
 */
#define gfx_draw_filled_rect(x, y, width, height, color) \
		gfx_generic_draw_filled_rect(x, y, width, height, color)

/**
 * HX8347A display driver uses generic gfx implementation for this function. See
 * \ref gfx_generic_draw_circle
 */
#define gfx_draw_circle(x, y, radius, color, octant_mask) \
		gfx_generic_draw_circle(x, y, radius, color, octant_mask)

/**
 * HX8347A display driver uses generic gfx implementation for this function. See
 * \ref gfx_generic_draw_filled_circle
 */
#define gfx_draw_filled_circle(x, y, radius, color, quadrant_mask) \
		gfx_generic_draw_filled_circle(x, y, radius, color, quadrant_mask)

/**
 * HX8347A display driver uses generic gfx implementation for this function. See
 * \ref gfx_generic_get_pixmap
 */
#define gfx_get_pixmap(pixmap, map_width, map_x, map_y, x, y, width, height) \
		gfx_generic_get_pixmap(pixmap, map_width, map_x, map_y, x, y, width, \
				height)

/**
 * HX8347A display driver uses generic gfx implementation for this function. See
 * \ref gfx_generic_put_pixmap
 */
#define gfx_put_pixmap(pixmap, map_width, map_x, map_y, x, y, width, height) \
		gfx_generic_put_pixmap(pixmap, map_width, map_x, map_y, x, y, width, \
				height)


typedef uint16_t gfx_color_t;
typedef int16_t gfx_coord_t;

#define GFX_COLOR(r, g, b) \
	((((uint16_t)r) & 0x00f8) | \
	((((uint16_t)b) << 5) & 0x1f00) | \
	((((uint16_t)g) >> 5) & 0x0007) | \
	((((uint16_t)g) << 11) & 0xe000))

/**
 * It is not possible to define a color that is outside the color spectrum for
 * the HX8347A driver, hence use a dark color as invalid color.
 */
#define GFX_COLOR_INVALID       GFX_COLOR(1,2,3)

#define GFX_COLOR_TRANSPARENT   GFX_COLOR(254,0,0)

//! @}

#endif // GFX_HX8347A_H_INCLUDED

/**
* \file
*
* \brief general graphics routines
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

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <gfx/gfx.h>

void gfx_generic_draw_horizontal_line(gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t length, gfx_color_t color)
{
	gfx_draw_filled_rect(x, y, length, 1, color);
}

void gfx_generic_draw_vertical_line(gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t length, gfx_color_t color)
{
	gfx_draw_filled_rect(x, y, 1, length, color);
}

void gfx_generic_draw_line(gfx_coord_t x1, gfx_coord_t y1,
		gfx_coord_t x2, gfx_coord_t y2, gfx_color_t color)
{
	gfx_coord_t x;
	gfx_coord_t y;
	int16_t xinc;
	int16_t yinc;
	int16_t dx;
	int16_t dy;

	// Compute deltas, ie. "width" and "height" of line, then
	// compute x and y direction, and make deltas positive for later use.
	// Start off assuming direction is positive, ie. right and down.
	xinc = 1;
	dx = x2 - x1;
	if (dx < 0) {
		xinc = -1;
		dx = -dx;
	}
	yinc = 1;
	dy = y2 - y1;
	if (dy < 0) {
		yinc = -1;
		dy = -dy;
	}

	// Set up current point and prepare bottom right corner of draw area.
	x = x1;
	y = y1;
	gfx_set_bottom_right_limit(gfx_width - 1, gfx_height - 1);

	/*
	 * A "flat" line (dx>dy) is handled differently from a "steep"
	 * line (dx<dy).
	 */
	if (dx > dy) {
		gfx_coord_t i;
		int16_t e = dx >> 1;

		// Walk along X, draw pixel, and step Y when required.
		for (i = 0; i <= dx; ++i) {
			gfx_draw_line_pixel(x, y, color);
			// Update fractional part ("error"),
			// and step Y when it crosses 0.
			e -= dy;
			if (e < 0) {
				e += dx;
				y += yinc;
			}
			// Walk one step along X.
			x += xinc;
		}
	} else {
		gfx_coord_t i;
		int16_t e = dy >> 1;

		// Walk along Y, draw pixel, and step X when required.
		for (i = 0; i <= dy; ++i) {
			gfx_draw_line_pixel(x,y,color);
			// Update fractional part ("error"),
			// and step X when it crosses 0.
			e -= dx;
			if (e < 0){
				e += dy;
				x += xinc;
			}
			// Walk one step along Y.
			y += yinc;
		}
	}
}



void gfx_generic_draw_rect(gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t width, gfx_coord_t height,
		gfx_color_t color)
{
	gfx_draw_horizontal_line(x, y, width, color);
	gfx_draw_horizontal_line(x, y + height - 1, width, color);
	gfx_draw_vertical_line(x, y, height, color);
	gfx_draw_vertical_line(x + width - 1, y, height, color);
}


void gfx_generic_draw_filled_rect(gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t width, gfx_coord_t height,
		gfx_color_t color)
{
	gfx_coord_t x2;
	gfx_coord_t y2;

	if ((width == 0) || (height == 0))
		return;

	// Invert if width or height is negative.
	if (width < 0) {
		width = -width;
		x -= width - 1;
	}

	if (height < 0) {
		height = -height;
		y -= height - 1;
	}

#ifdef CONFIG_GFX_USE_CLIPPING
	// Nothing to do if entire rectangle is outside clipping region.
	if ((x > gfx_max_x) || (y > gfx_max_y)
			|| ((x + width) <= gfx_min_x)
			|| ((y + height) <= gfx_min_y))
		return;

	// Clip if outside left X limit.
	if (x < gfx_min_x) {
		width -= gfx_min_x - x;
		x = gfx_min_x;
	}

	// Clip if outside top Y limit.
	if (y < gfx_min_y) {
		height -= gfx_min_y - y;
		y = gfx_min_y;
	}
#endif

	// Compute bottom right point.
	x2 = x + width - 1;
	y2 = y + height - 1;

#ifdef CONFIG_GFX_USE_CLIPPING
	// Clip if outside right X limit.
	if (x2 > gfx_max_x) {
		x2 = gfx_max_x;
		width = x2 - x + 1;
	}

	// Clip if outside bottom Y limit.
	if (y2 > gfx_max_y) {
		y2 = gfx_max_y;
		height = y2 - y + 1;
	}
#endif

	// Set up draw area and duplicate pixel color until area is full.
	gfx_set_limits(x, y, x2, y2);
	gfx_duplicate_pixel(color, (uint32_t)width * height);
}

void gfx_generic_draw_circle(gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t radius, gfx_color_t color, uint8_t octant_mask)
{
	gfx_coord_t offset_x;
	gfx_coord_t offset_y;
	int16_t error;

	// Draw only a pixel if radius is zero.
	if (radius == 0) {
		gfx_draw_pixel(x, y, color);
		return;
	}

	// Set up start iterators.
	offset_x = 0;
	offset_y = radius;
	error = 3 - 2 * radius;

	// Iterate offsetX from 0 to radius.
	while (offset_x <= offset_y) {
		// Draw one pixel for each octant enabled in octant_mask.
		if (octant_mask & GFX_OCTANT0)
			gfx_draw_pixel(x + offset_y, y - offset_x, color);
		if (octant_mask & GFX_OCTANT1)
			gfx_draw_pixel(x + offset_x, y - offset_y, color);
		if (octant_mask & GFX_OCTANT2)
			gfx_draw_pixel(x - offset_x, y - offset_y, color);
		if (octant_mask & GFX_OCTANT3)
			gfx_draw_pixel(x - offset_y, y - offset_x, color);
		if (octant_mask & GFX_OCTANT4)
			gfx_draw_pixel(x - offset_y, y + offset_x, color);
		if (octant_mask & GFX_OCTANT5)
			gfx_draw_pixel(x - offset_x, y + offset_y, color);
		if (octant_mask & GFX_OCTANT6)
			gfx_draw_pixel(x + offset_x, y + offset_y, color);
		if (octant_mask & GFX_OCTANT7)
			gfx_draw_pixel(x + offset_y, y + offset_x, color);

		// Update error value and step offset_y when required.
		if (error < 0) {
			error += ((offset_x << 2) + 6);
		} else {
			error += (((offset_x - offset_y) << 2) + 10);
			--offset_y;
		}

		// Next X.
		++offset_x;
	}
}


void gfx_generic_draw_filled_circle(gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t radius, gfx_color_t color, uint8_t quadrant_mask)
{
	gfx_coord_t offset_x;
	gfx_coord_t offset_y;
	int16_t error;

	// Draw only a pixel if radius is zero.
	if (radius == 0) {
		gfx_draw_pixel(x, y, color);
		return;
	}

	// Set up start iterators.
	offset_x = 0;
	offset_y = radius;
	error = 3 - 2 * radius;

	// Iterate offset_x from 0 to radius.
	while (offset_x <= offset_y) {
		// Draw vertical lines tracking each quadrant.
		if (quadrant_mask & GFX_QUADRANT0) {
			gfx_draw_vertical_line(x + offset_y,
					y - offset_x, offset_x + 1, color);
			gfx_draw_vertical_line(x + offset_x,
					y - offset_y, offset_y + 1, color);
		}
		if (quadrant_mask & GFX_QUADRANT1) {
			gfx_draw_vertical_line(x - offset_y,
					y - offset_x, offset_x + 1, color);
			gfx_draw_vertical_line(x - offset_x,
					y - offset_y, offset_y + 1, color);
		}
		if (quadrant_mask & GFX_QUADRANT2) {
			gfx_draw_vertical_line(x - offset_y,
					y, offset_x + 1, color);
			gfx_draw_vertical_line(x - offset_x,
					y, offset_y + 1, color);
		}
		if (quadrant_mask & GFX_QUADRANT3) {
			gfx_draw_vertical_line(x + offset_y,
					y, offset_x + 1, color);
			gfx_draw_vertical_line(x + offset_x,
					y, offset_y + 1, color);
		}

		// Update error value and step offset_y when required.
		if (error < 0) {
			error += ((offset_x << 2) + 6);
		} else {
			error += (((offset_x - offset_y) << 2) + 10);
			--offset_y;
		}

		// Next X.
		++offset_x;
	}
}


void gfx_generic_get_pixmap(gfx_color_t *pixmap, gfx_coord_t map_width,
		gfx_coord_t map_x, gfx_coord_t map_y,
		gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t width, gfx_coord_t height)
{
	gfx_coord_t x2;
	gfx_coord_t y2;

	// Nothing to do if width or height is zero.
	if ((width == 0) || (height == 0))
		return;

	// Sanity check on parameters.
	assert(pixmap != NULL);
	assert((map_x + width) <= map_width);
	assert(map_x >= 0);
	assert(map_y >= 0);
	assert(width > 0);
	assert(height > 0);

#ifdef CONFIG_GFX_USE_CLIPPING
	// Nothing to do if entire rectangle is outside clipping region.
	if ((x > gfx_max_x) || (y > gfx_max_y)
			|| ((x + width) <= gfx_min_x)
			|| ((y + height) <= gfx_min_y))
		return;

	// Clip if outside left X limit.
	if (x < gfx_min_x) {
		width -= gfx_min_x - x;
		map_x += gfx_min_x - x;
		x = gfx_min_x;
	}

	// Clip if outside top Y limit.
	if (y < gfx_min_y) {
		height -= gfx_min_y - y;
		map_y += gfx_min_y - y;
		y = gfx_min_y;
	}
#endif

	// Compute bottom right point.
	x2 = x + width - 1;
	y2 = y + height - 1;

#ifdef CONFIG_GFX_USE_CLIPPING
	// Clip if outside right X limit.
	if (x2 > gfx_max_x) {
		x2 = gfx_max_x;
		width = x2 - x + 1;
	}

	// Clip if outside bottom Y limit.
	if (y2 > gfx_max_y) {
		y2 = gfx_max_y;
		height = y2 - y + 1;
	}
#endif

	// Offset into pixmap.
	pixmap += map_x;
	if (map_y > 0)
		pixmap += (uint32_t)map_y * map_width;

	// Set up read area.
	gfx_set_bottom_right_limit(x2, y2);

	// In case of no horizontal pixmap clipping, easier handling is
	// possible.
	if ((map_width == width) && (map_x == 0)) {
		gfx_set_top_left_limit(x, y);
		gfx_copy_pixels_from_screen(pixmap, (uint32_t)width * height);
	} else {
		// Copy line by line from screen.
		gfx_coord_t lines_left = height;

		while (lines_left > 0) {
			// Set up read area.
			gfx_set_top_left_limit(x, y);
			++y;

			// Get the pixels.
			gfx_copy_pixels_from_screen(pixmap, width);
			pixmap += map_width;
			--lines_left;
		}
	}
}


void gfx_generic_put_pixmap(const gfx_color_t *pixmap, gfx_coord_t map_width,
		gfx_coord_t map_x, gfx_coord_t map_y,
		gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t width, gfx_coord_t height)
{
	gfx_coord_t x2;
	gfx_coord_t y2;

	// Nothing to do if width or height is zero.
	if ((width == 0) || (height == 0))
		return;

	// Sanity check on parameters.
	assert(pixmap != NULL);
	assert((map_x + width) <= map_width);
	assert(map_x >= 0);
	assert(map_y >= 0);
	assert(width > 0);
	assert(height > 0);

#ifdef CONFIG_GFX_USE_CLIPPING
	// Nothing to do if entire rectangle is outside clipping region.
	if ((x > gfx_max_x) || (y > gfx_max_y)
			|| ((x + width) <= gfx_min_x)
			|| ((y + height) <= gfx_min_y))
		return;

	// Clip if outside left X limit.
	if (x < gfx_min_x) {
		width -= gfx_min_x - x;
		map_x += gfx_min_x - x;
		x = gfx_min_x;
	}

	// Clip if outside top Y limit.
	if (y < gfx_min_y) {
		height -= gfx_min_y - y;
		map_y += gfx_min_y - y;
		y = gfx_min_y;
	}
#endif

	// Compute bottom right point.
	x2 = x + width - 1;
	y2 = y + height - 1;

#ifdef CONFIG_GFX_USE_CLIPPING
	// Clip if outside right X limit.
	if (x2 > gfx_max_x) {
		x2 = gfx_max_x;
		width = x2 - x + 1;
	}

	// Clip if outside bottom Y limit.
	if (y2 > gfx_max_y) {
		y2 = gfx_max_y;
		height = y2 - y + 1;
	}
#endif

	// Offset into pixmap.
	pixmap += map_x;
	if (map_y > 0)
		pixmap += (uint32_t)map_y * map_width;

	// Set up draw area.
	gfx_set_bottom_right_limit(x2, y2);

	// In case of no horizontal pixmap clipping, easier handling is
	// possible.
	if ((map_width == width) && (map_x == 0)) {
		gfx_set_top_left_limit(x, y);
		gfx_copy_pixels_to_screen(pixmap, (uint32_t)width * height);
	} else {
		gfx_coord_t lines_left = height;

		// Copy line by line to screen.
		while (lines_left > 0) {
			// Set up draw area.
			gfx_set_top_left_limit(x, y);
			++y;

			// Do the pixel copying.
			gfx_copy_pixels_to_screen(pixmap, width);
			pixmap += map_width;
			--lines_left;
		}
	}
}

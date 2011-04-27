/**
* \file
*
* \brief Bitmap drawing graphics routines
*
* Copyright (C) 2009-2010 Atmel Corporation. All rights reserved.
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


/**
 * \brief Draw a bitmap
 *
 * Draw a bitmap to the screen on the given display coordinates.
 *
 * \param bmp Pointer to bitmap
 * \param x   Display X coordinate
 * \param y   Display Y coordinate
 */
void gfx_draw_bitmap(const struct gfx_bitmap *bmp, gfx_coord_t x, gfx_coord_t y)
{
	assert(bmp);

	gfx_put_bitmap(bmp, 0, 0, x, y, bmp->width, bmp->height);
}

/**
 * \brief Draw a bitmap to the screen tiled to cover the output area.
 *
 * The function starts at the tile origin and searches to find the coordinate
 * closest to the top left of the output area that is a multiple of the bitmap
 * width/height. It then draws the bitmap as many times as needed to cover
 * the entire output area.
 *
 * \image html gfx_draw_bitmap_tiled.png
 *
 * To draw a bitmap tiled on the entire screen, regardless of the bitmap
 * size, the function can be used like this:
 *
 * \code
 * gfx_draw_bitmap_tiled(bmp, 0, 0, gfx_get_width(), gfx_get_height(), 0, ,0);
 * \endcode
 *
 * The function is used by the \ref gfx_win "window system" to redraw the
 * background of a parent window when a transparent child window needs to be
 * redrawn. In this case the full parent window does not need redrawing,
 * only the area of the parent window that is under the child window.
 * The tiles of the background image will for all windows start at the top left
 * corner of the window, so the function is used like this:
 *
 * \code
 * gfx_draw_bitmap_tiled(parent_background_bmp,
 *                       child_window_x1, child_window_y1,
 *                       child_window_x2, child_window_y2,
 *                       parent_window_origin_x, parent_window_origin_y);
 * \endcode
 *
 * \note If \ref CONFIG_WIN_USE_CLIPPING is used, the output is clipped to the
 * output area and nothing will be drawn outside the output area.
 *
 * \param bmp           Pointer to bitmap
 * \param x1            Output area left coordinate
 * \param y1            Output area top coordinate
 * \param x2            Output area right coordinate
 * \param y2            Output area bottom coordinate
 * \param tile_origin_x Tile origin left coordinate
 * \param tile_origin_y Tile origin top coordinate
 */
void gfx_draw_bitmap_tiled(const struct gfx_bitmap *bmp, gfx_coord_t x1,
		gfx_coord_t y1, gfx_coord_t x2, gfx_coord_t y2,
		gfx_coord_t tile_origin_x, gfx_coord_t tile_origin_y)
{
	gfx_coord_t map_width;
	gfx_coord_t map_height;
	gfx_coord_t start_x;
	gfx_coord_t start_y;
	gfx_coord_t index_x;
	gfx_coord_t index_y;

	// Sanity check on parameters.
	assert(bmp);
	assert(x1 >= 0);
	assert(y1 >= 0);
	assert(x2 > x1);
	assert(y2 > y1);
	assert(tile_origin_x <= x1);
	assert(tile_origin_y <= y1);

	// Faster handling for solid color bitmaps
	if (bmp->type == BITMAP_SOLID) {
		gfx_draw_filled_rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1,
				bmp->data.color);
		return;
	}

	// Find starting position
	map_width = bmp->width;
	map_height = bmp->height;
	start_x = tile_origin_x;
	start_y = tile_origin_y;

	while (start_x <= (x1 - map_width)) {
		start_x += map_width;
	}
	while (start_y <= (y1 - map_height)) {
		start_y += map_height;
	}

	// Draw tiles to cover the output area
	for (index_y = start_y; index_y <= y2; index_y += map_height) {
		for (index_x = start_x; index_x <= x2; index_x += map_width) {
			gfx_put_bitmap(bmp, 0, 0, index_x, index_y, map_width, map_height);
		}
	}
}

/**
 * \brief Write a rectangular block of pixels from a bitmap to
 * the screen.
 *
 * This function draws a subset of a bitmap to the screen. The subset
 * is given by (map_x, map_y, width, height), and is outputted
 * to the screen at the screen coordinates (x, y).
 *
 * If the area to write is outside the clipping region, those pixels
 * will not be written.
 *
 * \note This function fails if the width or height is negative or the
 *       pixel rectangle is outside the pixmap buffer extents. Clipping
 *       is only performed on the screen, not inside the pixmap buffer itself.
 *
 * \note There is no checking if the pixel rectangle exceeds the bottom of
 *       the pixmap buffer.
 *
 * \param bmp       Pointer to bitmap.
 * \param map_x     X coordinate inside pixel buffer.
 * \param map_y     Y coordinate inside pixel buffer.
 * \param x         X coordinate on screen.
 * \param y         Y coordinate on screen.
 * \param width     Width of pixel rectangle to copy.
 * \param height    Height of pixel rectangle to copy.
 */
void gfx_put_bitmap(const struct gfx_bitmap *bmp,
		gfx_coord_t map_x, gfx_coord_t map_y,
		gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t width, gfx_coord_t height)
{

	gfx_coord_t x2;
	gfx_coord_t y2;
	gfx_coord_t map_width = bmp->width;
	gfx_color_t *pixmap;
	const gfx_color_t __progmem_arg *progmem_pixmap;
#ifdef CONFIG_HUGEMEM
	hugemem_ptr_t hugemem_pixmap;
#endif



	// Nothing to do if width or height is zero.
	if ((width == 0) || (height == 0))
		return;

	// Sanity check on parameters.
	assert(bmp);
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

	switch (bmp->type) {
	case BITMAP_SOLID:
		gfx_draw_filled_rect(x, y, x2 - x, y2 - y, bmp->data.color);
		break;

	case BITMAP_RAM:
		pixmap = bmp->data.pixmap;
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
		break;

	case BITMAP_PROGMEM:
		progmem_pixmap = bmp->data.progmem;
		// Offset into pixmap.
		progmem_pixmap += map_x;

		if (map_y > 0) {
			progmem_pixmap += (uint32_t) map_y * map_width;
		}

		// Set up draw area.
		gfx_set_bottom_right_limit(x2, y2);

		/* In case of no horizontal pixmap clipping, easier handling is
		 * possible.
		 */
		if ((map_width == width) && (map_x == 0)) {
			gfx_set_top_left_limit(x, y);
			gfx_copy_progmem_pixels_to_screen(progmem_pixmap,
					(uint32_t)width * height);
		} else {
			gfx_coord_t lines_left = height;

			// Copy line by line to screen.
			while (lines_left > 0) {
				// Set up draw area.
				gfx_set_top_left_limit(x, y);
				++y;

				// Do the pixel copying.
				gfx_copy_progmem_pixels_to_screen(progmem_pixmap, width);
				progmem_pixmap += map_width;
				--lines_left;
			}
		}
		break;

#ifdef CONFIG_HUGEMEM
	case BITMAP_HUGEMEM:
		hugemem_pixmap = bmp->data.hugemem;
		// Offset into pixmap.
		hugemem_pixmap = (hugemem_ptr_t)((uint32_t)hugemem_pixmap +
				map_x);

		if (map_y > 0) {
			hugemem_pixmap = (hugemem_ptr_t)
				((uint32_t)hugemem_pixmap +
				 ((uint32_t)map_y *
				  (uint32_t)map_width));
		}

		// Set up draw area.
		gfx_set_bottom_right_limit(x2, y2);

		/* In case of no horizontal pixmap clipping, easier handling is
		 * possible.
		 */
		if ((map_width == width) && (map_x == 0)) {
			gfx_set_top_left_limit(x, y);
			gfx_copy_hugemem_pixels_to_screen(hugemem_pixmap,
					(uint32_t)width * height);
		} else {
			gfx_coord_t lines_left = height;

			// Copy line by line to screen.
			while (lines_left > 0) {
				// Set up draw area.
				gfx_set_top_left_limit(x, y);
				++y;

				// Do the pixel copying.
				gfx_copy_hugemem_pixels_to_screen(
						hugemem_pixmap, width);
				hugemem_pixmap =
					(hugemem_ptr_t)((uint32_t)hugemem_pixmap
							+ map_width);
				--lines_left;
			}
		}
		break;
#endif

#ifdef CONFIG_GRADIENT
	case BITMAP_GRADIENT:
		gfx_gradient_draw(bmp->data.gradient, map_x, map_y, x, y, width, height);
		break;
#endif
	}
}


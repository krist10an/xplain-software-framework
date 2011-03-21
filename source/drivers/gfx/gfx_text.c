/**
 * \file
 *
 * \brief Font and text drawing routines
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
#include <stddef.h>
#include <assert.h>
#ifdef CONFIG_HUGEMEM
#include <hugemem.h>
#endif

#include <gfx/gfx.h>

#ifndef CONFIG_FONT_PIXELS_PER_BYTE
# define CONFIG_FONT_PIXELS_PER_BYTE    8
#endif

#define EXTMEM_BUF_SIZE                 20

#if defined(CONFIG_HUGEMEM) || defined(__DOXYGEN__)
/**
 * \brief Helper function that draws a character from a font in hugemem
 *        to the display
 *
 * If \a background_color is set to \ref GFX_COLOR_TRANSPARENT, no background
 * is drawn.
 *
 * \param ch       Character to be drawn
 * \param x        X coordinate on screen.
 * \param y        Y coordinate on screen.
 * \param font     Font to draw character in
 * \param color    Foreground color of character
 * \param background_color   Background color of character
 */
static void gfx_draw_char_hugemem(char ch, gfx_coord_t x, gfx_coord_t y,
		struct font *font, gfx_color_t color, gfx_color_t background_color)
{
	uint8_t         i;
	uint8_t         char_row_size;
	uint8_t         glyph_size;
	uint16_t        glyph_data_offset;
	uint8_t         char_buff[EXTMEM_BUF_SIZE];
	uint8_t         buffer_pos;
	uint8_t         scale = font->scale;
	uint8_t         rows_left;

	gfx_coord_t inc_x = x;
	gfx_coord_t inc_y = y;

	char_row_size = font->width / CONFIG_FONT_PIXELS_PER_BYTE;
	if (font->width % CONFIG_FONT_PIXELS_PER_BYTE)
		char_row_size++;

	glyph_size = char_row_size * font->height;
	glyph_data_offset = glyph_size * ((uint8_t)ch - font->first_char);
	buffer_pos = EXTMEM_BUF_SIZE;
	rows_left = font->height;

	do {
		static uint8_t  glyph_byte = 0;
		uint8_t         pixelsToDraw = font->width;

		for (i = 0; i < pixelsToDraw; i++) {
			if (i % CONFIG_FONT_PIXELS_PER_BYTE == 0) {
				// Read another byte from hugemem
				if (buffer_pos >= EXTMEM_BUF_SIZE) {
					hugemem_ptr_t source =
						font->data.hugemem;
					source = (hugemem_ptr_t)
						((uint32_t)source +
						 glyph_data_offset);

					hugemem_read_block(char_buff, source,
							EXTMEM_BUF_SIZE);

					glyph_data_offset += EXTMEM_BUF_SIZE;
					buffer_pos = 0;
				}

				glyph_byte = char_buff[buffer_pos];
				buffer_pos++;
			}

			// Draw bit of glyph to screen
			if ((glyph_byte & 0x80)) {
				if (scale == 1)
					gfx_draw_pixel(inc_x, inc_y, color);
				else
					gfx_draw_filled_rect(inc_x, inc_y,
							scale, scale, color);
			}

			inc_x += scale;
			glyph_byte <<= 1;
		}

		inc_y += scale;
		inc_x = x;
	} while (--rows_left > 0);
}
#endif

/**
 * \brief Helper function that draws a character from a font in progmem
 *        to the display
 *
 * If \a background_color is set to \ref GFX_COLOR_TRANSPARENT, no background
 * is drawn.
 *
 * \param ch       Character to be drawn
 * \param x        X coordinate on screen.
 * \param y        Y coordinate on screen.
 * \param font     Font to draw character in
 * \param color    Foreground color of character
 * \param background_color   Background color of character
 */
static void gfx_draw_char_progmem(char ch, gfx_coord_t x, gfx_coord_t y,
		struct font *font, gfx_color_t color,
		gfx_color_t background_color)
{
	const uint8_t __progmem_arg     *glyph_data;
	uint16_t                        glyph_data_offset;
	uint8_t                         scale = font->scale;
	uint8_t                         char_row_size;
	uint8_t                         rows_left;
	uint8_t                         i;

	gfx_coord_t inc_x = x;
	gfx_coord_t inc_y = y;

	char_row_size = font->width / CONFIG_FONT_PIXELS_PER_BYTE;
	if (font->width % CONFIG_FONT_PIXELS_PER_BYTE)
		char_row_size++;

	glyph_data_offset = char_row_size * font->height *
		((uint8_t)ch - font->first_char);
	glyph_data = font->data.progmem + glyph_data_offset;
	rows_left = font->height;

	do {
		uint8_t glyph_byte = 0;
		uint8_t pixelsToDraw = font->width;

		for (i = 0; i < pixelsToDraw; i++) {
			if (i % CONFIG_FONT_PIXELS_PER_BYTE == 0) {
				glyph_byte = progmem_read8(glyph_data);
				glyph_data++;
			}

			if ((glyph_byte & 0x80)) {
				if (scale == 1)
					gfx_draw_pixel(inc_x, inc_y, color);
				else
					gfx_draw_filled_rect(inc_x, inc_y,
							scale, scale, color);
			}

			inc_x += scale;
			glyph_byte <<= 1;
		}

		inc_y += scale;
		inc_x = x;
		rows_left--;
	} while (rows_left > 0);
}

void gfx_draw_char(char c, gfx_coord_t x, gfx_coord_t y, struct font* font,
		gfx_color_t color, gfx_color_t background_color)
{
	// Sanity check.
	if ((uint8_t)c < font->first_char)
		c = font->first_char;
	if ((uint8_t)c > font->last_char)
		c = font->last_char;

	assert(font->scale > 0);

	// Clear background if needed
	if (background_color != GFX_COLOR_TRANSPARENT) {
		gfx_draw_filled_rect(x, y, font->width * font->scale,
				font->height * font->scale, background_color);
	}

	switch (font->type) {
	case FONT_LOC_PROGMEM:
		gfx_draw_char_progmem(c, x, y, font, color, background_color);
		break;
#ifdef CONFIG_HUGEMEM
	case FONT_LOC_HUGEMEM:
		gfx_draw_char_hugemem(c, x, y, font, color, background_color);
		break;
#endif
	default:
		//unsported mode
		unhandled_case(font->type);
		break;
	}
}

void gfx_draw_string(char* str, gfx_coord_t x, gfx_coord_t y, struct font* font,
		gfx_color_t color, gfx_color_t background_color)
{
	uint8_t scale = font->scale;

	// Save X in order to know where to return to on CR.
	gfx_coord_t const start_of_string_position_x = x;

	// Sanity check on parameters.
	assert(scale > 0);
	assert(font);
	assert(str);

	// Draw characters until trailing null byte
	do {
		// Handle '\n' as newline, draw normal characters.
		if (*str == '\n') {
			x = start_of_string_position_x;
			y += font->height * scale;
		}
		else if (*str == '\r') {
			/* Skip '\r' characters. */
		} else {
			gfx_draw_char(*str, x, y, font, color,
					background_color);
			x += font->width * scale;
		}
	} while (*(++str));
}

void gfx_draw_progmem_string(const char __progmem_arg *str, gfx_coord_t x,
		gfx_coord_t y, struct font *font, gfx_color_t color,
		gfx_color_t background_color)
{
	char            temp_char;
	uint8_t         scale = font->scale;

	assert(scale > 0);
	assert(font);
	assert(str);

	// Save X in order to know where to return to on CR.
	gfx_coord_t const start_of_string_position_x = x;

	// Draw characters until trailing null byte
	temp_char = progmem_read8((uint8_t __progmem_arg *)str);

	while (temp_char) {
		// Handle '\n' as newline, draw normal characters.
		if (temp_char == '\n') {
			x = start_of_string_position_x;
			y += font->height * scale;
		}
		else if (*str == '\r') {
			/* Skip '\r' characters. */
		} else {
			gfx_draw_char(temp_char, x, y, font, color,
					background_color);
			x += font->width * scale;
		}

		temp_char = progmem_read8((uint8_t __progmem_arg *)(++str));
	}
}

void gfx_get_string_bounding_box(char const *str, struct font *font,
		gfx_coord_t * width, gfx_coord_t * height)
{
	gfx_coord_t font_width = font->width * font->scale;
	gfx_coord_t font_height = font->height * font->scale;

	gfx_coord_t max_width = 1;
	gfx_coord_t max_height = font_height;
	gfx_coord_t x = 0;

	assert(str);
	assert(font);
	assert(width);
	assert(height);

	// Handle each character until trailing null byte
	do {
		// Handle '\n' as newline, draw normal characters.
		if (*str == '\n') {
			x = 0;
			max_height += font_height;
		}
		else if (*str == '\r') {
			/* Skip '\r' characters. */
		} else {
			x += font_width;
			if (x > max_width)
				max_width = x;
		}
	} while (*(++str));

	// Return values through references
	*width = max_width;
	*height = max_height;
}

void gfx_get_progmem_string_bounding_box(const char __progmem_arg *str,
		struct font *font, gfx_coord_t *width, gfx_coord_t *height)
{
	gfx_coord_t font_width = font->width * font->scale;
	gfx_coord_t font_height = font->height * font->scale;

	char            temp_char;
	gfx_coord_t     max_width = 1;
	gfx_coord_t     max_height = font_height;
	gfx_coord_t     x = 0;

	assert(str);
	assert(font);
	assert(width);
	assert(height);

	// Handle each character until trailing null byte
	temp_char = progmem_read8((uint8_t __progmem_arg *)str);

	while (temp_char) {
		// Handle '\n' as newline, draw normal characters.
		if (temp_char == '\n') {
			x = 0;
			max_height += font_height;
		}
		else if (*str == '\r') {
			/* Skip '\r' characters. */
		} else {
			x += font_width;
			if (x > max_width)
				max_width = x;
		}

		temp_char = progmem_read8((uint8_t __progmem_arg *)(++str));
	}

	// Return values through references
	*width = max_width;
	*height = max_height;
}

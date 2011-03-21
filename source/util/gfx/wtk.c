/**
 * \file
 *
 * \brief Graphical Widget toolkit
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

/**
 * \todo group some of the largest event handler functions into smaller
 * helpers. They look too big and complex. By splitting up in smaller
 * functions, the code would be easier to read and follow.
 *
 * \todo generalize shaded frame to work for e.g. menu window as well. Now it's
 * taylored for a wtk_frame. It could easily be changed to shade any
 * rectangular area. The shading itself would be the same, only how to get the
 * coordinates differ.
 *
 * \todo ensure proper handling if resize through win_set_area is used on
 * frame, since now it would resize container without doing anything to the
 * other windows. The contents and resize window would look strange, malsized
 * and malplaced. Somehow catch the ATTRIBUTES event and use the
 * wtk_resize_frame() function. Move works fine though, since container is the
 * parent window.
 *
 * \todo optimize/generalize code for controls like checkbox, radiobutton,
 * label, since A LOT of the code is equal: container frame, caption etc.
 */
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <membag.h>
#include <string.h>
#include "gfx/wtk.h"

/**
 * \weakgroup gfx_wtk
 * @{
 */

/**
 * \brief Rescale 8-bit value.
 * \internal
 *
 * This function is used by, e.g., the slider widget for computing the slider
 * knob position from a given value and vice versa. It returns a value with the
 * same data width as the input, meaning \a value cannot exceed \a from_scale,
 * or overflows will occur.\par
 *
 * The formula is: \code new_value = (value / from_scale) * to_scale \endcode
 *
 * \todo Revisit, support larger integers given a config symbol.
 *
 * \param value Value to rescale.
 * \param from_scale Maximum of old scale for value.
 * \param to_scale Maximum of scale for new value.
 *
 * \return Rescaled 8-bit value.
 */
uint8_t wtk_rescale_value(uint8_t value, uint8_t from_scale, uint8_t to_scale)
{
	uint16_t new_value;

	assert(to_scale > 0);
	assert(from_scale > 0);
	assert(value <= from_scale);

	new_value = value;
	new_value *= to_scale;
	new_value /= from_scale;

	return new_value;
}

/**
* This function copies the string contents, including the zero terminator,
* from source to destination. The caller is responsible for providing
* memory for the destination string, either on the stack, static, or
* dynamically allocated.
*
* \param dest Destination string.
* \param source Source string, zero terminated.
*/
void wtk_copy_string(char *dest, char const *source)
{
	assert(dest);
	assert(source);

	// Copy characters, including zero-terminator.
	while (*source != '\0') {
		*dest = *source;
		++dest;
		++source;
	}

	// Copy the terminating zero.
	*dest = *source;
}

/**
 * This function shades one pixel. Alpha equal to 0 gives a black pixel. Alpha
 * equal to 255 gives an untouched pixel.
 *
 * \note Color format is "red:green:blue" (5:6:5), but bytes are swapped, so
 * the result is not rrrrr:gggggg:bbbbb, but ggg:bbbbb:rrrrr:ggg.
 *
 * \param pixel Pointer to pixel to shade.
 * \param alpha Shade value. Lower value gives blacker pixel.
 */
void wtk_shade_pixel(gfx_color_t *pixel, uint8_t alpha)
{
	uint16_t green;
	uint16_t blue;
	uint16_t red;

	// Local workspace variables.
	gfx_color_t oldcol = *pixel;
	gfx_color_t newcol = 0x0000;

	// Split out red color, scale it, put into "newcol".
	red = oldcol & 0x00f8;
	red = (red * alpha) + red;
	red >>= 8;
	red &= 0x00f8;
	newcol |= red;

	// Split out green color, scale it, put into "newcol".
	green = ((oldcol << 3) | (oldcol >> 13)) & 0x003f;
	green = (green * alpha) + green;
	green >>= 8;
	green = (green << 13) | (green >> 3);
	newcol |= green;

	// Split out blue color, scale it, put into "newcol".
	blue = (oldcol >> 8) & 0x001f;
	blue = (blue * alpha) + blue;
	blue &= 0xff00;
	newcol |= blue;

	// Write back result.
	*pixel = newcol;
}

//! @}

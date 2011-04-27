/**
* \file
*
* \brief Gradient drawing graphics routines
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


#ifdef CONFIG_GRADIENT

/**
 * \brief Generate gradient values
 *
 * Generate gradient values from two colors and the length.
 *
 * \param gradient    Pointer to gradient.
 * \param red_from    RGB red value.
 * \param green_from  RGB green value.
 * \param blue_from   RGB blue value.
 * \param red_to      RGB red value.
 * \param green_to    RGB green value.
 * \param blue_to     RGB blue value.
 * \param length      Length of gradient to draw.
 * \param option      Gradient options.
 */

void gfx_gradient_set_values(struct gfx_gradient *gradient,
		uint8_t red_from, uint8_t green_from, uint8_t blue_from,
		uint8_t red_to,   uint8_t green_to,   uint8_t blue_to,
		gfx_coord_t length, uint8_t option)
{
	// sanity check
	assert(gradient);
	assert(length); 
	
	
	
	gradient->start_r = red_from;
	gradient->start_g = green_from;
	gradient->start_b = blue_from;
	gradient->length  = length;
	gradient->option  = option;
	
	/*calculate the 16-bit signed differential per pixel line in 8-bit 
	fixed point math for the three colors, taking care to avoid sign 
	bit overflow*/
	
	if (red_from == red_to){
		gradient->delta_r = 0;
	} else {
		gradient->delta_r =((((int16_t)(red_to - red_from)) << 7 )
				/ length) << 1;
	}
	
	if (green_from == green_to){
		gradient->delta_g = 0;
	} else {
		gradient->delta_g =((((int16_t)(green_to - green_from)) << 7 ) 
				/ length) << 1;
	}
	
	if (blue_from == blue_to){
		gradient->delta_b = 0;
	} else {
		gradient->delta_b =((((int16_t)(blue_to - blue_from)) << 7 )
				/ length) << 1;
	}

}


/**
 * \brief Set gradient options
 *
 * Set gradient options. faster than full reset.
 *
 * \param gradient    Pointer to gradient.

 * \param option      Gradient options.
 */

void gfx_gradient_set_options(struct gfx_gradient *gradient, uint8_t option)
{
	assert(gradient);
	gradient->option = option;
}

/**
 * \brief Draw a gradient
 *
 * Draw a bitmap to the screen on the given display coordinates.
 *
 * \param gradient  Pointer to gradient.
 * \param map_x     X coordinate inside gradient.
 * \param map_y     Y coordinate inside gradient.
 * \param x         X coordinate on screen.
 * \param y         Y coordinate on screen.
 * \param width     Width of gradient to draw.
 * \param height    Height of gradient to draw.
 */

 void gfx_gradient_draw(struct gfx_gradient *gradient, 
		gfx_coord_t map_x, gfx_coord_t map_y,
		gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t width,gfx_coord_t height)
{

	assert(gradient);
	assert(width);
	assert(height);

	// load and reformat colors to 8-bit fixed point.
	uint16_t color_r= (((uint16_t)(gradient->start_r)) << 8);
	uint16_t color_g= (((uint16_t)(gradient->start_g)) << 8);
	uint16_t color_b= (((uint16_t)(gradient->start_b)) << 8);

	int16_t delta_r= gradient->delta_r;
	int16_t delta_g= gradient->delta_g;
	int16_t delta_b= gradient->delta_b;

	/* if gradient is inverted set start color to calculated end color,
	 * and invert delta color */
	if ((gradient->option)&(GFX_GRADIENT_INVERT)){
		color_r += (delta_r * (gradient->length - 1));
		color_g += (delta_g * (gradient->length - 1));
		color_b += (delta_b * (gradient->length - 1));

		delta_r = -delta_r;
		delta_g = -delta_g;
		delta_b = -delta_b;
	}

	
	if (((gradient->option) &
			(GFX_GRADIENT_MIRROR|GFX_GRADIENT_HORIZONTAL)) == 
			(GFX_GRADIENT_MIRROR|GFX_GRADIENT_HORIZONTAL)){

		x -= map_x;

		delta_r *= 2;
		delta_g *= 2;
		delta_b *= 2;
		
		gfx_coord_t x_middle = (gradient->length / 2);
		gfx_coord_t x_end = x + gradient->length;
		gfx_coord_t x_end_mirrored = gradient->length - (width + map_x); 

		for(gfx_coord_t index_x = 0; index_x <= x_middle; index_x++){
			if (index_x >= map_x){
				gfx_draw_vertical_line(x + index_x, y, height,
						GFX_COLOR(
						(uint8_t)(color_r >> 8),
						(uint8_t)(color_g >> 8),
						(uint8_t)(color_b >> 8)));
			}
			if (index_x>=x_end_mirrored){
				gfx_draw_vertical_line(x_end - index_x, y, height,
						GFX_COLOR(
						(uint8_t)(color_r >> 8),
						(uint8_t)(color_g >> 8),
						(uint8_t)(color_b >> 8)));
			}
			
			color_r += delta_r;
			color_g += delta_g;
			color_b += delta_b;
		}

	} else 	if (((gradient->option) &
			(GFX_GRADIENT_MIRROR|GFX_GRADIENT_VERTICAL)) == 
			(GFX_GRADIENT_MIRROR|GFX_GRADIENT_VERTICAL)){

		y -= map_y;
		
		delta_r *= 2;
		delta_g *= 2;
		delta_b *= 2;
	
	
		gfx_coord_t y_middle = (gradient->length / 2);
		gfx_coord_t y_end = y + gradient->length;
		gfx_coord_t y_end_mirrored = gradient->length - (height + map_y); 
		
		for(gfx_coord_t index_y = 0 ; index_y <= y_middle; index_y++){

			if (index_y >= map_y){
				gfx_draw_horizontal_line(x, y + index_y, width,
						GFX_COLOR(
						(uint8_t)(color_r >> 8),
						(uint8_t)(color_g >> 8),
						(uint8_t)(color_b >> 8)));
			}
			if (index_y >= y_end_mirrored){
				gfx_draw_horizontal_line(x, y_end - index_y, width,
						GFX_COLOR(
						(uint8_t)(color_r >> 8),
						(uint8_t)(color_g >> 8),
						(uint8_t)(color_b >> 8)));
			}

			color_r += delta_r;
			color_g += delta_g;
			color_b += delta_b;
		}

	} else {

		if ((gradient->option) & (GFX_GRADIENT_HORIZONTAL)){

			gfx_coord_t x_end = x + width;
			if (map_x > 0){
				color_r += (delta_r * map_x);
				color_g += (delta_g * map_x);
				color_b += (delta_b * map_x);
			}

			for(gfx_coord_t index_x = x; index_x <= x_end ; index_x++){

				gfx_draw_vertical_line(index_x, y, height,
						GFX_COLOR(
						(uint8_t)(color_r >> 8),
						(uint8_t)(color_g >> 8),
						(uint8_t)(color_b >> 8)));
				color_r += delta_r;
				color_g += delta_g;
				color_b += delta_b;
			}
		} else {

			gfx_coord_t y_end = y + width;
			if (map_y  > 0){
				color_r += (delta_r * map_y);
				color_g += (delta_g * map_y);
				color_b += (delta_b * map_y);
			}
			for(gfx_coord_t index_y = y; index_y <= y_end ; index_y++){

				gfx_draw_horizontal_line(x, index_y, width,
						GFX_COLOR(
						(uint8_t)(color_r >> 8),
						(uint8_t)(color_g >> 8),
						(uint8_t)(color_b >> 8)));
				color_r += delta_r;
				color_g += delta_g;
				color_b += delta_b;
			}
		}
	}
}
#endif


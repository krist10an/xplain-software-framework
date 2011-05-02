/**
 * \file
 *
 * \brief General graphics routines header file
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

#ifndef GFX_H_INCLUDED
#define GFX_H_INCLUDED

#include <assert.h>
#include <hugemem.h>
#include <progmem.h>

/**
 * \defgroup gfx Graphical display system
 * The following image illustrates the modules of the graphical display system.
 * Refer to the individual modules for detailed information on each of the
 * modules.
 *
 * \image html gfx_overview.png
 *
 */

#ifdef CONFIG_GFX_HX8347A
# include <gfx/gfx_hx8347a.h>
# include <gfx/gfx_generic.h>
#endif

/**
 * \ingroup gfx
 * \defgroup gfx_gfx Graphics driver
 *
 * This library provides an interface to drawing graphics on a graphical display
 *
 * The graphics drivers consists of the following:
 * - Display driver interface (gfx.h)
 * - General graphics drawing primitives (gfx_generic.h)
 * - Display specific implementation (ex. gfx_hx8347a.h)
 *
 * The display driver interface provides an general interface to be used by
 * the application or by libraries like \ref gfx_sysfont "Sysfont",
 * \ref gfx_win "Window system", \ref gfx_wtk "Widget toolkit" or other
 * libraries.
 *
 * The generic drawing primitives is a library  of functions for drawing
 * graphics primitives such as lines, rectangles and circles. It uses
 * other functions implemented by the display driver for drawing the
 * primitives. The implementation of these functions can optionally be
 * used by a display driver, but if the hardware of the display allows
 * faster handling of any of the primitives, the display driver can
 * implement it directly.
 *
 * The display specific drivers provides an interface to the graphical
 * display. It implements the low level communication with the display
 * hardware, putting pixels on the display and drawing primitives such
 * as lines, circles and rectangles. Depending on the display driver
 * implementation, drawing the graphics primitives might be handled by
 * the generic graphics drawing primitves rather than the display driver
 * itself.
 *
 * @{
 */

//! \name Driver-specific Definitions
//@{
/**
 * \typedef gfx_color_t
 * \brief Data type for color values native to the display.
 */
/**
 * \typedef gfx_coord_t
 * \brief Data type representing a coordinate on the screen.
 */
/**
 * \def GFX_COLOR(r, g, b)
 * \brief Generate a display-native color value from constant RGB
 * components.
 *
 * This macro does the same job as gfx_color(), except that the RGB
 * values must be constant, and that it is usable as a static
 * initializer.
 */
/**
 * \def GFX_COLOR_INVALID
 * \brief Value returned by gfx_get_pixel() for pixels outside clipping
 * region.
 */
/**
 * \def GFX_COLOR_TRANSPARENT
 * \brief Value used as input to font functions to give a transparent background
 * region.
 */
//@}

//! \name Circle Sector Definitions
//@{

//! Bitmask for drawing circle octant 0.
#define GFX_OCTANT0 (1 << 0)
//! Bitmask for drawing circle octant 1.
#define GFX_OCTANT1 (1 << 1)
//! Bitmask for drawing circle octant 2.
#define GFX_OCTANT2 (1 << 2)
//! Bitmask for drawing circle octant 3.
#define GFX_OCTANT3 (1 << 3)
//! Bitmask for drawing circle octant 4.
#define GFX_OCTANT4 (1 << 4)
//! Bitmask for drawing circle octant 5.
#define GFX_OCTANT5 (1 << 5)
//! Bitmask for drawing circle octant 6.
#define GFX_OCTANT6 (1 << 6)
//! Bitmask for drawing circle octant 7.
#define GFX_OCTANT7 (1 << 7)

//! Bitmask for drawing circle quadrant 0.
#define GFX_QUADRANT0 (GFX_OCTANT0 | GFX_OCTANT1)
//! Bitmask for drawing circle quadrant 1.
#define GFX_QUADRANT1 (GFX_OCTANT2 | GFX_OCTANT3)
//! Bitmask for drawing circle quadrant 2.
#define GFX_QUADRANT2 (GFX_OCTANT4 | GFX_OCTANT5)
//! Bitmask for drawing circle quadrant 3.
#define GFX_QUADRANT3 (GFX_OCTANT6 | GFX_OCTANT7)

//! Bitmask for drawing left half of circle.
#define GFX_LEFTHALF (GFX_QUADRANT3 | GFX_QUADRANT0)
//! Bitmask for drawing top half of circle.
#define GFX_TOPHALF (GFX_QUADRANT0 | GFX_QUADRANT1)
//! Bitmask for drawing right half of circle.
#define GFX_RIGHTHALF (GFX_QUADRANT1 | GFX_QUADRANT2)
//! Bitmask for drawing bottom half of circle.
#define GFX_BOTTOMHALF (GFX_QUADRANT2 | GFX_QUADRANT3)

//! Bitmask for drawing whole circle.
#define GFX_WHOLE 0xFF

//@}

//! \name Flags for gfx_set_orientation()
//@{
//! Bitmask for flipping X for gfx_set_orientation
#define GFX_FLIP_X 1
//! Bitmask for flipping Y for gfx_set_orientation
#define GFX_FLIP_Y 2
//! Bitmask for swapping X and Y for gfx_set_orientation
#define GFX_SWITCH_XY 4
//@}


#ifdef CONFIG_GRADIENT

/**
 * \defgroup gfx_gradient Bitmap gradients
 * @{
 */
 
/**
 * \defgroup gfx_gradient_options Gradient options
 * These options can be ORed together to specify the behaviour of a
 * gradient when generating it with \ref wtk_gradient_set_values
 * @{
 */
 
 /**
 * \name gradient orientation configuration options.
 * For use with the option parameter of \ref wtk_gradient_set_values
 * @{
 */

//! Bitmask for horizontal gradients.
#define GFX_GRADIENT_VERTICAL (0<<0)
//! Bitmask for vertical gradients.
#define GFX_GRADIENT_HORIZONTAL (1<<0)
//! Bitmask for inverted gradients
#define GFX_GRADIENT_INVERT (1<<1)
//! Bitmask for mirrored gradients
#define GFX_GRADIENT_MIRROR (1<<2)



//! @}
//! @}
//! @}

#endif

/**
 * \name Screen geometry and clipping variables
 * \internal
 *
 * These global variables should not be accessed directly. They cannot be
 * marked as private/static since they are shared within multiple C files
 * of the graphics drivers.
 *
 * Use \ref gfx_set_clipping to change clipping region.
 */
//! @{
#if defined(CONFIG_GFX_USE_CLIPPING) || defined(__DOXYGEN__)
extern gfx_coord_t gfx_min_x;	//!< Minimum X of current clipping region.
extern gfx_coord_t gfx_min_y;	//!< Maximum Y of current clipping region.
extern gfx_coord_t gfx_max_x;	//!< Minimum X of current clipping region.
extern gfx_coord_t gfx_max_y;	//!< Maximum Y of current clipping region.
#endif

extern gfx_coord_t gfx_width;  //!< Current width of screen.
extern gfx_coord_t gfx_height; //!< Current height of screen.
//@}

/** Valid storage locations for font data
 *
 * \todo Add support for fonts in regular ram
 */
enum font_data_type{
	//! Font data stored in program/flash memory.
	FONT_LOC_PROGMEM,
	//! Font data stored in HUGEMEM.
	FONT_LOC_HUGEMEM,
};

//! Storage structure for font metadata.
struct font {
	//! Type of storage used for binary font data. See \ref font_data_type.
	enum font_data_type type;
	union {
		/** Pointer to where the binary font data is stored. This
		 * variable is accessed either through hugemem or progmem
		 * depending on the value of \a type.
		 */
		hugemem_ptr_t                   hugemem;
		const uint8_t __progmem_arg     *progmem;
	} data;
	//! Width of one font character, in pixles.
	uint8_t width;
	//! Height of one font character, in pixles.
	uint8_t height;
	//! Number of times characters are scaled up when being drawn.
	uint8_t scale;
	//! ASCII value of first character in font set.
	uint8_t first_char;
	//! ASCII value of last character in ont set.
	uint8_t last_char;
};

/**
 * \brief Get scaled height of a \a font.
 *
 * \return Scaled height in pixels for \a font
 */
static inline uint_fast8_t gfx_font_get_height(struct font *font)
{
	return (font->height * font->scale);
}

/**
 * \brief Get scaled width of a \a font.
 *
 * \return Scaled width in pixels for \a font
 */
static inline uint_fast8_t gfx_font_get_width(struct font *font)
{
	return (font->width * font->scale);
}

#ifdef CONFIG_GRADIENT
 
/**
 * \ingroup gfx_gradient
 * @{
 */
 
/**
 * \brief Storage structure for gradient data and metadata
 */
 struct gfx_gradient {
	//! Starting RGB red value
	uint8_t start_r;
	//! Starting RGB green value
	uint8_t start_g;
	//! Starting RGB blue value
	uint8_t start_b;
	//! Change in RGB red value per line.
	int16_t delta_r;
	//! Change in RGB green value per line.
	int16_t delta_g;
	//! Change in RGB blue value per line.
	int16_t delta_b;
	//! Configuration of orientation and behavior.
	uint8_t option;
	//! Length in pixels along the gradient.
	gfx_coord_t length;
 };
 

 
void gfx_gradient_set_options(struct gfx_gradient *gradient, uint8_t option);

void gfx_gradient_set_values(struct gfx_gradient *gradient,
		uint8_t red_from, uint8_t green_from, uint8_t blue_from,
		uint8_t red_to,   uint8_t green_to,   uint8_t blue_to,
		gfx_coord_t length, uint8_t option);

void gfx_gradient_draw(struct gfx_gradient *gradient, gfx_coord_t map_x,
		gfx_coord_t map_y,gfx_coord_t x, gfx_coord_t y,
		gfx_coord_t width,gfx_coord_t height);
 
 //! @}
 #endif
 
/**
 * \name Bitmap functions and structures
 */
//@{

/**
 * \ brief Valid bitmap types
 */
enum gfx_bitmap_type {
	//! Solid color bitmap
	BITMAP_SOLID,
	//! Bitmap stored in SRAM
	BITMAP_RAM,
	//! Bitmap stored in progmem
	BITMAP_PROGMEM,
#ifdef CONFIG_HUGEMEM
	//! Bitmap stored in hugemem
	BITMAP_HUGEMEM,
#endif
#ifdef CONFIG_GRADIENT
	//! Gradient bitmap.
	BITMAP_GRADIENT,
#endif
};

 
/**
 * \brief Storage structure for bitmap pixel data and metadata
 */
struct gfx_bitmap {
	//! Width of bitmap
	gfx_coord_t                            width;
	//! Height of bitmap
	gfx_coord_t                            height;
	//! Bitmap type
	enum gfx_bitmap_type                   type;
	union {
		//! Color for solid color bitmaps
		gfx_color_t                        color;
		//! Pointer to pixels for bitmap stored in SRAM
		gfx_color_t                       *pixmap;
		//! Pointer to pixels for bitmap stored in progmem
		const gfx_color_t __progmem_arg   *progmem;
#ifdef CONFIG_HUGEMEM
		//! Pointer to pixels for bitmap stored in hugemem
		hugemem_ptr_t                      hugemem;
#endif
#ifdef CONFIG_GRADIENT
		//! Pointer to gradient data
		struct gfx_gradient               *gradient;
#endif
	}                                      data;
};

void gfx_draw_bitmap(const struct gfx_bitmap *bmp, gfx_coord_t x,
		gfx_coord_t y);

void gfx_draw_bitmap_tiled(const struct gfx_bitmap *bmp, gfx_coord_t x1,
		gfx_coord_t y1, gfx_coord_t x2, gfx_coord_t y2,
		gfx_coord_t tile_origin_x, gfx_coord_t tile_origin_y);

void gfx_put_bitmap(const struct gfx_bitmap *bmp, gfx_coord_t map_x,
		gfx_coord_t map_y, gfx_coord_t x, gfx_coord_t y, gfx_coord_t width,
		gfx_coord_t height);

//@}


//! \name Display driver management functions
//@{

/**
 * \brief Initialize the low-level graphics device driver
 *
 * This function should be called before doing any graphics.
 */
void gfx_init(void);

/**
 * \brief Synchronize access to the display
 *
 * In case of a graphics driver implementation that queues graphics
 * operations, this function will make sure all pending operations are
 * completed before exiting. That way, if you want to read pixels from
 * the display, you can be sure that the display information is fresh
 * and updated. For implementations that does not queue operations,
 * this function may do nothing, but for portability it should always be
 * called before reading pixels, or performing other operations which
 * rely on all pending drawing operations having actually been
 * completed.
 */
void gfx_sync(void);

//@}

//! \name Display Geometry
//@{

/**
 * \brief Change display orientation
 *
 * Changes the display orientation for displays that support this.
 * Check the driver specific documentation if this is supported.
 *
 * The following masks are available and can be ORed together:
 * - #GFX_FLIP_X Flip X axis
 * - #GFX_FLIP_Y Flip Y axis
 * - #GFX_SWITCH_XY Switch X/Y
 *
 * Note that rotating the screen 90 degress means switching X/Y _and_
 * mirroring one of the axes. It is not enough to just switch X/Y.
 *
 * \param flags A bitmask of which axes to flip and/or switch.
 */
void gfx_set_orientation(uint8_t flags);

/**
 * \brief Return the current width of the screen
 *
 * Return the current width of the screen for the current orientation. Screen
 * can be rotated runtime on supported displays with \ref gfx_set_orientation.
 *
 * \return Width of the screen in pixels.
 */
gfx_coord_t gfx_get_width(void);

/**
 * \brief Return the current height of the screen
 *
 * Return the current height of the screen for the current orientation. Screen
 * can be rotated runtime on supported displays with \ref gfx_set_orientation.
 *
 * \return Height of the screen in pixels.
 */
gfx_coord_t gfx_get_height(void);

/**
 * \name Display Clipping
 *
 * The display driver uses software and hardware clipping. In the graphics
 * drivers software clipping is referred to as clipping and hardware clipping
 * as limits.
 *
 * Software clipping is used to ensure that nothing outside the set
 * region is draw to the screen by any of the drawing functions. Eliminating
 * unwanted drawing to the display will give a higher performance on
 * displays which has a low bandwidth from the CPU.
 * Software is enabled by the CONFIG_GFX_USE_CLIPPING configuration symbol.
 * Clipping region is set with the \ref gfx_set_clipping function.
 *
 * Hardware clipping is used in the supported display drivers to efficiently
 * draw primitives on a subset of the display. Example: when drawing a
 * filled rectangle the limits is set to the size of the rectangle, and the
 * driver sends the correct number of pixels to fill the entire rectangle.
 * Without hardware clipping/limits the driver will have to to move the
 * graphics ram (GRAM) position/pointer manually.
 * Hardware clipping/limits is not used like software clipping to eliminate
 * drawing outside a given region.
 * Hardware clipping is set with the \ref gfx_set_top_left_limit,
 * \ref gfx_set_bottom_right_limit and \ref gfx_set_limits functions.
 * In display drivers that do not use hardware clipping these functions
 * will not have any effect.
 */
//@{

/**
 * \brief Set the clipping region
 *
 * Set the clipping region for all subsequent graphics operations.
 * Nothing will be drawn outside the clipping region. Clipping is handled
 * in software, and is enabled with CONFIG_GFX_USE_CLIPPING.
 *
 * \param min_x Left edge of region.
 * \param min_y Top edge of region.
 * \param max_x Right edge of region.
 * \param max_y Bottom edge of region.
 */
void gfx_set_clipping(gfx_coord_t min_x, gfx_coord_t min_y,
		gfx_coord_t max_x, gfx_coord_t max_y);

/**
 * \brief Set the top-left limit for the display.
 *
 * Set top-left clipping limit for displays that needs to set a hardware
 * clipping region.
 *
 * \param x Top left x coordinate.
 * \param y Top left y coordinate.
 */
void gfx_set_top_left_limit(gfx_coord_t x, gfx_coord_t y);

/**
 * \brief Set the bottom-right limit for the display.
 *
 * Set bottom-right clipping limit for displays that needs to set a hardware
 * clipping region.
 *
 * \param x Bottom right x coordinate.
 * \param y Bottom right y coordinate.
 */
void gfx_set_bottom_right_limit(gfx_coord_t x, gfx_coord_t y);

/**
 * \brief Set the limits for the display.
 *
 * Set clipping limit for displays that needs to set a hardware
 * clipping region.
 *
 * \param x1 Top left x coordinate.
 * \param y1 Top left y coordinate.
 * \param x2 Bottom right x coordinate.
 * \param y2 Bottom right y coordinate.
 */
void gfx_set_limits(gfx_coord_t x1, gfx_coord_t y1,
		gfx_coord_t x2, gfx_coord_t y2);

//@}

//! \name Low-level pixel drawing operations
//@{

/**
 * \brief Generate native color value from R/G/B values.
 *
 * Converts 8-bit R/G/B values to a color value native to the display.
 * The format of the resulting color value depends on the display.
 *
 * The macro \ref GFX_COLOR does the same, only allows the preprocessor to
 * handle the calculations.
 *
 * \param r 8-bit red value.
 * \param g 8-bit green value.
 * \param b 8-bit blue value.
 *
 * \return Color value in display native format.
 */
gfx_color_t gfx_color(uint8_t r, uint8_t g, uint8_t b);

/**
 * \brief Draw a single pixel on the screen
 *
 * Draw a single pixel on screen setting top and bottom limits.
 * If outside the clipping region, nothing is drawn.
 *
 * \param x X coordinate of the pixel to be drawn.
 * \param y Y coordinate of the pixel to be drawn.
 * \param color Color value of the pixel in display native format.
 */
void gfx_draw_pixel(gfx_coord_t x, gfx_coord_t y, gfx_color_t color);

/**
 * \brief Draw a single pixel on the screen (for lines, apparently)
 *
 * This function draws a single pixel on screen only setting top limit
 * which makes it more suitable for line drawing.
 * If outside the clipping region, nothing is drawn.
 *
 * \param x X coordinate of the pixel to be drawn.
 * \param y Y coordinate of the pixel to be drawn.
 * \param color Color value of the pixel in display native format.
 */
void gfx_draw_line_pixel(gfx_coord_t x, gfx_coord_t y, gfx_color_t color);

/**
 * \brief Get the color of a pixel on the display.
 *
 * \param x X coordinate of the pixel to be sampled.
 * \param y Y coordinate of the pixel to be sampled.
 *
 * \return Color value of the pixel at (x, y) in display native format,
 * or #GFX_COLOR_INVALID if outside the clipping region.
 */
gfx_color_t gfx_get_pixel(gfx_coord_t x, gfx_coord_t y);

/**
 * \brief Draw multiple pixels all having the same color.
 *
 * Draw \a count pixels using \a color within the current clipping
 * limits.
 *
 * \param color Color value in display native format.
 * \param count Number of times to write the color
 */
void gfx_duplicate_pixel(gfx_color_t color, uint32_t count);

/**
 * \brief Copy a block of pixels from data memory to screen.
 *
 * Copy pixels from RAM to the screen, given current limits.
 *
 * \param pixels An array of pixel values in display native format.
 * \param count Number of pixels to copy from the array.
 */
void gfx_copy_pixels_to_screen(const gfx_color_t *pixels, uint32_t count);

/**
 * \brief Copy a block of pixels from program memory to screen.
 *
 * Copy a block of pixels from program memory to screen, given current
 * limits.
 *
 * \param pixels An array of pixel values in display native format,
 *      stored in program (flash) memory.
 * \param count Number of pixels to copy from the array.
 */
void gfx_copy_progmem_pixels_to_screen(const gfx_color_t __progmem_arg *pixels,
		uint32_t count);

/**
 * \brief Copy a block of pixels from hugemem to screen.
 *
 * Copy a block of pixels from hugemem to screen, given current
 * limits.
 *
 * \param pixels An array of pixel values in display native format,
 *      stored in program (flash) memory.
 * \param count Number of pixels to copy from the array.
 */
void gfx_copy_hugemem_pixels_to_screen(const hugemem_ptr_t pixels,
		uint32_t count);

/**
 * \brief Copy a block of pixels from screen to data memory.
 *
 * Copy a block of pixels from the screen into RAM, given current limits
 *
 * \param pixels Array into which the pixel values are to be stored.
 * \param count Number of pixels to copy into the array.
 */
void gfx_copy_pixels_from_screen(gfx_color_t *pixels, uint32_t count);

//@}

//! \name Graphics Drawing Primitives
//@{

/**
 * \def gfx_draw_horizontal_line(x, y, length, color)
 * \brief Draw a horizontal line, one pixel wide.
 *
 * \param  x          X coordinate of leftmost pixel.
 * \param  y          Y coordinate of the line.
 * \param  length     Length of the line in pixels.
 * \param  color      Color of the line, in display native format.
 */

/**
 * \def gfx_draw_vertical_line(x, y, length, color)
 * \brief Draw a vertical line, one pixel wide.
 *
 * \param  x          X coordinate of the line.
 * \param  y          Y coordinate of the topmost pixel.
 * \param  length     Length of the line in pixels.
 * \param  color      Color of the line, in display native format.
 */

/**
 * \def gfx_draw_line(x1, y1, x2, y2, color)
 * \brief Draw a line between two arbitrary points.
 *
 * \param  x1          Start X coordinate.
 * \param  y1          Start Y coordinate.
 * \param  x2          End X coordinate.
 * \param  y2          End Y coordinate.
 * \param  color       Color of the line, in display native format.
 */

/**
 * \def gfx_draw_rect(x, y, width, height, color)
 * \brief Draw an outline of a rectangle.
 *
 * \param  x           X coordinate of the left side.
 * \param  y           Y coordinate of the top side.
 * \param  width       Width of the rectangle.
 * \param  height      Height of the rectangle.
 * \param  color       Color of the rectangle, in display native format.
 */

/**
 * \def gfx_draw_filled_rect(x, y, width, height, color)
 * \brief Draw a filled rectangle.
 *
 * \param  x           X coordinate of the left side.
 * \param  y           Y coordinate of the top side.
 * \param  width       Width of the rectangle.
 * \param  height      Height of the rectangle.
 * \param  color       Color of the rectangle, in display native format.
 */

/**
 * \def gfx_draw_circle(x, y, radius, color, octant_mask)
 * \brief Draw an outline of a circle or arc.
 *
 * The radius is the distance from the center to the circumference,
 * which means that the total width or height of a circle will be
 * (radius*2+1).
 *
 * The octant_mask parameter is a bitmask that decides which octants of
 * the circle to draw. Use the GFX_OCTANTn, GFX_QUADRANTn, GFX_xHALF and
 * GFX_WHOLE contants and OR them together if required. Radius equal to
 * zero gives a single pixel.
 *
 * \param  x           X coordinate of center.
 * \param  y           Y coordinate of center.
 * \param  radius      Circle radius in pixels.
 * \param  color       Circle color in display native format.
 * \param  octant_mask Bitmask indicating which octants to draw.
 */

/**
 * \def gfx_draw_filled_circle(x, y, radius, color, quadrant_mask)
 * \brief Draw a filled circle or sector.
 *
 * The radius is the distance from the center to the circumference,
 * which means that the total width or height of a circle will be
 * (radius*2+1).
 *
 * The quadrant_mask parameter is a bitmask that decides which quadrants
 * of the circle to draw. Use the GFX_QUADRANTn, GFX_xHALF and
 * GFX_WHOLE contants and OR them together if required. Radius equal to
 * zero gives a single pixel.
 *
 * \note This function only supports quadrants while gfx_draw_circle()
 *       supports octants. This is to improve performance on drawing
 *       filled circles.
 *
 * \param  x           X coordinate of center.
 * \param  y           Y coordinate of center.
 * \param  radius      Circle radius in pixels.
 * \param  color       Circle color in display native format.
 * \param  quadrant_mask Bitmask indicating which quadrants to draw.
 */

/**
 * \def gfx_get_pixmap(pixmap, map_width, map_x,  map_y, x, y, width, height)
 * \brief Read a rectangular block of pixels from the screen into data
 * memory.
 *
 * If the area to read is outside the clipping region, those pixels will
 * not be read, and the corresponding data memory will be left
 * untouched.  Note that this function fails if width or height is
 * negative or the pixel rectangle is outside the pixmap buffer extents.
 * Clipping is only performed on the screen, not inside the pixmap
 * buffer itself.  Note that there is no checking if the pixel rectangle
 * exceeds the bottom of the pixmap buffer.
 *
 * \param  pixmap    Pointer to pixel buffer in data memory.
 * \param  map_width Width of the pixel buffer.
 * \param  map_x     X coordinate inside pixel buffer.
 * \param  map_y     Y coordinate inside pixel buffer.
 * \param  x         X coordinate on screen.
 * \param  y         Y coordinate on screen.
 * \param  width     Width of pixel rectangle to copy.
 * \param  height    Height of pixel rectangle to copy.
 */

/**
 * \def gfx_put_pixmap(pixmap, map_width, map_x, map_y, x, y, width, height);
 * \brief Write a rectangular block of pixels stored in data memory to
 * the screen.
 *
 * If the area to write is outside the clipping region, those pixels
 * will not be written.  Note that this function fails if the width or
 * height is negative or the pixel rectangle is outside the pixmap
 * buffer extents. Clipping is only performed on the screen, not inside
 * the pixmap buffer itself.  Note that there is no checking if the
 * pixel rectangle exceeds the bottom of the pixmap buffer.
 *
 * \param  pixmap    Pointer to pixel buffer in data memory.
 * \param  map_width Width of the pixel buffer.
 * \param  map_x     X coordinate inside pixel buffer.
 * \param  map_y     Y coordinate inside pixel buffer.
 * \param  x         X coordinate on screen.
 * \param  y         Y coordinate on screen.
 * \param  width     Width of pixel rectangle to copy.
 * \param  height    Height of pixel rectangle to copy.
 */

/**
 * \brief Draws a character to the display
 *
 * If \a background_color is set to \ref GFX_COLOR_TRANSPARENT, no background
 * is drawn.
 *
 * \param c        Character to be drawn
 * \param x        X coordinate on screen.
 * \param y        Y coordinate on screen.
 * \param font     Font to draw character in
 * \param color    Foreground color of character
 * \param background_color   Background color of character
 */
void gfx_draw_char(char c, gfx_coord_t x, gfx_coord_t y, struct font* font,
		gfx_color_t color, gfx_color_t background_color);

/**
 * \brief Draws a string to the display
 *
 * If \a background_color is set to \ref GFX_COLOR_TRANSPARENT, no background
 * is drawn.
 *
 * \param str       Pointer to string
 * \param x         X coordinate on screen.
 * \param y         Y coordinate on screen.
 * \param font      Font to draw string in
 * \param color     Foreground color of string
 * \param background_color   Background color of string
 */
void gfx_draw_string(char* str, gfx_coord_t x, gfx_coord_t y, struct font* font,
		gfx_color_t color, gfx_color_t background_color);

/*!
 * This function draws a string from progmem onto the screen.
 *
 * If \a background_color is set to \ref GFX_COLOR_TRANSPARENT, no background
 * is drawn.
 *
 * \param str              Pointer to string in progmem
 * \param x                X coordinate on screen.
 * \param y                Y coordinate on screen.
 * \param font             Font to draw string in
 * \param color            Foreground color of string
 * \param background_color Backround color of string
 */
void gfx_draw_progmem_string(const char __progmem_arg *str, gfx_coord_t x,
		gfx_coord_t y, struct font *font, gfx_color_t color,
		gfx_color_t background_color);

/**
 * \brief Computes the bounding box of a string
 *
 * If string is empty the returned width will be 1 pixel and the height equal
 * to the font height.
 *
 * \param str      String to calculate bounding box for
 * \param font     Font used
 * \param width    Pointer to width result
 * \param height   Pointer to height result
 */
void gfx_get_string_bounding_box(char const *str, struct font* font,
		gfx_coord_t *width, gfx_coord_t *height);

/**
 * \brief Computes the bounding box of a string stored in progmem
 *
 * If string is empty the returned width will be 1 pixel and the height equal
 * to the font height.
 *
 * \param str      String to calculate bounding box for
 * \param font     Font used
 * \param width    Pointer to width result
 * \param height   Pointer to height result
 */
void gfx_get_progmem_string_bounding_box(const char __progmem_arg *str,
		struct font *font, gfx_coord_t *width, gfx_coord_t *height);

//@}

//! @}

#endif /* GFX_H_INCLUDED */

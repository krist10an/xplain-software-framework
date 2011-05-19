/**
 * \file
 *
 * \brief Gauge widget implementation
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
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <membag.h>
#include <string.h>
#include <gfx/wtk.h>
#include <gfx/gfx.h>
#include <progmem.h>
#include <trigtable.h>

/**
 * \ingroup gfx_wtk_gauge
 * @{
 */

/**
 * \brief Gauge control struct.
 * \internal
 *
 * Contains a pointer to the Gauge's containing window, and non-window
 * information necessary for its operation. Structs of this type are created
 * with \ref wtk_gauge_create().
 *
 * \note Structure fields are only internally accessible. Only pointers to
 * these structures are needed externally.
 */

struct wtk_gauge {
	//! Container window of gauge.
	struct win_window       *container;
	//! Maximum value of gauge.
	uint8_t                 maximum;
	//! Value of gauge.
	uint8_t                 value;
	//! End position of gauge.
	uint8_t                 position;
	//! Configuration of orientation and behavior.
	uint8_t                 option;

	//! Color for gauge fill.
	gfx_color_t             fill_color;
	//! Color for gauge background.
	gfx_color_t             background_color;

	//! Boolean for drawing gauge background once.
	bool                    redraw_background;

	//! Boolean for drawing after transparent background redraw parent.
	bool                    solidbg;

	//! Variables for resize of the gauge variables.
	//! Rescaled data position value, rescaled to match trigtable array
	uint8_t                 rescale;

	//! Trigtable cos value accessed with rescale
	uint8_t                 xangle;
	//! Trigtable sine value accessed with rescale
	uint8_t                 yangle;

	//! Rescaled xangle for drawing the outer gauge line position correctly
	uint8_t                 xrescale;
	//! Rescaled yangle for drawing the outer gauge line position correctly
	uint8_t                 yrescale;

	//! Rescaled xangle for drawing the inner gauge line position correctly
	uint8_t                 x2rescale;
	//! Rescaled yangle for drawing the inner gauge line position correctly
	uint8_t                 y2rescale;

	//! Rescaled percent value for gauge outer line length
	uint8_t                 g_outer_pos;
	//! Rescaled percent value for gauge inner line length
	uint8_t                 g_inner_pos;
};

/**
 * \brief Get pointer to gauge window.
 *
 * Returns a reference to the window that should be used when managing the
 * widget, such as destroying, moving or reparenting it.
 *
 * \param gauge Pointer to wtk_gauge_gauge struct to manage.
 *
 * \return Pointer to window to be used for managing the gauge.
 */
struct win_window *wtk_gauge_as_child(struct wtk_gauge *gauge)
{
	assert(gauge);
	return gauge->container;
}

/**
 * \brief Set new gauge value.
 *
 * Updates the current value and issues a redrawing of the gauge if its
 * value was indeed changed.
 *
 * \param gauge Pointer to wtk_gauge struct to set new value for.
 * \param value New value for the gauge.
 *
 * \return True if gauge's value was changed.
 */
bool wtk_gauge_set_value(struct wtk_gauge *gauge, uint8_t value)
{
	uint8_t length;
	uint8_t option;
	uint8_t maximum;
	struct win_area const *area;

	assert(gauge);
	assert(value <= gauge->maximum);

	if (value != gauge->value) {
		gauge->value = value;
		option = gauge->option;
		maximum = gauge->maximum;
		area = win_get_area(gauge->container);

		length = area->size.x;

		length -= 2;

		if (option & WTK_GAUGE_INVERT) {
			value = maximum - value;
		}

		gauge->position = wtk_rescale_value(value, maximum, length);
		win_redraw(gauge->container);

		return true;

	} else {
		return false;
	}
}

/**
 * \brief Get gauge value.
 *
 * \param gauge Pointer to wtk_gauge struct to get value from.
 *
 * \return Value of gauge.
 */
uint8_t wtk_gauge_get_value(struct wtk_gauge *gauge)
{
	assert(gauge);
	return gauge->value;
}

/**
 * \brief Set new gauge colors.
 *
 * This sets new fill and background colors for the gauge. 
 *
 * \param gauge Pointer to wtk_gauge struct to set colors for.
 * \param fill_color Fill color to set for gauge.
 * \param background_color Background color to set for gauge.
 */
void wtk_gauge_set_colors(struct wtk_gauge *gauge, gfx_color_t fill_color,
		gfx_color_t background_color)
{
	assert(gauge);

	gauge->fill_color = fill_color;

	gauge->background_color = background_color;
}

/**
 * \brief Gauge draw functions
 * 
 * Multiple functions related to the gauge draw action.
 */

static void wtk_gauge_line_erase(struct win_clip_region const *clip,
		struct win_area const *area, struct wtk_gauge *gauge)
{
	//! The outer x-axis start position of the gauge needle
	gfx_coord_t gauge_needle_x_outer =
		clip->origin.x + gauge->xrescale + gauge->g_outer_pos;

	/**
	 * The outer y-axis start position of the gauge needle. Has a -3 offset
	 * to ensure that its within the gauge draw area.
	 */
	gfx_coord_t gauge_needle_y_outer =
		clip->origin.y + area->size.y - gauge->yrescale - 3;

	/**
	 * The inner x-axis start position of the gauge needle. Has a -3 offset
	 * to ensure that its within the gauge draw area.
	 */
	gfx_coord_t gauge_needle_x_inner =
		clip->origin.x + area->size.x - gauge->g_inner_pos +
		gauge->x2rescale - 3;

	/**
	 * The inner y-axis start position of the gauge needle. Has a -3 offset
	 * to ensure that its within the gauge draw area.
	 */
	gfx_coord_t gauge_needle_y_inner =
		clip->origin.y + area->size.y - gauge->y2rescale - 3;

	//! Gauge middle line
	gfx_draw_line(gauge_needle_x_outer, gauge_needle_y_outer,
			gauge_needle_x_inner, gauge_needle_y_inner,
			gauge->background_color);

	#if WTK_GAUGE_NEEDLE_LINE_THICKNESS > 1
	//! Right line has +1 offset on x-axis
	gfx_draw_line(gauge_needle_x_outer + 1, gauge_needle_y_outer,
			gauge_needle_x_inner + 1, gauge_needle_y_inner,
			gauge->background_color);

	//! Left line has +1 offset on y-axis
	gfx_draw_line(gauge_needle_x_outer, gauge_needle_y_outer + 1,
			gauge_needle_x_inner, gauge_needle_y_inner + 1,
			gauge->background_color);
	#endif
}

static void wtk_gauge_draw_background(struct win_clip_region const *clip,
		struct win_area const *area, struct wtk_gauge *gauge)
{
	/**
	 * The x-dimension of the gauge circles
	 * The -2 is an offset, so that the drawn lines are within the gauge
	 * area
	 */
	gfx_coord_t gauge_area_length = area->size.x - 2;

	/** 
	 * The y-dimension of the gauge circles
	 * The -2 is an offset, so that the drawn lines are within the gauge
	 * area
	 */
	gfx_coord_t gauge_area_height = area->size.y - 2;

	/** 
	 * Drawa a window border. Has a +1 to the offset, so that its outside 
	 * the filled circles
	 */
	gfx_draw_horizontal_line(clip->origin.x,
			clip->origin.y + gauge_area_height + 1,
			area->size.y, WTK_GAUGE_OUTER_LINE_COLOR);

	gfx_draw_vertical_line(clip->origin.x + gauge_area_length + 1,
			clip->origin.y, area->size.x, WTK_GAUGE_OUTER_LINE_COLOR);

	//! Outer filled circle
	gfx_draw_filled_circle(clip->origin.x + gauge_area_length,
			clip->origin.y + gauge_area_height, gauge_area_length,
			gauge->background_color, GFX_QUADRANT1);

	/** 
	 * Inner filled circle. Has -2 offset to the radius to ensure that its 
	 * within the gauge area.
	 */
	gfx_draw_filled_circle(clip->origin.x + gauge_area_length,
			clip->origin.y + gauge_area_height,
			gauge->g_inner_pos - 2, gauge->fill_color,
			GFX_QUADRANT1);

	/**
	 * Inner circle black line. Has -2 offset to the radius to ensure that
	 * its within the gauge area.
	 */
	gfx_draw_circle(clip->origin.x + gauge_area_length,
			clip->origin.y + gauge_area_height,
			gauge->g_inner_pos - 2, WTK_GAUGE_INNER_LINE_COLOR,
			GFX_QUADRANT1);

	//! Draws gauge track circle in quadrant 1
	//! Outer edge black line.
	gfx_draw_circle(clip->origin.x + gauge_area_length,
			clip->origin.y + gauge_area_height, gauge_area_length,
			WTK_GAUGE_OUTER_LINE_COLOR, GFX_QUADRANT1);

}

static void wtk_gauge_draw_line(struct win_clip_region const *clip,
		struct win_area const *area, struct wtk_gauge *gauge)
{
	//! The outer x-axis start position of the gauge needle
	gfx_coord_t gauge_needle_x_outer =
		clip->origin.x + gauge->xrescale + gauge->g_outer_pos;

	/**
	 * The outer y-axis start position of the gauge needle. Has a -3 offset
	 * to ensure that its within the gauge draw area.
	 */
	gfx_coord_t gauge_needle_y_outer =
		clip->origin.y + area->size.y - gauge->yrescale - 3;

	/**
	 * The inner x-axis start position of the gauge needle. Has a -3 offset
	 * to ensure that its within the gauge draw area.
	 */
	gfx_coord_t gauge_needle_x_inner =
		clip->origin.x + area->size.x - gauge->g_inner_pos + 
		gauge->x2rescale - 3;

	/**
	 * The inner y-axis start position of the gauge needle. Has a -3 offset
	 * to ensure that its within the gauge draw area.
	 */
	gfx_coord_t gauge_needle_y_inner =
		clip->origin.y + area->size.y - gauge->y2rescale - 3;

	//! Draws the gauge middle line from the rescaled position value
	gfx_draw_line(gauge_needle_x_outer, gauge_needle_y_outer,
			gauge_needle_x_inner, gauge_needle_y_inner,
			WTK_GAUGE_NEEDLE_COLOR);

	#if WTK_GAUGE_NEEDLE_LINE_THICKNESS > 1
	//! Right line has +1 offset on x-axis
	gfx_draw_line(gauge_needle_x_outer + 1, gauge_needle_y_outer,
			gauge_needle_x_inner + 1, gauge_needle_y_inner,
			WTK_GAUGE_NEEDLE_COLOR);

	//! Left line has +1 offset on y-axis
	gfx_draw_line(gauge_needle_x_outer, gauge_needle_y_outer + 1,
			gauge_needle_x_inner, gauge_needle_y_inner + 1,
			WTK_GAUGE_NEEDLE_COLOR);
	#endif
}

static void wtk_gauge_redraw_background(struct wtk_gauge *gauge)
{
	//! Enables the redraw of gauge background
	gauge->redraw_background = true;
}

/**
 * \brief Gauge event handler.
 *
 * This is the window event handler for gauge widgets. It handles the two
 * relevant event types sent to a gauge's container window, i.e., drawing,
 * and destroy events.
 *
 * \param win Window receiving the event.
 * \param type The event type.
 * \param data Custom data, depending on event type.
 *
 * \return True if the event was recognized and accepted.
 */
static bool wtk_gauge_handler(struct win_window *win,
		enum win_event_type type, void const *data)
{
	struct win_clip_region const    *clip;
	struct win_area const           *area;
	struct wtk_gauge                *gauge;
	uint8_t                         position;
	uint8_t                         option;

	 gauge = (struct wtk_gauge *)win_get_custom_data(win);

	//! Window receiving the event should be the widget's containing window.
	assert(win == gauge->container);

	switch (type) {
	case WIN_EVENT_DRAW:
		/* For DRAW events, the data parameter points to the clipping
		 * region. The window area parameter is needed because it
		 * contains the size of the widget.
		 */
		clip = (struct win_clip_region const *)data;
		area = win_get_area(win);

		position = gauge->position;
		option = gauge->option;

	 
	/** Erases the previous gauge line using old x\y values, 
	 * not enabled at first draw event
	 */
	
 		if (!gauge->redraw_background && gauge->solidbg) {
			//! Erases gauge line
			wtk_gauge_line_erase(clip, area, gauge);
		}
		
		
		//! Draw the gauge background elements once
		if (gauge->redraw_background && gauge->solidbg) {
			
			//! Draws background
			wtk_gauge_draw_background(clip, area, gauge);
		
			/** Sets redraw_background to false so the background 
			 * draw is halted and enables the gauge line erase 
			 * function for next draw event
			 */
			gauge->redraw_background = false;
		}

		/**
		 * Rescales the position value for accessing data in the
		 * trigtable array.The -2 after area-> size.x is a offset,
		 * so that the line is within the frame
		 */
		gauge->rescale = wtk_rescale_value(position, 
				area->size.x - 2, WTK_TRIG_TABLE_MAX_VALUE / 2);

		//! Reads x trigonometric value from PROGMEM array
		gauge->xangle = WTK_TRIG_TABLE_MAX_VALUE 
				- wtk_trigtable_cos(gauge->rescale);

		//! Reads y trigonometric value from PROGMEM array
		gauge->yangle = wtk_trigtable_sin(gauge->rescale);

		/**
		 * Rescales the first x trigonometric value for 
		 * usage in the draw function. Uses -3 offset to ensure that
		 * its witin draw area
		 */
		gauge->xrescale = wtk_rescale_value(gauge->xangle,
				WTK_TRIG_TABLE_MAX_VALUE, 
				area->size.x - 3 - gauge->g_outer_pos);

		/**
		 * Rescales the first y trigonometric value for usage in the 
		 * draw function Uses -3 offset to ensure that its witin draw area
		 */
		gauge->yrescale = wtk_rescale_value(gauge->yangle,
				WTK_TRIG_TABLE_MAX_VALUE, 
				area->size.y - 3 - gauge->g_outer_pos);

		/**
		 * Rescales the second x trigonometric value for usage in the 
		 * draw function
		 */
		gauge->x2rescale = wtk_rescale_value(gauge->xangle,
				WTK_TRIG_TABLE_MAX_VALUE, gauge->g_inner_pos);

		/**
		 * Rescales the second y trigonometric value for usage in the 
		 * draw function
		 */
		gauge->y2rescale = wtk_rescale_value(gauge->yangle,
				WTK_TRIG_TABLE_MAX_VALUE, gauge->g_inner_pos);

		//! Draws gauge line
		wtk_gauge_draw_line(clip, area, gauge);

		/** Always accept DRAW events, as the return value is ignored
		 * anyway for that event type.
		 */
		return true;

	case WIN_EVENT_DESTROY:
		/**
		 * Free up all memory allocated by widget.
		 * The window is freed by the window system
		 */
		membag_free(gauge);
		return true;

	default:
		return false;
	}
}

/**
 * \brief Create a new gauge widget.
 *
 * Allocates the necessary memory and intializes the window and data for
 * gauge widgets. If there is not enough memory, the function returns
 * NULL.
 * 
 * To destroy a gauge widget and all its contents, and free its
 * memory, call \ref win_destroy() on the gauge's child reference, given
 * by \ref wtk_gauge_as_child(), like this:
 * "win_destroy(wtk_gauge_as_child(my_gauge_ptr));".\par
 *
 * Gauge widgets divide their window area in two non-overlapping
 * rectangles: one with a fill color, and one with a background color.
 * The ratio between the two rectangles' sizes is given by the gauge's
 * value relative to its maximum: a higher value gives a larger fill.\par
 *
 * By default, a vertically oriented gauge fills from the top, while a
 * horizontal one fills from the left. The gauge's orientation and fill
 * direction can both be configured at the time of creation. The fill and
 * background colors can be changed at runtime.\par
 *
 * Refer to <gfx/wtk.h> for available configuration options.
 *
 * \todo Revisit, support larger gauges and values given a config symbol.
 *
 * \param parent Pointer to parent ref win_window struct.
 * \param area Pointer to ref win_area struct with position and size of the
 * gauge. Minimum size in both x and y direction is 3 pixels.
 * \param maximum Maximum value of the gauge.
 * \param value Initial value of the gauge.
 * \param g_outer_pos Value for the endpoint of the needle in percent of
 * total diameter
 * \param g_inner_pos Value for the start point of the needle in percent of
 * total diameter
 * \param fill_color Color for filled area.
 * \param background_color Color for background area.
 * \param option \ref gfx_wtk_gauge_options "Configuration options for gauge"
 * \return Pointer to new gauge, if memory allocation was successful.
 */
struct wtk_gauge *wtk_gauge_create(struct win_window *parent,
		struct win_area const *area, struct gfx_bitmap *background,
		uint8_t maximum, uint8_t value, uint8_t g_outer_pos,
		uint8_t g_inner_pos, gfx_color_t fill_color,
		gfx_color_t background_color, uint8_t option)
{
	uint8_t length;

	// Do sanity check on parameters.
	assert(maximum > 0);
	assert(value <= maximum);
	assert(area);
	assert(parent);

	// Attributes scratchpad.
	struct win_attributes attr;

	// Allocate memory for the control data.
	struct wtk_gauge *gauge =
			membag_alloc(sizeof(struct wtk_gauge));
	if (!gauge) {
		goto outofmem_gauge;
	}

	// Initialize the gauge data.
	gauge->maximum = maximum;
	gauge->value = value;
	gauge->option = option;
	// Enable background redraw
	wtk_gauge_redraw_background(gauge);

	// Set the gauge's colors
	gauge->fill_color = fill_color;
		
	gauge->background_color = background_color;
	
	if (option & WTK_GAUGE_INVERT) {
		value = maximum - value;
	}

	// Set up handling information.
	attr.event_handler = wtk_gauge_handler;
	attr.custom = gauge;

	/* Do sanity check of specified window area parameters
	 * according to the orientation of the gauge.
	 */
	attr.area = *area;
	assert(attr.area.size.x > 3);
	assert(attr.area.size.y > 3);


	assert(attr.ares.size.x <= 255);
	length = attr.area.size.x;


	length -= 2;

	// Checks if line pos is an accepted value, else set to max, fail safe
	if(g_outer_pos < WTK_GAUGE_MIN_NEEDLE_PERCENT_SIZE ||
			g_outer_pos > WTK_GAUGE_MAX_NEEDLE_PERCENT_SIZE) {
		g_outer_pos = WTK_GAUGE_MAX_NEEDLE_PERCENT_SIZE;
	}
	
	if(g_inner_pos < WTK_GAUGE_MIN_NEEDLE_PERCENT_SIZE ||
			g_inner_pos > WTK_GAUGE_MAX_NEEDLE_PERCENT_SIZE) {
		g_inner_pos = WTK_GAUGE_MAX_NEEDLE_PERCENT_SIZE;
	}

	// Rescales 0-100% into approptiate gauge length size
	gauge->g_outer_pos = 
		wtk_rescale_value(WTK_GAUGE_MAX_NEEDLE_PERCENT_SIZE - g_outer_pos,
				WTK_GAUGE_MAX_NEEDLE_PERCENT_SIZE, area->size.x - 2);
	gauge->g_inner_pos = wtk_rescale_value(g_inner_pos,
			WTK_GAUGE_MAX_NEEDLE_PERCENT_SIZE, area->size.x - 2);

	// Set the gauge's end position.
	gauge->position = wtk_rescale_value(value, maximum, length);

	// Set background for gauge window
	if (background) {
		// solid background bitmap
		attr.background = NULL;
		attr.behavior = 0;
		gauge->solidbg = true;
	} else {
		// transparent, redraw parent
		attr.background = NULL;
		attr.behavior = WIN_BEHAVIOR_REDRAW_PARENT;
		gauge->solidbg = false;
	}

	// Create a new window for the gauge.
	gauge->container = win_create(parent, &attr);
	if (!gauge->container) {
		goto outofmem_container;
	}

	return gauge;

outofmem_container:
	membag_free(gauge);

outofmem_gauge:
	return NULL;
}

//! @}

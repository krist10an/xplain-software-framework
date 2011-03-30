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
#include <gfx/gfx_generic.h>
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
	struct win_window		*container;
	//! Maximum value of gauge.
	uint8_t					maximum;
	//! Value of gauge.
	uint8_t					value;
	//! End position of gauge.
	uint8_t					position;
	//! Configuration of orientation and behavior.
	uint8_t					option;
	
	//! Color for gauge fill.
	gfx_color_t				fill_color;
	//! Color for gauge background.
	gfx_color_t				background_color;
	//! Color for parent background.
	gfx_color_t				parent_background_color;
	
	//! Boolean for drawing gauge background once.
	bool					start;
	
	//! Boolean for drawing after transparent background redraw parent.
	bool					solidbg;
	
	/* Variables for resizeing the gauge variables. */

	/* Rescaled data position value, rescaled to fit accessing trigtable 
	 * array 128 values
	 */
	uint8_t					rescale;
	
	//! Inverted trigtable value accessed with rescale
	uint8_t					xangle;
	//! Trigtable value accessed with rescale
    uint8_t					yangle;
	
	//! Rescaled xangle for drawing the first gauge line position correctly
	uint8_t					xrescale;
	//! Rescaled yangle for drawing the first gauge line position correctly
	uint8_t					yrescale;
	
	//! Rescaled xangle for drawing the second gauge line position correctly
	uint8_t					x2rescale;
	//! Rescaled yangle for drawing the second gauge line position correctly
	uint8_t					y2rescale;

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
 * \Returns the gauge struct values for testing
 * \temporary test function
 */
uint8_t wtk_gauge_get_test(struct wtk_gauge *gauge)
{
	assert(gauge);
	return gauge->rescale;
}

/**
 * \Trigtable access
 * \temporary test function
 * \TODO: remove
 */
 
 uint8_t wtk_gauge_trigtable(uint8_t angle)
{
	return progmem_read8(&(trigtable[angle]));
}


/**
 * \brief Set new gauge colors.
 *
 * This sets new fill and background colors for the gauge. If the gauge is
 * inverted, the two colors are switched.
 *
 * \param gauge Pointer to wtk_gauge struct to set colors for.
 * \param fill_color Fill color to set for gauge.
 * \param background_color Background color to set for gauge.
 */
void wtk_gauge_set_colors(struct wtk_gauge *gauge,
		gfx_color_t fill_color, gfx_color_t background_color)
{
	assert(gauge);
  
	gauge->fill_color = fill_color;
	
	gauge->background_color = background_color;

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

	// Window receiving the event should be the widget's containing window.
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


		/*
		* !! GAUGE !! 
		*
		* Draw gauge!
		*
		* Need to define more external colors
		* 
		*/
		
		
	/* New gauge bit option, for drawing different gauges
	 * Yet to be implemented!
	 */
	if (option & WTK_GAUGE_CIRCLE) {
	
	}
		
	else {
		
	
	
	/* Erases the previous gauge line using old x\y values, 
	 * not enabled at first draw event
	 */
 		if (!gauge->start&gauge->solidbg) {
			//! Middle line
			gfx_generic_draw_line(
				clip->origin.x + gauge->xrescale + 
					area->size.x / 7,
				clip->origin.y + area->size.y - 
					gauge->yrescale - 3,
				clip->origin.x + area->size.x - 3 - 
					area->size.x / 3 + gauge->x2rescale,
				clip->origin.y + area->size.y - 3 - 
					gauge->y2rescale,
				gauge->background_color);
	
			//! Right line +1 X -1 Y                   
			gfx_generic_draw_line(
				clip->origin.x + gauge->xrescale + 
					area->size.x / 7 + 1,
				clip->origin.y + area->size.y - 
					gauge->yrescale - 3,
				clip->origin.x + area->size.x - 3 - 
					area->size.x / 3 + gauge->x2rescale + 1,
				clip->origin.y + area->size.y - 3 - 
					gauge->y2rescale,
				gauge->background_color);
			//! Left line -1 X +1 Y
			gfx_generic_draw_line(
				clip->origin.x + gauge->xrescale + 
					area->size.x / 7,
				clip->origin.y + area->size.y - 
					gauge->yrescale - 3 + 1,
				clip->origin.x + area->size.x - 3 - 
					area->size.x / 3 + gauge->x2rescale,
				clip->origin.y + area->size.y - 3 - 
					gauge->y2rescale + 1,
				gauge->background_color);
		}
		
		
		//! Draw the gauge background elements once
		if (gauge->start&gauge->solidbg) {
		
		//! Draw a window border.
			gfx_draw_rect(clip->origin.x, clip->origin.y, 
				area->size.x, area->size.y, 
				WTK_PROGRESS_BAR_BORDER_COLOR);
				
			gfx_draw_filled_rect(clip->origin.x,
				clip->origin.y,
				area->size.x - 2,
				area->size.y - 2,
				GFX_COLOR(77, 77, 77));
				
				
			/* Draws gauge track circle in quadrant 1 */
			//! Outer edge black line
			gfx_draw_circle(clip->origin.x + area->size.x - 2,
				clip->origin.y + area->size.y - 2,
				area->size.x - 2,
				GFX_COLOR(0, 0, 0), GFX_QUADRANT1);
			//! Outer filled circle
			gfx_draw_filled_circle(clip->origin.x + area->size.x - 2,
				clip->origin.y + area->size.y - 2,
				area->size.x - 3,
				gauge->background_color, GFX_QUADRANT1);
			//! Inner circle black line
			gfx_draw_circle(clip->origin.x + area->size.x - 2,
				clip->origin.y + area->size.y - 2,
				area->size.x / 3 - 2,
				GFX_COLOR(0, 0, 0), GFX_QUADRANT1);
			//! Inner filled circle
			gfx_draw_filled_circle(clip->origin.x + area->size.x - 2,
				clip->origin.y + area->size.y - 2,
				area->size.x / 3 - 3,
				GFX_COLOR(150, 150, 150), GFX_QUADRANT1);
				
			/* Sets start to false so the background draw is halted 
			 * and enables the gauge line erase function for next 
			 * draw event
			 */
			gauge->start = false;
		}
		
		//! For draw functions with transparent background
		if(!gauge->solidbg) {

		}
		

		
		/* Rescales the position value for accessing 
		 * data in the trigtable array
		*/
		
		gauge->rescale = wtk_rescale_value(position, 
			area->size.x - 2, 127);
		
		
		
		//! Reads x trigonometric value from PROGMEM array
		gauge->xangle = 255 - progmem_read8(&
			(trigtable[127 - gauge->rescale]));
			
		//! Reads x trigonometric value from PROGMEM array
		gauge->yangle = progmem_read8(&(trigtable[gauge->rescale]));
				
		/* Rescales the first x trigonometric value for 
		 * usage in the draw function
		 */
		gauge->xrescale = wtk_rescale_value(gauge->xangle, 255, 
			area->size.x - 3 - area->size.x / 7);
		
		/* Rescales the first y trigonometric value for usage in the 
		 * draw function
		 */
		gauge->yrescale = wtk_rescale_value(gauge->yangle, 255, 
			area->size.y - 3 - area->size.x / 7);
		
		/* Rescales the second x trigonometric value for usage in the 
		 * draw function
		 */
		gauge->x2rescale = wtk_rescale_value(gauge->xangle, 255, 
			area->size.x / 3);
		
		/* Rescales the second y trigonometric value for usage in the 
		 * draw function
		 */
		gauge->y2rescale = wtk_rescale_value(gauge->yangle, 255, 
			area->size.y / 3);
		

		//! Draws the gauge line from the rescaled position value
		gfx_generic_draw_line(clip->origin.x + gauge->xrescale + 
			area->size.x / 7,
			clip->origin.y + area->size.y - gauge->yrescale - 3,
			clip->origin.x + area->size.x - 3 - area->size.x / 3 + 
				gauge->x2rescale,
			clip->origin.y + area->size.y - 3 - gauge->y2rescale,
			GFX_COLOR(200, 0, 0));
		
		//! Right line +1 X -1 Y
		gfx_generic_draw_line(clip->origin.x + gauge->xrescale + 
				area->size.x / 7 + 1,
			clip->origin.y + area->size.y - gauge->yrescale - 3,
			clip->origin.x + area->size.x - 3 - area->size.x / 3 + 
				gauge->x2rescale + 1,
			clip->origin.y + area->size.y - 3 - gauge->y2rescale,
			gauge->fill_color);
		
		//! Left line -1 X +1 Y
		gfx_generic_draw_line(clip->origin.x + gauge->xrescale + 
				area->size.x / 7,
			clip->origin.y + area->size.y - gauge->yrescale - 3 + 1,
			clip->origin.x + area->size.x - 3 - area->size.x / 3 + 
				gauge->x2rescale,
			clip->origin.y + area->size.y - 3 - gauge->y2rescale + 1,
			GFX_COLOR(170, 0, 0));
		
		
		/* Positions Xplained!
		 * clip->origin.(x\y)                               
		 * -- the frames start position top left cord(0.0) - start pixel
		 * area->size.(x\y)                                 
		 * -- the area size aquired from area defined in widget
		 * clip->origin.(x\y) + area.size.(x\y) - 1               
		 * -- the frames right bottom corner - end pixel
		 * clip->origin.(x\y) (+ 1)(+ area->size.y - 3)     
		 * -- the gauges start position 
		 */


		/* Always accept DRAW events, as the return value is ignored
		 * anyway for that event type.
		 */
		 }
		return true;

	case WIN_EVENT_DESTROY:
		/* Free up all memory allocated by widget.
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
 * progress gauge widgets. If there is not enough memory, the function returns
 * NULL.\n To destroy a gauge widget and all its contents, and free its
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
 * \param parent Pointer to parent win_window struct.
 * \param area Pointer to win_area struct with position and size of the
 *             gauge. Minimum size in both x and y direction is 3 pixels.
 * \param maximum Maximum value of the gauge.
 * \param value Initial value of the gauge.
 * \param fill_color Color for filled area.
 * \param background_color Color for background area.
 * \param option Configuration options for gauge.
 *
 * \return Pointer to new gauge, if memory allocation was successful.
 */
struct wtk_gauge *wtk_gauge_create(struct win_window *parent,
		struct win_area const *area, struct gfx_bitmap *background, 
		uint8_t maximum, uint8_t value,	gfx_color_t fill_color, 
		gfx_color_t background_color, 
		gfx_color_t parent_background_color, uint8_t option)
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
	gauge->start = true;

	/* Set the gauge's colors
	 * 
	 */
	 
	gauge->fill_color = fill_color;
		
	gauge->background_color = background_color;
	
	gauge->parent_background_color = parent_background_color;
	
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


	assert(attr.area.size.x < (uint8_t) ~ 0);
	length = attr.area.size.x;


	length -= 2;

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

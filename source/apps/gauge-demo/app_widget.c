//gauge_dev
/**
 * \file
 *
 * \brief Gauge demo application
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

#include <led.h>
#include <board.h>
#include <string.h>


#include <gfx/gfx.h>
#include <gfx/win.h>
#include <gfx/wtk.h>
#include <gfx/sysfont.h>
#include <membag.h>
#include <debug.h>
#include <mainloop.h>
#include <string.h>
#include <stream.h>



#include "app_widget.h"



/**
 * \weakgroup app_widget_group
 *
 * @{
 */

/**
 * \brief Event command ID for the application widgets.
 *
 * \note The command IDs cannot be 0, since this value is reserved for no
 * command event callback for certain widgets.
 */
enum app_widget_ids {
	//! Event command ID for the slider.
	SLIDER_ID = 1,
	//! Event command ID for the second slider.
	SLIDER_TWO_ID = 2,
};

/**
 * \name Color scheme
 *
 * @{
 */

//! Background color for application
#define APP_BACKGROUND_COLOR        GFX_COLOR(77, 77, 77)

//! @}

/**
 * \name Widget sizes and positions
 *
 * @{
 */

//! Label position on top of display
#define LABEL_POS_X                 6
//! Label position on top of display
#define LABEL_POS_Y                 6

//! Slider position
#define SLIDER_POS_X                5
//! Slider position
#define SLIDER_POS_Y                25
//! Slider size on display
#define SLIDER_SIZE_X               25
//! Slider size on display
#define SLIDER_SIZE_Y               180

//! Gauge position
#define GAUGE_POS_X                 50
//! Gauge position
#define GAUGE_POS_Y                 40
//! Gauge size on display
#define GAUGE_SIZE                  150


//! Slider_two position
#define SLIDER_TWO_POS_X            60
//! Slider_two position
#define SLIDER_TWO_POS_Y            205
//! Slider_two size on display
#define SLIDER_TWO_SIZE_X           255
//! Slider_two size on display
#define SLIDER_TWO_SIZE_Y           25

//! Gauge_two position
#define GAUGE_TWO_POS_X             210
//! Gauge_two position
#define GAUGE_TWO_POS_Y             80
//! Gauge_two size on display
#define GAUGE_TWO_SIZE              100

//! Gauge line distance modifier (0-100%) (OUTER>INNER)
#define GAUGE_OUTER_POS             90
#define GAUGE_INNER_POS             30

//! Second gauge line distance modifier (0-100%)
#define GAUGE_TWO_OUTER_POS         100
#define GAUGE_TWO_INNER_POS         0


//! Gauge line height start position
#define GAUGE_START_DISPLACEMENT    0


//! @}

/**
 * \name Widget configurations
 *
 * @{
 */

//! Max value for slider
#define SLIDER_MAX_VALUE            240
//! Max value for slidertwo
#define SLIDER_TWO_MAX_VALUE        240


//! @}

/**
 * \name Static text strings
 *
 * @{
 */

//! Description for label
const static char                   *demo_string = "Gauge Demo";

//! @}

/**
 * \name Static variables
 *
 * @{
 */

//! Pointer to frame for application
static struct wtk_basic_frame       *frame;
//! Pointer to slider
static struct wtk_slider            *slider;
//! Pointer to slidertwo
static struct wtk_slider            *slidertwo;

//! Frame background bitmap
static struct gfx_bitmap            frame_background;
//! Pointer to the sub-frame
static struct wtk_basic_frame       *sub_frame;

//! Gauge background bitmap
static struct gfx_bitmap            gauge_background;

//! Pointer to gauge
static struct wtk_gauge             *gauge;
//! Pointer to gauge_two
static struct wtk_gauge             *gauge_two;


//! @}

/**
 * \brief Frame command events handler
 *
 * This function handles the command events generated by the widgets.
 *
 * \sa wtk_basic_frame_command_handler_t
 */
static bool widget_frame_command_handler(struct wtk_basic_frame *frame,
		win_command_t command_data)
{
	char command = (char)(uintptr_t)command_data;

	switch (command) {
	case SLIDER_ID:
		wtk_gauge_set_value(gauge,
		wtk_slider_get_value(slider));
		break;

	case SLIDER_TWO_ID:
		wtk_gauge_set_value(gauge_two,
		wtk_slider_get_value(slidertwo));
		break;
	}

	return false;
}

/**
 * \brief Frame draw event handler
 *
 * This function will draw the contents of the sub-frame.
 *
 * \sa wtk_basic_frame_draw_handler_t
 */

/**
 * \brief Setup gauge demo
 *
 * This function launches the gauge demo.
 *
 * \param task Workqueue task used to start the application.
 */
void app_widget_launch(struct workqueue_task *task) {

	struct win_window       *win_root;
	struct win_window       *parent;
	struct win_area         area;
	struct wtk_label        *lbl;

	// Use normal sysfont for this app
	sysfont.scale = 2;

	// Get pointer to root window
	win_root = win_get_root();

	// Application frame

	// Create a background bitmap using a solid color.
	frame_background.type = BITMAP_SOLID;
	frame_background.data.color = APP_BACKGROUND_COLOR;

	// Set the area to fill the entire screen
	area.pos.x = 0;
	area.pos.y = 0;
	area.size.x = gfx_get_width();
	area.size.y = gfx_get_height();

	/*
	 * Create a basic frame with a specified background and command event
	 * handler. Check the return value if an error occured while creating
	 * the widget.
	 */
	frame = wtk_basic_frame_create(win_root, &area, &frame_background, NULL,
			widget_frame_command_handler, NULL);
	if (!frame) {
		goto error_frame;
	}

	// Get a pointer to the widget's window for adding sub-widgets.
	parent = wtk_basic_frame_as_child(frame);
	/*
	 * Draw the frame by showing the frame widget's window. Any
	 * child-widgets and windows will not be shown before the parent
	 * widget/window is shown.
	 */
	win_show(parent);

	// Application label
	area.pos.x = LABEL_POS_X;
	area.pos.y = LABEL_POS_Y;
	// Find an optimal size for the widget.
	wtk_label_size_hint(&area.size, demo_string);

	/*
	 * Create the label and check the return value if an error occured
	 * while creating the label.
	 */
	lbl = wtk_label_create(parent, &area, demo_string, false);
	if (!lbl) {
		goto error_widget;
	}

	// Draw the label by showing the label widget's window.
	win_show(wtk_label_as_child(lbl));

	// Application slider
	area.pos.x = SLIDER_POS_X;
	area.pos.y = SLIDER_POS_Y;
	area.size.x = SLIDER_SIZE_X;
	area.size.y = SLIDER_SIZE_Y;

	/*
	 * Create the slider and check the return value if an error occured
	 * while creating the slider.
	 */
	slider = wtk_slider_create(parent, &area, SLIDER_MAX_VALUE,
			SLIDER_MAX_VALUE / 2, 
			WTK_SLIDER_VERTICAL|WTK_SLIDER_CMD_RELEASE,
			(win_command_t)SLIDER_ID);
	if (!slider) {
		goto error_widget;
	}

	// Draw the slider by showing the slider widget's window.
	win_show(wtk_slider_as_child(slider));

	// Application slidertwo
	area.pos.x = SLIDER_TWO_POS_X;
	area.pos.y = SLIDER_TWO_POS_Y;
	area.size.x = SLIDER_TWO_SIZE_X;
	area.size.y = SLIDER_TWO_SIZE_Y;

	/*
	 * Create the slidertwo and check the return value if an error occured
	 * while creating the slidertwo.
	 */
	slidertwo = wtk_slider_create(parent, &area, SLIDER_TWO_MAX_VALUE,
			SLIDER_TWO_MAX_VALUE / 2, WTK_SLIDER_CMD_RELEASE,
			(win_command_t)SLIDER_TWO_ID);
	if (!slidertwo) {
		goto error_widget;
	}



	// Draw the slider by showing the slider widget's window.
	win_show(wtk_slider_as_child(slidertwo));

	// Application gauge.
	area.pos.x = GAUGE_POS_X;
	area.pos.y = GAUGE_POS_Y;
	area.size.x = GAUGE_SIZE;
	area.size.y = GAUGE_SIZE;


	/*
	 * Create the first gauge and check the return value if an error
	 * occured while creating the gauge.
	 */

	gauge_background.type = BITMAP_SOLID;
	gauge_background.data.color = GFX_COLOR(255, 255, 255);

	gauge = wtk_gauge_create(parent, &area, &gauge_background, 
		SLIDER_MAX_VALUE + GAUGE_START_DISPLACEMENT,
			(SLIDER_MAX_VALUE + GAUGE_START_DISPLACEMENT) / 2, 
			GAUGE_OUTER_POS, GAUGE_INNER_POS, GFX_COLOR(255, 0, 0),
			GFX_COLOR(100, 100, 100), WTK_GAUGE_INVERT);
	if (!gauge) {
		goto error_widget;
	}

	// Draw the gauge by showing the gauge widget's window.
	win_show(wtk_gauge_as_child(gauge));


	// Application gauge_two.
	area.pos.x = GAUGE_TWO_POS_X;
	area.pos.y = GAUGE_TWO_POS_Y;
	area.size.x = GAUGE_TWO_SIZE;
	area.size.y = GAUGE_TWO_SIZE;


	/*
	 * Create the second gauge and check the return value if an error
	 * occured while creating the gauge.
	 */
	gauge_two = wtk_gauge_create(parent, &area, NULL, 
		SLIDER_TWO_MAX_VALUE + GAUGE_START_DISPLACEMENT,
		(SLIDER_TWO_MAX_VALUE + GAUGE_START_DISPLACEMENT) / 2, 
		GAUGE_TWO_OUTER_POS, GAUGE_TWO_INNER_POS, GFX_COLOR(255, 0, 0),
		GFX_COLOR(100, 100, 100), 0);
	if (!gauge_two) {
		goto error_widget;
	}

	// Draw the second gauge by showing the gauge widget's window.
	win_show(wtk_gauge_as_child(gauge_two));

	//! Gauge frame code
	area.pos.x += area.size.x + 10;
	area.size.x = 50;
	area.size.y = 20;

	win_show(wtk_basic_frame_as_child(sub_frame));

	return;

error_widget:
	// Destroy widget and all sub-widgets.
	win_destroy(wtk_basic_frame_as_child(frame));
error_frame:
	// Wait forever if an error occured during setup.
	while(1);
}

//! @}

/**
 * \file
 *
 * \brief Display demo widget application
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

#include "app_desktop.h"
#include "app_widget.h"

/**
 * \weakgroup app_widget_group
 *
 * @{
 */

/**
 * \brief Command event IDs
 *
 * \note The first entry in this enum is zero-valued, and therefore reserved for
 * widgets that should not issue command events. \sa gfx_wtk
 */
enum command_id {
	//! Unused command event ID.
	DUMMY_ID,
	//! Command event ID for slider
	SLIDER_ID,
	//! Command event ID for check box
	CHECK_BOX_ID,
	//! Command event ID for button 1
	RADIO_BUTTON_1_ID,
	//! Command event ID for button 2
	RADIO_BUTTON_2_ID,
	//! Command event ID for quit button
	BUTTON_QUIT_ID,
};

/**
 * \name Color scheme
 *
 * @{
 */

//! Color scheme 0 foreground color
#define COLOR_SCHEME_0_FG           GFX_COLOR(255,255,0)
//! Color scheme 0 background color
#define COLOR_SCHEME_0_BG           GFX_COLOR(0,50,0)
//! Color scheme 1 foreground color
#define COLOR_SCHEME_1_FG           GFX_COLOR(0,255,0)
//! Color scheme 1 background color
#define COLOR_SCHEME_1_BG           GFX_COLOR(0,100,100)
//! Background color for application
#define APP_BACKGROUND_COLOR        GFX_COLOR(0,0,0)
//! Background color for subframe
#define SUBFRAME_BACKGROUND_COLOR   GFX_COLOR(90,90,90)

//! @}

/**
 * \name Widget positions and sizes
 *
 * @{
 */

//! Label X coordinate
#define LABEL_POS_X                 10
//! Label Y coordinate
#define LABEL_POS_Y                 10

//! Spacing on each side of the subframe
#define SUBFRAME_HORIZONTAL_SPACING 45
//! Spacing from label to subframe
#define SUBFRAME_TOP_SPACING        10
//! Spacing from subframe to bottom of screen
#define SUBFRAME_BOTTOM_SPACING     30

//! Leftmost X coordinate in subframe for the widgets
#define WIDGET_POS_X                10

//! Slider X coordinate, inside subframe
#define SLIDER_POS_Y                10
//! Width of slider
#define SLIDER_SIZE_X               80
//! Height of slider
#define SLIDER_SIZE_Y               40

//! Spacing between the slider and progress bars
#define SLIDER_PB_SPACING_X         10
//! Width of progress bar
#define PB_SIZE_X                   SLIDER_SIZE_X
//! Height of progress bar
#define PB_SIZE_Y                   SLIDER_SIZE_Y

//! Spacing above check box to slider and progress bars
#define CHECK_BOX_SPACING_Y         20

//! Spacing above each radio button
#define RADIO_BUTTON_SPACING_Y      5

//! @}

//! Description for label
const static char demo_string[]     = "Demonstrating widgets";
//! Description for check box
const static char checkbox_string[] = "Invert colors";
//! Description for radio buttons
const static char rb1_string[]      = "Color scheme 1";
//! Description for radio buttons
const static char rb2_string[]      = "Color scheme 2";

//! Widget application context
struct widget_context {
	//! Pointer to frame for application
	struct wtk_basic_frame  *frame;
	//! Pointer to slider
	struct wtk_slider       *slider;
	//! Pointer to progress bar
	struct wtk_progress_bar *pb;
	//! Background bitmap used
	struct gfx_bitmap       background;
	//! Bitmap used as background in the subframe
	struct gfx_bitmap       subframe_background;
	//! Copy of the sysfont before it is modified by the application
	struct font             old_sysfont;
	//! Current color scheme in use
	uint8_t                 color_scheme;
	//! Current color scheme has inverted colors
	bool                    color_invert;

};

//! Statically allocated context pointer
static struct widget_context *widget_ctx;

/**
 * \brief Update colors
 *
 * Updates the colors for the progress bar and issues redrawing of the widgets.
 *
 * \param widget Context for widget application
 */
static void app_widget_update_colors(struct widget_context *widget)
{
	gfx_color_t temp;
	gfx_color_t fg;
	gfx_color_t bg;

	switch (widget->color_scheme) {
	case 1:
		// Set colors for color scheme 1
		fg = COLOR_SCHEME_1_FG;
		bg = COLOR_SCHEME_1_BG;
		break;

	default:
		// Set default color scheme
		fg = COLOR_SCHEME_0_FG;
		bg = COLOR_SCHEME_0_BG;
		break;
	}

	if (widget->color_invert) {
		temp = bg;
		bg = fg;
		fg = temp;
	}

	wtk_progress_bar_set_colors(widget->pb, fg, bg);
	win_redraw(wtk_progress_bar_as_child(widget->pb));
}

/**
 * \brief Frame handler for the application
 *
 * Handles all command events from the widgets in the application frame.
 *
 * \sa wtk_basic_frame_command_handler_t
 *
 * \param frame Pointer to the application frame
 * \param command_data Command event ID
 *
 * \return True if exiting, to destroy the window
 */
static bool widget_frame_command_handler(struct wtk_basic_frame *frame,
		win_command_t command_data)
{
	struct widget_context *widget;
	char                  command;

	widget = (struct widget_context *)wtk_basic_frame_get_custom_data(frame);
	command = (char)(uintptr_t)command_data;

	switch (command) {
	case BUTTON_QUIT_ID:
		// Restore sysfont
		memcpy(&sysfont, &widget_ctx->old_sysfont, sizeof(struct font));
		// Free memory
		membag_free(widget);
		// Restart desktop
		app_desktop_restart();
		// Close frame and all widgets
		return true;

	case SLIDER_ID:
		// Update the progress bar when slider value changes.
		wtk_progress_bar_set_value(widget->pb,
				wtk_slider_get_value(widget->slider));
		break;

	case CHECK_BOX_ID:
		// Invert the colors for the progress bar.
		widget->color_invert = !widget->color_invert;
		app_widget_update_colors(widget);
		break;

	case RADIO_BUTTON_1_ID:
		// Set first color scheme for the progress bar.
		widget->color_scheme = 0;
		app_widget_update_colors(widget);
		break;

	case RADIO_BUTTON_2_ID:
		// Set second color scheme for the progress bar.
		widget->color_scheme = 1;
		app_widget_update_colors(widget);
		break;
	}
	return false;
}

/**
 * \brief Setup widget demo
 *
 * Allocates memory for the application context, and creates all widgets that
 * make up its interface. If memory allocation or widget creation fails, the
 * application exits immediately.
 *
 * \param task Workqueue task used to start the application.
 */
void app_widget_launch(struct workqueue_task *task) {

	struct win_window       *win_root;
	struct win_window       *parent;
	struct win_window       *subparent;
	struct win_area         area;
	struct wtk_basic_frame  *subframe;
	struct wtk_label        *lbl;
	struct wtk_button       *btn;
	struct wtk_check_box    *cb;
	struct wtk_radio_group  *rg;
	struct wtk_radio_button *rb;

	widget_ctx = membag_alloc(sizeof(struct widget_context));

	if (!widget_ctx) {
		goto error_ctx;
	}
	widget_ctx->color_scheme = 0;
	widget_ctx->color_invert = 0;

	// Use larger sysfont for this app
	memcpy(&widget_ctx->old_sysfont, &sysfont, sizeof(struct font));
	sysfont.scale = 2;

	win_root = win_get_root();

	// Create main application frame
	widget_ctx->background.type = BITMAP_SOLID;
	widget_ctx->background.data.color = APP_BACKGROUND_COLOR;

	area.pos.x = 0;
	area.pos.y = 0;
	area.size.x = gfx_get_width();
	area.size.y = gfx_get_height();

	widget_ctx->frame = wtk_basic_frame_create(win_root, &area,
			&widget_ctx->background, NULL,
			widget_frame_command_handler, widget_ctx);
	if (!widget_ctx->frame) {
		goto error_frame;
	}

	parent = wtk_basic_frame_as_child(widget_ctx->frame);
	win_show(parent);

	// Create label for application
	area.pos.x = LABEL_POS_X;
	area.pos.y = LABEL_POS_Y;
	// Find optimal size for widget
	wtk_label_size_hint(&area.size, demo_string);

	lbl = wtk_label_create(parent, &area, demo_string, false);
	if (!lbl) {
		goto error_widget;
	}
	win_show(wtk_label_as_child(lbl));

	// Create subframe for all widgets below the label
	widget_ctx->subframe_background.type = BITMAP_SOLID;
	widget_ctx->subframe_background.data.color = SUBFRAME_BACKGROUND_COLOR;

	area.pos.x = SUBFRAME_HORIZONTAL_SPACING;
	area.pos.y += area.size.y + SUBFRAME_TOP_SPACING;
	area.size.x = gfx_get_width() - (2 * SUBFRAME_HORIZONTAL_SPACING);
	area.size.y = gfx_get_height() - area.pos.y - SUBFRAME_BOTTOM_SPACING;

	subframe = wtk_basic_frame_create(parent, &area,
			&widget_ctx->subframe_background, NULL, NULL, NULL);
	if (!subframe) {
		goto error_widget;
	}
	subparent = wtk_basic_frame_as_child(subframe);
	win_show(subparent);

	// Create slider inside subframe
	area.pos.x = WIDGET_POS_X;
	area.pos.y = SLIDER_POS_Y;
	area.size.x = SLIDER_SIZE_X;
	area.size.y = SLIDER_SIZE_Y;

	widget_ctx->slider = wtk_slider_create(subparent, &area, 100,
			50, WTK_SLIDER_HORIZONTAL|WTK_SLIDER_CMD_RELEASE,
			(win_command_t)SLIDER_ID);
	if (!widget_ctx->slider) {
		goto error_widget;
	}
	win_show(wtk_slider_as_child(widget_ctx->slider));

	// Create progress bar to the right of the slider
	area.pos.x += area.size.x + SLIDER_PB_SPACING_X;
	area.size.x = PB_SIZE_X;
	area.size.y = PB_SIZE_Y;

	widget_ctx->pb = wtk_progress_bar_create(subparent, &area, 100,
			50, GFX_COLOR(0, 0, 0), GFX_COLOR(0, 0, 0),
			WTK_PROGRESS_BAR_HORIZONTAL);

	if (!widget_ctx->pb) {
		goto error_widget;
	}
	win_show(wtk_progress_bar_as_child(widget_ctx->pb));
	app_widget_update_colors(widget_ctx);

	// Create check box below slider and progress bar
	area.pos.x = WIDGET_POS_X;
	area.pos.y += area.size.y + CHECK_BOX_SPACING_Y;

	wtk_check_box_size_hint(&area.size, checkbox_string);

	cb = wtk_check_box_create(subparent, &area, checkbox_string,
			false, (win_command_t)CHECK_BOX_ID);
	if (!cb) {
		goto error_widget;
	}
	win_show(wtk_check_box_as_child(cb));

	// Create radio buttons below check box
	rg = wtk_radio_group_create();
	if (!rg) {
		goto error_widget;
	}

	area.pos.x = WIDGET_POS_X;
	area.pos.y += area.size.y + RADIO_BUTTON_SPACING_Y;
	wtk_radio_button_size_hint(&area.size, rb1_string);

	rb = wtk_radio_button_create(subparent, &area, rb1_string,
			true, rg, (win_command_t)RADIO_BUTTON_1_ID);
	if (!rb) {
		goto error_widget;
	}
	win_show(wtk_radio_button_as_child(rb));

	area.pos.y += area.size.y + RADIO_BUTTON_SPACING_Y;

	rb = wtk_radio_button_create(subparent, &area, rb2_string,
			false, rg, (win_command_t)RADIO_BUTTON_2_ID);
	if (!rb) {
		goto error_widget;
	}
	win_show(wtk_radio_button_as_child(rb));

	area.size.x = APP_EXIT_BUTTON_SIZE_X;
	area.size.y = APP_EXIT_BUTTON_SIZE_Y;
	area.pos.x  = APP_EXIT_BUTTON_POS_X;
	area.pos.y  = APP_EXIT_BUTTON_POS_Y;

	// Create Exit button
	btn = wtk_button_create(parent, &area, APP_EXIT_BUTTON_TEXT,
			(win_command_t)BUTTON_QUIT_ID);
	if (!btn) {
		goto error_widget;
	}
	win_show(wtk_button_as_child(btn));

	return;

error_widget:
	win_destroy(wtk_basic_frame_as_child(widget_ctx->frame));
error_frame:
	memcpy(&sysfont, &widget_ctx->old_sysfont, sizeof(struct font));
	membag_free(widget_ctx);
error_ctx:
	app_desktop_restart();
}

//! @}

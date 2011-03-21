/**
 * \file
 *
 * \brief Display Xplained clock application
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
#include <assert.h>
#include <string.h>

#include <gfx/gfx.h>
#include <gfx/win.h>
#include <gfx/wtk.h>
#include <gfx/sysfont.h>

#include "app_clock.h"
#include "app_desktop.h"

/**
 * \weakgroup app_clock_group
 *
 * @{
 */

/**
 * \name Clock color scheme.
 *
 * @{
 */

//! Color on text strings.
#define COLOR_TEXT              GFX_COLOR(255, 255, 255)
//! Background color.
#define COLOR_BACKGROUND        GFX_COLOR(0, 0, 0)
//! Color on frame border.
#define COLOR_BORDER            GFX_COLOR(150, 150, 150)

//! @}

/**
 * \name Clock frame size and layout.
 *
 * @{
 */

//! Padding in pixels from screen border to frame.
#define FRAME_PADDING           40
//! Height of frame.
#define FRAME_HEIGHT            (gfx_get_height() - (2 * FRAME_PADDING))
//! Width of frame.
#define FRAME_WIDTH             (gfx_get_width() - (2 * FRAME_PADDING))
//! X position of top left corner.
#define FRAME_POS_X             FRAME_PADDING
//! Y position of top left corner.
#define FRAME_POS_Y             FRAME_PADDING

//! @}

/**
 * \name Clock text appearance.
 *
 * @{
 */

//! Number of pixels to indent a string.
#define TEXT_INDENT             15
//! Number of pixels to pad after a string newline.
#define TEXT_PADDING_NEWLINE    4

//! @}

/**
 * \brief Event command ID for the application widgets.
 */
enum app_clock_button_ids {
	//! Event command ID for the exit button.
	BUTTON_EXIT_ID = 1,
};

/**
 * \brief Context for the clock application.
 */
struct app_clock {
	//! Pointer to the main frame of the application.
	struct wtk_basic_frame  *frame;
	//! Background bitmap for application.
	struct gfx_bitmap       background;
	//! Font used to store a copy of the system font while running.
	struct font             prev_sysfont;
};

/**
 * \brief Pointer to the clock application context.
 *
 * \note This pointer is statically allocated, but the context itself is only
 * allocated when the application is launched, and then destroyed upon exit to
 * conserve memory.
 */
static struct app_clock *the_clock_app;

/**
 * \brief Frame draw handler handling draw events.
 *
 * This function will draw the contents of the frame to the screen.
 *
 * \sa wtk_basic_frame_draw_handler_t
 */
static void app_clock_frame_draw_handler(struct win_window *win,
		const struct win_clip_region *clip)
{
	gfx_coord_t y = FRAME_POS_Y + TEXT_INDENT;

	gfx_draw_rect(FRAME_POS_X, FRAME_POS_Y, FRAME_WIDTH, FRAME_HEIGHT,
			COLOR_BORDER);

	gfx_draw_string("Clock application",
			FRAME_POS_X + TEXT_INDENT, y, &sysfont, COLOR_TEXT,
			GFX_COLOR_TRANSPARENT);

	/*
	 * The system font will print very dense if not adding some extra
	 * padding between lines, hence padding with TEXT_PADDING_NEWLINE.
	 */
	y += (2 * gfx_font_get_height(&sysfont)) + TEXT_PADDING_NEWLINE;
	gfx_draw_string("This application",
			FRAME_POS_X + TEXT_INDENT, y, &sysfont, COLOR_TEXT,
			GFX_COLOR_TRANSPARENT);

	y += gfx_font_get_height(&sysfont) + TEXT_PADDING_NEWLINE;
	gfx_draw_string("is left as an",
			FRAME_POS_X + TEXT_INDENT, y, &sysfont, COLOR_TEXT,
			GFX_COLOR_TRANSPARENT);

	y += gfx_font_get_height(&sysfont) + TEXT_PADDING_NEWLINE;
	gfx_draw_string("exercise for the",
			FRAME_POS_X + TEXT_INDENT, y, &sysfont, COLOR_TEXT,
			GFX_COLOR_TRANSPARENT);

	y += gfx_font_get_height(&sysfont) + TEXT_PADDING_NEWLINE;
	gfx_draw_string("user to implement.",
			FRAME_POS_X + TEXT_INDENT, y, &sysfont, COLOR_TEXT,
			GFX_COLOR_TRANSPARENT);
}

/**
 * \brief Frame command handler handling the button events.
 *
 * \sa wtk_basic_frame_command_handler_t
 *
 * \param frame Pointer to the basic frame object
 * \param command_data Command data provided with the event, holds button ID
 *
 * \return true when application is exiting, false otherwise
 */
static bool app_clock_frame_command_handler(struct wtk_basic_frame *frame,
		win_command_t command_data)
{
	uint8_t command = (uint8_t)(uintptr_t)command_data;

	switch (command) {
	case BUTTON_EXIT_ID:
		memcpy(&sysfont, &the_clock_app->prev_sysfont,
				sizeof(struct font));
		membag_free(the_clock_app);
		app_desktop_restart();
		return true;

	default:
		break;
	}

	return false;
}

/**
 * \brief Launcher for the clock application.
 *
 * \param task Workqueue task to use for the application's worker functions.
 */
void app_clock_launch(struct workqueue_task *task)
{
	struct win_window       *win_root;
	struct win_window       *parent;
	struct win_area         area;
	struct wtk_button       *button;

	the_clock_app = membag_alloc(sizeof(struct app_clock));
	if (!the_clock_app)
		goto error_membag_alloc;

	/* Store previous system font and scale it to double size. */
	memcpy(&the_clock_app->prev_sysfont, &sysfont, sizeof(struct font));
	sysfont.scale = 2;

	/* Initialize and set the background. */
	the_clock_app->background.type       = BITMAP_SOLID;
	the_clock_app->background.data.color = COLOR_BACKGROUND;

	win_root = win_get_root();

	/*
	 * Create a transparent basic frame with height and width equal to half
	 * the screen height and width. Position the basic frame in the center
	 * of the screen.
	 */
	area.pos.x = FRAME_POS_X;
	area.pos.y = FRAME_POS_Y;
	area.size.x = FRAME_WIDTH;
	area.size.y = FRAME_HEIGHT;

	the_clock_app->frame = wtk_basic_frame_create(win_root, &area,
			&the_clock_app->background,
			app_clock_frame_draw_handler,
			app_clock_frame_command_handler, &the_clock_app);
	if (!the_clock_app->frame)
		goto error_text_frame;

	parent = wtk_basic_frame_as_child(the_clock_app->frame);
	win_show(parent);

	/* Create an exit button in the lower right corner of the frame. */
	area.size.x = APP_EXIT_BUTTON_SIZE_X;
	area.size.y = APP_EXIT_BUTTON_SIZE_Y;
	/* Offset one into the frame to avoid the border. */
	area.pos.x = FRAME_WIDTH - APP_EXIT_BUTTON_SIZE_X - 1;
	area.pos.y = FRAME_HEIGHT - APP_EXIT_BUTTON_SIZE_Y - 1;

	button = wtk_button_create(parent, &area, APP_EXIT_BUTTON_TEXT,
			(win_command_t)BUTTON_EXIT_ID);
	if (!button)
		goto error_widget;

	win_show(wtk_button_as_child(button));

	return;

error_widget:
	win_destroy(parent);
error_text_frame:
	memcpy(&sysfont, &the_clock_app->prev_sysfont, sizeof(struct font));
	membag_free(the_clock_app);
error_membag_alloc:
	app_desktop_restart();
}

//! @}

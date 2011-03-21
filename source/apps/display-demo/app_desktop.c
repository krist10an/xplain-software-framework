/**
 * \file
 *
 * \brief Display demo desktop application
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
#include <membag.h>
#include <string.h>
#include <mainloop.h>

#include <fs/tsfs.h>

#include <gfx/gfx.h>
#include <gfx/win.h>
#include <gfx/wtk.h>
#include <gfx/sysfont.h>

#include "app_desktop.h"
#include "app_calibrate.h"
#include "app_calc.h"
#include "app_clock.h"
#ifdef CONFIG_HUGEMEM
# include "app_fonts.h"
# include "app_slideshow.h"
# include "app_tank.h"
# include "app_files.h"
# include "app_memgame.h"
#endif
#include "app_widget.h"

#include "file_loader.h"

/**
 * \weakgroup app_desktop_group
 *
 * @{
 */

/**
 * \name Application menu configuration
 *
 * @{
 */

//! Number of apps per column
#define DESKTOP_NUM_APPS_PER_COLUMN     4
//! Number of apps per row
#define DESKTOP_NUM_APPS_PER_ROW        3
//! Total number of apps
#define DESKTOP_APP_COUNT               (DESKTOP_NUM_APPS_PER_COLUMN * \
		DESKTOP_NUM_APPS_PER_ROW)

//! @}

/**
 * \name Color scheme
 *
 * @{
 */

//! Desktop background color
#define DESKTOP_BACKGROUND_COLOR        GFX_COLOR(0, 0, 0)
//! Text color for icons
#define DESKTOP_ICON_TEXT_COLOR         GFX_COLOR(255, 255, 255)
//! Desktop pop-up border color
#define DESKTOP_POPUP_BORDER_COLOR      GFX_COLOR(255, 42, 42)

//! @}

/**
 * \name Icon configuration
 *
 * @{
 */

//! Size of string for icon name
#define DESKTOP_ICON_NAME_SIZE          9
//! Size of string for text below each icon
#define DESKTOP_ICON_TEXT_SIZE          11
//! Size of application launch icon and text
#define DESKTOP_ICON_SPACING_X          80
//! Size of application launch icon and text
#define DESKTOP_ICON_SPACING_Y          80
//! X position of icon
#define DESKTOP_ICON_POS_X              11
//! Y position of icon
#define DESKTOP_ICON_POS_Y              7
//! Width of icon
#define DESKTOP_ICON_SIZE_X             57
//! Height of icon
#define DESKTOP_ICON_SIZE_Y             57
//! Position for text describing each icon
#define DESKTOP_ICON_TEXT_X             (DESKTOP_ICON_SPACING_X / 2)
//! Position for text describing each icon
#define DESKTOP_ICON_TEXT_Y             ((DESKTOP_ICON_POS_Y + \
			DESKTOP_ICON_SIZE_Y + DESKTOP_ICON_SPACING_Y) / 2)

//! @}

/**
 * \name Pop-up size and position configuration
 *
 * @{
 */

//! Position from display edge to start of pop-up box.
#define DESKTOP_POPUP_PADDING           20
//! Height of pop-up box.
#define DESKTOP_POPUP_HEIGHT            (gfx_font_get_height(&sysfont) * 5)
//! Width of pop-up box.
#define DESKTOP_POPUP_WIDTH             (gfx_get_width() - 2 * \
		DESKTOP_POPUP_PADDING)
//! X position of pop-up box.
#define DESKTOP_POPUP_POS_X             DESKTOP_POPUP_PADDING
//! Y position of pop-up box.
#define DESKTOP_POPUP_POS_Y             ((gfx_get_height() / 2) - \
		(DESKTOP_POPUP_HEIGHT / 2))

//! @}

//! Convenience macro for registering applications
#define DESKTOP_REGISTER_APP(icon,text,start_task)  \
		{ .icon_name = icon, .icon_text=text,.task=start_task }

//! Desktop context
struct app_desktop {
	//! Task for loading the desktop
	struct workqueue_task desktop_worker_task;
	//! Task used to launch each application
	struct workqueue_task application_launcher_task;
	//! Status of desktop
	bool                  desktop_enabled;
};

//! Statically allocated desktop context
static struct app_desktop the_app_desktop;

#ifdef CONFIG_FS_TSFS
//! TSFS control struct, needed for sanity check of file system.
extern struct tsfs              myfs;
#endif /* CONFIG_FS_TSFS */

//! Application metadata
struct app_config {
	//! Filename of icon
	char             icon_name[DESKTOP_ICON_NAME_SIZE];
	//! Text to draw under the icon
	char             icon_text[DESKTOP_ICON_TEXT_SIZE];
	//! Worker function to launch application with
	workqueue_func_t task;
};

/**
 * \brief Desktop application menu items
 *
 * This array contains the data for all the applications to be shown on the
 * desktop. They will be listed in the order that they are registered here.
 *
 * \todo Store application data array in PROGMEM to reduce memory usage.
 */
struct app_config apps[DESKTOP_APP_COUNT] = {
	DESKTOP_REGISTER_APP("i_calc", "Calc", app_calc_launch),
	DESKTOP_REGISTER_APP("i_sett", "Widget", app_widget_launch),
	DESKTOP_REGISTER_APP("i_clock", "Clock", app_clock_launch),
#ifdef CONFIG_FS_TSFS
	DESKTOP_REGISTER_APP("i_fonts", "Fonts", app_fonts_launch),

	DESKTOP_REGISTER_APP("i_pics", "Pictures", app_slideshow_pics_launch),
	DESKTOP_REGISTER_APP("i_tank", "Water tank", app_tank_launch),
	DESKTOP_REGISTER_APP("i_files", "Files", app_files_launch),
	DESKTOP_REGISTER_APP("i_games", "Memory", app_memgame_launch),

	DESKTOP_REGISTER_APP("i_avr", "8-bit", app_slideshow_avr_launch),
	DESKTOP_REGISTER_APP("i_uc3", "32-bit", app_slideshow_uc3_launch),
	DESKTOP_REGISTER_APP("i_xmega", "XMEGA", app_slideshow_xmega_launch),
	DESKTOP_REGISTER_APP("i_avr", "This kit", app_slideshow_dx_launch),
#else
	DESKTOP_REGISTER_APP("", "", NULL),
	DESKTOP_REGISTER_APP("", "", NULL),
	DESKTOP_REGISTER_APP("", "", NULL),
	DESKTOP_REGISTER_APP("", "", NULL),

	DESKTOP_REGISTER_APP("", "", NULL),
	DESKTOP_REGISTER_APP("", "", NULL),
	DESKTOP_REGISTER_APP("", "", NULL),
	DESKTOP_REGISTER_APP("", "", NULL),
#endif
};

/**
 * \brief Enable desktop
  *
 * This function enables the event handling in the desktop window.
 */
static void app_desktop_enable(void)
{
	the_app_desktop.desktop_enabled = true;
}

/**
 * \brief Disable desktop
 *
 * This function disables the event handling in the desktop window to ensure
 * that the desktop does not slow down another running application.
 */
static void app_desktop_disable(void)
{
	the_app_desktop.desktop_enabled = false;
}

/**
 * \brief Check if desktop is enabled
 *
 * \return True if desktop is enabled, otherwise false.
 */
static bool app_desktop_is_enabled(void)
{
	return the_app_desktop.desktop_enabled;
}

/**
 * \brief Restart desktop
 *
 * Restarts the desktop when an application has finished execution by
 * scheduling a task to the \ref main_workqueue to start loading the desktop
 * with \ref load_desktop.
 *
 * Launched applications must call this before exiting for the desktop to
 * reappear.
 */
void app_desktop_restart(void)
{
	workqueue_add_task(&main_workqueue, &the_app_desktop.desktop_worker_task);
}


/**
 * \brief Helper function to get icon position
 *
 * Returns the X and Y coordinates of a specific icon.
 *
 * \param iconnum Icon number of which to find position
 *
 * \retval x Returned X coordinate of icon
 * \retval y Returned Y coordinate of icon
 */
static void get_icon_pos(uint8_t iconnum, gfx_coord_t *x, gfx_coord_t *y)
{
	*x = iconnum % DESKTOP_NUM_APPS_PER_COLUMN * DESKTOP_ICON_SPACING_X;
	*y = iconnum / DESKTOP_NUM_APPS_PER_COLUMN * DESKTOP_ICON_SPACING_Y;
}

/**
 * \brief Helper function for drawing icon text
 *
 * Draws text that is centered on the specified coordinates regardless of the
 * length of the string.
 *
 * \param text The string to draw
 * \param center_x Center X coordinate of string
 * \param center_y Center Y coordinate of string
 */
static void draw_icon_text(const char text[], gfx_coord_t center_x,
		gfx_coord_t center_y)
{
	gfx_coord_t x;
	gfx_coord_t y;

	gfx_get_string_bounding_box(text, &sysfont, &x, &y);
	gfx_draw_string((char *)text, center_x - x / 2, center_y - y / 2,
			&sysfont, DESKTOP_ICON_TEXT_COLOR,
			GFX_COLOR_TRANSPARENT);
}

#ifdef CONFIG_FS_TSFS
/**
 * \brief Pop-up a warning box about empty file system
 *
 * This function will draw a pop-up box to the middle of the screen, informing
 * the user about missing contents in the file system.
 */
static void app_desktop_popup_empty_tsfs(void)
{
	gfx_draw_filled_rect(DESKTOP_POPUP_POS_X, DESKTOP_POPUP_POS_Y,
			DESKTOP_POPUP_WIDTH, DESKTOP_POPUP_HEIGHT,
			DESKTOP_BACKGROUND_COLOR);
	gfx_draw_rect(DESKTOP_POPUP_POS_X, DESKTOP_POPUP_POS_Y,
			DESKTOP_POPUP_WIDTH, DESKTOP_POPUP_HEIGHT,
			DESKTOP_POPUP_BORDER_COLOR);
	gfx_draw_string("Warning: file system empty, please program\n\n"
			"the DataFlash to contain a proper TSFS image.",
			DESKTOP_POPUP_POS_X + gfx_font_get_height(&sysfont),
			DESKTOP_POPUP_POS_Y + gfx_font_get_width(&sysfont),
			&sysfont, DESKTOP_ICON_TEXT_COLOR,
			GFX_COLOR_TRANSPARENT);
}
#endif /* CONFIG_FS_TSFS */

/**
 * \brief Event handler for desktop window
 *
 * Handles all events received by the desktop application window. Only pointer
 * events require handling in this application: These are used to select and
 * launch applications if the desktop is enabled.
 *
 * \param win Pointer to window receiving the event
 * \param type Indicates type of event
 * \param data Event type-specific data
 *
 * \return True, to accept all events
 */
static bool app_desktop_handler(struct win_window *win,
		enum win_event_type type, void const *data)
{
	static gfx_coord_t last_x;
	static gfx_coord_t last_y;
	static gfx_coord_t x = 0;
	static gfx_coord_t y = 0;

	// Do not do any event handling if desktop is not enabled
	if (!app_desktop_is_enabled()) {
		return true;
	}

	if (type == WIN_EVENT_POINTER) {
		// For POINTER events, the data parameter points to the pointer
		// event information.
		struct win_pointer_event const *event =
				(struct win_pointer_event const *)data;
		uint8_t i;

		switch (event->type) {
		case WIN_POINTER_PRESS:
			// Draw white outline around icon under the pointer.
			x = event->pos.x / DESKTOP_ICON_SPACING_X;
			y = event->pos.y / DESKTOP_ICON_SPACING_Y;

			last_x = x * DESKTOP_ICON_SPACING_X;
			last_y = y * DESKTOP_ICON_SPACING_Y;
			gfx_draw_rect(last_x, last_y,
					DESKTOP_ICON_SPACING_X,
					DESKTOP_ICON_SPACING_Y,
					DESKTOP_ICON_TEXT_COLOR);
			break;

		case WIN_POINTER_RELEASE:
			/* Remove the white outline, then launch the application
			 * if an application launch worker function is set.
			 */
			gfx_draw_rect(last_x, last_y,
					DESKTOP_ICON_SPACING_X,
					DESKTOP_ICON_SPACING_Y,
					DESKTOP_BACKGROUND_COLOR);

			i = x + y * DESKTOP_NUM_APPS_PER_COLUMN;

			if ((i < DESKTOP_APP_COUNT) && (apps[i].task)) {
				app_desktop_disable();

				workqueue_task_set_work_func
						(&the_app_desktop.application_launcher_task,
						apps[i].task);

				workqueue_add_task(&main_workqueue,
						&the_app_desktop.application_launcher_task);
			} else {
				dbg_error("Not implemented\n");
			}

			break;

		default:
			break;
		}
	}
	// Accept all events
	return true;
}


#ifdef CONFIG_FS_TSFS
/**
 * \brief Loading destop icons
 *
 * This task worker function draws the desktop with application icons.
 * It is scheduled at startup with the \ref app_desktop_setup function, and by
 * exiting applications with \ref app_desktop_restart.
 *
 * The function loops through all the registered applications in \ref apps and
 * draws their respective icon text before scheduling loading of the icon
 * directly from a \ref tsfs_group "TSFS" to the display.
 *
 * The file loading is done with the \ref appsutil_fileloader_group.
 *
 * \param task Workqueue task for this worker function.
 */
static void load_desktop(struct workqueue_task *task)
{
	static uint8_t i = 0;
	gfx_coord_t x;
	gfx_coord_t y;
	enum status_code result;

	if (file_loader_busy()) {
		// Image loader busy, reschedule to check later
		workqueue_add_task(&main_workqueue, task);
		return;
	}

	// Check if we are finished
	if (i > DESKTOP_APP_COUNT) {
		/*
		 * Check if file system is empty, if empty encurage the user to
		 * fill the DataFlash with a TSFS image.
		 */
		if (!tsfs_nr_files(&myfs)) {
			app_desktop_popup_empty_tsfs();
		}

		// Reset to be able to restart drawing background
		i = 0;
		// Enable event handling
		app_desktop_enable();
		return;
	}

	// Setup for first icon
	if (i == 0) {
		// Disable event handling
		app_desktop_disable();

		// Redraw background
		gfx_set_clipping(0, 0, gfx_get_width(), gfx_get_height());
		gfx_draw_filled_rect(0, 0, gfx_get_width(), gfx_get_height(),
				DESKTOP_BACKGROUND_COLOR);
		win_redraw(win_get_root());
	}

	// Get icon position
	get_icon_pos(i, &x, &y);
	draw_icon_text(apps[i].icon_text, x + DESKTOP_ICON_TEXT_X,
			y + DESKTOP_ICON_TEXT_Y);

	x += DESKTOP_ICON_POS_X;
	y += DESKTOP_ICON_POS_Y;

	// Load icon if it is specified
	if (strlen(apps[i].icon_name) > 0) {
		// Load image and schedule this task when done
		result = load_file_to_screen(apps[i].icon_name, x, y,
				DESKTOP_ICON_SIZE_X, DESKTOP_ICON_SIZE_Y, task);

		if (result != STATUS_OK) {
			// Unable to load icon, draw a cross instead
			gfx_draw_rect(x, y, DESKTOP_ICON_SIZE_X,
					DESKTOP_ICON_SIZE_Y,
					DESKTOP_ICON_TEXT_COLOR);
			gfx_draw_line(x, y, x + DESKTOP_ICON_SIZE_X - 1,
					y + DESKTOP_ICON_SIZE_Y - 1,
					DESKTOP_ICON_TEXT_COLOR);
			gfx_draw_line(x, y + DESKTOP_ICON_SIZE_Y - 1,
					x + DESKTOP_ICON_SIZE_X - 1, y,
					DESKTOP_ICON_TEXT_COLOR);
			// Reschedule for drawing next icon
			workqueue_add_task(&main_workqueue, task);
		}
	} else {
		// Reschedule for drawing next icon
		workqueue_add_task(&main_workqueue, task);
	}
	// Load next icon
	i++;
}
#else
/**
 * \brief Load desktop
 *
 * This task worker function draws the desktop.
 * It is scheduled at startup with the \ref app_desktop_setup function, and by
 * exiting applications with \ref app_desktop_restart.
 *
 * The function loops through all the registered applications in \ref apps and
 * draws their respective icon text before drawing a shaded rectangle in place
 * of its icon.
 */
static void load_desktop(struct workqueue_task *task)
{
	uint8_t     i;
	uint8_t     shade;
	gfx_coord_t x;
	gfx_coord_t y;

	// Set up desktop
	app_desktop_disable();

	gfx_set_clipping(0, 0, gfx_get_width(), gfx_get_height());
	gfx_draw_filled_rect(0, 0, gfx_get_width(), gfx_get_height(),
			DESKTOP_BACKGROUND_COLOR);
	win_redraw(win_get_root());

	// Set start color for blue shade
	shade = 128;
	for (i = 0; i < ARRAY_LEN(apps); i++) {
		get_icon_pos(i, &x, &y);
		draw_icon_text(apps[i].icon_text, x + DESKTOP_ICON_TEXT_X,
				y + DESKTOP_ICON_TEXT_Y);

		x += DESKTOP_ICON_POS_X;
		y += DESKTOP_ICON_POS_Y;

		if (strlen(apps[i].icon_name) > 0) {
			// Draw rectangles with the color for current application
			gfx_draw_filled_rect(x, y, DESKTOP_ICON_SIZE_X,
					DESKTOP_ICON_SIZE_Y,
					gfx_color(76, 76, shade));
			// Increment blue comonent to give the shading effect
			shade += 6;
		}
	}

	app_desktop_enable();
}
#endif /* CONFIG_FS_TSFS */

/**
 * \brief Setup desktop
 *
 * Allocates the desktop application window and initializes the workqueue tasks.
 *
 * This function should be called from the main function to set up the desktop
 * for first use.
 *
 * If CONFIG_FS_TSFS is defined, the \ref appsutil_fileloader_group is
 * initialized to prepare for loading of application icons.
 *
 * If CONFIG_WIN_USE_TOUCH is defined, this function will enqueue the
 * \ref app_calibrate_group "touch calibration" on the \ref main_workqueue
 * "main workqueue". When calibration is finished, or if touch is not enabled,
 * it will schedule a task with \ref load_desktop for loading the desktop.
 */
void app_desktop_setup(void)
{
	struct win_window     *win_root;
	struct win_attributes attr;

	win_root = win_get_root();

	attr = *win_get_attributes(win_root);
	attr.background = NULL;
	attr.event_handler = app_desktop_handler;
	win_set_attributes(win_root, &attr,
			WIN_ATTR_BACKGROUND | WIN_ATTR_EVENTHANDLER);

	win_show(win_root);

#ifdef CONFIG_FS_TSFS
	file_loader_init();
#endif
	workqueue_task_init(&the_app_desktop.desktop_worker_task, load_desktop);
	workqueue_task_init(&the_app_desktop.application_launcher_task, NULL);

#if CONFIG_GFX_WIN_USE_TOUCH
	app_touch_calibrate_setup(&the_app_desktop.desktop_worker_task);
#else
	workqueue_add_task(&main_workqueue,
			&the_app_desktop.desktop_worker_task);
#endif
}

//! @}

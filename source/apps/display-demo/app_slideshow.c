/**
 * \file
 *
 * \brief Display demo slideshow
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
#include <board.h>
#include <interrupt.h>
#include <led.h>
#include <mainloop.h>
#include <string.h>
#include <timer.h>
#include <fs/tsfs.h>
#include <gfx/gfx.h>
#include <gfx/win.h>

#include "app_desktop.h"
#include "app_slideshow.h"
#include "file_loader.h"

/**
 * \weakgroup apps_slide_group
 *
 * @{
 */

//! Filesystem control struct, needed when searching for files.
extern struct tsfs myfs;

/**
 * \name Color scheme
 *
 * @{
 */

//! Color to initally fill display with.
#define COLOR_BACKGROUND     GFX_COLOR(0, 0, 0)

//! @}

/**
 * \name Slideshow timing configuration
 *
 * @{
 */

//! Number of seconds to wait before loading the next slide.
#define SECONDS_PER_SLIDE    4
/**
 * \brief Number of seconds to wait before loading the next slide, when the user
 * previously selected the previous slide.
 */
#define SECONDS_PER_PAUSE    16
/**
 * \brief Minimum timer clock rate to use.
 *
 * \note Assuming a 16-bit timer, a clock rate below 2^16 will allow for
 * detection of whole seconds with the \ref timer_group "Timer driver". The
 * clock rate is divided by a factor of four to ensure that a low enough clock
 * rate is selected by the driver during initialization.
 */
#define TIMER_CLOCK_RATE     (0xffff / 4)

//! @}

/**
 * \name Slideshow image size
 *
 * @{
 */

//! Width of slideshow images.
#define SLIDE_SIZE_X         320
//! Height of slideshow images.
#define SLIDE_SIZE_Y         240

//! @}

/**
 * \name Slideshow filename prefix
 * @{
 */

//! Filename prefix for picture slideshow.
#define SLIDE_PREFIX_PICS    "s_"
//! Filename prefix for AVR presentation slideshow.
#define SLIDE_PREFIX_AVR     "avr"
//! Filename prefix for UC3 presentation slideshow.
#define SLIDE_PREFIX_UC3     "uc3"
//! Filename prefix for AVR XMEGA presentation slideshow.
#define SLIDE_PREFIX_XMEGA   "xmega"
//! Filename prefix for Display Xplained presentation slideshow.
#define SLIDE_PREFIX_DX      "dispxpl"

//! @}

//! Context for the slideshow application.
struct slide_context {
	//! Pointer to the workqueue task of the application.
	struct workqueue_task *task;
	//! Pointer to left application window.
	struct win_window     *left;
	//! Pointer to middle application window.
	struct win_window     *middle;
	//! Pointer to right application window.
	struct win_window     *right;
	//! Timer struct needed for the timer driver.
	struct timer          timer;
	//! Timer delay needed for detection of 1 elapsed second.
	uint16_t              timer_delay;
	//! Seconds to go before next slide is loaded.
	uint8_t               secs_to_go;
	//! Flag indicating that a slide is currently being loaded.
	bool                  busy;
	//! Flag indicating that application shall exit.
	bool                  exit_now;
	//! Pointer to string with file prefix for current slideshow.
	char                  *file_prefix;
	//! Index of current slideshow file in TSFS.
	uint8_t               file_index;
};

/**
 * \brief Pointer to slideshow application context.
 *
 * \note This pointer is statically allocated, but the context itself is only
 * allocated when the application is launched, and then destroyed upon exit to
 * conserve memory.
 */
static struct slide_context *slide_context;

/**
 * \brief Find index of next slideshow file to load.
 *
 * This function searches through the TSFS file indexes until a file with the
 * currently set filename prefix is found. The order in which the function
 * searches through the slideshow files is selected with the parameter
 * \a increment.
 *
 * \param increment Indicate whether to increment (true) or decrement (false)
 * the file index.
 */
static void slide_get_next_file(bool increment)
{
	char filename[TSFS_FILENAME_LEN + 1];
	uint8_t file_index;
	uint8_t prefix_len;
	uint8_t nr_of_files;

	file_index = slide_context->file_index;
	prefix_len = strlen(slide_context->file_prefix);
	nr_of_files = tsfs_nr_files(&myfs);

	// Search for next file with matching filename prefix.
	do {
		if (increment) {
			file_index++;
		} else {
			file_index--;
		}

		// Handle out-of-bounds file indexes.
		if (file_index > nr_of_files) {
			file_index -= nr_of_files;
		}
		if (file_index < 1) {
			file_index += nr_of_files;
		}

		// Break out of the loop if no new file could be found.
		if (file_index == slide_context->file_index) {
			break;
		}

		/* Get the filename associated with the file index for
		 * comparison with the current filename prefix.
		 */
		tsfs_get_filename(&myfs, file_index, (uint8_t *)filename);
	} while (strncmp(slide_context->file_prefix, filename, prefix_len) !=
			0);

	slide_context->file_index = file_index;
}

/**
 * \brief Start loading current file to screen.
 *
 * Starts the loading to screen of the slideshow file indicated by the current
 * file index. If the file cannot be loaded, the timer for autoloading of the
 * next slide is started with a one second timeout.
 */
static void slide_show_file(void)
{
	enum status_code result;
	uint8_t filename[TSFS_FILENAME_LEN + 1];

	tsfs_get_filename(&myfs, slide_context->file_index, filename);

	result = load_file_to_screen((char *)filename, 0, 0, SLIDE_SIZE_X,
			SLIDE_SIZE_Y, slide_context->task);

	if (result == STATUS_OK) {
		slide_context->busy = true;
	} else {
		slide_context->secs_to_go = 1;
		timer_start(CONFIG_TIMER_ID, &slide_context->timer);
		timer_set_alarm(CONFIG_TIMER_ID, &slide_context->timer,
				slide_context->timer_delay);
	}
}

/**
 * \brief Timer alarm callback for automatic loading.
 *
 * This callback function is used with the \ref timer_group "Timer driver" for
 * autoloading of the slides. It keeps track of the number of elapsed seconds
 * since the currently shown slide was loaded, and will start loading of the
 * next slide once a timeout is detected.
 *
 * \note The timeout is set either by the application window event handler, or
 * by this function after a timeout.
 *
 * \param timer Pointer to timer struct associated with the callback.
 */
static void slide_timer_callback(struct timer *timer)
{
	uint8_t secs_to_go;

	secs_to_go = slide_context->secs_to_go;

	// Start loading next slide on timeout, otherwise reset the alarm.
	if (--secs_to_go == 0) {
		secs_to_go = SECONDS_PER_SLIDE;
		slide_get_next_file(true);
		slide_show_file();
	} else {
		timer_set_alarm(CONFIG_TIMER_ID, timer,
				slide_context->timer_delay);
	}
	slide_context->secs_to_go = secs_to_go;
}

/**
 * \brief Event handler for the application windows.
 *
 * This function handles the relevant events for the three application windows.
 * In this application, only pointer releases require handling.
 *
 * A pointer release in the left window triggers loading of the previous slide,
 * the middle window triggers application exit, while the right window triggers
 * loading of the next slide.
 *
 * Depending on whether the previous or next slide was selected, different
 * timeouts for the slide autoloading are set, i.e., to grant the user more time
 * to read the previous slide.
 *
 * \param win Pointer to application window.
 * \param type The type of the event.
 * \param data Pointer to data for the event.
 */
static bool slide_window_handler(struct win_window *win,
		enum win_event_type type, void const *data)
{
	irqflags_t flags;

	if (type == WIN_EVENT_POINTER) {
		// The data parameter points to the POINTER event information.
		struct win_pointer_event const *event =
				(struct win_pointer_event const *)data;

		// Wait for the pointer to be released.
		if (event->type == WIN_POINTER_RELEASE) {
			/* The timer must be stopped before checking if
			 * the application is currently loading a slide.
			 */
			flags = cpu_irq_save();
			timer_stop(CONFIG_TIMER_ID, &slide_context->timer);
			cpu_irq_restore(flags);

			if (slide_context->busy) {
				// Do nothing if a slide is currently loading.
				return true;

			} else if (win == slide_context->left) {
				/* Start loading of previous slide, and set
				 * the longer pause timeout for the autoloading.
				 */
				slide_context->secs_to_go = SECONDS_PER_PAUSE;
				slide_get_next_file(false);
				slide_show_file();

			} else if (win == slide_context->middle) {
				// Exit the application immediately.
				win_destroy(slide_context->left);
				win_destroy(slide_context->middle);
				win_destroy(slide_context->right);
				membag_free(slide_context);
				app_desktop_restart();

			} else if (win == slide_context->right) {
				/* Start loading of next slide, and set the
				 * regular timeout for the autoloading.
				 */
				slide_context->secs_to_go = SECONDS_PER_SLIDE;
				slide_get_next_file(true);
				slide_show_file();
			}
		}
	}

	// Always accept all events
	return true;
}

/**
 * \brief Application task worker.
 *
 * This task worker is called by the \ref appsutil_fileloader_group "Fileloader
 * utility" when loading of a slide has finished, and will start the timer for
 * slide autoloading.
 *
 * \param task Workqueue task for this worker function.
 */
static void slide_worker(struct workqueue_task *task)
{
	slide_context->busy = false;

	timer_start(CONFIG_TIMER_ID, &slide_context->timer);
	timer_set_alarm(CONFIG_TIMER_ID, &slide_context->timer,
			slide_context->timer_delay);
}

/**
 * \brief Common slideshow launcher.
 *
 * This function handles all the memory allocation, window creation and
 * initialization of the slideshow application.
 *
 * Three invisible windows that span the entire display are created, two of
 * which cover the left and right quarter width of the screen, respectively.
 * Loading of the first file in the slideshow indicated by \a file_prefix
 * is then started.
 *
 * If memory for the application context cannot be allocated or any window
 * cannot be created, the application will exit immediately.
 *
 * \param task Workqueue task to use for this application.
 * \param file_prefix Pointer to string with the filename prefix for the desired
 * slideshow.
 */
static void slide_launch_helper(struct workqueue_task *task,
		char *file_prefix)
{
	struct win_attributes attr;
	struct win_window     *root_win = win_get_root();
	struct timer          *timer;
	gfx_coord_t           gfx_width = gfx_get_width();
	gfx_coord_t           gfx_height = gfx_get_height();
	timer_res_t           timer_res;
	uint32_t              timer_clk;

	assert(task);

	// Allocate memory for the application context.
	slide_context = membag_alloc(sizeof(struct slide_context));
	if (!slide_context) {
		goto exit_no_context;
	}

	// Clear the display.
#ifdef CONFIG_GFX_USE_CLIPPING
	gfx_set_clipping(0, 0, gfx_width, gfx_height);
#endif /* CONFIG_GFX_USE_CLIPPING */
	gfx_draw_filled_rect(0, 0, gfx_width, gfx_height, COLOR_BACKGROUND);

	// Initialize the timer with the configured resolution.
	timer = &slide_context->timer;
	timer_init(CONFIG_TIMER_ID, timer, slide_timer_callback);
	timer_res = timer_set_resolution(CONFIG_TIMER_ID, timer,
			TIMER_CLOCK_RATE);
	timer_write_resolution(CONFIG_TIMER_ID, timer, timer_res);
	timer_clk = timer_get_resolution(CONFIG_TIMER_ID, timer, timer_res);

	// Timer clock should be slow enough to allow for detection of 1 second.
	assert(timer_clk <= 0xffff);

	// Initialize the application context and task.
	slide_context->timer_delay = timer_clk;
	slide_context->secs_to_go = SECONDS_PER_SLIDE;
	slide_context->file_prefix = file_prefix;
	slide_context->file_index = 0;
	slide_context->task = task;
	workqueue_task_set_work_func(task, slide_worker);

	/* Create windows for the left, middle and right sections of display.
	 * The middle window is half the width of the display, while the two
	 * others are a quarter of the width.
	 * These will use a common window event handler.
	 */
	attr.area.pos.x = 0;
	attr.area.pos.y = 0;
	attr.area.size.x = gfx_width / 4;
	attr.area.size.y = gfx_height;
	attr.background = NULL;
	attr.event_handler = slide_window_handler;
	slide_context->left = win_create(root_win, &attr);
	if (!slide_context->left) {
		goto exit_no_left_window;
	}
	attr.area.pos.x += attr.area.size.x;
	attr.area.size.x = gfx_width / 2;
	slide_context->middle = win_create(root_win, &attr);
	if (!slide_context->middle) {
		goto exit_no_middle_window;
	}

	attr.area.pos.x += attr.area.size.x;
	attr.area.size.x = gfx_width - attr.area.pos.x;
	slide_context->right = win_create(root_win, &attr);
	if (!slide_context->right) {
		goto exit_no_right_window;
	}

	// Show the windows to enable user input.
	win_show(slide_context->left);
	win_show(slide_context->middle);
	win_show(slide_context->right);

	// Find first slideshow file and start loading.
	slide_get_next_file(true);
	slide_show_file();

	return;

	// Handle allocation errors.
exit_no_right_window:
	win_destroy(slide_context->middle);
exit_no_middle_window:
	win_destroy(slide_context->left);
exit_no_left_window:
	membag_free(slide_context);
exit_no_context:
	app_desktop_restart();
	return;
}

/**
 * \brief Launch picture slideshow.
 *
 * This launches the application with the filename prefix for the picture
 * slideshow.
 *
 * \param task Workqueue task to use for this application.
 */
void app_slideshow_pics_launch(struct workqueue_task *task)
{
	slide_launch_helper(task, SLIDE_PREFIX_PICS);
}

/**
 * \brief Launch 8-bit AVR presentation slideshow.
 *
 * This launches the application with the filename prefix for the AVR
 * presentation slideshow.
 *
 * \param task Workqueue task to use for this application.
 */
void app_slideshow_avr_launch(struct workqueue_task *task)
{
	slide_launch_helper(task, SLIDE_PREFIX_AVR);
}

/**
 * \brief Launch UC3 presentation slideshow.
 *
 * This launches the application with the filename prefix for the AVR UC3
 * presentation slideshow.
 *
 * \param task Workqueue task to use for this application.
 */
void app_slideshow_uc3_launch(struct workqueue_task *task)
{
	slide_launch_helper(task, SLIDE_PREFIX_UC3);
}

/**
 * \brief Launch AVR XMEGA presentation slideshow.
 *
 * This launches the application with the filename prefix for the AVR XMEGA
 * presentation slideshow.
 *
 * \param task Workqueue task to use for this application.
 */
void app_slideshow_xmega_launch(struct workqueue_task *task)
{
	slide_launch_helper(task, SLIDE_PREFIX_XMEGA);
}

/**
 * \brief Launch Display Xplain presentation slideshow.
 *
 * This launches the application with the filename prefix for the Display
 * Xplained presentation slideshow.
 *
 * \param task Workqueue task to use for this application.
 */
void app_slideshow_dx_launch(struct workqueue_task *task)
{
	slide_launch_helper(task, SLIDE_PREFIX_DX);
}

//! @}

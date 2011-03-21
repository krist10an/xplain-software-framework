/**
 * \file
 *
 * \brief Display demo process control application
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
#include <gfx/gfx.h>
#include <gfx/win.h>
#include <gfx/wtk.h>
#include <gfx/sysfont.h>
#include <mainloop.h>
#include <membag.h>
#include <physmem.h>
#include <status_codes.h>
#include <string.h>
#include <timer.h>
#include <util.h>

#include "app_tank.h"
#include "app_desktop.h"
#include "file_loader.h"

/**
 * \weakgroup apps_tank_group
 *
 * @{
 */

/**
 * \name Color scheme
 * @{
 */

//! Color to initially fill screen with.
#define COLOR_WIN_BACKGROUND     GFX_COLOR(0, 0, 0)
//! Fill color for tank level indicator.
#define COLOR_LEVEL_FILL          GFX_COLOR(0, 0, 255)
//! Background color for tank level indicator.
#define COLOR_LEVEL_BACKGROUND    GFX_COLOR(32, 32, 32)
//! Normal fill color for demand indicator.
#define COLOR_DEMAND_NORMAL      GFX_COLOR(64, 192, 64)
//! Critical fill color for demand indicator.
#define COLOR_DEMAND_CRITICAL    GFX_COLOR(192, 64, 64)
//! Background color for demand indicator.
#define COLOR_DEMAND_BACKGROUND  GFX_COLOR(32, 32, 32)

//! @}

/**
 * \name Maximum and initial values of process parameters
 *
 * @{
 */

//! Maximum level of tank.
#define VALUE_LEVEL_MAXIMUM             127
//! Initial level of tank.
#define VALUE_LEVEL_INITIAL             0
//! Maximum value of supply.
#define VALUE_SUPPLY_MAXIMUM           VALUE_LEVEL_MAXIMUM
//! Initial value of supply.
#define VALUE_SUPPLY_INITIAL           (VALUE_SUPPLY_MAXIMUM / 2)
//! Maximum value of demand.
#define VALUE_DEMAND_MAXIMUM           VALUE_LEVEL_MAXIMUM
//! Initial value of demand.
#define VALUE_DEMAND_INITIAL           0

//! @}

/**
 * \name Widget sizes and positions
 *
 * @{
 */

//! Height of progress bar for tank level.
#define WIDGET_LEVEL_SIZE_X              52
//! Width of progress bar for tank level.
#define WIDGET_LEVEL_SIZE_Y              121
//! X coordinate of progress bar for tank level.
#define WIDGET_LEVEL_POSITION_X          134
//! Y coordinate of progress bar for tank level.
#define WIDGET_LEVEL_POSITION_Y          59

//! Height of slider for supply.
#define WIDGET_SUPPLY_SIZE_X            37
//! Width of slider for supply.
#define WIDGET_SUPPLY_SIZE_Y            105
//! X coordinate of slider for supply.
#define WIDGET_SUPPLY_POSITION_X        39
//! Y coordinate of slider for supply.
#define WIDGET_SUPPLY_POSITION_Y        85

//! Height of progress bar for demand indication.
#define WIDGET_DEMAND_SIZE_X            20
//! Width of progress bar for demand indication.
#define WIDGET_DEMAND_SIZE_Y            45
//! X coordinate of progress bar for demand indication.
#define WIDGET_DEMAND_POSITION_X        250
//! Y coordinator of progress bar for demand indication.
#define WIDGET_DEMAND_POSITION_Y        100

//! @}

/**
 * \name Bitmap filenames, sizes and positions
 *
 * @{
 */

//! Filename of background bitmap.
#define BITMAP_BACKGROUND_FILENAME      "p_tankbg"
//! Width of background bitmap.
#define BITMAP_BACKGROUND_SIZE_X        320
//! Height of background bitmap.
#define BITMAP_BACKGROUND_SIZE_Y        240
//! X coordinate of background bitmap.
#define BITMAP_BACKGROUND_POSITION_X    0
//! Y coordinate of background bitmap.
#define BITMAP_BACKGROUND_POSITION_Y    0

//! Filename of red alarm light bitmap.
#define BITMAP_RED_LIGHT_FILENAME       "p_lgtred"
//! Filename of green alarm light bitmap.
#define BITMAP_GREEN_LIGHT_FILENAME     "p_lgtgrn"
//! Width of alarm light bitmap.
#define BITMAP_LIGHT_SIZE_X             38
//! Height of alarm light bitmap.
#define BITMAP_LIGHT_SIZE_Y             38
//! X coordinate of alarm light bitmap.
#define BITMAP_LIGHT_POSITION_X         241
//! Y coordinate of alarm light bitmap.
#define BITMAP_LIGHT_POSITION_Y         26

//! @}

/**
 * \name Application timing configuration
 *
 * @{
 */

/**
 * \brief Application tick rate.
 *
 * This configures how often the application timer enqueues the application
 * task, i.e., how often the application is updated.
 */
#define TICK_RATE                       30
/**
 * \brief Application ticks between each update of random variable.
 *
 * To get a smoother process simulation, the random variable is not updated with
 * every application update. This defines how many ticks should pass before this
 * variable is also updated.
 */
#define TICKS_PER_RANDOM_UPDATE         9

//! @}

//! States for the application loader task.
enum tank_loader_state {
	//! Red alarm light bitmap will be loaded to hugemem.
	LOAD_RED_LIGHT,
	//! Green alarm light bitmap will be loaded to hugemem.
	LOAD_GREEN_LIGHT,
	//! Background bitmap will be shown.
	LOAD_BACKGROUND,
	//! The application's widgets will be shown, and its timer started.
	LOAD_FINISHED,
};

/**
 * \brief Enumeration of bitmaps to load to hugemem.
 *
 * This enum is used for indexing in \ref tank_bitmap_data when loading and
 * drawing bitmaps.
 */
enum tank_bitmap_id {
	//! ID and index of bitmap for red alarm light.
	BITMAP_RED_LIGHT,
	//! ID and index of bitmap for green alarm light.
	BITMAP_GREEN_LIGHT,
	//! Total number of bitmaps loaded to hugemem.
	NR_OF_BITMAPS,
};

/**
 * \brief Pointers to bitmap data in hugemem.
 *
 * \note To avoid allocating hugemem and loading the bitmaps more than once,
 * these pointers are statically allocated.
 */
static hugemem_ptr_t tank_bitmap_data[NR_OF_BITMAPS];

/**
 * \brief Event command ID for application widgets.
 *
 * \note The first entry is zero-valued and therefore only used for widgets that
 * should not generate command events, i.e., it will disable this feature for
 * most interactive widgets. Refer to the \ref gfx_wtk "Widget toolkit" for more
 * information.
 */
enum tank_command_id {
	//! Command event value to use for supply-slider.
	CMD_NONE,
	//! Command event value to use for exit-button.
	CMD_EXIT,
};

//! Context for the water tank application.
struct tank_context {
	//! Pointer to the workqueue task of the application.
	struct workqueue_task   *task;
	//! Pointer to the main frame of the application.
	struct wtk_basic_frame  *frame;
	//! Copy of the sysfont before it is modified by the application
	struct font             old_sysfont;
	//! Pointer to progress bar widget for tank level.
	struct wtk_progress_bar *level;
	//! Pointer to slider widget for supply.
	struct wtk_slider       *supply;
	//! Pointer to progress bar widget for demand.
	struct wtk_progress_bar *demand;
	//! Timer struct needed for the timer driver.
	struct timer            timer;
	//! State variable for application loader task.
	enum tank_loader_state  loader_state;
	//! Flag indicating critical tank level.
	bool                    level_alarm;
	//! Flag indicating that demand is greater than supply and tank level.
	bool                    flow_alarm;
	//! Timer delay needed for application tick rate.
	uint16_t                timer_delay;
	//! Tick count until update of random variable.
	uint16_t                rand_ticks;
	//! Random variable to get time-varying water demand.
	int32_t                 rand;
	//! Metadata for alarm light bitmaps.
	struct gfx_bitmap       bitmaps[NR_OF_BITMAPS];
};

/**
 * \brief Pointer to water tank application context.
 *
 * \note This pointer is statically allocated, but the context itself is only
 * allocated when the application is launched, and then destroyed upon exit to
 * conserve memory.
 */
static struct tank_context *tank_ctx;

/**
 * \brief Command event handler for the application's frame.
 *
 * This function handles the relevant command events for the application's
 * frame. In this application, only the exit-button requires handling, and will
 * cause an immediate exit from the application.
 *
 * \note The handler function of the \ref gfx_wtk "Widget toolkit" will destroy
 * the frame and the widgets when this function returns true.
 *
 * \param basic_frame Pointer to the application frame.
 * \param command_data Data for the command event.
 */
static bool tank_frame_handler(struct wtk_basic_frame *basic_frame,
		win_command_t command_data)
{
	switch ((enum tank_command_id)(uintptr_t)command_data) {
	case CMD_EXIT:
		// Stop the application timer first.
		timer_stop(CONFIG_TIMER_ID, &tank_ctx->timer);

		// Free all memory and return to desktop.
		memcpy(&sysfont, &tank_ctx->old_sysfont,
				sizeof(struct font));
		membag_free(tank_ctx);
		tank_ctx = NULL;
		app_desktop_restart();
		return true;

	default:
		break;
	}

	return false;
}

/**
 * \brief Compute new random value via a logistic map.
 *
 * This function computes a pseudo-random value by use of a scaled logistic map.
 *
 * Refer to the detailed description of \ref apps_tank_group for information
 * on how this is computed.
 *
 * \param rand Current random value to map.
 *
 * \return New random value, in the range 0 to VALUE_DEMAND_MAXIMUM.
 */
static int32_t tank_logistic_map(int32_t rand)
{
	/* Ensure that the current value will not map to 0, as the logistic map
	 * will then get "stuck".
	 */
	rand = max_s(rand, 1);
	rand = min_s(rand, VALUE_DEMAND_MAXIMUM - 1);

	/* Map to a new value with the logistic function.
	 * See application description for explanation.
	 */
	rand *= VALUE_DEMAND_MAXIMUM - rand;
	rand *= 79;
	rand /= VALUE_DEMAND_MAXIMUM * 20;

	return rand;
}

/**
 * \brief Application worker function.
 *
 * This task worker updates the parameters of the simulated process and the
 * display unless the application context has been cleared, in which case the
 * function returns immediately.
 *
 * Refer to the detailed description of \ref apps_tank_group for information on
 * the simulated process.
 *
 * If the tank level reaches 0, the indicator for demand will change color to
 * indicate that the supply is insufficient. If, on the other hand, it reaches
 * \ref VALUE_LEVEL_MAXIMUM, the alarm light changes color to indicate that the
 * supply is too great.
 *
 * \note The task for this worker function is enqueued by the timer callback
 * function.
 *
 * \param task Workqueue task for this worker function.
 */
static void tank_worker(struct workqueue_task *task)
{
	struct gfx_bitmap   *alarm_bitmap = NULL;
	bool                alarm_update = false;
	int32_t             rand;
	uint8_t             supply;
	uint16_t            demand;
	int16_t             flow;
	int16_t             level;

	// Quit immediately if application context has been cleared.
	if (!tank_ctx) {
		return;
	}

	// Get current values of the process parameters.
	level = wtk_progress_bar_get_value(tank_ctx->level);
	supply = wtk_slider_get_value(tank_ctx->supply);
	demand = wtk_progress_bar_get_value(tank_ctx->demand);
	rand = tank_ctx->rand;

	// Update the random variable if enough ticks have passed.
	if (--(tank_ctx->rand_ticks) == 0) {
		tank_ctx->rand_ticks = TICKS_PER_RANDOM_UPDATE;

		// Flip some LSBs to help avoid a stuck process.
		rand ^= demand & 0x03;

		// Compute new random value and store it for next iteration.
		rand = tank_logistic_map(rand);
		tank_ctx->rand = rand;
	}

	// Now, compute the new demand from a weighted scheme.
	demand = rand + (2 * demand) + level;
	demand /= 4;
	demand = min_u(demand, VALUE_DEMAND_MAXIMUM);

	/* Compute the total flow and scale it down for a smoother
	 * simulation.
	 */
	flow = supply;
	flow -= demand;
	flow /= 4;

	// Compute new level for the tank.
	level += flow;

	/* Update the demand indicator. Change its color if the demand
	 * exceeds the supply and available water in the tank.
	 */
	if (level <= 0) {
		if (!tank_ctx->flow_alarm) {
			tank_ctx->flow_alarm = true;
			wtk_progress_bar_set_colors(tank_ctx->demand,
					COLOR_DEMAND_CRITICAL,
					COLOR_DEMAND_BACKGROUND);
		}
		level = 0;
	} else {
		if (tank_ctx->flow_alarm) {
			tank_ctx->flow_alarm = false;
			wtk_progress_bar_set_colors(tank_ctx->demand,
					COLOR_DEMAND_NORMAL,
					COLOR_DEMAND_BACKGROUND);
		}
	}
	wtk_progress_bar_set_value(tank_ctx->demand, demand);

	/* Update the tank level indicator.
	 * The level of the tank cannot exceed the defined maximum,
	 * and its alarm light must be updated whenever this level
	 * is crossed.
	 */
	if (level >= VALUE_LEVEL_MAXIMUM) {
		if (!tank_ctx->level_alarm) {
			tank_ctx->level_alarm = true;
			alarm_update = true;
			alarm_bitmap = &tank_ctx->bitmaps[BITMAP_RED_LIGHT];
		}
		level = VALUE_LEVEL_MAXIMUM;
	} else {
		if (tank_ctx->level_alarm) {
			tank_ctx->level_alarm = false;
			alarm_update = true;
			alarm_bitmap = &tank_ctx->bitmaps[BITMAP_GREEN_LIGHT];
		}
	}
	wtk_progress_bar_set_value(tank_ctx->level, level);

	/* If a level alarm update occurred, set up clipping area and draw
	 * the new alarm light bitmap.
	 */
	if (alarm_update) {
#ifdef CONFIG_GFX_USE_CLIPPING
		gfx_set_clipping(BITMAP_LIGHT_POSITION_X,
				BITMAP_LIGHT_POSITION_Y,
				BITMAP_LIGHT_POSITION_X
					+ BITMAP_LIGHT_SIZE_X - 1,
				BITMAP_LIGHT_POSITION_Y
					+ BITMAP_LIGHT_SIZE_Y - 1
				);
#endif /* CONFIG_GFX_USE_CLIPPING */
		gfx_draw_bitmap(alarm_bitmap,
				BITMAP_LIGHT_POSITION_X,
				BITMAP_LIGHT_POSITION_Y);
	}
}

/**
 * \brief Application timer callback function.
 *
 * This callback function is used with the \ref timer_group "Timer driver" and
 * will enqueue the task for application updates.
 *
 * \param timer Pointer to timer struct associated with the callback.
 */
static void tank_timer_callback(struct timer *timer)
{
	timer_set_alarm(CONFIG_TIMER_ID, &tank_ctx->timer,
			tank_ctx->timer_delay);
	workqueue_add_task(&main_workqueue, tank_ctx->task);
}

/**
 * \brief Application loader.
 *
 * This worker function loads the alarm light bitmaps, then loads the background
 * image before showing all the widgets and starting the process simulation.
 *
 * If memory for bitmaps cannot be allocated, or the background image cannot be
 * loaded, the application will exit immediately.
 *
 * \param task Workqueue task for this worker function.
 */
static void tank_loader(struct workqueue_task *task)
{
	enum status_code    result;
	hugemem_ptr_t       bitmap_data;

	/* Handle the individual bitmap load states.
	 *
	 * All states, except LOAD_FINISHED, will cause the current workqueue
	 * task to be re-enqueued after loading of the corresponding bitmap has
	 * completed. This task worker will thus go through the states in
	 * sequence.
	 */
	switch (tank_ctx->loader_state) {
	case LOAD_RED_LIGHT:
		// Enqueue loading of the red alarm light bitmap.
		bitmap_data = load_file_to_hugemem(BITMAP_RED_LIGHT_FILENAME,
				task);

		/* If memory could be allocated for the bitmap, update the
		 * pointer in the bitmap metadata and set next load state.
		 * Otherwise, exit the application load error.
		 */
		if (bitmap_data != HUGEMEM_NULL) {
			tank_bitmap_data[BITMAP_RED_LIGHT] = bitmap_data;
			tank_ctx->bitmaps[BITMAP_RED_LIGHT].data.hugemem =
					bitmap_data;

			tank_ctx->loader_state = LOAD_GREEN_LIGHT;
		} else {
			goto exit_load_error;
		}
		break;

	case LOAD_GREEN_LIGHT:
		bitmap_data = load_file_to_hugemem(BITMAP_GREEN_LIGHT_FILENAME,
				task);

		if (bitmap_data != HUGEMEM_NULL) {
			tank_bitmap_data[BITMAP_GREEN_LIGHT] = bitmap_data;
			tank_ctx->bitmaps[BITMAP_GREEN_LIGHT].data.hugemem
					= bitmap_data;

			tank_ctx->loader_state = LOAD_BACKGROUND;
		} else {
			goto exit_load_error;
		}
		break;

	case LOAD_BACKGROUND:
		// Now load the background image.
		result = load_file_to_screen(BITMAP_BACKGROUND_FILENAME,
				BITMAP_BACKGROUND_POSITION_X,
				BITMAP_BACKGROUND_POSITION_Y,
				BITMAP_BACKGROUND_SIZE_X,
				BITMAP_BACKGROUND_SIZE_Y, task);

		if (result == STATUS_OK) {
			tank_ctx->loader_state = LOAD_FINISHED;
		} else {
			goto exit_load_error;
		}
		break;

	case LOAD_FINISHED:
		// Now show the application's frame. This will draw all widgets.
		win_show(wtk_basic_frame_as_child(tank_ctx->frame));

		// Set the worker function that updates the application.
		workqueue_task_set_work_func(task, tank_worker);

		// Set the timer alarm to trigger application updates.
		timer_start(CONFIG_TIMER_ID, &tank_ctx->timer);
		timer_set_alarm(CONFIG_TIMER_ID, &tank_ctx->timer,
				tank_ctx->timer_delay);
		break;

	default:
		// Should not end up here.
		unhandled_case(tank_ctx->loader_state);
		break;
	}

	return;

	// If a load error occurred, go back to the desktop.
exit_load_error:
	win_destroy(wtk_basic_frame_as_child(tank_ctx->frame));
	memcpy(&sysfont, &tank_ctx->old_sysfont, sizeof(struct font));
	membag_free(tank_ctx);
	tank_ctx = NULL;
	app_desktop_restart();
}

/**
 * \brief Launch the water tank application.
 *
 * This function allocates memory, creates the application frame and widgets,
 * initializes the timer and initiates loading of the required bitmaps.
 *
 * One basic frame is created, along with four widgets: one slider, two progress
 * bars and a command button. These are not shown until the background image has
 * been loaded.
 *
 * If the alarm light bitmaps have already been loaded, i.e., the application
 * has already been run, the application will skip directly to loading of the
 * background image.
 *
 * If memory for the application context cannot be allocated or the frame or
 * widgets cannot be created, the application will exit immediately.
 *
 * \param task Workqueue task to use for the application's worker functions.
 */
void app_tank_launch(struct workqueue_task *task)
{
	struct win_attributes    attr;
	struct win_window        *win;
	struct gfx_bitmap        bitmap;
	struct wtk_basic_frame   *frame;
	struct wtk_slider        *slider;
	struct wtk_button        *button;
	struct wtk_progress_bar  *pbar;
	struct timer             *timer;
	timer_res_t              timer_res;
	uint32_t                 timer_clk;

	assert(task);

	// Clear the screen right away.
#ifdef CONFIG_GFX_USE_CLIPPING
	gfx_set_clipping(0, 0, gfx_get_width(), gfx_get_height());
#endif /* CONFIG_GFX_USE_CLIPPING */
	gfx_draw_filled_rect(0, 0, gfx_get_width(), gfx_get_height(),
			COLOR_WIN_BACKGROUND);

	// Allocate the application context first.
	tank_ctx = membag_alloc(sizeof(struct tank_context));
	if (!tank_ctx)
		goto exit_no_context;

	// Use larger sysfont for this app.
	memcpy(&tank_ctx->old_sysfont, &sysfont, sizeof(struct font));
	sysfont.scale = 2;

	// Create a basic frame to contain the widgets.
	attr.area.pos.x = 0;
	attr.area.pos.y = 0;
	attr.area.size.x = gfx_get_width();
	attr.area.size.y = gfx_get_height();

	frame = wtk_basic_frame_create(win_get_root(), &attr.area, NULL,
			NULL, tank_frame_handler, NULL);
	if (!frame) {
		goto exit_no_frame;
	}
	tank_ctx->frame = frame;

	// Get the frame's window to use as parent for widgets.
	win = wtk_basic_frame_as_child(frame);

	// Initialize the application timer.
	timer = &tank_ctx->timer;
	timer_init(CONFIG_TIMER_ID, timer, tank_timer_callback);
	timer_res = timer_set_resolution(CONFIG_TIMER_ID, timer,
			TICK_RATE);
	timer_write_resolution(CONFIG_TIMER_ID, timer, timer_res);

	// Get the timer alarm delay to use for the configured tick rate.
	timer_clk = timer_get_resolution(CONFIG_TIMER_ID, timer, timer_res);
	tank_ctx->timer_delay = timer_clk / TICK_RATE;

	// Initialize random variable and tick count.
	tank_ctx->rand = 1;
	tank_ctx->rand_ticks = TICKS_PER_RANDOM_UPDATE;

	// Create the supply slider.
	attr.area.pos.x = WIDGET_SUPPLY_POSITION_X;
	attr.area.pos.y = WIDGET_SUPPLY_POSITION_Y;
	attr.area.size.x = WIDGET_SUPPLY_SIZE_X;
	attr.area.size.y = WIDGET_SUPPLY_SIZE_Y;

	slider = wtk_slider_create(win, &attr.area, VALUE_SUPPLY_MAXIMUM,
			VALUE_SUPPLY_INITIAL, WTK_SLIDER_VERTICAL |
					WTK_SLIDER_INVERT, CMD_NONE);
	if (!slider) {
		goto exit_no_widget;
	}
	tank_ctx->supply = slider;
	win_show(wtk_slider_as_child(slider));

	// Create the tank level progress bar.
	attr.area.pos.x = WIDGET_LEVEL_POSITION_X;
	attr.area.pos.y = WIDGET_LEVEL_POSITION_Y;
	attr.area.size.x = WIDGET_LEVEL_SIZE_X;
	attr.area.size.y = WIDGET_LEVEL_SIZE_Y;

	pbar = wtk_progress_bar_create(win, &attr.area, VALUE_LEVEL_MAXIMUM,
			VALUE_LEVEL_INITIAL, COLOR_LEVEL_FILL,
			COLOR_LEVEL_BACKGROUND, WTK_PROGRESS_BAR_VERTICAL |
					WTK_PROGRESS_BAR_INVERT);
	if (!pbar) {
		goto exit_no_widget;
	}
	tank_ctx->level = pbar;
	win_show(wtk_progress_bar_as_child(pbar));

	// Create the demand progress bar.
	attr.area.pos.x = WIDGET_DEMAND_POSITION_X;
	attr.area.pos.y = WIDGET_DEMAND_POSITION_Y;
	attr.area.size.x = WIDGET_DEMAND_SIZE_X;
	attr.area.size.y = WIDGET_DEMAND_SIZE_Y;

	pbar = wtk_progress_bar_create(win, &attr.area, VALUE_DEMAND_MAXIMUM,
			VALUE_DEMAND_INITIAL, COLOR_DEMAND_NORMAL,
			COLOR_DEMAND_BACKGROUND, WTK_PROGRESS_BAR_VERTICAL |
					WTK_PROGRESS_BAR_INVERT);
	if (!pbar) {
		goto exit_no_widget;
	}
	tank_ctx->demand = pbar;
	win_show(wtk_progress_bar_as_child(pbar));

	// Create the exit button with the standard settings.
	attr.area.pos.x = APP_EXIT_BUTTON_POS_X;
	attr.area.pos.y = APP_EXIT_BUTTON_POS_Y;
	attr.area.size.x = APP_EXIT_BUTTON_SIZE_X;
	attr.area.size.y = APP_EXIT_BUTTON_SIZE_Y;

	button = wtk_button_create(win, &attr.area, APP_EXIT_BUTTON_TEXT,
			(win_command_t)CMD_EXIT);
	if (!button) {
		goto exit_no_widget;
	}
	win_show(wtk_button_as_child(button));

	// Set the tank alarm to trigger initial drawing of alarm light.
	tank_ctx->level_alarm = true;
	tank_ctx->flow_alarm = false;
	tank_ctx->task = task;

	/* Initialize bitmap data and set initial application loader state:
	 * If the alarm light bitmaps have already been loaded, skip right to
	 * loading of the application background bitmap.
	 */
	bitmap.width = BITMAP_LIGHT_SIZE_X;
	bitmap.height = BITMAP_LIGHT_SIZE_Y;
	bitmap.type = BITMAP_HUGEMEM;
	tank_ctx->bitmaps[BITMAP_RED_LIGHT] = bitmap;
	tank_ctx->bitmaps[BITMAP_GREEN_LIGHT] = bitmap;

	if (tank_bitmap_data[BITMAP_GREEN_LIGHT]) {
		tank_ctx->loader_state = LOAD_BACKGROUND;
		tank_ctx->bitmaps[BITMAP_RED_LIGHT].data.hugemem =
				tank_bitmap_data[BITMAP_RED_LIGHT];
		tank_ctx->bitmaps[BITMAP_GREEN_LIGHT].data.hugemem =
				tank_bitmap_data[BITMAP_GREEN_LIGHT];
	} else {
		tank_ctx->loader_state = LOAD_RED_LIGHT;
	}
	workqueue_task_set_work_func(task, tank_loader);
	workqueue_add_task(&main_workqueue, task);
	return;

	// Handle allocation errors.
exit_no_widget:
	win_destroy(wtk_basic_frame_as_child(tank_ctx->frame));
exit_no_frame:
	memcpy(&sysfont, &tank_ctx->old_sysfont, sizeof(struct font));
	membag_free(tank_ctx);
exit_no_context:
	app_desktop_restart();
	return;
}

//! @}

/**
 * \file
 *
 * \brief Display Xplained calibrate application
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
#include <string.h>
#include <progmem.h>
#include <mainloop.h>

#include <gfx/gfx.h>
#include <gfx/win.h>
#include <gfx/wtk.h>
#include <gfx/sysfont.h>

#include <touch/touch.h>

#include "app_calibrate.h"

/**
 * \weakgroup app_calibrate_group
 *
 * @{
 */

/**
 * \name Application configuration
 *
 * @{
 */

//! Offset from the display edges to calibration circles
#define CAL_OFFSET      50
//! Radius of calibration circles
#define CAL_RADIUS      5
//! Threshold for unintended touches on display
#define CAL_TRESHOLD    500
//! Background color for the application
#define CAL_BG_COLOR    GFX_COLOR(0, 0, 255)
//! Foreground color for the application
#define CAL_FG_COLOR    GFX_COLOR(255, 255, 255)

//! @}

//! Convenience macro for getting absolute value of an integer
#define abs(a)          (((a) < 0) ? -(a) : (a))

//! Application context
struct touch_calibrate_context {
	//! Task used for this application
	struct workqueue_task           task;
	//! Task to schedule when done
	struct workqueue_task           *completed_task;
	//! Calibration matrix to compute
	struct touch_calibration_matrix cal_matrix;
	//! Touch event handler to restore when done
	touch_event_handler_t           old_handler;
	//! Calibration points to sample
	touch_calibration_points_t      cal_points;
	//! Previous touch event
	struct touch_event              event;
	//! State of the calibration
	uint8_t                         state;
};

//! Context pointer
static struct touch_calibrate_context *calibrate_context;

//! Application font to use
static struct font sysfont2x;

/**
 * \brief Calibration guide text
 *
 * \note Stored in PROGMEM to reduce memory usage.
 */
static DEFINE_PROGMEM(char, calibrate_help_text[]) =
		"Touch screen calibration.\n\nTouch inside the circles.";

/**
 * \brief Touch event handler
 *
 * Stores the current touch event to be processed by
 * \ref touch_calibrate_task_handler.
 *
 * \param event Touch event data
 */
static void touch_calibrate_event_handler(const struct touch_event *event)
{
	calibrate_context->event = *event;
}

/**
 * \brief Application task worker
 *
 * Waits for the touch release events generated after the user has touched a
 * calibration circle drawn on screen, then stores the calibration data and
 * draws the circle for the next calibration point.
 *
 * Three such calibration points are stored before the calibration matrix for
 * the touch driver is computed and assigned to it.
 *
 * \sa touch_driver_group
 *
 * \note If the raw samples of a calibration point do not differ by at least
 * \ref CAL_TRESHOLD from the previous calibration point, it is interpreted as
 * an unintended touch and ignored.
 *
 * \param task Workqueue task for this worker function
 */
static void touch_calibrate_task_handler(struct workqueue_task *task)
{
	int16_t                         dx;
	int16_t                         dy;
	struct touch_calibrate_context  *cal_ctx;

	cal_ctx = container_of(task, struct touch_calibrate_context, task);

	switch (cal_ctx->state) {
	case 0:	/* Fall through */
	case 1:
	case 2:
		// Schedule task to run once more
		workqueue_add_task(&main_workqueue, &cal_ctx->task);

		// Run until touch is released
		if (cal_ctx->event.type != TOUCH_RELEASE)
			break;

		// Store calibration values
		cal_ctx->event.type = TOUCH_NO_EVENT;
		cal_ctx->cal_points[cal_ctx->state].raw_x =
				cal_ctx->event.point.raw_x;
		cal_ctx->cal_points[cal_ctx->state].raw_y =
				cal_ctx->event.point.raw_y;

		dx = cal_ctx->cal_points[cal_ctx->state - 1].raw_x -
				cal_ctx->cal_points[cal_ctx->state].raw_x;
		dy = cal_ctx->cal_points[cal_ctx->state - 1].raw_y -
				cal_ctx->cal_points[cal_ctx->state].raw_y;
		dx = abs(dx);
		dy = abs(dy);

		// If point is too close to the last one, wait for another touch
		if ((dx < CAL_TRESHOLD) && (dy < CAL_TRESHOLD))
			break;

		// Clear old circle before moving on.
		gfx_draw_circle(cal_ctx->cal_points[cal_ctx->state].panel_x,
				cal_ctx->cal_points[cal_ctx->state].panel_y,
				CAL_RADIUS, CAL_BG_COLOR, GFX_WHOLE);

		// Move to next point
		cal_ctx->state++;

		/* Skip drawing last circles if we're done. */
		if (cal_ctx->state >= 3)
			break;

		// Draw next circle.
		gfx_draw_circle(cal_ctx->cal_points[cal_ctx->state].panel_x,
				cal_ctx->cal_points[cal_ctx->state].panel_y,
				CAL_RADIUS, CAL_FG_COLOR, GFX_WHOLE);
		break;

	case 3:
		// Calibration completed

		// Clear circle before moving on.
		gfx_draw_circle(cal_ctx->cal_points[2].panel_x,
				cal_ctx->cal_points[2].panel_y,
				CAL_RADIUS, CAL_BG_COLOR, GFX_WHOLE);

		// Compute and assign the calibration matrix to the driver.
		touch_compute_calibration_matrix(cal_ctx->cal_points,
				&cal_ctx->cal_matrix);

		touch_set_calibration_matrix(&cal_ctx->cal_matrix);

		// Restore handler
		touch_set_event_handler(cal_ctx->old_handler);

		// Can now free memory
		membag_free(calibrate_context);

		// Schedule task if available
		if (calibrate_context->completed_task) {
			workqueue_add_task(&main_workqueue,
					calibrate_context->completed_task);
		}

		break;
	}
}

/**
 * \brief Set up calibration
 *
 * Allocates and initializes the application context; sets up the font, touch
 * event handler and calibration data; updates the display and then schedules
 * the calibration task.
 *
 * \param completed_task Task to schedule when calibration is complete
 */
void app_touch_calibrate_setup(struct workqueue_task *completed_task)
{
	calibrate_context =
			membag_alloc(sizeof(struct touch_calibrate_context));
	assert(calibrate_context != NULL);

	// Use twice as large font for this application.
	memcpy(&sysfont2x, &sysfont, sizeof(sysfont));
	sysfont2x.scale = 2;

	// Temporarily replace touch event handler.
	calibrate_context->old_handler = touch_get_event_handler();
	touch_set_event_handler(touch_calibrate_event_handler);

	// Clear the screen and draw the calibration guide text.
	gfx_set_clipping(0, 0, gfx_get_width(), gfx_get_height());
	gfx_draw_filled_rect(0, 0, gfx_get_width(), gfx_get_height(),
			CAL_BG_COLOR);
	gfx_draw_progmem_string((const char __progmem_arg *)
			&calibrate_help_text, 10, 80, &sysfont2x, CAL_FG_COLOR,
			GFX_COLOR_TRANSPARENT);

	// Set panel coordinates for all calibration points.
	calibrate_context->cal_points[0].panel_x =
			(gfx_get_width() - CAL_OFFSET - 1);
	calibrate_context->cal_points[0].panel_y =
			(gfx_get_height() - CAL_OFFSET - 1);
	calibrate_context->cal_points[1].panel_x = (CAL_OFFSET);
	calibrate_context->cal_points[1].panel_y =
			(gfx_get_height() - CAL_OFFSET - 1);
	calibrate_context->cal_points[2].panel_x = (CAL_OFFSET);
	calibrate_context->cal_points[2].panel_y = (CAL_OFFSET);

	// Draw circle for first calibration point.
	gfx_draw_circle(calibrate_context->cal_points[0].panel_x,
			calibrate_context->cal_points[0].panel_y,
			CAL_RADIUS, CAL_FG_COLOR, GFX_WHOLE);

	// Initialize the calibration state and tasks before scheduling it.
	calibrate_context->state = 0;
	calibrate_context->completed_task = completed_task;

	workqueue_task_init(&calibrate_context->task,
			touch_calibrate_task_handler);
	workqueue_add_task(&main_workqueue, &calibrate_context->task);
}

//! @}

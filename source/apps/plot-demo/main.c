/**
 * \file
 *
 * \brief Plot widget demo application.
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

/**
 * \mainpage Plot widget demo
 * \section plot_widget_demo_intro Introduction
 *
 * The plot widget demo is an application that introduces users to
 * the plot widget. The application will let you add values to the plot from
 * a slider and change the draw color of the plot.
 *
 * - \ref gfx "Graphics display system"
 * - \ref gfx_gfx "Graphics drivers"
 * - \ref gfx_win "Window system"
 * - \ref gfx_wtk "Widget toolkit"
 * - \ref touch_driver_group "Touch drivers"
 * - \ref apps_group
 *
 */

/**
 * \defgroup apps_group Applications
 */
#include <dma.h>
#include <led.h>
#include <board.h>
#include <mainloop.h>

#include <clk/sys.h>

#include <gfx/gfx.h>
#include <gfx/win.h>

#include <touch/touch.h>

#include "app_calibrate.h"
#include "app_widget.h"

static struct workqueue_task launch_application;

static void setup_root_window(void)
{
	// Set transparent background window
	struct win_window *win_root;
	struct win_attributes attr;

	win_root = win_get_root();
	attr = *win_get_attributes(win_root);
	attr.background = NULL;
	win_set_attributes(win_root, &attr, WIN_ATTR_BACKGROUND);
	win_show(win_root);
}

int main(void)
{
	cpu_irq_enable();
	sysclk_init();
	dbg_init();
	board_init();
	led_activate(BOARD_LED0_ID);
	workqueue_init(&main_workqueue);
#ifdef CONFIG_TOUCH_RESISTIVE
	touch_init();
	touch_enable();
#endif
	gfx_init();
	membag_init(CPU_DMA_ALIGN);
	win_init();

	setup_root_window();

	// Setup task for starting the application
	workqueue_task_init(&launch_application, app_widget_launch);

	// Start calibration with task for starting application afterwards
	app_touch_calibrate_setup(&launch_application);

	// Start processing workqueue
	mainloop_run(&main_workqueue);
}

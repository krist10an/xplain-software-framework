/**
 * \file
 *
 * \brief Plot widget demo application
 *
 * Copyright (C) 2011 Atmel Corporation. All rights reserved.
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
#ifndef APP_WIDGET_H_INCLUDED
#define APP_WIDGET_H_INCLUDED

#include <mainloop.h>

/**
 * \ingroup apps_group
 * \defgroup app_widget_group Plot demo application
 *
 * \brief This application is made to introduce users to the plot widget
 * in the \ref gfx_wtk "Widget toolkit".
 *
 * Initially this application will draw a
 * \ref gfx_wtk_basic_frame "basic frame" filling the entire screen and add a
 * \ref gfx_wtk_plot "plot" with the size 320 x 220 pixels, leaving a space of
 * 320 x 20 on the bottom of the screen for a \ref gfx_wtk_slider "slider"
 * and two \ref gfx_wtk_button "buttons" to control the widget.
 *
 * The plot widget will plot 33 values from 0 to 100 on the x-axis.
 * The add value button will add the value of the slider to the end of the plot,
 * shifting the first value out of the screen.
 * The Change color button will change the draw color of the plot.
 *
 *
 * @{
 */

void app_widget_launch(struct workqueue_task *task);

//! @}

#endif /* APP_WIDGET_H_INCLUDED */

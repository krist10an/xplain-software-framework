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

#ifndef APP_SLIDESHOW_H
#define APP_SLIDESHOW_H

#include <workqueue.h>

/**
 * \ingroup apps_group
 * \defgroup apps_slide_group Slideshow application
 *
 * \brief The slideshow application is a timed, automatic image loader. It
 * demonstrates use of the \ref gfx_win "Window toolkit" and \ref tsfs_group
 * "Tiny Simple file system".
 *
 * The user can manually control the loading of the slides by tapping the left
 * or right sections of the display to select the previous or next slide,
 * respectively.
 * If the user selects the previous slide, there is a 16 second delay before the
 * automatic loading continues. If the next slide is selected, the delay
 * defaults to the same as during automatic loading: 4 seconds.
 *
 * To exit the application, the user must tap the middle section of the display
 * when the application is not loading a slide.
 *
 * Five application launcher functions are supplied, and these will each start
 * a slideshow with:
 * - various pictures
 * - 8-bit AVR presentation
 * - 32-bit AVR presentation
 * - AVR XMEGA presentation
 * - Display Xplained presentation
 *
 * This application makes use of the \ref timer_group "Timer driver" for timing
 * of the automatic loading and the \ref appsutil_fileloader_group
 * "Fileloader utility" for loading files directly from filesystem to screen.
 */

void app_slideshow_pics_launch(struct workqueue_task *task);
void app_slideshow_avr_launch(struct workqueue_task *done);
void app_slideshow_uc3_launch(struct workqueue_task *done);
void app_slideshow_xmega_launch(struct workqueue_task *done);
void app_slideshow_dx_launch(struct workqueue_task *done);

#endif /* APP_SLIDESHOW_H */

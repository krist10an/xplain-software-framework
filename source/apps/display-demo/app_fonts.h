/**
 * \file
 *
 * \brief Display Xplained fonts application
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
#ifndef APP_FONTS_H_INCLUDED
#define APP_FONTS_H_INCLUDED

#include <mainloop.h>

/**
 * \ingroup apps_group
 * \defgroup app_fonts_group Fonts demonstration application
 *
 * \brief The font application demonstrates use of fonts with the
 * \ref gfx "Graphics display system.
 *
 * This application will load four fonts from the TSFS file system and use them
 * on various pages drawn to the screen. The first time the application is
 * launched the fonts are loaded into hugemem, and a progress bar of the work
 * is shown on the screen. Consecutive executions will reuse the already loaded
 * fonts.
 *
 * There are two navigation buttons at the bottom right corner, along with an
 * exit button. The navigation buttons navigates between the different
 * demonstration pages, while the exit button restarts the desktop application.
 *
 * @{
 */

void app_fonts_launch(struct workqueue_task *task);

//! @}

#endif /* APP_FONTS_H_INCLUDED */

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
#ifndef APP_DESKTOP_H_INCLUDED
#define APP_DESKTOP_H_INCLUDED

/**
 * \defgroup apps_group Applications
 *
 */

/**
 * \ingroup apps_group
 * \defgroup app_desktop_group Desktop application
 *
 * \brief The desktop application handles launching of additional applications.
 *
 * Icons and names for the individual applications which have been registered
 * are displayed as an on-screen menu. The user can launch the applications with
 * pointer presses on their respective icons.
 *
 * The desktop is implemented on the \ref win_get_root "root window", without
 * any additional \ref win_window "windows" on top, to ensure minimal memory
 * usage and event handling when applications are launched.
 *
 * @{
 */

/**
 * \name Default exit-button appearance
 *
 * @{
 */

//! Common application exit button size
#define APP_EXIT_BUTTON_SIZE_X 40
//! Common application exit button size
#define APP_EXIT_BUTTON_SIZE_Y 40
//! Common application exit button position
#define APP_EXIT_BUTTON_POS_X  (gfx_get_width() - APP_EXIT_BUTTON_SIZE_X)
//! Common application exit button position
#define APP_EXIT_BUTTON_POS_Y  (gfx_get_height() - APP_EXIT_BUTTON_SIZE_Y)
//! Common application exit button text
#define APP_EXIT_BUTTON_TEXT   "X"

//! @}

void app_desktop_setup(void);
void app_desktop_restart(void);

//! @}

/**
 * \ingroup apps_group
 * \defgroup appsutil_group Application utilities
 *
 * \brief Application utilities are helper utilities which are useful for the
 * applications.
 */

#endif /* APP_DESKTOP_H_INCLUDED */

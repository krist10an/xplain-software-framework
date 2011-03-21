/**
 * \file
 *
 * \brief Display demo memory game header file
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

#ifndef APP_GAME_H
#define APP_GAME_H

#include <workqueue.h>

/**
 * \ingroup apps_group
 * \defgroup apps_memgame_group Memory game application
 *
 * \brief The memory game application is a simple game where the user has to
 * find pairs of game pieces, i.e., ones that have the same image.
 * It demonstrates usage of the button and label widgets in the \ref gfx_wtk,
 * drawing text with the \ref gfx and basic use of the \ref gfx_win.
 *
 * The application displays 12 game pieces and the number of times the user has
 * tried to find matching pairs of them. Two buttons allow the user to restart
 * or end the game when the game is not busy drawing game pieces to screen.
 * If the user selects mismatched pieces, the game will pause for 1.5 seconds to
 * allow the user to memorize their position, before hiding them again.
 *
 * This application makes use of the \ref timer_group "Timer driver" for pausing
 * the game after after mismatching game pieces have been selected, and the
 * \ref appsutil_fileloader_group for loading the game piece images directly to
 * screen.
 */

void app_memgame_launch(struct workqueue_task *task);

#endif /* APP_GAME_H */

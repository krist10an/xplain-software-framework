/**
 * \file
 *
 * \brief Display demo memory game implementation
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

#include <gfx/gfx.h>
#include <gfx/win.h>
#include <gfx/wtk.h>
#include <gfx/sysfont.h>
#include <stream.h>
#include <string.h>
#include <timer.h>

#include "app_desktop.h"
#include "app_memgame.h"
#include "file_loader.h"

/**
 * \weakgroup apps_memgame_group
 *
 * @{
 */

/**
 * \name General game board configuration
 *
 * @{
 */

//! Number of piece pairs in the game.
#define NR_OF_PIECE_PAIRS            6
//! Number of pieces in the game.
#define NR_OF_PIECES                 (2 * NR_OF_PIECE_PAIRS)
//! Number of pieces per row on the board.
#define NR_OF_BOARD_COLUMNS          4
//! X-coordinate to start drawing first piece at.
#define BOARD_POS_X                  30
//! Y-coordinate to start drawing first piece at.
#define BOARD_POS_Y                  5
//! Background color for the game.
#define BACKGROUND_COLOR             GFX_COLOR(0, 0, 0)
//! Font scaling for button widgets.
#define WIDGET_FONT_SCALE            2

//! @}

/**
 * \name Game piece configuration
 *
 * @{
 */

//! Image file to load when showing piece pair 0.
#define PIECE_PAIR_FILE_0            "i_fonts"
//! Image file to load when showing piece pair 1.
#define PIECE_PAIR_FILE_1            "i_pics"
//! Image file to load when showing piece pair 2.
#define PIECE_PAIR_FILE_2            "i_avr"
//! Image file to load when showing piece pair 3.
#define PIECE_PAIR_FILE_3            "i_clock"
//! Image file to load when showing piece pair 4.
#define PIECE_PAIR_FILE_4            "i_files"
//! Image file to load when showing piece pair 5.
#define PIECE_PAIR_FILE_5            "i_games"
//! Image file to load when showing the back of game pieces.
#define PIECE_BACK_FILE              "cardback"
//! Width of piece images.
#define PIECE_SIZE_X                 57
//! Height of piece images.
#define PIECE_SIZE_Y                 PIECE_SIZE_X
//! Spacing between pieces in X-dimension.
#define PIECE_SPACING_X              10
//! Spacing between pieces in Y-dimension.
#define PIECE_SPACING_Y              PIECE_SPACING_X
//! Index to use to indicate no piece is selected.
#define PIECE_INVALID_INDEX          0xFF

//! @}

/**
 * \name New game-button configuration
 *
 * @{
 */

//! Text to draw on button.
#define BTN_NEW_GAME_TEXT            "New"
//! X-coordinate of button.
#define BTN_NEW_GAME_POS_X           (APP_EXIT_BUTTON_POS_X -        \
		2 * APP_EXIT_BUTTON_SIZE_X)
//! Y-coordinate of button.
#define BTN_NEW_GAME_POS_Y           APP_EXIT_BUTTON_POS_Y
//! Width of button.
#define BTN_NEW_GAME_SIZE_X          (2 * APP_EXIT_BUTTON_SIZE_X)
//! Height of button.
#define BTN_NEW_GAME_SIZE_Y          APP_EXIT_BUTTON_SIZE_Y

//! @}

/**
 * \name Number of tries-counter configuration
 *
 * @{
 */

//! Text to print, for use with \ref snprintf().
#define MSG_TRIES_TEXT               "Tries: %i"
//! Maximum number of characters in text string.
#define MSG_TRIES_LENGTH             10
//! X-coordinate of label.
#define MSG_TRIES_POS_X              0
//! Y-coordinate of label, vertically centered on buttons.
#define MSG_TRIES_POS_Y              (APP_EXIT_BUTTON_POS_Y +         \
		(APP_EXIT_BUTTON_SIZE_Y / 2) -                        \
		(gfx_font_get_height(&sysfont) / 2))
//! Width of label widget.
#define MSG_TRIES_SIZE_X             (gfx_get_width() -               \
		APP_EXIT_BUTTON_SIZE_X - BTN_NEW_GAME_SIZE_X)
//! Height of label widget.
#define MSG_TRIES_SIZE_Y             gfx_font_get_height(&sysfont)

//! @}

/**
 * \name Game over-message configuration
 *
 * @{
 */

//! Text to print when game is completed.
#define MSG_GAME_OVER_TEXT           "Well done!"
//! Font scaling for text string.
#define MSG_GAME_OVER_SCALE          4
//! X-coordinate of game-over text.
#define MSG_GAME_OVER_POS_X          45
//! Y-coordinate of game-over text.
#define MSG_GAME_OVER_POS_Y          105
//! Offset in position to use for getting a glow-effect.
#define MSG_GAME_OVER_GLOW_OFFSET    1
//! Color to print foreground text with.
#define MSG_GAME_OVER_COLOR          GFX_COLOR(255, 160, 0)
//! Color to print background "glow" text with.
#define MSG_GAME_OVER_GLOW_COLOR     GFX_COLOR(250, 250, 250)

//! @}

/**
 * \name Application timing configuration
 *
 * @{
 */

//! Number of half seconds before hiding mismatched game pieces.
#define TIMER_PAUSE_HALF_SECONDS     3
/**
 * \brief Minimum timer clock rate to use.
 *
 * Clock rate to use for detecting elapsed half-seconds with the
 * \ref timer_group "Timer driver".
 *
 * \note Assuming a 16-bit timer, a clock rate below 2^17 will allow for
 * detection of half seconds with the \ref timer_group "Timer driver". The
 * clock rate is divided by a factor of eight to ensure that a low enough clock
 * rate is selected by the driver during initialization.
 */
#define TIMER_CLOCK_RATE             (0x1ffffUL / 8)

//! @}

//! Event command IDs for application widgets.
enum memgame_command_id {
	//! Unused command event value.
	CMD_NONE,
	//! Command event value for new game-button.
	CMD_NEW_GAME,
	//! Command event value for exit-button.
	CMD_EXIT,
};

//! States of the memory game.
enum memgame_state {
	//! Draw all pieces on the board.
	DRAW_ALL_PIECES,
	//! Waiting for player to select first piece.
	SELECT_FIRST_PIECE,
	//! First selected piece has been shown.
	SHOWN_FIRST_PIECE,
	//! Waiting for player to select second piece.
	SELECT_SECOND_PIECE,
	//! Second selected piece has been shown.
	SHOWN_SECOND_PIECE,
	//! Hide first selected piece after mismatch.
	HIDE_FIRST_PIECE,
	//! Hide second selected piece after mismatch.
	HIDE_SECOND_PIECE,
	//! Both selected pieces have been hidden.
	HIDDEN_BOTH_PIECES,
	//! Game is over, wait for user to restart the game.
	GAME_OVER,
};

//! Game piece data.
struct memgame_piece {
	//! Which pair the piece belongs to.
	uint8_t pair;
	//! Flag indicating that the piece has been found.
	bool    found;
};

/**
 * \brief Context for the memory game application.
 *
 * \note The index of a piece in \ref pieces and its position on the game board
 * are equivalent.
 */
struct memgame_context {
	//! Pointer to the application's main window.
	struct win_window      *win;
	//! Bitmap metadata for application window background.
	struct gfx_bitmap      bitmap;
	//! Pointer to workqueue task for the application.
	struct workqueue_task  *task;
	//! Backup copy of sysfont, to restore upon application exit.
	struct font            old_sysfont;
	//! Timer struct needed for the timer driver.
	struct timer           timer;
	//! Timer delay needed to detect elapsed half-seconds.
	uint16_t               timer_delay;
	//! Application timer countdown.
	uint8_t                ticks_to_go;
	//! Data for all the game pieces.
	struct memgame_piece   pieces[NR_OF_PIECES];
	//! Current state of the game.
	enum memgame_state     state;
	//! Index and position of first selected piece.
	uint8_t                piece_1;
	//! Index and position of second selected piece.
	uint8_t                piece_2;
	//! Number of piece pairs left before game over.
	uint8_t                pairs_left;
	//! Number of tries the user has had.
	uint8_t                tries;
	//! Label widget for displaying number of tries.
	struct wtk_label       *tries_label;
	//! Flag indicating that the application is busy loading images.
	bool                   busy;
};

/**
 * \brief Pointer to memory game application context.
 *
 * \note This pointer is statically allocated, but the context itself is only
 * allocated when the application is launched, and then destroyed upon exit to
 * conserve memory.
 */
static struct memgame_context *game_ctx;

/**
 * \brief Simple pseudo-random number generator.
 *
 * \return Pseudo-random 16-bit number.
 */
static uint16_t memgame_rand(void)
{
	static uint16_t seed = 12345;
	seed = (25037 * seed) + 1;
	return seed;
}

/**
 * \brief Convenience macro for drawing text.
 *
 * Draws text string with the sysfont and specified color, with a transparent
 * background.
 *
 * \param str Text string to print.
 * \param x X-coordinate to draw text at.
 * \param y Y-coordinate to draw text at.
 * \param color Color to draw text with.
 */
#define memgame_print_helper(str, x, y, color) \
	gfx_draw_string(str, x, y, &sysfont, color, GFX_COLOR_TRANSPARENT)

/**
 * \brief Print game over-message.
 *
 * Writes a text string to the screen to notify the user that the game is over.
 * This is done several times, with different offsets and color to get a glow-
 * or outline-effect on the text.
 */
static void memgame_print_game_over(void)
{
#ifdef CONFIG_GFX_USE_CLIPPING
	gfx_set_clipping(0, 0, gfx_get_width(), gfx_get_height());
#endif /* CONFIG_GFX_USE_CLIPPING */

	// Temporarily enlarge the sysfont.
	sysfont.scale = MSG_GAME_OVER_SCALE;

	// Print the string with diagonal offsets.
	memgame_print_helper(MSG_GAME_OVER_TEXT,
			MSG_GAME_OVER_POS_X + MSG_GAME_OVER_GLOW_OFFSET,
			MSG_GAME_OVER_POS_Y + MSG_GAME_OVER_GLOW_OFFSET,
			MSG_GAME_OVER_GLOW_COLOR);

	memgame_print_helper(MSG_GAME_OVER_TEXT,
			MSG_GAME_OVER_POS_X - MSG_GAME_OVER_GLOW_OFFSET,
			MSG_GAME_OVER_POS_Y - MSG_GAME_OVER_GLOW_OFFSET,
			MSG_GAME_OVER_GLOW_COLOR);

	memgame_print_helper(MSG_GAME_OVER_TEXT,
			MSG_GAME_OVER_POS_X + MSG_GAME_OVER_GLOW_OFFSET,
			MSG_GAME_OVER_POS_Y - MSG_GAME_OVER_GLOW_OFFSET,
			MSG_GAME_OVER_GLOW_COLOR);

	memgame_print_helper(MSG_GAME_OVER_TEXT,
			MSG_GAME_OVER_POS_X - MSG_GAME_OVER_GLOW_OFFSET,
			MSG_GAME_OVER_POS_Y + MSG_GAME_OVER_GLOW_OFFSET,
			MSG_GAME_OVER_GLOW_COLOR);

	// Print with no offset.
	memgame_print_helper(MSG_GAME_OVER_TEXT, MSG_GAME_OVER_POS_X,
			MSG_GAME_OVER_POS_Y, MSG_GAME_OVER_COLOR);

	// Restore font scale for the widgets.
	sysfont.scale = WIDGET_FONT_SCALE;
}

/**
 * \brief Print user's number of tries.
 *
 * Creates a new text string that contains the number of tries, and updates the
 * label widget with it.
 */
static void memgame_print_tries(void)
{
	char str[MSG_TRIES_LENGTH + 1];

	// Create new string and update the label.
	snprintf(str, MSG_TRIES_LENGTH + 1, MSG_TRIES_TEXT, game_ctx->tries);
	wtk_label_change(game_ctx->tries_label, str);
}

/**
 * \brief Get the coordinates for specified piece.
 *
 * Computes the X and Y coordinates of the specified piece position in the game
 * board grid.
 *
 * \param position Position of piece.
 * \param pos_x Address to store X coordinate of piece at.
 * \param pos_y Address to store Y coordinate of piece at.
 *
 * \return Coordinates of specified piece \a position stored to \a pos_x and
 * \a pos_y.
 */
static void memgame_get_piece_coordinates(uint8_t position, gfx_coord_t *pos_x,
		gfx_coord_t *pos_y)
{
	*pos_x = BOARD_POS_X + (position % NR_OF_BOARD_COLUMNS) *
			(PIECE_SIZE_X + PIECE_SPACING_X);

	*pos_y = BOARD_POS_Y + (position / NR_OF_BOARD_COLUMNS) *
			(PIECE_SIZE_Y + PIECE_SPACING_Y);
}

/**
 * \brief Enqueue drawing of specified piece.
 *
 * Enqueues loading of the specified game piece's image to screen. If the piece
 * is to be shown, the image file is determined by which pair the piece belongs
 * to. If the piece is not to be shown, a common image for the pieces' backside
 * is loaded.
 *
 * \param index Index of piece.
 * \param show Indicates whether to draw the front (\a true) or back (\a false)
 * of the game piece.
 */
static void memgame_draw_piece(uint8_t index, bool show)
{
	gfx_coord_t            pos_x;
	gfx_coord_t            pos_y;
	char                   *filename;
	enum status_code       result;

	// Get image filename for piece.
	if (show) {
		switch (game_ctx->pieces[index].pair) {
		case 0:
			filename = PIECE_PAIR_FILE_0;
			break;

		case 1:
			filename = PIECE_PAIR_FILE_1;
			break;

		case 2:
			filename = PIECE_PAIR_FILE_2;
			break;

		case 3:
			filename = PIECE_PAIR_FILE_3;
			break;

		case 4:
			filename = PIECE_PAIR_FILE_4;
			break;

		case 5:
			filename = PIECE_PAIR_FILE_5;
			break;

		default:
			filename = PIECE_BACK_FILE;
			break;
		}
	} else {
		filename = PIECE_BACK_FILE;
	}
	memgame_get_piece_coordinates(index, &pos_x, &pos_y);

	// Now load the file to the piece's position.
	result = load_file_to_screen(filename, pos_x, pos_y, PIECE_SIZE_X,
			PIECE_SIZE_Y, game_ctx->task);

	/* If piece could not be loaded, exit the application since only
	 * this game should currently be using the file loader.
	 */
	if (result != STATUS_OK) {
		win_destroy(game_ctx->win);
		memcpy(&sysfont, &game_ctx->old_sysfont, sizeof(struct font));
		membag_free(game_ctx);
		game_ctx = NULL;
		app_desktop_restart();
	};
}

/**
 * \brief Pick a random piece index from array.
 *
 * \param indexes Array containing currently available piece indexes.
 *
 * \return Random entry from \a indexes[], which was not already taken.
 */
static uint8_t memgame_get_random_index(uint8_t indexes[])
{
	uint8_t array_index;
	uint8_t piece_index;

	piece_index = PIECE_INVALID_INDEX;

	// Do a random search for the first free index in the supplied list.
	do {
		array_index = memgame_rand() % NR_OF_PIECES;
		piece_index = indexes[array_index];
	} while (piece_index == PIECE_INVALID_INDEX);

	// Mark the piece position as taken.
	indexes[array_index] = PIECE_INVALID_INDEX;

	return piece_index;
}

/**
 * \brief Start new game.
 *
 * Clears the application window, reinitializes the game state and randomizes
 * the game pieces. The application worker task is then queued to start the
 * game.
 */
static void memgame_start_new_game(void)
{
	struct memgame_piece   *p1;
	struct memgame_piece   *p2;
	uint8_t                indexes[NR_OF_PIECES];
	uint8_t                i;

	// Force redraw of application window.
	win_redraw(game_ctx->win);

	// Initialize list of uninitialized pieces.
	for (i = 0; i < NR_OF_PIECES; i++) {
		indexes[i] = i;
	}

	// Initialize the game piece pairs with random locations.
	for (i = 0; i < NR_OF_PIECE_PAIRS; i++) {
		p1 = &game_ctx->pieces[memgame_get_random_index(indexes)];
		p1->pair = i;
		p1->found = false;

		p2 = &game_ctx->pieces[memgame_get_random_index(indexes)];
		p2->pair = i;
		p2->found = false;
	}

	// Initialize the game state and start drawing all game pieces.
	game_ctx->tries = 0;
	game_ctx->busy = true;
	game_ctx->piece_1 = 0;
	game_ctx->pairs_left = NR_OF_PIECE_PAIRS;
	game_ctx->state = DRAW_ALL_PIECES;
	workqueue_add_task(&main_workqueue, game_ctx->task);
}

/**
 * \brief Update the game.
 *
 * This is the main game function, which handles the piece selection and image
 * loading according to the current state of the game.
 *
 * The game will go through the game states in the order they are enumerated in
 * \ref memgame_state, but may stay in a state if the user does not select a
 * valid game piece or the game must be restarted.
 *
 * This function is called by \ref memgame_worker once the game starts or an
 * image is loaded, and by \ref memgame_handle_pointer_event if the user
 * selects a piece while the game is not busy loading images.
 *
 * \param new_piece Index and position of piece that was selected.
 */
static void memgame_update(uint8_t new_piece)
{
	struct memgame_piece   *pieces = game_ctx->pieces;
	uint8_t                p_1 = game_ctx->piece_1;
	uint8_t                p_2 = game_ctx->piece_2;

	// Act according to current game state.
	switch(game_ctx->state) {
	case DRAW_ALL_PIECES:
		// Draw all pieces, then wait for selection of first piece.
		if (p_1 < NR_OF_PIECES) {
			memgame_draw_piece(p_1, false);
			p_1++;
		} else {
			memgame_print_tries();
			game_ctx->state = SELECT_FIRST_PIECE;
			game_ctx->busy = false;
		}
		game_ctx->piece_1 = p_1;
		break;

	case SELECT_FIRST_PIECE:
		// If piece was not already found, accept selection and draw it.
		if (game_ctx->pieces[new_piece].found == false) {
			game_ctx->piece_1 = new_piece;
			memgame_draw_piece(new_piece, true);
			game_ctx->state = SHOWN_FIRST_PIECE;
			game_ctx->busy = true;
		}
		break;

	case SHOWN_FIRST_PIECE:
		// Drawing finished, now wait for second piece selection.
		game_ctx->state = SELECT_SECOND_PIECE;
		game_ctx->busy = false;
		break;

	case SELECT_SECOND_PIECE:
		/* If selected piece is not the same as the first one and it
		 * has not already been found, accept selection and draw it.
		 */
		if ((new_piece != p_1) && (game_ctx->pieces[new_piece].found
				== false)) {
			game_ctx->piece_2 = new_piece;
			memgame_draw_piece(new_piece, true);
			game_ctx->tries++;
			memgame_print_tries();
			game_ctx->state = SHOWN_SECOND_PIECE;
			game_ctx->busy = true;
		}
		break;

	case SHOWN_SECOND_PIECE:
		// Drawing finished. Check if pieces match.
		if (pieces[p_1].pair == pieces[p_2].pair) {
			// Check if game has completed.
			if (--game_ctx->pairs_left == 0) {
				memgame_print_game_over();
				game_ctx->state = GAME_OVER;
			} else {
				pieces[p_1].found = true;
				pieces[p_2].found = true;
				game_ctx->state = SELECT_FIRST_PIECE;
			}
			game_ctx->busy = false;
		} else {
			// Start timer to pause before hiding the pieces again.
			game_ctx->ticks_to_go = TIMER_PAUSE_HALF_SECONDS;
			timer_start(CONFIG_TIMER_ID, &game_ctx->timer);
			timer_set_alarm(CONFIG_TIMER_ID, &game_ctx->timer,
					game_ctx->timer_delay);
			game_ctx->state = HIDE_FIRST_PIECE;
		}
		break;

	case HIDE_FIRST_PIECE:
		// Timer has expired. Now hide the first piece.
		memgame_draw_piece(p_1, false);
		game_ctx->state = HIDE_SECOND_PIECE;
		break;

	case HIDE_SECOND_PIECE:
		// Now hide the second one.
		memgame_draw_piece(p_2, false);
		game_ctx->state = HIDDEN_BOTH_PIECES;
		break;

	case HIDDEN_BOTH_PIECES:
		// Clear busy flag and wait for new piece selection.
		game_ctx->state = SELECT_FIRST_PIECE;
		game_ctx->busy = false;
		break;

	case GAME_OVER:
		// Do nothing. User must restart the game.
		break;

	default:
		break;
	}
}

/**
 * \brief Command event handler for application.
 *
 * Handles the command events issued by widgets in the application window.
 * Only the exit- and new game-buttons require handling in this application.
 *
 * \param data Command event ID passed from button.
 */
static void memgame_handle_command_event(win_command_t data)
{
	switch ((enum memgame_command_id)(uintptr_t)data) {
	case CMD_NEW_GAME:
		memgame_start_new_game();
		break;

	case CMD_EXIT:
		win_destroy(game_ctx->win);
		memcpy(&sysfont, &game_ctx->old_sysfont, sizeof(struct font));
		membag_free(game_ctx);
		game_ctx = NULL;
		app_desktop_restart();
		break;

	default:
		break;
	}
}

/**
 * \brief Pointer event handler for application.
 *
 * Handles the pointer events within the application window, but not within any
 * of the widgets. The pointer presses are used to select game pieces, and their
 * indexes are passed on to \ref memgame_update.
 *
 * \param data Pointer event data.
 */
static void memgame_handle_pointer_event(struct win_pointer_event const *data)
{
	gfx_coord_t x;
	gfx_coord_t y;
	uint8_t     i;

	// Only handle pointer presses.
	if (data->type != WIN_POINTER_PRESS) {
		return;
	}

	// Find the piece under the pointer, if any.
	for (i = 0; i < NR_OF_PIECES; i++) {
		memgame_get_piece_coordinates(i, &x, &y);

		// If a piece was found, update the game.
		if ((data->pos.x >= x) && (data->pos.y >= y) &&
				(data->pos.x < (x + PIECE_SIZE_X)) &&
				(data->pos.y < (y + PIECE_SIZE_Y))) {
			memgame_update(i);
			break;
		}
	}
}

/**
 * \brief Window event handler for application.
 *
 * Handles all events issued to the application window.
 * Only command and pointer events are relevant to this application, and these
 * are passed on to their respective handler functions if the application is not
 * currently busy loading images.
 *
 * \param win Pointer to the window receiving the event.
 * \param type Indicates the type of the event.
 * \param data Event type-specific data.
 *
 * \retval true To accept all events from the window system.
 */
static bool memgame_window_handler(struct win_window *win,
		enum win_event_type type, void const *data)
{
	// Only process input if application is not currently busy.
	if (!game_ctx->busy) {
		switch (type) {
		case WIN_EVENT_COMMAND:
			memgame_handle_command_event((win_command_t)data);
			break;

		case WIN_EVENT_POINTER:
			memgame_handle_pointer_event(
					(struct win_pointer_event const *)data);
			break;

		default:
			break;
		}
	}

	return true;
}

/**
 * \brief Application task worker.
 *
 * This is enqueued by \ref appsutil_fileloader_group after images have loaded
 * or by \ref memgame_timer_callback when the application alarm timer expires,
 * and will simply update the game state.
 *
 * \param task Workqueue task for this worker function.
 */
static void memgame_worker(struct workqueue_task *task)
{
	memgame_update(PIECE_INVALID_INDEX);
}

/**
 * \brief Timer callback function.
 *
 * Callback function used with the \ref timer_group "timer driver" for
 * delayed queueing of the application's workqueue task.
 *
 * \param timer Pointer to timer struct associated with the callback.
 */
static void memgame_timer_callback(struct timer *timer)
{
	if (--game_ctx->ticks_to_go > 0) {
		timer_set_alarm(CONFIG_TIMER_ID, &game_ctx->timer,
				game_ctx->timer_delay);
	} else {
		timer_stop(CONFIG_TIMER_ID, &game_ctx->timer);
		workqueue_add_task(&main_workqueue, game_ctx->task);
	}
}

/**
 * \brief Memory game application launcher.
 *
 * Allocates memory for the application context, initializes the timer and
 * creates the window and button widgets before starting a new game.
 *
 * If memory cannot be allocated, or creation of the window or any widget
 * fails, the application exits immediately.
 *
 * \param task Workqueue task to use for this application.
 */
void app_memgame_launch(struct workqueue_task *task)
{
	struct win_attributes  attr;
	struct win_window      *win;
	struct wtk_button      *button;
	struct wtk_label       *label;
	struct timer           *timer;
	timer_res_t            timer_res;
	uint32_t               timer_clk;
	gfx_coord_t            width = gfx_get_width();
	gfx_coord_t            height = gfx_get_height();

	// Clear the screen right away.
#ifdef CONFIG_GFX_USE_CLIPPING
	gfx_set_clipping(0, 0, width, height);
#endif /* CONFIG_GFX_USE_CLIPPING */
	gfx_draw_filled_rect(0, 0, width, height, BACKGROUND_COLOR);

	// Allocate the application context.
	game_ctx = membag_alloc(sizeof(struct memgame_context));
	if(!game_ctx){
		goto exit_no_context;
	}

	assert(task);
	workqueue_task_set_work_func(task, memgame_worker);
	game_ctx->task = task;

	// Use larger sysfont for this app.
	memcpy(&game_ctx->old_sysfont, &sysfont, sizeof(struct font));
	sysfont.scale = WIDGET_FONT_SCALE;

	// Initialize the timer with the configured resolution.
	timer = &game_ctx->timer;
	timer_init(CONFIG_TIMER_ID, timer, memgame_timer_callback);
	timer_res = timer_set_resolution(CONFIG_TIMER_ID, timer,
			TIMER_CLOCK_RATE);
	timer_write_resolution(CONFIG_TIMER_ID, timer, timer_res);
	timer_clk = timer_get_resolution(CONFIG_TIMER_ID, timer, timer_res);

	assert(timer_clk <= (uint32_t)0x1ffff);
	game_ctx->timer_delay = timer_clk / 2;

	// Set up bitmap for window background.
	game_ctx->bitmap.type = BITMAP_SOLID;
	game_ctx->bitmap.data.color = BACKGROUND_COLOR;

	// Create main application window.
	attr.area.pos.x = 0;
	attr.area.pos.y = 0;
	attr.area.size.x = width;
	attr.area.size.y = height;
	attr.background = &game_ctx->bitmap;
	attr.event_handler = memgame_window_handler;
	win = win_create(win_get_root(), &attr);
	if (!win) {
		goto exit_no_window;
	}
	game_ctx->win = win;

	// Create standard exit-button.
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

	// Create new game-button.
	attr.area.pos.x = BTN_NEW_GAME_POS_X;
	attr.area.pos.y = BTN_NEW_GAME_POS_Y;
	attr.area.size.x = BTN_NEW_GAME_SIZE_X;
	attr.area.size.y = BTN_NEW_GAME_SIZE_Y;
	button = wtk_button_create(win, &attr.area, BTN_NEW_GAME_TEXT,
			(win_command_t)CMD_NEW_GAME);
	if (!button) {
		goto exit_no_widget;
	}
	win_show(wtk_button_as_child(button));

	// Create label to show number of tries.
	attr.area.pos.x = MSG_TRIES_POS_X;
	attr.area.pos.y = MSG_TRIES_POS_Y;
	attr.area.size.x = MSG_TRIES_SIZE_X;
	attr.area.size.y = MSG_TRIES_SIZE_Y;
	label = wtk_label_create(win, &attr.area, " ", false);
	if (!label) {
		goto exit_no_widget;
	}
	game_ctx->tries_label = label;
	win_show(wtk_label_as_child(label));

	// Show the application window and start a new game.
	win_show(win);
	memgame_start_new_game();
	return;

	// Handle memory allocation errors.
exit_no_widget:
	win_destroy(game_ctx->win);
exit_no_window:
	memcpy(&sysfont, &game_ctx->old_sysfont, sizeof(struct font));
	membag_free(game_ctx);
	game_ctx = NULL;
exit_no_context:
	app_desktop_restart();
	return;
}

//! @}

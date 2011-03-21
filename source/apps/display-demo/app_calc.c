/**
 * \file
 *
 * \brief Display demo calc application
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

#include <led.h>
#include <board.h>
#include <string.h>

#include <gfx/gfx.h>
#include <gfx/win.h>
#include <gfx/wtk.h>
#include <gfx/sysfont.h>
#include <membag.h>
#include <debug.h>
#include <stream.h>
#include <mainloop.h>

#include "app_desktop.h"
#include "app_calc.h"

/**
 * \weakgroup app_calc_group
 *
 * @{
 */

/**
 * \name Keypad configuration
 *
 * @{
 */

//! Start position for keypad
#define POS_X        10
//! Start position for keypad
#define POS_Y        50
//! Size of each key
#define SIZE_X       45
//! Size of each key
#define SIZE_Y       35
//! Size of each key including space to next key
#define SPACE_X      (SIZE_X + 10)
//! Size of each key including space to next key
#define SPACE_Y      (SIZE_Y + 10)
//! Number of keys pr row
#define KEYS_PR_ROW  4

//! @}

/**
 * \name Calculator display configuration
 *
 * @{
 */

//! Max value to display
#define MAX_VALUE    999999
//! Max digits needed to store display value and terminating null byte
#define MAX_DIGITS   (10+1)

//! @}

/**
 * Command event IDs
 *
 * \note The widgets are given command event IDs starting at 123 to avoid values
 * in the ASCII character code range used for the keypad button widgets.
 *
 * \note The command IDs cannot be 0, since this value is reserved for no
 * command event callback for certain widgets.
 */
enum command_id {
	//! Command event ID for DEC radio button
	RADIO_DEC_ID = 123,
	//! Command event ID for HEX radio button
	RADIO_HEX_ID,
	//! Command event ID for exit button
	BUTTON_EXIT_ID,
};

//! The calculator modes
enum calc_mode {
	//! Calculator is in decimal mode
	CALC_DEC,
	//! Calculator is in hexadecimal mode
	CALC_HEX,
};

//! Calculator application context
struct calc_context {
	//! Pointer to application frame
	struct wtk_basic_frame *frame;
	//! Pointer to display label for showing calc result
	struct wtk_label       *display_lbl;
	//! Copy of the sysfont before it is modified by the application
	struct font            old_sysfont;
	//! Background bitmap for application
	struct gfx_bitmap      background;
	//! Background bitmap for display label
	struct gfx_bitmap      label_background;

	//! Buffer for converting text to show on calc display
	char                   text[MAX_DIGITS];
	//! Current value in stack
	int32_t                stack_value;
	//! Current value on display
	int32_t                disp_value;
	//! Current operator (+,-,*,/) to use when = is pressed
	char                   operator;
	//! Mode selected for displaying the result
	enum calc_mode         mode;
};

/**
 * \brief Update calculator display
 *
 * Update the display of the calculator with the new value, in the selected
 * number base.
 *
 * \param calc Context for calculator
 */
static void calc_update_display(struct calc_context *calc)
{
	assert(calc);

	// Generate string according to the selected number base.
	switch (calc->mode) {
	case CALC_HEX:
		snprintf(calc->text, MAX_DIGITS, "0x%lx", calc->disp_value);
		break;
	default:
		snprintf(calc->text, MAX_DIGITS, "%ld", calc->disp_value);
		break;
	}

	wtk_label_change(calc->display_lbl, calc->text);
}

/**
 * \brief Frame handler for calculator
 *
 * Handle all the command events of the calculator, i.e., the keypad and radio
 * buttons.
 *
 * \sa wtk_basic_frame_command_handler_t
 *
 * \param frame Pointer to the application frame
 * \param command_data The command event ID
 */
static bool calc_frame_command_handler(struct wtk_basic_frame *frame,
		win_command_t command_data)
{
	struct calc_context *calc;
	char command;

	calc = wtk_basic_frame_get_custom_data(frame);

	command = (char)(uintptr_t)command_data;
	switch (command) {
	// Multiply current display value by 10, and add new number.
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		if (calc->disp_value < MAX_VALUE) {
			calc->disp_value *= 10;
			calc->disp_value += command - '0';
			calc_update_display(calc);
		}

		break;

	// Change the arithmetic operator to use when '=' is pressed.
	case '+':
	case '-':
	case '/':
	case '*':
		if (calc->disp_value) {
			calc->stack_value = calc->disp_value;
			calc->disp_value = 0;
			calc->operator = command;

			calc_update_display(calc);
		}
		break;

	// Compute according to selected operator, display the result.
	case '=':
		switch (calc->operator) {
		case '+':
			calc->disp_value = calc->stack_value + calc->disp_value;
			break;

		case '-':
			calc->disp_value = calc->stack_value - calc->disp_value;
			break;

		case '*':
			calc->disp_value = calc->stack_value * calc->disp_value;
			break;

		case '/':
			if (calc->disp_value != 0) {
				calc->disp_value = calc->stack_value / calc->disp_value;
			}
			break;

		default:
			break;
		}
		calc->operator = 0;
		// Saturate value
		if (calc->disp_value > 0 ) {
			calc->disp_value = min_s(MAX_VALUE, calc->disp_value);
		} else {
			calc->disp_value = max_s(-MAX_VALUE, calc->disp_value);
		}
		calc_update_display(calc);
		break;

	// Clear all entered values.
	case 'C':
		calc->disp_value = 0;
		calc->stack_value = 0;
		calc_update_display(calc);
		break;

	// Switch to base 10 (decimal) number representation.
	case RADIO_DEC_ID:
		calc->mode = CALC_DEC;
		calc_update_display(calc);
		break;

	// Switch to base 16 (hexdecimal) number representation.
	case RADIO_HEX_ID:
		calc->mode = CALC_HEX;
		calc_update_display(calc);
		break;

	// Exit the application.
	case BUTTON_EXIT_ID:
		// Restore sysfont
		memcpy(&sysfont, &calc->old_sysfont, sizeof(struct font));
		// Restart desktop
		app_desktop_restart();
		// Free memory
		membag_free(calc);
		// Close the application
		return true;
	}

	return false;
}

/**
 * \brief Start calculator app
 *
 * Allocates the application context and creates all widgets for it.
 *
 * The command event IDs of the keypad button widgets correspond to ASCII
 * character codes that correspond to their value or function.
 *
 * If memory allocation or widget creation fails, the application exits
 * immediately.
 *
 * \param task Workqueue task used to start the calculator.
 */
void app_calc_launch(struct workqueue_task *task)
{
	struct wtk_radio_group  *rg;
	struct wtk_radio_button *rb;
	struct wtk_basic_frame  *lframe;
	struct win_window       *parent;
	struct win_area         area;
	struct wtk_button       *btn;
	struct calc_context     *calc;
	uint8_t                 i;
	char                    caption[2] = "0";
	char                    keys[] = {
		'7','8','9','+',
		'4','5','6','-',
		'3','2','1','*',
		'0','C','=','/',
	};

	calc = membag_alloc(sizeof(struct calc_context));
	if (!calc) {
		goto error_ctx;
	}

	// Use larger sysfont for this app
	memcpy(&calc->old_sysfont, &sysfont, sizeof(struct font));
	sysfont.scale = 2;

	calc->mode = CALC_DEC;
	calc->disp_value = 0;
	calc->stack_value = 0;

	// Create main calculator window
	calc->background.type = BITMAP_SOLID;
	calc->background.data.color = GFX_COLOR(0, 0, 0);

	area.pos.x = 0;
	area.pos.y = 0;
	area.size.x = gfx_get_width();
	area.size.y = gfx_get_height();

	calc->frame = wtk_basic_frame_create(win_get_root(), &area,
			&calc->background, NULL, calc_frame_command_handler,
			calc);
	if (!calc->frame) {
		goto error_frame;
	}

	parent = wtk_basic_frame_as_child(calc->frame);
	win_show(parent);

	// Create background for label
	calc->label_background.type = BITMAP_SOLID;
	calc->label_background.data.color = WTK_BUTTON_BACKGROUND_COLOR;

	area.pos.x = POS_X;
	area.pos.y = 10;
	area.size.x = SPACE_X * KEYS_PR_ROW - 10;
	area.size.y = 30;

	lframe = wtk_basic_frame_create(parent, &area, &calc->label_background,
			NULL, NULL, NULL);
	if (!lframe) {
		goto error_widget;
	}
	win_show(wtk_basic_frame_as_child(lframe));

	// Create label for result
	area.pos.x = 5;
	area.pos.y = 10;
	area.size.x -= area.pos.x + 20;
	area.size.x -= area.pos.y;
	calc->display_lbl = wtk_label_create(wtk_basic_frame_as_child(lframe),
			&area, "0", true);
	if (!calc->display_lbl) {
		goto error_widget;
	}
	win_show(wtk_label_as_child(calc->display_lbl));

	// Create buttons
	area.pos.x = POS_X;
	area.pos.y = POS_Y;
	area.size.x = SIZE_X;
	area.size.y = SIZE_Y;
	for (i = 0; i < ARRAY_LEN(keys); i++) {
		caption[0] = keys[i];
		btn = wtk_button_create(parent, &area, caption,
				(win_command_t)(uintptr_t)caption[0]);
		if (!btn) {
			goto error_widget;
		}
		win_show(wtk_button_as_child(btn));

		if (((i + 1) % KEYS_PR_ROW) == 0) {
			area.pos.x = POS_X;
			area.pos.y += SPACE_Y;
		} else {
			area.pos.x += SPACE_X;
		}
	}

	// Create value mode radio buttons
	rg = wtk_radio_group_create();

	area.pos.x = SPACE_X * KEYS_PR_ROW + 10;
	area.pos.y = POS_Y + 20;
	area.size.x = 60;
	area.size.y = 30;

	rb = wtk_radio_button_create(parent, &area, "Dec", true, rg,
			(win_command_t)RADIO_DEC_ID);
	if (!rb) {
		goto error_widget;
	}
	win_show(wtk_radio_button_as_child(rb));

	area.pos.y += area.size.y + 10;
	rb = wtk_radio_button_create(parent, &area, "Hex", false, rg,
			(win_command_t)RADIO_HEX_ID);
	if (!rb) {
		goto error_widget;
	}
	win_show(wtk_radio_button_as_child(rb));

	// Create exit button
	area.size.x = APP_EXIT_BUTTON_SIZE_X;
	area.size.y = APP_EXIT_BUTTON_SIZE_Y;
	area.pos.x  = APP_EXIT_BUTTON_POS_X;
	area.pos.y  = APP_EXIT_BUTTON_POS_Y;

	btn = wtk_button_create(parent, &area, APP_EXIT_BUTTON_TEXT,
			(win_command_t)BUTTON_EXIT_ID);
	if (!btn) {
		goto error_widget;
	}
	win_show(wtk_button_as_child(btn));

	return;

error_widget:
	win_destroy(wtk_basic_frame_as_child(calc->frame));
error_frame:
	memcpy(&sysfont, &calc->old_sysfont, sizeof(struct font));
	membag_free(calc);
error_ctx:
	app_desktop_restart();
}

//! @}

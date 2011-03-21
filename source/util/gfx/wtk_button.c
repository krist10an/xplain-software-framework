/**
 * \file
 *
 * \brief Button widget implementation
 *
 * Copyright (C) 2009 - 2010 Atmel Corporation. All rights reserved.
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
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <membag.h>
#include <string.h>
#include <gfx/wtk.h>

/**
 * \ingroup gfx_wtk_button
 * @{
 */

/** Valid states for button, relevant to pointer and keyboard operations.
 * \internal
 */
enum wtk_button_state {
	//! Normal state, no highlight or press.
	WTK_BUTTON_NORMAL,
	//! Widget pressed, action occurs when released.
	WTK_BUTTON_PRESSED,
};

/**
 * This is the button control struct. It contains references to the windows
 * composing the button, as well as non-window information required.
 *
 * \internal
 * \note Structure fields are only internally accessible, only pointers to
 *       structure is needed externally.
 */
struct wtk_button {
	//! Container, ultimate widget parent.
	struct win_window       *container;
	//! Copy of caption string.
	char                    *caption;
	//! Custom command data, used when "clicked".
	win_command_t           command_data;
	//! Current state.
	enum wtk_button_state   state;
};

/**
 * This function returns a reference to the window that should be used when
 * managing the widget, such as move, resize, destroy and reparenting.
 *
 * \param button Button widget to manage.
 *
 * \return Window to be used for managing the button.
 */
struct win_window *wtk_button_as_child(struct wtk_button *button)
{
	assert(button);
	return button->container;
}

/**
 * This function is the window event handler for button widgets.
 * It handles all events sent to the windows composing the widget.
 *
 * \param win Window receiving the event.
 * \param type The event type.
 * \param data Custom data, depending on event type.
 *
 * \return True if the event was recognized and accepted.
 */
static bool wtk_button_handler(struct win_window *win,
		enum win_event_type type, void const *data)
{
	// Custom data for windows of a widget points back to the widget itself.
	struct wtk_button *button =
			(struct wtk_button *)win_get_custom_data(win);

	switch (type) {
	case WIN_EVENT_DRAW:
		{
			// For DRAW events, the data parameter points to the
			// clipping region.
			struct win_clip_region const *clip =
					(struct win_clip_region const *)data;
			struct win_area const *area = win_get_area(win);

			// Preare background and caption colors depending on state of
			// button. If pressed/highlighted, the colors are inverted.
			gfx_color_t background_color;
			gfx_color_t caption_color;

			// There should not be other windows in this widget.
			assert(win == button->container);

			switch (button->state) {
			case WTK_BUTTON_NORMAL:
				background_color = WTK_BUTTON_BACKGROUND_COLOR;
				caption_color = WTK_BUTTON_CAPTION_COLOR;
				break;

			case WTK_BUTTON_PRESSED:
				background_color = WTK_BUTTON_CAPTION_COLOR;
				caption_color = WTK_BUTTON_BACKGROUND_COLOR;
				break;

			default:
				assert(false);
				background_color = WTK_BUTTON_BACKGROUND_COLOR;
				caption_color = WTK_BUTTON_CAPTION_COLOR;
			}

			// Draw background.
			gfx_draw_filled_rect(clip->origin.x, clip->origin.y,
					area->size.x, area->size.y,
					background_color);

			// Draw border.
			gfx_draw_rect(clip->origin.x, clip->origin.y,
					area->size.x, area->size.y,
					WTK_BUTTON_BORDER_COLOR);

			/* Get string size and draw the caption text in the
			 * center of the button.
			 */
			gfx_coord_t width;
			gfx_coord_t height;

			gfx_get_string_bounding_box(button->caption, &sysfont,
					&width, &height);

			gfx_draw_string(button->caption,
					clip->origin.x + (area->size.x / 2) -
					(width / 2),
					clip->origin.y + (area->size.y / 2) -
					(height / 2), &sysfont, caption_color,
					GFX_COLOR_TRANSPARENT);

			/* Always accept DRAW events, as the return value is
			 * ignored anyway for that event type.
			 */
			return true;
		}

	case WIN_EVENT_POINTER:
		{
			// There should not be other windows in this widget.
			assert(win == button->container);

			/* For POINTER events, the data parameter points to the
			 * pointer event information.
			 */
			struct win_pointer_event const *event =
					(struct win_pointer_event const *)data;

			switch (event->type) {
			case WIN_POINTER_PRESS:
				/* When button pressed, grab pointer and wait
				 * for release inside button borders. Other
				 * widgets won't get pointer events before it
				 * is released, and the pointer ungrabbed by
				 * us.
				 */
				if (button->state == WTK_BUTTON_NORMAL) {
					win_grab_pointer(button->container);
					button->state = WTK_BUTTON_PRESSED;
					win_redraw(button->container);
				}
				break;

			case WIN_POINTER_RELEASE:
				/* When button released, take action only if
				 * released inside widget extents.
				 */
				if (button->state == WTK_BUTTON_PRESSED) {
					// Ungrab pointer.
					win_grab_pointer(NULL);
					button->state = WTK_BUTTON_NORMAL;
					win_redraw(button->container);

					// Check release position.
					bool isInside = win_is_inside_window
							(button->container,
							&(event->pos));

					// Send command event if inside.
					if (isInside) {
						struct win_command_event
								command;
						command.sender = button->
								container;
						command.recipient =
								button->
								container;
						command.data = button->
								command_data;
						win_queue_command_event
								(&command);
					}
				}
				break;

			default:
				break;
			}

			/* Accept all POINTER events since all acitivity inside
			 * the widget extents is relevant to us.
			 */
			return true;
		}

	case WIN_EVENT_DESTROY:
		{
			// There should not be other windows in this widget.
			assert(win == button->container);

			/* Memory allocated for windows will be automatically
			 * destroyed by the window system. We must destroy
			 * other allocations.
			 */
			membag_free(button->caption);
			membag_free(button);

			/* Always accept DESTROY events, as the return value is
			 * ignored anyway for that event type.
			 */
			return true;
		}

	default:
		// Reject unknown event types.
		return false;
	}
}

/**
 * \brief Get minimum widget size
 *
 * This function returns the minimum size that is required for showing
 * the full widget and the caption.
 *
 * \retval size   Suggested size for the widget
 * \param caption Pointer to caption string to use for widget.
 */
void wtk_button_size_hint(struct win_point *size, const char *caption)
{
	assert(size);
	assert(caption);

	gfx_get_string_bounding_box(caption, &sysfont, &size->x, &size->y);
	// Add 2 pixels for the button border
	size->x += 2;
	size->y += 2;
}


/**
 * This function creates a new button widget. It allocates required memory and
 * intializes necessary windows to create the widget. If there is not enough
 * memory, the function returns NULL.
 *
 * To destroy the widget and all its contents, and free its memory, call
 * win_destroy() on the button's child reference, given by wtk_button_as_child(),
 * like this: "win_destroy(wtk_button_as_child(myButtonPtr));".
 * Usually it will be destroyed automatically when it's parent is destroyed.
 *
 * \param parent Parent window, possibly wtk_frame_as_parent(myFramePtr).
 * \param area Area of the internal contents.
 * \param caption Pointer to caption string. Will be copied into widget.
 * \param command_data Custom data to put into a command event when "clicked".
 *                     Must be non-zero.
 *
 * \return Pointer to button, or NULL if failed.
 */
struct wtk_button *wtk_button_create(struct win_window *parent,
		struct win_area const *area,
		char const *caption, win_command_t command_data)
{
	struct win_attributes   attr;
	struct wtk_button       *button;

	assert(area);
	assert(caption);
	assert(parent);
	assert(command_data);

	// Allocate memory for button control data.
	button = membag_alloc(sizeof(struct wtk_button));
	if (!button) {
		goto outofmem_button;
	}

	button->state = WTK_BUTTON_NORMAL;
	button->command_data = command_data;

	// Allocate memory for caption string, and copy text.
	button->caption = membag_alloc((strlen(caption) + 1) * sizeof(char));
	if (!button->caption) {
		goto outofmem_caption;
	}

	wtk_copy_string(button->caption, caption);

	// Handling information.
	attr.event_handler = wtk_button_handler;
	attr.custom = button;

	// Prepare container frame.
	attr.area = *area;
	attr.background = NULL;
	attr.behavior = 0x00;

	button->container = win_create(parent, &attr);
	if (!button->container) {
		goto outofmem_container;
	}

	return button;

outofmem_container:
	membag_free(button->caption);

outofmem_caption:
	membag_free(button);

outofmem_button:
	return NULL;
}

//! @}

/**
 * \file
 *
 * \brief Graphical Widget toolkit
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
#ifndef WTK_H_INCLUDED
#define WTK_H_INCLUDED

#include <app/wtk.h>
#include <gfx/win.h>
#include <gfx/sysfont.h>

/**
 * \ingroup gfx
 * \defgroup gfx_wtk Widget toolkit
 *
 * The Widget toolkit implements many common widgets for use on a graphical
 * display using the functionality of \ref gfx_win "Window system"
 * and \ref gfx_gfx "Graphics library"
 *
 * The following widgets are implemented:
 * - \ref gfx_wtk_frame
 * - \ref gfx_wtk_button
 * - \ref gfx_wtk_check_box
 * - \ref gfx_wtk_progress_bar
 * - \ref gfx_wtk_radio_button
 * - \ref gfx_wtk_slider
 * - \ref gfx_wtk_label
 * - \ref gfx_wtk_gauge
 * - \ref gfx_wtk_trigtable
 * @{
 */

/**
 * \defgroup gfx_wtk_frame Frame widget
 *
 * Frame widget provides a frame with border and a title bar. It can be
 * configured to be resizable and movable.
 * @{
 */

struct wtk_frame;

//! Frame command event handler callback
typedef bool(*wtk_frame_handler_t) (struct wtk_frame * frame,
		win_command_t command_data);

struct wtk_frame *wtk_frame_create(struct win_window *parent,
		struct win_area const *area,
		char const *caption,
		bool allow_resize,
		wtk_frame_handler_t frame_handler, void *custom_data);
struct win_window *wtk_frame_as_parent(struct wtk_frame *frame);
struct win_window *wtk_frame_as_child(struct wtk_frame *frame);
void *wtk_frame_get_custom_data(struct wtk_frame const *frame);

//! @}

/**
 * \defgroup gfx_wtk_basic_frame Basic frame widget
 *
 * Basic frame widget provides a frame with only a background color.
 * @{
 */

struct wtk_basic_frame;

/**
 * \brief Basic frame command event handler callback
 *
 * The basic frame command event handler callback is used to handle
 * command events from widgets inside the basic frame. Command events
 * are typically queued when the user has given some input to a widget.
 * Examples are pushing a button widget or checking a check box widget.
 *
 * \note The maximum stack usage can be affected by the implementation of the
 *       event handler. The handler will be called from within the event
 *       handling of the \ref gfx_win "window system". This event handling
 *       is done recursively to traverse all child windows connected to the
 *       \ref win_root "window root".
 *
 * \param basic_frame  Pointer to the basic frame which contains the widget that
 *                     generated the event.
 * \param command_data The command data that was set for the specific widget.
 *                     This is typically an ID for the widget or a pointer to
 *                     data needed by the widget. This is widget-specific.
 * \return             True if the basic frame and it's children should be
 *                     destroyed, false otherwise.
 */
typedef bool(*wtk_basic_frame_command_handler_t) (
		struct wtk_basic_frame *basic_frame, win_command_t command_data);

/**
 * \brief Basic frame draw event handler callback
 *
 * The basic frame draw event handler callback can be used to draw simple
 * graphical display elements without the need for implementing a custom
 * widget.
 * The handler is called to redraw the full or a subregion of basic frame
 * after the background for the widget has been drawn.
 *
 * If \ref CONFIG_GFX_USE_CLIPPING has been set the clipping region is set
 * before the handler is called.
 *
 * \note The maximum stack usage can be affected by the implementation of the
 *       event handler. The handler will be called from within the event
 *       handling of the \ref gfx_win "window system". This event handling
 *       is done recursively to traverse all child windows connected to the
 *       \ref win_root "window root".
 *
 * \param win  Pointer to the window of the basic frame widget to be drawn.
 * \param clip Region of the basic frame to be drawn in global display
 *             coordinates.
 */
typedef void(*wtk_basic_frame_draw_handler_t) (
		struct win_window *win, struct win_clip_region const *clip);

struct wtk_basic_frame *wtk_basic_frame_create(struct win_window *parent,
		const struct win_area *area, struct gfx_bitmap *background,
		wtk_basic_frame_draw_handler_t draw_handler,
		wtk_basic_frame_command_handler_t frame_handler, void *custom_data);
struct win_window *wtk_basic_frame_as_child(
		struct wtk_basic_frame *basic_frame);
void *wtk_basic_frame_get_custom_data(
		const struct wtk_basic_frame *basic_frame);

//! @}


/**
 * \defgroup gfx_wtk_button Button widget
 * @{
 */

struct wtk_button;

struct wtk_button *wtk_button_create(struct win_window *parent,
		struct win_area const *area,
		char const *caption, win_command_t command_data);
struct win_window *wtk_button_as_child(struct wtk_button *button);
void wtk_button_size_hint(struct win_point *size, const char *caption);

//! @}


/**
 * \defgroup gfx_wtk_check_box Check box widget
 * @{
 */

struct wtk_check_box;

struct wtk_check_box *wtk_check_box_create(struct win_window *parent,
		struct win_area const *area,
		char const *caption, bool selected, win_command_t command_data);
struct win_window *wtk_check_box_as_child(struct wtk_check_box *check_box);
void wtk_check_box_toggle(struct wtk_check_box *check_box);
void wtk_check_box_set(struct wtk_check_box *check_box, bool selected);
bool wtk_check_box_is_selected(struct wtk_check_box const *check_box);
void wtk_check_box_size_hint(struct win_point *size, const char *caption);

//! @}


/**
 * \defgroup gfx_wtk_radio_button Radio button widget
 * @{
 */

struct wtk_radio_group;
struct wtk_radio_group *wtk_radio_group_create(void);

struct wtk_radio_button;

struct wtk_radio_button *wtk_radio_button_create(struct win_window *parent,
		struct win_area const *area, char const *caption, bool selected,
		struct wtk_radio_group *group, win_command_t command_data);
struct win_window *wtk_radio_button_as_child(struct wtk_radio_button
		*radio_button);
void wtk_radio_button_select(struct wtk_radio_button *radio_button);
bool wtk_radio_button_is_selected(struct wtk_radio_button const *radio_button);
void wtk_radio_button_size_hint(struct win_point *size, const char *caption);

//! @}


/**
 * \defgroup gfx_wtk_slider Slider widget
 * @{
 */

/**
 * \defgroup gfx_wtk_slider_options Slider widget options
 * These options can be ORed together to specify the behaviour of a
 * slider widget when creating it with \ref wtk_slider_create
 * @{
 */

/**
 * \name Slider widget command event configuration options.
 * For use with the option parameter of \ref wtk_slider_create
 * @{
 */

//! Never issue command events. (Default option)
#define WTK_SLIDER_CMD_NONE             (0 << 0)
//! Issue command event whenever slider value is updated.
#define WTK_SLIDER_CMD_MOVE             (1 << 0)
//! Issue command event whenever slider is released.
#define WTK_SLIDER_CMD_RELEASE          (1 << 1)

//! @}

/**
 * \name Slider widget orientation configuration options.
 * For use with the option parameter of \ref wtk_slider_create
 * @{
 */
//! Slider is horizontally oriented. (Default option)
#define WTK_SLIDER_HORIZONTAL           (0 << 0)
//! Slider is vertically oriented.
#define WTK_SLIDER_VERTICAL             (1 << 2)

//! @}

/**
 * \name Slider widget invert configuration options.
 * For use with the option parameter of \ref wtk_slider_create
 * @{
 */
//! Slider value is inverted.
#define WTK_SLIDER_INVERT               (1 << 3)

//! @}

struct wtk_slider;

struct wtk_slider *wtk_slider_create(struct win_window *parent,
		struct win_area const *area, uint8_t maximum, uint8_t value,
		uint8_t option, win_command_t command_data);
struct win_window *wtk_slider_as_child(struct wtk_slider *slider);
bool wtk_slider_is_moving(struct wtk_slider const *slider);
uint8_t wtk_slider_get_value(struct wtk_slider const *slider);
bool wtk_slider_set_value(struct wtk_slider *slider, uint8_t value);

//! @}

/**
 * \defgroup gfx_wtk_label Label widget
 * @{
 */

struct wtk_label;

struct wtk_label *wtk_label_create(struct win_window *parent,
		struct win_area const *area,
		char const *caption, bool align_right);
struct win_window *wtk_label_as_child(struct wtk_label *label);
bool wtk_label_change(struct wtk_label *label, const char *caption);
void wtk_label_size_hint(struct win_point *size, const char *caption);

//! @}


/**
 * \defgroup gfx_wtk_progress_bar Progress bar widget
 * @{
 */

/**
 * \defgroup gfx_wtk_progress_bar_options Progress bar widget options
 * These options can be ORed together to specify the behaviour of a
 * progress bar widget when creating it with \ref wtk_progress_bar_create
 * @{
 */

/**
 * \name Progress bar orientation configuration options.
 * For use with the option parameter of \ref wtk_progress_bar_create
 * @{
 */

//! Progress bar is horizontally oriented.
#define WTK_PROGRESS_BAR_HORIZONTAL    (0 << 0)
//! Progress bar is vertically oriented.
#define WTK_PROGRESS_BAR_VERTICAL      (1 << 0)

//! @}

/**
 * \name Progress bar widget invert configuration options.
 * For use with the option parameter of \ref wtk_progress_bar_create
 * @{
 */
//! Progress bar is inverted.
#define WTK_PROGRESS_BAR_INVERT        (1 << 1)

//! @}

//! @}

struct wtk_progress_bar;

struct wtk_progress_bar *wtk_progress_bar_create(struct win_window *parent,
		struct win_area const *area, uint8_t maximum, uint8_t value,
		gfx_color_t fill_color, gfx_color_t background_color,
		uint8_t option);
struct win_window *wtk_progress_bar_as_child(struct wtk_progress_bar *bar);
uint8_t wtk_progress_bar_get_value(struct wtk_progress_bar *bar);
bool wtk_progress_bar_set_value(struct wtk_progress_bar *bar, uint8_t value);
void wtk_progress_bar_set_colors(struct wtk_progress_bar *bar,
		gfx_color_t color1, gfx_color_t color2);
		
//! @}

//! @}
/**
 * \defgroup gfx_wtk_gauge Gauge widget
 *
 * Gauge widget provides a gauge. Size and position can be configured.
 * Can take input from userdefinable input.
 * @{
 */


//! Inverts the gauge traveling direction
#define WTK_GAUGE_INVERT        (1 << 1)
//! Unused: for drawing a normal 90deg gauge
#define WTK_GAUGE_NORMAL        (0 << 2)
//! Unused: for drawing a 360deg gauge
#define WTK_GAUGE_CIRCLE        (1 << 2)

//! Enables the use of thick indicator lines, comment out for thinner line.
#define CONFIG_WTK_GAUGE_USE_THICK_LINE

struct wtk_gauge;

struct wtk_gauge *wtk_gauge_create(struct win_window *parent,
		struct win_area const *area, struct gfx_bitmap *background, uint8_t maximum, uint8_t value, uint8_t g_outer_pos, uint8_t g_inner_pos,
		gfx_color_t fill_color, gfx_color_t background_color, gfx_color_t parent_background_color,
		uint8_t option);
struct win_window *wtk_gauge_as_child(struct wtk_gauge *gauge);
uint8_t wtk_gauge_get_value(struct wtk_gauge *gauge);
uint8_t wtk_trigtable_sin(uint8_t angle);
uint8_t wtk_trigtable_cos(uint8_t angle);
uint8_t wtk_gauge_get_test(struct wtk_gauge *gauge);
bool wtk_gauge_set_value(struct wtk_gauge *gauge, uint8_t value);
void wtk_gauge_set_colors(struct wtk_gauge *gauge,
		gfx_color_t color1, gfx_color_t color2);

//! @}


uint8_t wtk_rescale_value(uint8_t value, uint8_t from_scale, uint8_t to_scale);
void wtk_copy_string(char *dest, char const *source);

void wtk_shade_pixel(gfx_color_t *pixel, uint8_t alpha);
 
 /**
 * \defgroup gfx_wtk_trigtable Trigonometric sine table
 *
 * Contains a table holding trigonometric sine values from zero to ninety
 * degrees. Useful for drawing i.e. circles. 
 *
 * Contains two different functions for getting sin- and cos-values
 *
 * Best suited for small drawings, as the resolution of the table is quite low
 * @{
 */


//! @}

#endif /* WTK_H_INCLUDED */

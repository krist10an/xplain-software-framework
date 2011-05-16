/**
 * \file
 *
 * \brief Default configurations for widget toolkit
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

#ifndef DEFAULT_WTK_H_INCLUDED
#define DEFAULT_WTK_H_INCLUDED

/**
 * \weakgroup gfx_wtk
 * @{
 */

#include <stdint.h>
#include <gfx/gfx.h>

//! Datatype holding size of strings.
typedef uint16_t wtk_string_size_t;

/**
 * \ingroup gfx_wtk_frame
 * @{
 */

#ifndef WTK_FRAME_LEFTBORDER
//! Left frame border size.
# define WTK_FRAME_LEFTBORDER    2
#endif

#ifndef WTK_FRAME_RIGHTBORDER
//! Right frame border size.
# define WTK_FRAME_RIGHTBORDER   2
#endif

#ifndef WTK_FRAME_TOPBORDER
//! Top frame border size.
# define WTK_FRAME_TOPBORDER     2
#endif

#ifndef WTK_FRAME_BOTTOMBORDER
//! Bottom frame border size.
# define WTK_FRAME_BOTTOMBORDER  4
#endif

#ifndef WTK_FRAME_RESIZE_RADIUS
//! Radius of resize handle (second quadrant of circle).
# define WTK_FRAME_RESIZE_RADIUS 7
#endif

#ifndef WTK_FRAME_RESIZE_WIDTH
//! Width of resize handle.
# define WTK_FRAME_RESIZE_WIDTH  (WTK_FRAME_RESIZE_RADIUS + 1)
#endif

#ifndef WTK_FRAME_RESIZE_HEIGHT
//! Height of resize handle.
# define WTK_FRAME_RESIZE_HEIGHT (WTK_FRAME_RESIZE_RADIUS + 1)
#endif

#ifndef WTK_FRAME_MIN_WIDTH
//! Minimum width of contents window of a frame.
# define WTK_FRAME_MIN_WIDTH  (WTK_FRAME_RESIZE_WIDTH - WTK_FRAME_RIGHTBORDER)
#endif

#ifndef WTK_FRAME_MIN_HEIGHT
//! Minimum height of contents window of a frame.
# define WTK_FRAME_MIN_HEIGHT (WTK_FRAME_RESIZE_HEIGHT - WTK_FRAME_BOTTOMBORDER)
#endif

#ifndef WTK_FRAME_TITLEBAR_HEIGHT
//! Height of titlebar area.
# define WTK_FRAME_TITLEBAR_HEIGHT 9
#endif

#ifndef WTK_FRAME_CAPTION_X
//! Caption text X offset inside title bar.
# define WTK_FRAME_CAPTION_X       3
#endif

#ifndef WTK_FRAME_CAPTION_Y
//! Caption text Y offset inside title bar.
# define WTK_FRAME_CAPTION_Y       1
#endif

#ifndef WTK_FRAME_BACKGROUND_COLOR
//! Background color of frame contents window.
# define WTK_FRAME_BACKGROUND_COLOR GFX_COLOR(192, 192, 128)
#endif

#ifndef WTK_FRAME_BORDER_COLOR
//! Frame border color.
# define WTK_FRAME_BORDER_COLOR     GFX_COLOR(240, 240, 240)
#endif

#ifndef WTK_FRAME_TITLEBAR_COLOR
//! Background color of frame title bar.
# define WTK_FRAME_TITLEBAR_COLOR   GFX_COLOR(128, 128, 255)
#endif

#ifndef WTK_FRAME_CAPTION_COLOR
//! Caption text foreground color.
# define WTK_FRAME_CAPTION_COLOR    GFX_COLOR(255, 255, 255)
#endif

#ifndef WTK_FRAME_RESIZE_COLOR
//! Resize handle color.
# define WTK_FRAME_RESIZE_COLOR     GFX_COLOR(64, 192, 192)
#endif

#ifndef WTK_FRAME_SHADESIZE
//! Size of drop shadow area outside left/bottom/right border. 0 = no shadow.
# define WTK_FRAME_SHADESIZE 4
#endif

#ifndef WTK_FRAME_SHADES
//! Shade factor of drop shadow, first number is farthest from border.
# define WTK_FRAME_SHADES { 252, 242, 220, 180 }
#endif

#ifndef WTK_DRAG_THRESHOLD
//! Distance from origin before dragging operation kicks in.
# define WTK_DRAG_THRESHOLD     4
#endif

#ifndef WTK_DRAG_HANDLE_RADIUS
//! Radius of drag origin and target handles.
# define WTK_DRAG_HANDLE_RADIUS 4
#endif

#ifndef WTK_DRAG_PIXMAP_SIZE
//! Pixmap pixel count for storing drag handle background graphics.
# define WTK_DRAG_PIXMAP_SIZE   (WTK_DRAG_HANDLE_RADIUS * 2 + 1)
#endif

#ifndef WTK_DRAG_ORIGIN_COLOR
//! Drag origin handle color.
# define WTK_DRAG_ORIGIN_COLOR  GFX_COLOR(255, 0, 0)
#endif

#ifndef WTK_DRAG_TARGET_COLOR
//! Drag target handle color.
# define WTK_DRAG_TARGET_COLOR  GFX_COLOR(0, 255, 0)
#endif

//! @}

/**
 * \ingroup gfx_wtk_button
 * @{
 */

#ifndef WTK_BUTTON_BORDER_COLOR
//! Button border color.
# define WTK_BUTTON_BORDER_COLOR     GFX_COLOR(0, 0, 0)
#endif

#ifndef WTK_BUTTON_BACKGROUND_COLOR
//! Color of button background (text color when highlighted).
# define WTK_BUTTON_BACKGROUND_COLOR GFX_COLOR(128, 128, 128)
#endif

#ifndef WTK_BUTTON_CAPTION_COLOR
//! Color of button text (background color when highlighted).
# define WTK_BUTTON_CAPTION_COLOR    GFX_COLOR(255, 255, 255)
#endif

//! @}

/**
 * \ingroup gfx_wtk_check_box
 * @{
 */

#ifndef WTK_CHECKBOX_BOX_SIZE
//! Size of checkbox square.
# define WTK_CHECKBOX_BOX_SIZE   8
#endif

#ifndef WTK_CHECKBOX_BOX_X
//! Checkbox square X offset.
# define WTK_CHECKBOX_BOX_X      0
#endif

#ifndef WTK_CHECKBOX_BOX_Y
//! Checkbox square Y offset.
# define WTK_CHECKBOX_BOX_Y      0
#endif

#ifndef WTK_CHECKBOX_CAPTION_X
//! Checkbox caption text X offset.
# define WTK_CHECKBOX_CAPTION_X (WTK_CHECKBOX_BOX_SIZE + 4)
#endif

#ifndef WTK_CHECKBOX_CAPTION_Y
//! Checkbox caption text Y offset.
# define WTK_CHECKBOX_CAPTION_Y  0
#endif

#ifndef WTK_CHECKBOX_BOX_COLOR
//! Checkbox square frame color.
# define WTK_CHECKBOX_BOX_COLOR     GFX_COLOR(0, 0, 0)
#endif

#ifndef WTK_CHECKBOX_SELECT_COLOR
//! Checkbox square select mark color.
# define WTK_CHECKBOX_SELECT_COLOR  GFX_COLOR(128, 0, 0)
#endif

#ifndef WTK_CHECKBOX_CAPTION_COLOR
//! Checkbox caption text color.
# define WTK_CHECKBOX_CAPTION_COLOR GFX_COLOR(255, 255, 255)
#endif

//! @}

/**
 * \ingroup gfx_wtk_radio_button
 * @{
 */

//! Datatype holding reference count for radio button groups.
typedef uint8_t wtk_radio_group_size_t;

#ifndef WTK_RADIOBUTTON_RADIUS
//! Radius of radio button circle.
# define WTK_RADIOBUTTON_RADIUS      4
#endif

#ifndef WTK_RADIOBUTTON_BUTTON_X
//! Radio button circle center X offset.
# define WTK_RADIOBUTTON_BUTTON_X    (WTK_RADIOBUTTON_RADIUS)
#endif

#ifndef WTK_RADIOBUTTON_BUTTON_Y
//! Radio button circle center Y offset.
# define WTK_RADIOBUTTON_BUTTON_Y    (WTK_RADIOBUTTON_RADIUS)
#endif

#ifndef WTK_RADIOBUTTON_CAPTION_X
//! Radio button caption text X offset.
# define WTK_RADIOBUTTON_CAPTION_X  ((WTK_RADIOBUTTON_RADIUS * 2) + 4)
#endif

#ifndef WTK_RADIOBUTTON_CAPTION_Y
//! Radio button caption text Y offset.
# define WTK_RADIOBUTTON_CAPTION_Y   0
#endif

#ifndef WTK_RADIOBUTTON_BUTTON_COLOR
//! Radio button circle color.
# define WTK_RADIOBUTTON_BUTTON_COLOR  GFX_COLOR(0, 0, 0)
#endif

#ifndef WTK_RADIOBUTTON_SELECT_COLOR
//! Radio button select mark color.
# define WTK_RADIOBUTTON_SELECT_COLOR  GFX_COLOR(128, 0, 0)
#endif

#ifndef WTK_RADIOBUTTON_CAPTION_COLOR
//! Radio button caption text color.
# define WTK_RADIOBUTTON_CAPTION_COLOR GFX_COLOR(255, 255, 255)
#endif

//! @}

/**
 * \ingroup gfx_wtk_slider
 * @{
 */

#ifndef WTK_SLIDER_PARENT_MOVE_SUPPORT
/**
 * \brief Move support for sliders
 *
 * This define enables and disables move support for sliders. Move support
 * is needed if the application is using moving parent widgets/windows.
 * If not needed, disable it to save CPU cycles.
 *
 * Set to 1 to enable move support or 0 to disable move support.
 */
# define WTK_SLIDER_PARENT_MOVE_SUPPORT  0
#endif

#ifndef WTK_SLIDER_KNOB_WIDTH
//! Width of the slider knob, including borders, in pixels.
# define WTK_SLIDER_KNOB_WIDTH           10
#endif

#ifndef WTK_SLIDER_BORDER_COLOR
//! Color of all the slider's borders.
# define WTK_SLIDER_BORDER_COLOR         GFX_COLOR(0, 0, 0)
#endif

#ifndef WTK_SLIDER_BACKGROUND_COLOR
//! Color of the slider's background.
# define WTK_SLIDER_BACKGROUND_COLOR     GFX_COLOR(128, 128, 128)
#endif

#ifndef WTK_SLIDER_KNOB_COLOR_NORMAL
//! Color of the slider knob when normal.
# define WTK_SLIDER_KNOB_COLOR_NORMAL    GFX_COLOR(128, 0, 0)
#endif

#ifndef WTK_SLIDER_KNOB_COLOR_MOVING
//! Color of the slider knob when moving.
# define WTK_SLIDER_KNOB_COLOR_MOVING    GFX_COLOR(255, 255, 255)
#endif

//! @}

/**
 * \ingroup gfx_wtk_label
 * @{
 */

#ifndef WTK_STATICTEXT_CAPTION_COLOR
//! Static text caption text color.
# define WTK_STATICTEXT_CAPTION_COLOR GFX_COLOR(255, 255, 255)
#endif

//! @}

/**
 * \ingroup gfx_wtk_progress_bar
 * @{
 */

#ifndef WTK_PROGRESS_BAR_BORDER_COLOR
//! Color of the progress bar's frame border.
# define WTK_PROGRESS_BAR_BORDER_COLOR    GFX_COLOR(0, 0, 0)
#endif

//! @}

/**
 * \ingroup gfx_wtk_dialogue_box
 * @{
 */
/** 
 * \note defined positions and sizes for dialogue box.
 */

//! Dialogue box frame X position inside parent frame.
#define DIALOGUE_FRAME_POS_X        40
//! Dialogue box frame Y position inside parent frame.
#define DIALOGUE_FRAME_POS_Y        50
//! Dialogue box horisontal size X.
#define DIALOGUE_FRAME_SIZE_X      240
//! Dialogue box vertical size Y.
#define DIALOGUE_FRAME_SIZE_Y      120

//! Buttons Y position inside dialogue box frame.
#define BUTTON_POS_Y                60
//! Buttons horisontal size X.
#define BUTTON_SIZE_X               90
//! Buttons vertical size Y.
#define BUTTON_SIZE_Y               40
//! Horisontal spacing between buttons on X axis.
#define BUTTON_SPACING              20

//! Background color of dialogue box frame.
#define DIALOGUE_BOX_BACKGROUND     GFX_COLOR(100, 100, 100)
//! Frame line color of dialogue box frame.
#define DIALOGUE_FRAME_LINE_COLOR   GFX_COLOR(0, 0, 0)

//! Caption position centered on top of dialogue box by dividing size in half.
#define CAPTION_POS_X               DIALOGUE_FRAME_SIZE_X / 2
//! Caption distance down from top of dialogue box.
#define CAPTION_POS_Y               10

//! Second caption position centered on top of dialogue box.
#define SECOND_CAPTION_POS_X        CAPTION_POS_X
//! Second caption distance down from top of dialogue box.
#define SECOND_CAPTION_POS_Y        40

//! @}

//! @}

#endif /* DEFAULT_WTK_H_INCLUDED */

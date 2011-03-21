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
#include <assert.h>
#include <string.h>
#include <hugemem.h>
#include <physmem.h>
#include <status_codes.h>

#include <gfx/gfx.h>
#include <gfx/win.h>
#include <gfx/wtk.h>
#include <gfx/sysfont.h>

#include "app_fonts.h"
#include "app_desktop.h"
#include "file_loader.h"

/**
 * \weakgroup app_fonts_group
 *
 * @{
 */

//! \name Font definitions and configuration
//@{
//! Number of bytes in the header for the font files loaded from the TSFS.
#define FONT_HEADER_SIZE        16
//! Number of fonts present in the application.
#define NUMBER_OF_FONTS         4
//@}

//! \name Page configuration
//@{
/**
 * \brief Page number containing a blank page used while application is loading
 * the font files.
 */
#define PAGE_NUM_BLANK_SCREEN   0
//! Number of initial page to show, minimum value.
#define PAGE_NUM_MIN            1
//! Number of last page to show, maximum value.
#define PAGE_NUM_MAX            5
//@}

//! \name Color scheme
//@{
//! Color on text strings.
#define COLOR_TEXT              GFX_COLOR(255, 255, 255)
//! Color on shaded text strings.
#define COLOR_TEXT_SHADED       GFX_COLOR(150, 150, 150)
//! Background color.
#define COLOR_BACKGROUND        GFX_COLOR(0, 0, 0)
//! Color on filled rectangles, progress bar and horizontal lines.
#define COLOR_FILL              GFX_COLOR(48, 120, 158)
//! Color on filled rectangle borders.
#define COLOR_FILL_BORDER       GFX_COLOR(30, 74, 98)
//@}

//! \name Text appearance
//@{
//! Number of pixels to indent a string.
#define TEXT_INDENT             5
//! Number of pixels to add in front of a string.
#define TEXT_PADDING            3
//! Number of pixels for space after newline.
#define TEXT_PADDING_NEWLINE    2
//@}

//! Command event ID
enum app_fonts_button_ids {
	//! Command event ID for next page-button.
	BUTTON_NEXT_ID,
	//! Command event ID for previous page-button.
	BUTTON_PREV_ID,
	//! Command event ID for quit-button.
	BUTTON_QUIT_ID,
};

//! Application context
struct app_fonts {
	//! Progress bar-widget for font loading.
	struct wtk_progress_bar *progress_bar;
	//! The application frame.
	struct wtk_basic_frame  *frame;
	//! Workqueue task to use for the application.
	struct workqueue_task   task;
	//! Frame background bitmap.
	struct gfx_bitmap       background;
	//! Exit-button widget.
	struct wtk_button       *btn_exit;
	//! Next-button widget.
	struct wtk_button       *btn_next;
	//! Previous-button widget.
	struct wtk_button       *btn_prev;
	//! Buffer for font loading.
	struct font             *current_font_loading;
	//! Backup copy of the system font to restore upon exit.
	struct font             prev_sysfont;
	//! Current page to show.
	uint8_t                 page_number;
};

//! \name Font metadata
//@{
//! Erica Type
static struct font font_ericat = {
	.type           = FONT_LOC_HUGEMEM,
	.scale          = 1,
};

//! Fixed-width Russian
static struct font font_fixedrus = {
	.type           = FONT_LOC_HUGEMEM,
	.scale          = 1,
};

//! Larabie
static struct font font_larabie = {
	.type           = FONT_LOC_HUGEMEM,
	.scale          = 1,
};

//! Monofur
static struct font font_monofur = {
	.type           = FONT_LOC_HUGEMEM,
	.scale          = 1,
};
//@}

//! Statically allocated context pointer
static struct app_fonts *the_fonts_app;

/**
 * \brief Draw a filled box on the screen starting at line \a y.
 *
 * This function draws a filled box containing a demonstration of a given \a
 * font. It will output the \a fontname and print the "The Quick Brown Fox
 * Jumps Over the Lazy Dog" string in this font.
 *
 * \param y Y coordinate to draw the box at.
 * \param font Pointer to font metadata.
 * \param fontname Pointer to string with font name.
 */
static void draw_font_box(gfx_coord_t y, struct font *font, char *fontname)
{
	gfx_coord_t height = (font->height * font->scale * 2) +
		TEXT_PADDING_NEWLINE + (2 * TEXT_PADDING);

	gfx_draw_filled_rect(TEXT_PADDING, y, gfx_get_width() -
			(2 * TEXT_PADDING), height, COLOR_FILL);
	gfx_draw_rect(TEXT_PADDING, y, gfx_get_width() - (2 * TEXT_PADDING),
			height, COLOR_FILL_BORDER);

	gfx_draw_string(fontname, (2 * TEXT_INDENT), y + TEXT_PADDING, font,
			COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	gfx_draw_string("The Quick Brown Fox Jumps Over the Lazy Dog",
			(2 * TEXT_INDENT), y + TEXT_PADDING +
			TEXT_PADDING_NEWLINE + (font->height * font->scale),
			font, COLOR_TEXT, GFX_COLOR_TRANSPARENT);
}

/**
 * \brief Draw the fonts introduction splash page.
 *
 * This function will print two strings to the center of the screen.
 */
static void screen_draw_splash(void)
{
	gfx_draw_string("    Font and\r\nLanguage Support", 30, 60,
			&font_monofur, COLOR_TEXT,
			GFX_COLOR_TRANSPARENT);
	gfx_draw_string("- Ææ Øø Åå -", 95, 150, &font_ericat,
			COLOR_TEXT_SHADED, GFX_COLOR_TRANSPARENT);
}

/**
 * \brief Draw the fonts info page.
 *
 * This function will print generic information about the font system to the
 * screen.
 */
static void screen_draw_fonts_info(void)
{
	gfx_coord_t y = TEXT_INDENT;

	gfx_draw_horizontal_line(TEXT_PADDING, font_fixedrus.height *
			font_fixedrus.scale + TEXT_INDENT, gfx_get_width() -
			(2 * TEXT_PADDING), COLOR_FILL);

	gfx_draw_string("Font Information", TEXT_INDENT, y, &font_fixedrus,
			COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += font_fixedrus.height + TEXT_INDENT;

	gfx_draw_string("A powerful font system allows", TEXT_INDENT, y,
			&font_fixedrus, COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += font_fixedrus.height + TEXT_PADDING_NEWLINE;

	gfx_draw_string("any number of fonts in use at", TEXT_INDENT, y,
			&font_fixedrus, COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += font_fixedrus.height + TEXT_PADDING_NEWLINE;

	gfx_draw_string("the same time. This offers", TEXT_INDENT, y,
			&font_fixedrus, COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += font_fixedrus.height + TEXT_PADDING_NEWLINE;

	gfx_draw_string("great flexibility in how text", TEXT_INDENT, y,
			&font_fixedrus, COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += font_fixedrus.height + TEXT_PADDING_NEWLINE;

	gfx_draw_string("is presented to the user.", TEXT_INDENT, y,
			&font_fixedrus, COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += font_fixedrus.height + TEXT_INDENT;

	gfx_draw_string("The next screen showcases some", TEXT_INDENT, y,
			&font_fixedrus, COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += font_fixedrus.height + TEXT_PADDING_NEWLINE;

	gfx_draw_string( "of the fonts loaded in this", TEXT_INDENT, y,
			&font_fixedrus, COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += font_fixedrus.height + TEXT_PADDING_NEWLINE;

	gfx_draw_string("application.", TEXT_INDENT, y, &font_fixedrus,
			COLOR_TEXT, GFX_COLOR_TRANSPARENT);
}

/**
 * \brief Draw the fonts list page.
 *
 * This function will print the name and a test string of the four fonts loaded
 * in the application to the screen.
 */
static void screen_draw_fonts_list(void)
{
	gfx_coord_t y = TEXT_PADDING;

	draw_font_box(y, &font_monofur, "Monofur");
	y += (2 * font_monofur.height * font_monofur.scale) +
		(3 * TEXT_PADDING) + TEXT_PADDING_NEWLINE;
	draw_font_box(y, &sysfont, "System Font");
	y += (2 * sysfont.height * sysfont.scale) + (3 * TEXT_PADDING)
		+ TEXT_PADDING_NEWLINE;
	draw_font_box(y, &font_ericat, "Erica Type");
	y += (2 * font_ericat.height * font_ericat.scale) +
		(3 * TEXT_PADDING) + TEXT_PADDING_NEWLINE;
	draw_font_box(y, &font_fixedrus, "Fixedsys");
}

/**
 * \brief Draw the language information page.
 *
 * This function will print information regarding use of multiple fonts and
 * different languages with the \ref gfx "Graphics display system".
 */
static void screen_draw_language_info(void)
{
	gfx_coord_t y = TEXT_INDENT;

	gfx_draw_horizontal_line(TEXT_PADDING, font_fixedrus.height *
			font_fixedrus.scale + TEXT_INDENT, gfx_get_width() -
			(2 * TEXT_PADDING), COLOR_FILL);

	gfx_draw_string("Language Information", TEXT_INDENT, y, &font_fixedrus,
			COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += font_fixedrus.height + (2 * TEXT_PADDING_NEWLINE);

	gfx_draw_string("Full Extended ASCII support", TEXT_INDENT, y,
			&font_fixedrus, COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += font_fixedrus.height + TEXT_PADDING_NEWLINE;

	gfx_draw_string("makes it easy to switch", TEXT_INDENT, y,
			&font_fixedrus, COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += font_fixedrus.height + TEXT_PADDING_NEWLINE;

	gfx_draw_string("between local character sets ", TEXT_INDENT, y,
			&font_fixedrus, COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += font_fixedrus.height + TEXT_PADDING_NEWLINE;

	gfx_draw_string("and print text in different", TEXT_INDENT, y,
			&font_fixedrus, COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += font_fixedrus.height + TEXT_PADDING_NEWLINE;

	gfx_draw_string("languages, fonts or sizes.", TEXT_INDENT, y,
			&font_fixedrus, COLOR_TEXT, GFX_COLOR_TRANSPARENT);
}

/**
 * \brief Draw the messages in different languages page.
 *
 * This function will print some typical strings in different languages to the
 * screen, using different types of fonts. The hardcoded string positions are
 * spread across the display for presentational purposes.
 */
static void screen_draw_messages(void)
{
	/* Nice to meet you */
	gfx_draw_string("È un piacere conoscerla", 40, 5, &font_ericat,
			COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	/* Good day! How are you? */
	gfx_draw_string("Guten Tag! Wie geht's?", 5, 30, &sysfont, COLOR_TEXT,
			GFX_COLOR_TRANSPARENT);

	/* Hello and welcome! */
	gfx_draw_string("Hello and welcome!", 105, 50, &font_fixedrus,
			COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	/* Nice to meet you! */
	gfx_draw_string("Hyggelig å møtes!", 41, 70, &font_monofur, COLOR_TEXT,
			GFX_COLOR_TRANSPARENT);

	/* Please have a look! */
	gfx_draw_string("Jetez un coup d'œil,\r\ns'il vous plaît!", 10, 110,
			&font_ericat, COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	/* Can I help you? */
	gfx_draw_string("¿Podría Ayudarse?", 150, 155, &font_larabie,
			COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	/* Good day! */
	gfx_draw_string("Grüß Gott!", 80, 170, &font_monofur, COLOR_TEXT,
			GFX_COLOR_TRANSPARENT);

	/* To good health! */
	gfx_draw_string("\xC1\xF3\xC4\xE5\xEC \xE7\xC4\xEE\xF0\xEE\xE2\xDB!",
			30, 210, &font_fixedrus, COLOR_TEXT,
			GFX_COLOR_TRANSPARENT);
}

/**
 * \brief Frame draw handler.
 *
 * Handles all draw events received by the application frame.
 * This draws the currently selected page to screen.
 *
 * \sa wtk_basic_frame_draw_handler_t
 *
 * \param win Pointer to the basic frame's window.
 * \param clip Pointer to data for clipping region.
 */
static void app_fonts_frame_draw_handler(struct win_window *win,
		const struct win_clip_region *clip)
{
	switch (the_fonts_app->page_number) {
	case PAGE_NUM_BLANK_SCREEN:
		break;

	case 1:
		screen_draw_splash();
		break;

	case 2:
		screen_draw_fonts_info();
		break;

	case 3:
		screen_draw_fonts_list();
		break;

	case 4:
		screen_draw_language_info();
		break;

	case 5:
		screen_draw_messages();
		break;

	default:
		/* Not a valid page */
		unhandled_case(the_fonts_app->page_number);
		break;
	}
}

/**
 * \brief Frame command handler handling the button events.
 *
 * Handles all command events received by the application frame.
 * This changes the displayed page, or exits the application.
 *
 * \sa wtk_basic_frame_command_handler_t
 *
 * \param frame Pointer to the basic frame object
 * \param command_data Command data provided with the event, holds button ID
 *
 * \return true when application is exiting, false otherwise
 */
static bool app_fonts_frame_command_handler(struct wtk_basic_frame *frame,
		win_command_t command_data)
{
	uint8_t command = (uint8_t)(uintptr_t)command_data;

	switch (command) {
	case BUTTON_NEXT_ID:
		// Show next page.
		the_fonts_app->page_number++;
		if (the_fonts_app->page_number > PAGE_NUM_MAX)
			the_fonts_app->page_number = PAGE_NUM_MIN;
		win_redraw(wtk_basic_frame_as_child(the_fonts_app->frame));
		break;

	case BUTTON_PREV_ID:
		// Show previous page.
		the_fonts_app->page_number--;
		if (the_fonts_app->page_number < PAGE_NUM_MIN)
			the_fonts_app->page_number = PAGE_NUM_MAX;
		win_redraw(wtk_basic_frame_as_child(the_fonts_app->frame));
		break;

	case BUTTON_QUIT_ID:
		// Restore system font, free the context then restart desktop.
		memcpy(&sysfont, &the_fonts_app->prev_sysfont,
				sizeof(struct font));
		membag_free(the_fonts_app);
		app_desktop_restart();
		return true;

	default:
		break;
	}

	return false;
}

static status_t app_fonts_load(void);

/**
 * \brief Setup the font objects after loading fonts from the file system.
 *
 * This function is called when a font has been loaded from the file system. It
 * will setup the font object with the data read. Afterwards it will try to
 * load the next font, until done.
 *
 * When the function is done loading all the fonts, the initial page is shown
 * on the screen.
 *
 * \param task Pointer to work queue task
 */
static void app_fonts_load_worker(struct workqueue_task *task)
{
	status_t retval;

	if (the_fonts_app->current_font_loading) {
		struct font     *font = the_fonts_app->current_font_loading;
		uint8_t         buffer[FONT_HEADER_SIZE];

		the_fonts_app->current_font_loading = NULL;

		hugemem_read_block(buffer, font->data.hugemem,
				FONT_HEADER_SIZE);

		assert(buffer[0] == 'F' && buffer[1] == 'T');

		font->width        = buffer[2];
		font->height       = buffer[3];
		font->first_char   = buffer[4];
		font->last_char    = buffer[5];
		font->data.hugemem = (hugemem_ptr_t)
			((uint32_t)font->data.hugemem + FONT_HEADER_SIZE);
	}

	retval = app_fonts_load();
	if (retval == STATUS_OK) {
		/*
		 * All files are loaded, time to show the first page and the
		 * buttons.
		 */
		the_fonts_app->page_number = PAGE_NUM_MIN;
		win_redraw(wtk_basic_frame_as_child(the_fonts_app->frame));
		win_show(wtk_button_as_child(the_fonts_app->btn_exit));
		win_show(wtk_button_as_child(the_fonts_app->btn_next));
		win_show(wtk_button_as_child(the_fonts_app->btn_prev));
	}
	else if (retval == ERR_BUSY) {
		/* Continue waiting for font loading. */
	} else {
		/*
		 * File system return an unexpected error, restore system font,
		 * destroy the frame and restart the desktop application.
		 */
		win_destroy(wtk_basic_frame_as_child(the_fonts_app->frame));
		memcpy(&sysfont, &the_fonts_app->prev_sysfont,
				sizeof(struct font));
		membag_free(the_fonts_app);
		app_desktop_restart();
	}
}

/**
 * \brief Schedules loading of next non-loaded font.
 *
 * This function will schedule loading the next font not already loaded to
 * hugemem. It will also update the progress bar each time a newfont is
 * successfully scheduled to be loaded.
 *
 * \retval ERR_IO_ERROR if font is not present on file system, or other errors
 * \retval ERR_BUSY when scheduling loading a font
 * \retval STATUS_OK when all fonts are loaded successfully
 */
static status_t app_fonts_load(void)
{
	hugemem_ptr_t addr;

	// Load each font for which hugemem has not been allocated.
	if (!font_ericat.data.hugemem) {
		addr = load_file_to_hugemem("f_ericat", &the_fonts_app->task);
		if (addr == HUGEMEM_NULL)
			return ERR_IO_ERROR;

		font_ericat.data.hugemem = addr;
		the_fonts_app->current_font_loading = &font_ericat;
		wtk_progress_bar_set_value(the_fonts_app->progress_bar, 1);
		return ERR_BUSY;
	}

	if (!font_fixedrus.data.hugemem) {
		addr = load_file_to_hugemem("f_fixrus", &the_fonts_app->task);
		if (addr == HUGEMEM_NULL)
			return ERR_IO_ERROR;

		font_fixedrus.data.hugemem = addr;
		the_fonts_app->current_font_loading = &font_fixedrus;
		wtk_progress_bar_set_value(the_fonts_app->progress_bar, 2);

		return ERR_BUSY;
	}

	if (!font_larabie.data.hugemem) {
		addr = load_file_to_hugemem("f_larab", &the_fonts_app->task);
		if (addr == HUGEMEM_NULL)
			return ERR_IO_ERROR;

		font_larabie.data.hugemem = addr;
		the_fonts_app->current_font_loading = &font_larabie;
		wtk_progress_bar_set_value(the_fonts_app->progress_bar, 3);

		return ERR_BUSY;
	}

	if (!font_monofur.data.hugemem) {
		addr = load_file_to_hugemem("f_monfur", &the_fonts_app->task);
		if (addr == HUGEMEM_NULL)
			return ERR_IO_ERROR;

		font_monofur.data.hugemem = addr;
		the_fonts_app->current_font_loading = &font_monofur;
		wtk_progress_bar_set_value(the_fonts_app->progress_bar, 4);

		return ERR_BUSY;
	}

	/*
	 * Destroy the progress bar, since loading of fonts are complete. The
	 * progress bar would otherwise interfere with the fonts demo screens
	 * later.
	 */
	win_destroy(wtk_progress_bar_as_child(the_fonts_app->progress_bar));

	return STATUS_OK;
}

/**
 * \brief Launches the application
 *
 * Allocates and initializes the context, then allocates the basic frame and
 * button widgets that make up the application. Loading of the necessary fonts
 * is then scheduled, unless they have already been loaded.
 *
 * \param task Workqueue task to use with application
 */
void app_fonts_launch(struct workqueue_task *task)
{
	struct win_window       *win_root;
	struct win_window       *parent;
	struct win_area         area;
	status_t                retval;

	the_fonts_app = membag_alloc(sizeof(struct app_fonts));
	if (!the_fonts_app)
		goto error_membag_alloc;

	/* Always set page number to an initial black page. */
	the_fonts_app->page_number = PAGE_NUM_BLANK_SCREEN;

	/* Store previous system font and scale it to double size. */
	memcpy(&the_fonts_app->prev_sysfont, &sysfont, sizeof(struct font));
	sysfont.scale = 2;

	/*
	 * Initialize the work queue used when loading font files from the TSFS
	 * file system.
	 */
	workqueue_task_init(&the_fonts_app->task, app_fonts_load_worker);

	/*
	 * Initialize and set the background used on all the fonts demo
	 * screens. The background feature on the basic frame is not in use, as
	 * a redraw event will wipe out the text elements drawn to the screen.
	 * Calling redraw on the window will now only draw the buttons.
	 */
	the_fonts_app->background.type       = BITMAP_SOLID;
	the_fonts_app->background.data.color = COLOR_BACKGROUND;

	win_root = win_get_root();

	/*
	 * Create a transparent basic frame covering the entire screen to hold
	 * text and buttons.
	 */
	area.pos.x = 0;
	area.pos.y = 0;
	area.size.x = gfx_get_width();
	area.size.y = gfx_get_height();

	the_fonts_app->frame = wtk_basic_frame_create(win_root, &area,
			&the_fonts_app->background,
			app_fonts_frame_draw_handler,
			app_fonts_frame_command_handler, &the_fonts_app);
	if (!the_fonts_app->frame)
		goto error_text_frame;

	parent = wtk_basic_frame_as_child(the_fonts_app->frame);
	win_show(parent);

	/* Create an exit button. */
	area.size.x = APP_EXIT_BUTTON_SIZE_X;
	area.size.y = APP_EXIT_BUTTON_SIZE_Y;
	area.pos.x = APP_EXIT_BUTTON_POS_X;
	area.pos.y = APP_EXIT_BUTTON_POS_Y;

	the_fonts_app->btn_exit = wtk_button_create(parent, &area,
			APP_EXIT_BUTTON_TEXT, (win_command_t)BUTTON_QUIT_ID);
	if (!the_fonts_app->btn_exit)
		goto error_widget;

	/* Create a next button. */
	area.pos.x -= APP_EXIT_BUTTON_SIZE_X;

	the_fonts_app->btn_next = wtk_button_create(parent, &area, ">>",
			(win_command_t)BUTTON_NEXT_ID);
	if (!the_fonts_app->btn_next)
		goto error_widget;

	/* Create a previous button. */
	area.pos.x -= APP_EXIT_BUTTON_SIZE_X;

	the_fonts_app->btn_prev = wtk_button_create(parent, &area, "<<",
			(win_command_t)BUTTON_PREV_ID);
	if (!the_fonts_app->btn_prev)
		goto error_widget;

	/* Create a progress bar for font loading. */
	area.pos.x  = 0;
	area.size.x = gfx_get_width() - (3 * APP_EXIT_BUTTON_SIZE_X);

	the_fonts_app->progress_bar = wtk_progress_bar_create(parent, &area,
			NUMBER_OF_FONTS, 0, COLOR_FILL, COLOR_BACKGROUND,
			WTK_PROGRESS_BAR_HORIZONTAL);
	if (!the_fonts_app->progress_bar)
		goto error_widget;

	win_show(wtk_progress_bar_as_child(the_fonts_app->progress_bar));

	/*
	 * Load fonts to hugemem if not already loaded. The app_fonts_load
	 * function returns ERR_BUSY while busy loading files. All other error
	 * returns are fatal. If STATUS_OK is returned, all fonts are loaded
	 * and application is ready to show the first page of the fonts demo.
	 */
	retval = app_fonts_load();
	if (retval == STATUS_OK) {
		the_fonts_app->page_number = PAGE_NUM_MIN;
		win_redraw(parent);
		win_show(wtk_button_as_child(the_fonts_app->btn_exit));
		win_show(wtk_button_as_child(the_fonts_app->btn_next));
		win_show(wtk_button_as_child(the_fonts_app->btn_prev));
	}
	else if (retval != ERR_BUSY) {
		goto error_widget;
	}

	return;

error_widget:
	win_destroy(parent);
error_text_frame:
	memcpy(&sysfont, &the_fonts_app->prev_sysfont, sizeof(struct font));
	membag_free(the_fonts_app);
error_membag_alloc:
	app_desktop_restart();
}

//! @}

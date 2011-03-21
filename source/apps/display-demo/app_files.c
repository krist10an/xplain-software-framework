/**
 * \file
 *
 * \brief Display Xplained file details application
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
#include <stream.h>
#include <string.h>
#include <workqueue.h>

#include <gfx/gfx.h>
#include <gfx/win.h>
#include <gfx/wtk.h>
#include <gfx/sysfont.h>

#include <fs/tsfs.h>

#include "app_files.h"
#include "app_desktop.h"
#include "file_loader.h"

/**
 * \weakgroup app_files_group
 *
 * @{
 */

//! Number of bytes in the header for the font files loaded from the TSFS.
#define FONT_HEADER_SIZE        16

//! Number of files to present size information per page.
#define FILE_NAMES_PER_PAGE     8

//! File demo application static page numbers.
enum app_files_page_numbers {
	//! Initial black screen while loading font.
	PAGE_NUM_BLANK = 1,
	//! First info page.
	PAGE_NUM_FIRST = 2,
	//! Introduction screen page number.
	PAGE_NUM_INTRO_SCREEN = PAGE_NUM_FIRST,
	//! Info screen page number.
	PAGE_NUM_INFO_SCREEN,
	//! Last info page, not counting file info pages.
	PAGE_NUM_LAST = PAGE_NUM_INFO_SCREEN,
};

//! \name Color scheme
//@{
//! Color on text strings.
#define COLOR_TEXT              GFX_COLOR(250, 250, 250)
//! Color on shaded text strings.
#define COLOR_TEXT_SHADED       GFX_COLOR(150, 150, 150)
//! Background color.
#define COLOR_BACKGROUND        GFX_COLOR(0, 0, 0)
//! Color on horizontal lines.
#define COLOR_LINE              GFX_COLOR(48, 120, 158)
//@}

//! \name Text appearance
//@{
//! Height of common screen header in pixels.
#define TEXT_HEADER_HEIGHT      30
//! Number of pixels to indent a string.
#define TEXT_INDENT             5
//! Number of pixels to add in front of a string.
#define TEXT_PADDING            3
//! Number of pixels for space after newline.
#define TEXT_PADDING_NEWLINE    2
//@}

/**
 * \brief Event command ID for the application widgets.
 */
enum app_files_button_ids {
	//! Event command ID for the next button.
	BUTTON_NEXT_ID = 1,
	//! Event command ID for the previous button.
	BUTTON_PREV_ID,
	//! Event command ID for the exit button.
	BUTTON_QUIT_ID,
};

//! The files application context.
struct app_files {
	//! Basic frame covering entire screen used to hold text and buttons.
	struct wtk_basic_frame  *frame;
	//! Background bitmap for application
	struct gfx_bitmap       background;
	//! Pointer to application work queue used for loading the font.
	struct workqueue_task   *task;
	//! Font used to store a copy of the system font while running.
	struct font             old_sysfont;
	//! Current page number to be drawn on the screen.
	uint_fast8_t            page_number;
};

//! TSFS control struct, needed for file listing.
extern struct tsfs              myfs;

//! The files application context pointer.
static struct app_files         *the_app_files;

//! Fixed point russian font object.
static struct font              font_fixedrus = {
	.type   = FONT_LOC_HUGEMEM,
	.scale  = 1,
};

/**
 * \brief Format a file size into a string.
 *
 * This function will take a file size \a size and format it into the string \a
 * file_size_str. The function will format the size into B, kB, MB or GB
 * depending on which unit size best suits the input \a size.
 *
 * \param file_size_str Pointer to a string able to contain the size
 * \param size Size of file in bytes
 * \param str_size Size of file_size_str
 */
static void string_file_size(char *file_size_str, uint32_t size,
		size_t str_size)
{
	uint8_t order = 0;

	while (size > (1024 * 5) && order <= 3) {
		size /= 1024;
		order++;
	}

	switch (order) {
	case 0:
		snprintf(file_size_str, str_size, "%li B", size);
		break;

	case 1:
		snprintf(file_size_str, str_size, "%li kB", size);
		break;

	case 2:
		snprintf(file_size_str, str_size, "%li MB", size);
		break;

	case 3:
		snprintf(file_size_str, str_size, "%li GB", size);
		break;

	default:
		unhandled_case(order);
		break;

	};
}

/**
 * \brief Draw file system introduction page.
 *
 * This function will print an introduction page about the Tiny Simple File
 * System to the screen.
 */
static void screen_draw_introduction(void)
{
	gfx_coord_t     x = TEXT_INDENT;
	gfx_coord_t     y = TEXT_HEADER_HEIGHT;

	/* Draw a page header and a horizontal line. */
	gfx_draw_horizontal_line(TEXT_PADDING,
			gfx_font_get_height(&font_fixedrus) + TEXT_INDENT,
			gfx_get_width() - (2 * TEXT_PADDING), COLOR_LINE);
	gfx_draw_string("Tiny Simple File System", TEXT_INDENT, TEXT_INDENT,
			&font_fixedrus, COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	/* Draw the page contents. */
	gfx_draw_string("The TSFS is a fast and simple", x, y, &font_fixedrus,
			COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += gfx_font_get_height(&font_fixedrus) + TEXT_PADDING_NEWLINE;
	gfx_draw_string("read-only file system built for", x, y,
			&font_fixedrus, COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += gfx_font_get_height(&font_fixedrus) + TEXT_PADDING_NEWLINE;
	gfx_draw_string("speed in integrated systems.", x, y, &font_fixedrus,
			COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += gfx_font_get_height(&font_fixedrus) + TEXT_PADDING_NEWLINE +
		TEXT_INDENT;
	gfx_draw_string("TSFS makes it easy to load", x, y, &font_fixedrus,
			COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += gfx_font_get_height(&font_fixedrus) + TEXT_PADDING_NEWLINE;
	gfx_draw_string("resources like pictures or data", x, y, &font_fixedrus,
			COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += gfx_font_get_height(&font_fixedrus) + TEXT_PADDING_NEWLINE;
	gfx_draw_string("files from external storage to", x, y, &font_fixedrus,
			COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += gfx_font_get_height(&font_fixedrus) + TEXT_PADDING_NEWLINE;
	gfx_draw_string("RAM or directly to the screen.", x, y, &font_fixedrus,
			COLOR_TEXT, GFX_COLOR_TRANSPARENT);
}

/**
 * \brief Draw file system information page.
 *
 * This function will print information about the currently loaded file system
 * to the screen. Strings are formatted using \ref stream "streams".
 */
static void screen_draw_file_system_info(void)
{
	char            string[30];
	char            file_size_str[10];
	gfx_coord_t     x = TEXT_INDENT;
	gfx_coord_t     y = TEXT_HEADER_HEIGHT;

	/* Draw a page header and a horizontal line. */
	gfx_draw_horizontal_line(TEXT_PADDING,
			gfx_font_get_height(&font_fixedrus) + TEXT_INDENT,
			gfx_get_width() - (2 * TEXT_PADDING), COLOR_LINE);
	gfx_draw_string("Tiny Simple File System", TEXT_INDENT, TEXT_INDENT,
			&font_fixedrus, COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	/* Draw the page contents. */
	gfx_draw_string("File system details",
			x, y, &font_fixedrus, COLOR_TEXT,
			GFX_COLOR_TRANSPARENT);

	y += gfx_font_get_height(&font_fixedrus) + TEXT_PADDING_NEWLINE +
		TEXT_INDENT;
	snprintf(string, sizeof(string), "TSFS version    : v%i.0",
			myfs.header.version);
	gfx_draw_string(string, x, y, &font_fixedrus, COLOR_TEXT,
			GFX_COLOR_TRANSPARENT);

	y += gfx_font_get_height(&font_fixedrus) + TEXT_PADDING_NEWLINE;
	snprintf(string, sizeof(string), "Files in volume : %i",
			(int)myfs.header.nr_files);
	gfx_draw_string(string, x, y, &font_fixedrus, COLOR_TEXT,
			GFX_COLOR_TRANSPARENT);

	y += gfx_font_get_height(&font_fixedrus) + TEXT_PADDING_NEWLINE;
	string_file_size(file_size_str, myfs.header.volume_size,
			sizeof(file_size_str));
	snprintf(string, sizeof(string), "Volume size     : %s", file_size_str);
	gfx_draw_string(string, x, y, &font_fixedrus, COLOR_TEXT,
			GFX_COLOR_TRANSPARENT);

	y += gfx_font_get_height(&font_fixedrus) + TEXT_PADDING_NEWLINE +
		TEXT_INDENT;
	gfx_draw_string("The following pages will show", x, y, &font_fixedrus,
			COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += gfx_font_get_height(&font_fixedrus) + TEXT_PADDING_NEWLINE;
	gfx_draw_string("the contents of the file", x, y, &font_fixedrus,
			COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += gfx_font_get_height(&font_fixedrus) + TEXT_PADDING_NEWLINE;
	gfx_draw_string("system using the string", x, y, &font_fixedrus,
			COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	y += gfx_font_get_height(&font_fixedrus) + TEXT_PADDING_NEWLINE;
	gfx_draw_string("formatting utils.", x, y, &font_fixedrus, COLOR_TEXT,
			GFX_COLOR_TRANSPARENT);
}

/**
 * \brief Draw a list of files from file entry \a index.
 *
 * This function will fill the screen with information about files in the file
 * system starting at file entry \a index.
 *
 * \param index Index to entry in the file table
 */
static void screen_draw_file_list_from_index(uint_fast8_t index)
{
	char            string[30];
	char            file_size_str[10];
	uint8_t         file_name[TSFS_FILENAME_LEN + 1];
	uint8_t         x = TEXT_INDENT;
	uint8_t         y = TEXT_HEADER_HEIGHT;
	uint_fast8_t    file_index;
	uint_fast8_t    first_file;
	uint_fast8_t    last_file;

	first_file = index;
	last_file = min_u(first_file + FILE_NAMES_PER_PAGE - 1,
			myfs.header.nr_files - 1);

	/* Fill strings with trailing null bytes. */
	memset(string, 0, sizeof(string));
	memset(file_name, 0, sizeof(file_name));
	memset(file_size_str, 0, sizeof(file_size_str));

	/* Draw a page header and a horizontal line. */
	gfx_draw_horizontal_line(TEXT_PADDING,
			gfx_font_get_height(&font_fixedrus) + TEXT_INDENT,
			gfx_get_width() - (2 * TEXT_PADDING), COLOR_LINE);
	gfx_draw_string("String formatting TSFS contents", TEXT_INDENT, TEXT_INDENT,
			&font_fixedrus, COLOR_TEXT, GFX_COLOR_TRANSPARENT);

	/* Draw the page contents. */
	snprintf(string, sizeof(string), "Displaying files %i to %i",
			(int)first_file, (int)last_file);
	gfx_draw_string(string, x, y, &font_fixedrus, COLOR_TEXT,
			GFX_COLOR_TRANSPARENT);

	for (file_index = first_file; file_index <= last_file; file_index++) {
		struct tsfs_file        file;

		y += gfx_font_get_height(&font_fixedrus);

		tsfs_get_filename(&myfs, file_index, file_name);
		tsfs_open(&myfs, (char *)file_name, &file);

		string_file_size(file_size_str, tsfs_get_file_size(&file),
				sizeof(file_size_str));
		snprintf(string, sizeof(string), "%i) %s %s", (int)file_index,
				file_name, file_size_str);
		gfx_draw_string(string, x, y, &font_fixedrus, COLOR_TEXT,
				GFX_COLOR_TRANSPARENT);
	}
}

/**
 * \brief Frame draw handler handling draw events.
 *
 * \sa wtk_basic_frame_draw_handler_t
 */
static void app_files_frame_draw_handler(struct win_window *win,
		const struct win_clip_region *clip)
{
	uint_fast8_t page_number = the_app_files->page_number;

	if (page_number < PAGE_NUM_INTRO_SCREEN)
		return;

	if (page_number == PAGE_NUM_INTRO_SCREEN) {
		screen_draw_introduction();
	}
	else if (page_number == PAGE_NUM_INFO_SCREEN) {
		screen_draw_file_system_info();
	} else {
		uint_fast8_t file_offset;
		file_offset = (page_number - (PAGE_NUM_LAST + 1)) *
			FILE_NAMES_PER_PAGE;
		screen_draw_file_list_from_index(file_offset);
	}
}

/**
 * \brief Frame command handler handling the button events.
 *
 * \sa wtk_basic_frame_command_handler_t
 *
 * \param frame Pointer to the basic frame object
 * \param command_data Command data provided with the event, holds button ID
 *
 * \return true when application is exiting, false otherwise
 */
static bool app_files_frame_command_handler(struct wtk_basic_frame *frame,
		win_command_t command_data)
{
	uint8_t         command = (uint8_t)(uintptr_t)command_data;
	uint_fast8_t    nr_of_file_pages;

	nr_of_file_pages = (myfs.header.nr_files / FILE_NAMES_PER_PAGE) + 1;

	switch (command) {
	case BUTTON_NEXT_ID:
		the_app_files->page_number++;
		if (the_app_files->page_number >
				(PAGE_NUM_LAST + nr_of_file_pages)) {
			the_app_files->page_number = PAGE_NUM_FIRST;
		}
		win_redraw(wtk_basic_frame_as_child(the_app_files->frame));
		break;

	case BUTTON_PREV_ID:
		the_app_files->page_number--;
		if (the_app_files->page_number < PAGE_NUM_FIRST) {
			the_app_files->page_number = PAGE_NUM_LAST +
				nr_of_file_pages;
		}
		win_redraw(wtk_basic_frame_as_child(the_app_files->frame));
		break;

	case BUTTON_QUIT_ID:
		memcpy(&sysfont, &the_app_files->old_sysfont, sizeof(struct font));
		membag_free(the_app_files);
		app_desktop_restart();
		return true;

	default:
		break;
	}

	return false;
}

/**
 * \brief Setup the font object after loading the font from the file system.
 *
 * This function is called when the font has been loaded from the file system.
 * It will setup the font object with the data read. Afterwards it will show
 * the initial page on the screen.
 *
 * \param task Pointer to work queue task
 */
static void app_files_load_worker(struct workqueue_task *task)
{
	uint8_t         buffer[FONT_HEADER_SIZE];
	struct font     *font = &font_fixedrus;

	hugemem_read_block(buffer, font->data.hugemem, FONT_HEADER_SIZE);

	assert(buffer[0] == 'F' && buffer[1] == 'T');

	font->width        = buffer[2];
	font->height       = buffer[3];
	font->first_char   = buffer[4];
	font->last_char    = buffer[5];
	font->data.hugemem = (hugemem_ptr_t)
		((uint32_t)font->data.hugemem + FONT_HEADER_SIZE);

	the_app_files->page_number = PAGE_NUM_INTRO_SCREEN;
	win_show(wtk_basic_frame_as_child(the_app_files->frame));
}

/**
 * \brief Launch the files and string formatting application.
 *
 * \param task Workqueue task to use for the application's worker functions.
 */
void app_files_launch(struct workqueue_task *task)
{
	struct win_window       *win_root;
	struct win_window       *parent;
	struct wtk_button       *button;
	struct win_area         area;

	the_app_files = membag_alloc(sizeof(struct app_files));
	if (!the_app_files)
		goto error_membag_alloc;

	the_app_files->page_number = PAGE_NUM_BLANK;

	/* Store previous system font and scale it to double size. */
	memcpy(&the_app_files->old_sysfont, &sysfont, sizeof(struct font));
	sysfont.scale = 2;

	/*
	 * Initialize the work queue used when loading font files from the TSFS
	 * file system.
	 */
	the_app_files->task = task;
	workqueue_task_set_work_func(the_app_files->task,
			app_files_load_worker);

	// Initialize and set the background used on all the file demo screens.
	the_app_files->background.type       = BITMAP_SOLID;
	the_app_files->background.data.color = COLOR_BACKGROUND;

	win_root = win_get_root();

	/*
	 * Create a transparent basic frame covering the entire screen to hold
	 * text and buttons.
	 */
	area.pos.x = 0;
	area.pos.y = 0;
	area.size.x = gfx_get_width();
	area.size.y = gfx_get_height();

	the_app_files->frame = wtk_basic_frame_create(win_root, &area,
			&the_app_files->background,
			app_files_frame_draw_handler,
			app_files_frame_command_handler, &the_app_files);
	if (!the_app_files->frame)
		goto error_text_frame;

	parent = wtk_basic_frame_as_child(the_app_files->frame);

	/* Create an exit button. */
	area.size.x = APP_EXIT_BUTTON_SIZE_X;
	area.size.y = APP_EXIT_BUTTON_SIZE_Y;
	area.pos.x = APP_EXIT_BUTTON_POS_X;
	area.pos.y = APP_EXIT_BUTTON_POS_Y;

	button = wtk_button_create(parent, &area, APP_EXIT_BUTTON_TEXT,
			(win_command_t)BUTTON_QUIT_ID);
	if (!button)
		goto error_widget;

	win_show(wtk_button_as_child(button));

	/* Create a next button. */
	area.pos.x -= APP_EXIT_BUTTON_SIZE_X;

	button = wtk_button_create(parent, &area, ">>",
			(win_command_t)BUTTON_NEXT_ID);
	if (!button)
		goto error_widget;

	win_show(wtk_button_as_child(button));

	/* Create a previous button. */
	area.pos.x -= APP_EXIT_BUTTON_SIZE_X;

	button = wtk_button_create(parent, &area, "<<",
			(win_command_t)BUTTON_PREV_ID);
	if (!button)
		goto error_widget;

	win_show(wtk_button_as_child(button));

	/*
	 * Load the font if not already done, if the font is loaded show the
	 * intro screen.
	 */
	if (!font_fixedrus.data.hugemem) {
		hugemem_ptr_t addr = load_file_to_hugemem("f_fixrus",
				the_app_files->task);
		if (addr == HUGEMEM_NULL)
			goto error_widget;

		font_fixedrus.data.hugemem = addr;
	} else {
		the_app_files->page_number = PAGE_NUM_INTRO_SCREEN;
		win_show(wtk_basic_frame_as_child(the_app_files->frame));
	}

	return;

error_widget:
	win_destroy(wtk_basic_frame_as_child(the_app_files->frame));
error_text_frame:
	memcpy(&sysfont, &the_app_files->old_sysfont, sizeof(struct font));
	membag_free(the_app_files);
error_membag_alloc:
	app_desktop_restart();
}

//! @}

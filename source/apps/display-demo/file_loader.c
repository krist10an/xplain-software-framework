/**
 * \file
 *
 * \brief Display Xplained file loader
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
#include <dma.h>
#include <hugemem.h>
#include <physmem.h>
#include <mainloop.h>
#include <status_codes.h>

#include <board/physmem.h>

#include <fs/tsfs.h>

#include <gfx/gfx.h>
#include <gfx/win.h>
#include <gfx/wtk.h>
#include <gfx/sysfont.h>

#include "file_loader.h"

/**
 * \weakgroup appsutil_fileloader_group
 *
 * @{
 */

#define MAX_LOAD_PIXELS         64
#define MAX_LOAD_SIZE           (MAX_LOAD_PIXELS * sizeof(gfx_color_t))

extern struct tsfs              myfs;

struct file_loader {
	struct tsfs_file        file;
	struct workqueue_task   task;
	struct workqueue_task   *done_task;

	/*
	 * Since the file loader uses either the hugemem_ptr_t variables or the
	 * gfx_coord_t variables, the memory footprint can be optimized by
	 * making these an union.
	 */
	union {
		hugemem_ptr_t           hugemem_address;
		struct {
			gfx_coord_t     current_x;
			gfx_coord_t     current_y;
		};
	};
	union {
		uint32_t                offset;
		struct {
			gfx_coord_t     offset_x;
			gfx_coord_t     offset_y;
		};
	};

	gfx_coord_t             width;
	gfx_coord_t             height;

	uint16_t                load_size;
	uint8_t                 buffer[MAX_LOAD_SIZE];
	bool                    busy;
};

static struct file_loader      the_file_loader;

/**
 * \brief Load file data directly to screen worker
 *
 * This worker function puts the data fetched from the file system directly to
 * the screen.
 *
 * \param task Pointer to the current work queue task
 */
static void load_to_screen_worker(struct workqueue_task *task)
{
	struct file_loader      *floader = &the_file_loader;
	enum status_code        result;

	gfx_set_clipping(0, 0, gfx_get_width(), gfx_get_height());

	gfx_put_pixmap((gfx_color_t *)floader->buffer,
			floader->load_size,
			0,
			0,
			floader->current_x + floader->offset_x,
			floader->current_y + floader->offset_y,
			floader->load_size,
			1);

	floader->current_x += floader->load_size;
	if (floader->current_x >= floader->width) {
		floader->current_y++;
		floader->current_x = 0;
	}

	floader->load_size = min_u((floader->width - floader->current_x),
			MAX_LOAD_PIXELS);

	result = tsfs_read(&myfs, &floader->file, &floader->buffer,
			floader->load_size * sizeof(gfx_color_t),
			&floader->task);

	// If file we are done, run the image done task
	if ((result != STATUS_OK) || (floader->current_y > floader->height)) {
		floader->busy = false;

		if (floader->done_task)
			workqueue_add_task(&main_workqueue, floader->done_task);
	}
}

/**
 * \brief Load file data to hugemem worker
 *
 * This worker function puts the data fetched from the file system into the
 * allocated hugemem section.
 *
 * \param task Pointer to the current work queue task
 */
static void load_to_hugemem_worker(struct workqueue_task *task)
{
	struct file_loader      *floader = &the_file_loader;
	enum status_code        result = STATUS_OK;
	uint32_t                file_size;

	file_size = floader->file.end - floader->file.start;

	hugemem_write_block((hugemem_ptr_t)((uint32_t)floader->hugemem_address +
			floader->offset), floader->buffer, floader->load_size);

	floader->offset += floader->load_size;
	floader->load_size = min_u(file_size - floader->offset,
			MAX_LOAD_SIZE);

	if (floader->load_size) {
		result = tsfs_read(&myfs, &floader->file, &floader->buffer,
				floader->load_size, &floader->task);
	}

	if (!floader->load_size || result != STATUS_OK) {
		floader->busy = false;

		if (floader->done_task)
			workqueue_add_task(&main_workqueue,
					floader->done_task);
	}
}

/**
 * \brief Load file data directly to screen.
 *
 * This function opens a file from the DataFlash and loads the file data
 * directly to the screen.
 *
 * \param filename Name of file to load from file system.
 * \param pos_x X position on screen to start putting data
 * \param pos_y Y position on screen to start putting data
 * \param width Width in pixels for area to output data to
 * \param height Height in pixels for area to output data to
 * \param done_task Work queue task to callback when complete
 *
 * \return \ref STATUS_OK if successful or \ref status_code on error
 */
enum status_code load_file_to_screen(const char *filename, gfx_coord_t pos_x,
		gfx_coord_t pos_y, gfx_coord_t width, gfx_coord_t height,
		struct workqueue_task *done_task)
{
	struct file_loader      *floader = &the_file_loader;
	enum status_code        result;

	if (!tsfs_is_ready(&myfs))
		return ERR_IO_ERROR;

	if (floader->busy)
		return ERR_BUSY;

	floader->current_x      = 0;
	floader->current_y      = 0;
	floader->offset_x       = pos_x;
	floader->offset_y       = pos_y;
	floader->height         = height;
	floader->width          = width;

	result = tsfs_open(&myfs, filename, &floader->file);

	if (result != STATUS_OK)
		return ERR_INVALID_ARG;

	floader->busy = true;
	floader->done_task = done_task;

	floader->load_size = min_u(floader->width, MAX_LOAD_PIXELS);

	workqueue_task_set_work_func(&floader->task, load_to_screen_worker);

	result = tsfs_read(&myfs, &floader->file, &floader->buffer,
			floader->load_size * sizeof(gfx_color_t),
			&floader->task);

	return result;
}

/**
 * \brief Allocate space in hugemem and load an image into it.
 *
 * This function will allocate enough space in hugemem to contain the image
 * file and load the file from DataFlash and into the hugemem area allocated.
 *
 * \param filename Name of a file on the file system
 * \param task Pointer to work queue task to callback when done loading
 *
 * \return hugemem pointer to file data or \ref HUGEMEM_NULL if something went
 *                 wrong
 */
hugemem_ptr_t load_file_to_hugemem(const char *filename,
		struct workqueue_task *task)
{
	struct file_loader      *floader = &the_file_loader;
	enum status_code        status;
	hugemem_ptr_t           retval;
	uint32_t                file_size;

	if (!tsfs_is_ready(&myfs) || floader->busy)
		return HUGEMEM_NULL;

	status = tsfs_open(&myfs, filename, &floader->file);
	if (status != STATUS_OK)
		return HUGEMEM_NULL;

	file_size = floader->file.end - floader->file.start;

	retval = hugemem_alloc(&board_extram_pool, file_size, CPU_DMA_ALIGN);
	if (retval == HUGEMEM_NULL)
		return retval;

	floader->busy            = true;
	floader->done_task       = task;
	floader->offset          = 0;
	floader->hugemem_address = retval;
	floader->load_size       = min_u(file_size, MAX_LOAD_SIZE);

	workqueue_task_set_work_func(&floader->task, load_to_hugemem_worker);

	status = tsfs_read(&myfs, &floader->file, &floader->buffer,
			floader->load_size, &floader->task);
	if (status != STATUS_OK)
		return HUGEMEM_NULL;

	return retval;
}

/**
 * \brief Check if the file loader is busy loading a file.
 *
 * \retval true when busy loading a file
 * \retval false when not loading a file
 */
bool file_loader_busy()
{
	struct file_loader *floader = &the_file_loader;
	return floader->busy;
}

/**
 * \brief Initialize the file loader
 */
void file_loader_init()
{
	struct file_loader *floader = &the_file_loader;
	workqueue_task_init(&floader->task, load_to_screen_worker);
}

//! @}

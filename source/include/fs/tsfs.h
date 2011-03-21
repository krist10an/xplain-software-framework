/**
 * \file
 *
 * \brief Tiny Simple File System (TSFS) header file
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
#ifndef FS_TSFS_H
#define FS_TSFS_H

#include <stdint.h>
#include <mainloop.h>
#include <status_codes.h>

#include <block/device.h>

#ifdef CONFIG_FS_TSFS_USE_HUGEMEM
# include <hugemem.h>
#endif

/**
 * \defgroup tsfs_group Tiny Simple File System (TSFS), block device based
 * file system.
 *
 * This module provides mechanisms for data stored in a TSFS file system.
 * Note: All data is stored using big endigan encoding, and translated to
 * CPU specific endianess after being read into the file system.
 *
 * In order to improve execution times most functions in this module assume
 * input parameters are clean. This means that there are no sanity on any
 * parameters unless it is explisitly stated in this document.
 * @{
 */

//! Unique ID to identify TSFS.
#define TSFS_ID                 0x17c1
//! Max characters in a filename.
#define TSFS_FILENAME_LEN       8

#ifdef CONFIG_FS_TSFS_USE_HUGEMEM
//! Max number of files supported when hugemem is enabled.
# define TSFS_MAX_FILES         256
#else
//! Max number of files supported.
# define TSFS_MAX_FILES         31
#endif

//! Size of block in bytes.
#define TSFS_BLOCKSIZE          512

#define TSFS_FILETABLE_ENTRIES_PER_BLOCK \
	(TSFS_BLOCKSIZE / (sizeof(struct tsfs_filetable_entry)))

/**
 * \brief Holds information about a specific file within a file system
 */
struct tsfs_file {
	//! Address in storage where file starts
	uint32_t        start;
	//! Address in storage where file ends
	uint32_t        end;
	//! Pointer to next data to be read
	uint32_t        cursor;
};

/**
 * \brief Header structure. Holds generic information about the file system.
 */
struct tsfs_header {
	//! Two unique ID bytes to identify TSFS
	uint16_t        id;
	//! TSFS version
	uint8_t         version;
	//! Rereved for future use
	uint8_t         reserved1;
	//! Size of entire volume, including header
	uint32_t        volume_size;
	//! Number of files in the system
	uint32_t        nr_files;
	//! Rereved for future use
	uint32_t        reserved2;
};

/**
 * \brief Holds current read request being processed by a file system.
 */
struct tsfs_read_request {
	/**
	 * Pointer to where data is stored. This buffer is incremented as data
	 * is read.
	 */
	void                    *buffer;
	//! Pointer to next data byte to be read
	uint32_t                cursor;
	//! Number of bytes remaining in transfer
	uint32_t                remaining_bytes;
	//! Task to be scheduled after operation is complete.
	struct workqueue_task   *task;
};

/**
 * \brief Holds information on one file in the file table.
 */
struct tsfs_filetable_entry {
	//! Offset in media file starts
	uint32_t        file_offset;
	//! Size of file in bytes
	uint32_t        file_size;

	/**
	* \brief Name of the file on the volume.
	*
	* The name can be no longer than \ref TSFS_FILENAME_LEN. There is no
	* need for a trailing null byte ('\\0'). It is also possible for two
	* files to have the same filename.
	*/
	uint8_t         filename[TSFS_FILENAME_LEN];
};

/**
 * \brief Holds information on a TSFS instance.
 */
struct tsfs {
	//! Current state of file system, for valid states see enum tsfs_status.
	enum status_code                status;
	//! TSFS header data.
	struct tsfs_header              header;

	//! Table over all files present in file system. V1 max is 31 files.
#ifdef CONFIG_FS_TSFS_USE_HUGEMEM
	hugemem_ptr_t                   filetable_address;
	uint16_t                        filetable_entries_read;
#else
	struct tsfs_filetable_entry     filetable[TSFS_MAX_FILES];
#endif

	//! Pointer to file system's associated block device.
	struct block_device             *bdev;

	//! Current block issued request.
	struct block_request            *current_breq;

	//! Buffer to hold block page cache.
	uint8_t                         *buffer_data;

	//! Block buffer metadata.
	struct buffer                   buffer;

	//! Number of block-page currently in the \ref tsfs::buffer.
	block_addr_t                    lba_in_buf;

	//! Read request currently being handled.
	struct tsfs_read_request        current_read_request;

	//! Internal callback when block device completes a read operation.
	void(*page_read_callback)(struct tsfs *tsfs);
};

/**
 * \brief The relative starting point of seek operations
 *
 */
enum tsfs_seek_origin {
	//! Seek relative to beginning of file
	SEEK_SET,
	//! Seek relative to current cursor position
	SEEK_CUR,
	//! Seek relative to end of file
	SEEK_END,
};

status_t tsfs_init(struct tsfs *tsfs, struct block_device *system_blockdevice,
		struct workqueue_task *system_ready_task);

status_t tsfs_open(struct tsfs *tsfs, const char *filename,
		struct tsfs_file *filehandle);

status_t tsfs_seek(struct tsfs_file *file, int32_t offset,
		enum tsfs_seek_origin origin);

status_t tsfs_read(struct tsfs *tsfs, struct tsfs_file *file,
		void *buffer, uint32_t length,
		struct workqueue_task *task);

void tsfs_get_filename(struct tsfs *tsfs, uint_fast8_t file_index,
		uint8_t *buffer);

/**
 * \brief Get the file size in bytes for file \a file
 *
 * \return Size given in bytes for file \a file
 */
static inline uint32_t tsfs_get_file_size(struct tsfs_file *file)
{
	return (file->end - file->start);
}

/**
 * \brief Returns the number of files present in a file system
 *
 * \return The number of files in the file system \a tsfs
 */
static inline uint32_t tsfs_nr_files(struct tsfs *tsfs)
{
	return tsfs->header.nr_files;
}

/**
 * \brief Returns the size of the entire volume
 *
 * \return The size of file system \a tsfs
 */
static inline uint32_t tsfs_volume_size(struct tsfs *tsfs)
{
	return tsfs->header.volume_size;
}

/**
 * \brief Check if file system is ready for use
 *
 * \return Returns true if a file system \a tsfs is ready for use
 */
static inline uint32_t tsfs_is_ready(struct tsfs *tsfs)
{
	return (tsfs->bdev && tsfs->status == STATUS_OK);
}

//! @}

#endif /* FS_TSFS_H */

/**
 * \file
 *
 * \brief Block device core functionality
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
#include <assert.h>
#include <debug.h>
#include <malloc.h>
#include <string.h>
#include <types.h>
#include <block/device.h>

/**
 * \brief Allocate a block request
 *
 * \param bdev Block device which will handle the request
 *
 * \return A new block request object, or NULL if sufficient memory
 * isn't available.
 */
struct block_request *block_alloc_request(struct block_device *bdev)
{
	struct block_request	*req;

	assert(bdev);
	assert(bdev->alloc_req);

	req = bdev->alloc_req(bdev);
	if (likely(req))
		req->bdev = bdev;
	else
		dbg_warning("block: failed to allocate request from dev %p\n",
				bdev);

	return req;
}

/**
 * \brief Free a block request previously allocated using
 * block_alloc_request().
 *
 * \param bdev The block device for which \a req was allocated
 * \param req The block request object to be freed.
 */
void block_free_request(struct block_device *bdev, struct block_request *req)
{
	assert(bdev->free_req);

	if (!req)
		return;

	assert(bdev == req->bdev);

	bdev->free_req(bdev, req);
}

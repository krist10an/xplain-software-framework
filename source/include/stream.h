/**
 * \file
 *
 * \brief Character stream interface
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
#ifndef STREAM_H_INCLUDED
#define STREAM_H_INCLUDED

#include <assert.h>
#include <ring.h>
#include <stdarg.h>
#include <types.h>

/**
 * \defgroup stream Character Streams
 *
 * A <em>character stream</em> is a FIFO queue of characters. At the
 * front end, characters are inserted into the queue (the <em>stream
 * buffer</em>), while at the back end, characters are extracted from
 * the queue and usually processed in some way (e.g. sent to a hardware
 * device).
 *
 * The helper functions defined in this module ensure that the stream
 * backend driver is notified when new data is inserted into the buffer,
 * and they also wait for the backend to process some of the data when
 * the buffer becomes too full to insert all the requested data.
 *
 * @{
 */

/**
 * \brief A generic byte stream.
 *
 * A byte stream is a queue of byte-size characters. This structure
 * keeps track of the current stream state and allows safe insertion and
 * extraction of characters.
 */
struct stream {
	//! Stream backend operations
	const struct stream_ops	*ops;
	//! Character ring buffer state
	struct ring_head	ring;
	//! Size of the ring buffer minus one
	const unsigned int	ring_mask;
	//! Character data storage
	char			*data;
};

int stream_vprintf(struct stream *stream, const char *format, va_list ap);
int stream_printf(struct stream *stream, const char *format, ...)
	__printf_format(2, 3);
int stream_putstr(struct stream *stream, const char *str);
int stream_putchar(struct stream *stream, int c);

int snprintf(char *str, size_t size, const char *format, ...)
	__printf_format(3, 4);
int sprintf(char *str, const char *format, ...) __printf_format(2, 3);

/**
 * \defgroup stream_impl Character Stream Implementation
 * @{
 */

/**
 * \brief Stream backend operations
 *
 * These operations are implemented by the stream backend responsible
 * for consuming the character data.
 */
struct stream_ops {
	/**
	 * \brief Commit the stream buffer.
	 *
	 * Called after new data has been inserted into the stream
	 * buffer and the backend should start consuming them. The
	 * stream backend is not required to consume all the data before
	 * returning; it is enough to e.g. initiate a hardware transfer
	 * which will eventually consume the buffer.
	 *
	 * \param stream The stream which just had data inserted into it
	 */
	void (*commit)(struct stream *stream);
	/**
	 * \brief Try to make room for additional character data
	 *
	 * This function should try to consume data from the stream
	 * until there are \a goal bytes available for new character
	 * data. It may return earlier if this goal cannot be met (for
	 * example if \a goal is larger than the buffer size), but for
	 * performance reasons, it is recommended to get as close to the
	 * goal as possible.
	 *
	 * For asynchronous backends, it is also recommended to let the
	 * hardware keep consuming data even after the goal has been
	 * met, as it may eliminate the need to call this function
	 * later.
	 *
	 * \param stream The stream in which to make room
	 * \param goal The number of free bytes needed
	 *
	 * \retval true There is room for at least one byte in the
	 *	stream buffer
	 * \retval false The underlying buffer is completely full, and
	 *	the stream backend will not be able to consume any of
	 *	the data.
	 */
	bool (*make_room)(struct stream *stream, unsigned int goal);
};

//! Return the size of the buffer associated with \a stream
static inline unsigned int stream_buf_size(struct stream *stream)
{
	return stream->ring_mask + 1;
}

//! Return true if \a stream contains any data which can be extracted
static inline bool stream_buf_has_data(struct stream *stream)
{
	return !ring_is_empty(&stream->ring);
}

//! Return true if \a stream currently cannot accept any more data
static inline bool stream_buf_is_full(struct stream *stream)
{
	return ring_is_full(&stream->ring, stream_buf_size(stream));
}

//! Return the number of bytes available for insertion into \a stream
static inline unsigned int stream_buf_unused(struct stream *stream)
{
	return ring_entries_unused(&stream->ring, stream_buf_size(stream));
}

//! Return the number of bytes available for extraction from \a stream
static inline unsigned int stream_buf_used(struct stream *stream)
{
	return ring_entries_used(&stream->ring);
}

//! Return the number of unused bytes before the buffer wraps around
static inline unsigned int stream_buf_unused_before_end(struct stream *stream)
{
	return ring_entries_unused_before_end(&stream->ring,
			stream_buf_size(stream));
}

/**
 * \brief Return the number of bytes available for extraction from \a
 * stream before the buffer wraps around
 */
static inline unsigned int stream_buf_used_before_end(struct stream *stream)
{
	return ring_entries_used_before_end(&stream->ring,
			stream_buf_size(stream));
}

/**
 * \brief Return the \em head index of \a stream, i.e. the index at
 * which to insert the next byte.
 */
static inline unsigned int stream_buf_head(struct stream *stream)
{
	return ring_get_head(&stream->ring, stream_buf_size(stream));
}

/**
 * \brief Return the \em tail index of \a stream, i.e. the index at
 * which to extract the next byte.
 */
static inline unsigned int stream_buf_tail(struct stream *stream)
{
	return ring_get_tail(&stream->ring, stream_buf_size(stream));
}

/**
 * \brief Insert the character \a c at the head of \a stream
 * \pre \a stream has room for at least one more character
 */
static inline char stream_buf_insert_char(struct stream *stream, char c)
{
	assert(!stream_buf_is_full(stream));

	stream->data[stream_buf_head(stream)] = c;
	ring_insert_entries(&stream->ring, 1);

	return c;
}

/**
 * \brief Extract one character from the tail of \a stream
 * \pre \a stream contains at least one valid character
 */
static inline char stream_buf_extract_char(struct stream *stream)
{
	char	c;

	assert(stream_buf_has_data(stream));

	c = stream->data[stream_buf_tail(stream)];
	ring_extract_entries(&stream->ring, 1);

	return c;
}

//! @}
//! @}

#endif /* STREAM_H_INCLUDED */

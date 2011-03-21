/**
 * \file
 *
 * \brief Generic stream formatted print implementation
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
#include <util.h>
#include <string.h>
#include <stream.h>
#include <compiler.h>

/**
 * \internal
 * \brief Internal string stream buffer.
 */
#define BUFFER_STREAM_BUFFER_SIZE       32

/**
 * \internal
 * \brief string stream object to convert from a string buffer to a struct
 *        stream object.
 */
struct string_stream {
	/** Stream object. */
	struct stream   stream;
	/** Offset in the string stream to write data. */
	size_t          offset;
	/** Maximum size of the string stream for snprintf. */
	size_t          size;
	/** Pointer to the output string stream. */
	char            *string;
	/** Internal buffer for the stream object. */
	char            buffer[BUFFER_STREAM_BUFFER_SIZE];
};

/**
 * \internal
 * \brief Return the buffer stream containing the stream \a s
 */
static inline struct string_stream *string_stream_of(struct stream *s)
{
	return container_of(s, struct string_stream, stream);
}

/**
 * \internal
 * \see stream_ops::commit
 */
static void snprintf_stream_commit(struct stream *s)
{
	struct string_stream    *ss = string_stream_of(s);

	/* Make sure no data outside the output string buffer is written. */

	while (stream_buf_has_data(s) && ss->offset < ss->size) {
		ss->string[ss->offset] = stream_buf_extract_char(s);
		/*
		 * Move data offset in buffer to point to next available byte.
		 */
		ss->offset++;
	}

	/* Compensate offset for trailing null byte, i.e., add 1. */
	if (stream_buf_has_data(s) && (ss->offset + 1) >= ss->size) {
		/*
		 * If there still is data in the stream, do dummy read to free
		 * up the private buffer. Basically toss away the remaining
		 * characters.
		 */
		while (stream_buf_has_data(s)) {
			stream_buf_extract_char(s);
		}
	}
}

/**
 * \internal
 * \see stream_ops::commit
 */
static void sprintf_stream_commit(struct stream *s)
{
	struct string_stream    *ss = string_stream_of(s);

	while (stream_buf_has_data(s)) {
		ss->string[ss->offset] = stream_buf_extract_char(s);
		/*
		 * Move data offset in buffer to point to next available byte.
		 */
		ss->offset++;
	}
}

/**
 * \internal
 * \see stream_ops::make_room
 */
static bool string_stream_make_room(struct stream *s, unsigned int goal)
{
	s->ops->commit(s);
	return true;
}

static const struct stream_ops snprintf_stream_ops = {
	.commit         = snprintf_stream_commit,
	.make_room      = string_stream_make_room,
};

static const struct stream_ops sprintf_stream_ops = {
	.commit         = sprintf_stream_commit,
	.make_room      = string_stream_make_room,
};

/**
 * \brief Streamed formatted size limited output conversion.
 *
 * This is simply a convenience wrapper around stream_printf(), which will
 * translate a character buffer in memory into a stream object. It will at most
 * write \a size bytes to the \a str buffer from the \a format stream, and it
 * does also take a variable number of arguments.
 */
int snprintf(char *str, size_t size, const char *format, ...)
{
	int n;
	va_list ap;
	struct string_stream ss = {
		{
			.ops            = &snprintf_stream_ops,
			.ring_mask      = BUFFER_STREAM_BUFFER_SIZE - 1,
		},
		.string = str,
		.offset = 0,
		/*
		 * Subtract one from the size to have room for the trailing
		 * null byte.
		 */
		.size   = size - 1,
	};

	va_start(ap, format);

	memset(ss.buffer, 0, BUFFER_STREAM_BUFFER_SIZE);
	ss.stream.data = ss.buffer;
	n = stream_vprintf(&ss.stream, format, ap);

	va_end(ap);

	/* Add null character termination to stream. */
	if (ss.offset >= size)
		str[ss.offset - 1] = '\0';
	else
		str[ss.offset] = '\0';

	return n;
}

/**
 * \brief Streamed formatted output conversion.
 *
 * This is simply a convenience wrapper around stream_printf(), which will
 * translate a character buffer in memory into a stream object. It will write
 * to the \a str buffer from the \a format stream until it reaches the null
 * byte terminator, hence the user must be careful to avoid buffer overflow. It
 * does also take a variable number of arguments.
 */
int sprintf(char *str, const char *format, ...)
{
	int n;
	va_list ap;
	struct string_stream ss = {
		{
			.ops            = &sprintf_stream_ops,
			.ring_mask      = BUFFER_STREAM_BUFFER_SIZE - 1,
		},
		.string = str,
		.offset = 0,
	};

	va_start(ap, format);

	memset(ss.buffer, 0, BUFFER_STREAM_BUFFER_SIZE);
	ss.stream.data = ss.buffer;
	n = stream_vprintf(&ss.stream, format, ap);

	va_end(ap);

	/* Add null character termination to stream. */
	str[ss.offset] = '\0';

	return n;
}

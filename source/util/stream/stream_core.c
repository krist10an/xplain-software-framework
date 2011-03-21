/**
 * \file
 *
 * \brief Character stream core implementation
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
#include <interrupt.h>
#include <stream.h>
#include <stdarg.h>
#include <string.h>
#include <util.h>

/**
 * \weakgroup stream_impl
 * @{
 */

/**
 * \internal
 * \brief stream_vprintf() format conversion state.
 *
 * This indicates what the parser will be looking for next. The parser
 * may pass through several states for each character in the format
 * conversion spec, as many of the fields are optional.
 */
enum conversion_state {
	//! \internal Normal state, passing characters through to the console.
	STATE_NORMAL,
	//! \internal Parsing an optional conversion flag.
	STATE_FLAG,
	//! \internal Parsing an optional field width specifier.
	STATE_WIDTH,
	//! \internal Looking for a period indicating a precision specifier.
	STATE_PERIOD,
	//! \internal Parsing an optional precision specifier.
	STATE_PRECISION,
	//! \internal Parsing an optional length modifier.
	STATE_LENGTH,
	//! \internal Parsing the conversion specifier.
	STATE_CONVSPEC,
};

/**
 * \internal
 * \brief Conversion data for stream_vprintf().
 */
struct printf_conversion {
	//! Minimum field width, or 0 if unspecified.
	int width;
	//! Minimum precision, or 0 if unspecified.
	int precision;
	//! Length modifier. This can be 'h', 'l' or 0 (default.)
	char length;
	//! Conversion specifier.
	char spec;
	//! Character to use for padding to specified width
	char pad_char;
	//! Conversion argument extracted from \a ap.
	union {
		//! Signed integer argument.
		long d;
		//! Unsigned integer argument.
		unsigned long u;
		//! Floating-point argument.
		double f;
		//! String argument.
		const char *s;
		//! Pointer argument.
		void *p;
		//! Where to store the result of a %n conversion.
		int *n;
	} arg;
};

/**
 * \internal
 * \brief Write characters to the output stream.
 *
 * \param stream The stream to which the output is to be sent
 * \param data Pointer to the character buffer.
 * \param len Length of the character buffer in bytes.
 */
static void stream_priv_write(struct stream *stream,
		const char *data, size_t len)
{
	size_t partial;
	unsigned int head;
	irqflags_t iflags;

	while (len) {
		if (stream_buf_unused(stream) < len) {
			if (!stream->ops->make_room(stream, len))
				return;
		}

		iflags = cpu_irq_save();
		head = stream_buf_head(stream);
		partial = min_u(len, stream_buf_unused_before_end(stream));
		memcpy(&stream->data[head], data, partial);
		ring_insert_entries(&stream->ring, partial);
		cpu_irq_restore(iflags);

		data += partial;
		len -= partial;
	}
}

/**
 * \internal
 * \brief Send one character to the output stream
 *
 * \param stream The stream to which the output is to be sent
 * \param c Character to be inserted into the stream buffer
 */
static int stream_priv_putchar(struct stream *stream, char c)
{
	if (c == '\n')
		stream_priv_putchar(stream, '\r');

	stream_priv_write(stream, &c, 1);

	return c;
}

/**
 * \internal
 * \brief Send a nul-terminated string to the output stream
 *
 * \param stream The stream to which the output is to be sent
 * \param str Nul-terminated string to be inserted into the stream buffer
 */
static int stream_priv_putstr(struct stream *stream, const char *str)
{
	int len;

	len = strlen(str);
	stream_priv_write(stream, str, len);

	return len;
}

/**
 * \internal
 * \brief Commit the output stream
 *
 * This will tell the stream backend that the output buffer contains new
 * data, which may trigger the backend to push the data to the hardware.
 *
 * \param stream The stream into which new data has been inserted.
 */
static void stream_priv_commit(struct stream *stream)
{
	stream->ops->commit(stream);
}

/**
 * \internal
 * \brief Print a signed integer according to the conversion
 * specification.
 *
 * \param stream The stream to which the output is to be sent
 * \param conv conversion data parsed from the format string.
 *
 * \return The number of characters printed.
 */
static int stream_priv_print_signed(struct stream *stream,
		struct printf_conversion *conv)
{
	char buf[32];
	long number = conv->arg.d;
	bool negative = false;
	int i = sizeof(buf);
	int len;
	char c;

	if (number == 0)
		buf[--i] = '0';

	if (number < 0) {
		negative = true;
		number = -number;
	}

	while (number) {
		c = '0' + number % 10;
		number /= 10;
		buf[--i] = c;
	}

	if (negative)
		buf[--i] = '-';

	if (conv->width > sizeof(buf))
		conv->width = sizeof(buf);

	while ((sizeof(buf) - i) < conv->width)
		buf[--i] = conv->pad_char;

	len = sizeof(buf) - i;
	stream_priv_write(stream, buf + i, len);

	return len;
}

/**
 * \internal
 * \brief Print an unsigned integer according to the conversion
 * specification.
 *
 * \param stream The stream to which the output is to be sent
 * \param conv Conversion data parsed from the format string.
 *
 * \return The number of characters printed.
 */
static int stream_priv_print_unsigned(struct stream *stream,
		struct printf_conversion *conv)
{
	char buf[32];
	unsigned long number = conv->arg.u;
	int i = sizeof(buf);
	int len;
	char c;

	if (number == 0)
		buf[--i] = '0';

	switch (conv->spec) {
	case 'o':
		while (number) {
			c = '0' + (number & 7);
			number >>= 3;
			buf[--i] = c;
		}
		break;
	case 'u':
		while (number) {
			c = '0' + (number % 10);
			number /= 10;
			buf[--i] = c;
		}
		break;
	case 'x':
		while (number) {
			if ((number & 15) > 9)
				c = 'a' - 10 + (number & 15);
			else
				c = '0' + (number & 15);
			number >>= 4;
			buf[--i] = c;
		}
		break;
	case 'X':
		while (number) {
			if ((number & 15) > 9)
				c = 'A' - 10 + (number & 15);
			else
				c = '0' + (number & 15);
			number >>= 4;
			buf[--i] = c;
		}
		break;
	}

	if (conv->width > sizeof(buf))
		conv->width = sizeof(buf);

	while ((sizeof(buf) - i) < conv->width)
		buf[--i] = conv->pad_char;

	len = sizeof(buf) - i;
	stream_priv_write(stream, buf + i, len);

	return len;
}

//! @}

/**
 * \weakgroup stream
 * @{
 */

/**
 * \brief Write a string to a stream.
 *
 * \param stream The stream to which the output is to be sent
 * \param str NUL-terminated string.
 *
 * \return The number of characters written.
 */
int stream_putstr(struct stream *stream, const char *str)
{
	int len;

	len = stream_priv_putstr(stream, str);
	stream_priv_commit(stream);

	return len;
}

/**
 * \brief Write a single character to a stream.
 *
 * \param stream The stream to which the output is to be sent
 * \param c The character to write.
 *
 * \return \a c as an unsigned char cast to an int.
 */
int stream_putchar(struct stream *stream, int c)
{
	c = stream_priv_putchar(stream, c);
	stream_priv_commit(stream);

	return c;
}

/**
 * \brief Formatted output conversion to a stream
 *
 * Produce output according to \a format to the given stream. \a format
 * is interpreted as a regular printf()-style format string with a few
 * limitations (some specifiers are accepted but ignored.)
 *
 * \param stream The stream to which the output is to be sent
 * \param format Format specification.
 * \param ap Format arguments as a vararg list.
 *
 * \return The number of characters sent to \a stream. If \a stream
 *	signals that it cannot accept any more characters at some point,
 *	the return value indicates the number of characters that would
 *	have been sent if \a stream had accepted them.
 */
int stream_vprintf(struct stream *stream, const char *format, va_list ap)
{
	int state = STATE_NORMAL;
	struct printf_conversion conv;
	int n = 0;
	char c;

	while ((c = *format++)) {
		switch (state) {
		case STATE_NORMAL:
			if (c == '%') {
				state = STATE_FLAG;
				conv.width = 0;
				conv.precision = 0;
				conv.length = 0;
				conv.pad_char = ' ';
			} else {
				stream_priv_putchar(stream, c);
				n++;
			}
			break;

		case STATE_FLAG:
			state = STATE_WIDTH;

			/* We accept all standard flags, but we ignore some */
			switch (c) {
			case '0':
				conv.pad_char = '0';
				break;
			case '#':
			case '-':
			case ' ':
			case '+':
				break;

			case '%':
				/* %% -> output a literal '%' */
				stream_priv_putchar(stream, c);
				n++;
				state = STATE_NORMAL;
				break;

			default:
				goto state_width;
			}
			break;

		state_width:
		case STATE_WIDTH:
			if (isdigit(c) && (c != '0' || conv.width != 0)) {
				conv.width *= 10;
				conv.width += c - '0';
				break;
			}

			state = STATE_PERIOD;
			/* fall through */

		case STATE_PERIOD:
			if (c != '.') {
				state = STATE_LENGTH;
				goto state_length;
			}
			state = STATE_PRECISION;
			break;

		case STATE_PRECISION:
			/* accept but ignore */
			if (isdigit(c))
				break;

			state = STATE_LENGTH;
			/* fall through */

		state_length:
		case STATE_LENGTH:
			/* SUSv2 only accepts h, l and L */
			if (c == 'h' || c == 'l' || c == 'L') {
				conv.length = c;
				break;
			} else if (c == 'z') {
				if (sizeof(size_t) == sizeof(long))
					conv.length = 'l';
				break;
			}

			state = STATE_CONVSPEC;
			/* fall through */

		case STATE_CONVSPEC:
			conv.spec = c;

			switch (c) {
			case 'd':
			case 'i':
				if (conv.length == 'l')
					conv.arg.d = va_arg(ap, long);
				else
					conv.arg.d = va_arg(ap, int);
				n += stream_priv_print_signed(stream, &conv);
				break;
			case 'o':
			case 'u':
			case 'x':
			case 'X':
				if (conv.length == 'l')
					conv.arg.u = va_arg(ap, unsigned long);
				else
					conv.arg.u = va_arg(ap, unsigned int);
				n += stream_priv_print_unsigned(stream, &conv);
				break;
			case 'c':
				conv.arg.d = va_arg(ap, int);
				stream_priv_putchar(stream, conv.arg.d);
				n++;
				break;

			/* TODO: Handle floats if needed */

			case 's':
				conv.arg.s = va_arg(ap, const char *);
				n += stream_priv_putstr(stream, conv.arg.s);
				break;
			case 'p':
				conv.arg.p = va_arg(ap, void *);
				stream_priv_write(stream, "0x", 2);
				n += 2;
				conv.spec = 'x';
				n += stream_priv_print_unsigned(stream, &conv);
				break;
			case 'n':
				conv.arg.n = va_arg(ap, int *);
				*conv.arg.n = n;
				break;
			}

			state = STATE_NORMAL;
			break;
		}
	}

	stream_priv_commit(stream);

	return n;
}

/**
 * \brief Formatted output conversion.
 *
 * This is simply a convenience wrapper around stream_vprintf()
 * taking a variable number of arguments.
 */
int stream_printf(struct stream *stream, const char *format, ...)
{
	int n;
	va_list ap;

	va_start(ap, format);
	n = stream_vprintf(stream, format, ap);
	va_end(ap);

	return n;
}

//! @}

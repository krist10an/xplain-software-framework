/**
 * \file
 *
 * \brief Simple and efficient ring buffer implementation.
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
#ifndef RING_H_INCLUDED
#define RING_H_INCLUDED

#include <stdbool.h>

/**
 * \defgroup ring Generic Ring Buffer
 *
 * This is a generic, lockless ring buffer abstration. Generic because
 * it does not care about what kind of data is stored in the buffer, and
 * lockless because the producer and consumer states are tracked
 * separately. Therefore, the producer and consumer do not require
 * protection from each other even if they may run in different
 * contexts.
 *
 * Note however that if there are multiple producers or multiple
 * consumers running from different contexts, it may be necessary to
 * protect multiple clients on the same side from each other by,
 * for example, disabling interrupts.
 *
 * The \em head of the buffer indicates the current \em producer state,
 * i.e. the index at which the next produced object will be placed. The
 * \em tail of the buffer indicates the current \em consumer state, i.e.
 * the index of the oldest object which hasn't been consumed yet. If the
 * producer and consumer may run from different contexts (i.e. one from
 * an interrupt handler and the other from a workqueue), the caller must
 * ensure that the object in the underlying buffer is not accessed after
 * the head or tail has been updated. Both ring_extract_entries() and
 * ring_insert_entries() include an optimization barrier to ensure that
 * the compiler does not break this by reordering the data accesses.
 *
 * \section ring-restrictions Ring Buffer Restrictions
 *
 * In order to keep the ring buffer implementation efficient, there are
 * a few restrictions that must be observed by the user:
 *   - The ring buffer size must be a power of two. This is because the
 *     AND operator is used to wrap around when reaching the end of the
 *     buffer, rather than the much more expensive modulo operator.
 *   - The ring buffer size must not exceed half the maximum value of an
 *     unsigned int variable. If the entire range of unsigned int is
 *     used, there's no way to distinguish a completely filled buffer
 *     from an empty buffer.
 *
 * @{
 */

/**
 * \brief Ring buffer management data.
 *
 * This keeps track of the state of the ring buffer. The actual buffer
 * is user-defined. Typically, this struct is embedded into the same
 * struct as the ring buffer itself, but this is not a requirement.
 *
 * Note that \a head and \a tail are incremented without wrapping as
 * items are added to the buffer, so they can't be used directly as
 * offsets (they may point far outside the buffer). Always use
 * ring_get_head() and ring_get_tail(), which always return offsets
 * within the buffer, to access them.
 *
 * Also note that in order to keep the size and complexity of the code
 * down, this structure does not keep track of the size of the ring
 * buffer. The caller must pass the size as a parameter to the
 * functions which need it. This improves the code size and speed
 * significantly when the buffer size is constant.
 */
struct ring_head {
	//! Offset of the next free entry in the buffer.
	unsigned int head;
	//! Offset of the first used entry in the buffer.
	unsigned int tail;
};

#ifdef CONFIG_RING_DEBUG
#include <util.h>

ERROR_FUNC(ring_priv_bad_constant_size, "Ring size must be a power of two")

static inline void ring_priv_bad_size(unsigned int size)
{
	dbg_error("Bad ring size %u: Not a power of two!\n", size);
}

static inline void ring_priv_check_size(unsigned int size)
{
	if (!is_power_of_two(size)) {
		if (is_constant(size))
			ring_priv_bad_constant_size(size);
		else
			ring_priv_bad_size(size);
	}
}
#else
static inline void ring_priv_check_size(unsigned int size)
{

}
#endif

/**
 * \brief Get the offset of the next unused entry in the buffer.
 *
 * Note that the value returned by this function is only meaningful if
 * there actually are any unused entries in the buffer.
 *
 * \param ring The ring buffer
 * \param ring_size The total number of entries in the ring buffer
 *
 * \return The offset into the buffer of the next unused entry.
 */
static inline unsigned long ring_get_head(struct ring_head *ring,
		unsigned int ring_size)
{
	ring_priv_check_size(ring_size);

	return ring->head & (ring_size - 1);
}

/**
 * \brief Get the offset of the first used entry in the buffer.
 *
 * Note that the value returned by this function is only meaningful if
 * there actually are any used entries in the buffer.
 *
 * \param ring The ring buffer
 * \param ring_size The total number of entries in the ring buffer
 *
 * \return The offset into the buffer of the first used entry.
 */
static inline unsigned long ring_get_tail(struct ring_head *ring,
		unsigned int ring_size)
{
	ring_priv_check_size(ring_size);

	return ring->tail & (ring_size - 1);
}

/**
 * \brief Get the number of used entries in the buffer.
 *
 * \param ring The ring buffer
 *
 * \return The number of entries which can be extracted before the
 * buffer is empty.
 */
static inline unsigned int ring_entries_used(struct ring_head *ring)
{
	return ring->head - ring->tail;
}

/**
 * \brief Get the number of used entries in the buffer before it wraps.
 *
 * \param ring The ring buffer
 * \param ring_size The total number of entries in the ring buffer
 *
 * \return The number of entries which can be extracted before the
 * buffer is empty or wraps around to the beginning.
 */
static inline unsigned int ring_entries_used_before_end(struct ring_head *ring,
		unsigned int ring_size)
{
	unsigned int head = ring->head;
	unsigned int tail = ring->tail;

	ring_priv_check_size(ring_size);

	if ((head ^ tail) & ring_size)
		return ring_size - (tail & (ring_size - 1));
	else
		return head - tail;
}

/**
 * \brief Get the number of unused entries in the buffer.
 *
 * \param ring The ring buffer
 * \param ring_size The total number of entries in the ring buffer
 *
 * \return The number of entries which can be inserted before the
 * buffer is full.
 */
static inline unsigned int ring_entries_unused(struct ring_head *ring,
		unsigned int ring_size)
{
	return ring_size - ring_entries_used(ring);
}

/**
 * \brief Get the number of unused entries in the buffer before it
 * wraps.
 *
 * \param ring The ring buffer
 * \param ring_size The total number of entries in the ring buffer
 *
 * \return The number of entries which can be inserted before the
 * buffer is full or wraps around to the beginning.
 */
static inline unsigned int ring_entries_unused_before_end(
		struct ring_head *ring, unsigned int ring_size)
{
	unsigned int head = ring->head;
	unsigned int tail = ring->tail;

	ring_priv_check_size(ring_size);

	if ((head ^ tail) & ring_size)
		return ring_size + tail - head;
	else
		return ring_size - (head & (ring_size - 1));
}

/**
 * \brief Test if the ring buffer is empty.
 *
 * \param ring The ring buffer
 *
 * \retval true There are no used entries in the buffer.
 * \retval false There is at least one used entry in the buffer.
 */
static inline bool ring_is_empty(struct ring_head *ring)
{
	return ring->head == ring->tail;
}

/**
 * \brief Test if the ring buffer is full.
 *
 * \param ring The ring buffer
 * \param ring_size The total number of entries in the ring buffer
 *
 * \retval true The ring buffer is full
 * \retval false There is room for at least one more entry in the buffer
 */
static inline bool ring_is_full(struct ring_head *ring,
		unsigned int ring_size)
{
	return ring_entries_used(ring) == ring_size;
}

/**
 * \brief Insert entries into the ring buffer.
 *
 * Adjust the buffer head to account for entries being inserted into
 * the buffer. Normally, this should be called after the actual buffer
 * contents have been updated; the barrier ensures that the compiler
 * doesn't move any buffer accesses after updating the head.
 *
 * \param ring The ring buffer
 * \param nr_entries The number of entries to insert
 */
static inline void ring_insert_entries(struct ring_head *ring,
		unsigned int nr_entries)
{
	barrier();
	ring->head += nr_entries;
}

/**
 * \brief Extract entries from the ring buffer.
 *
 * Adjust the buffer tail to account for entries being extracted from
 * the buffer. Normally, this should be called after the actual buffer
 * contents have been read; the barrier ensures that the compiler
 * doesn't move any buffer accesses after updating the tail.
 *
 * \param ring The ring buffer
 * \param nr_entries The number of entries to extract
 */
static inline void ring_extract_entries(struct ring_head *ring,
		unsigned int nr_entries)
{
	barrier();
	ring->tail += nr_entries;
}

/**
 * \brief Reset the ring buffer.
 *
 * This will mark the ring buffer as empty, and move both head and
 * tail to the beginning of the buffer.
 *
 * \param ring The ring buffer
 */
static inline void ring_reset(struct ring_head *ring)
{
	ring->head = ring->tail = 0;
}

//! @}

#endif /* RING_H_INCLUDED */

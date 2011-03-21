/**
 * \file
 *
 * \brief Soft interrupt handling
 *
 * This file delegates most actual declarations and definitions to the
 * architecture, but it includes documentation of common functionality.
 *
 * \see softirq_group
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
#ifndef SOFTIRQ_H_INCLUDED
#define SOFTIRQ_H_INCLUDED

#include <bitops.h>
#include <util.h>
#include <app/softirq.h>

/**
 * \defgroup softirq_group Soft Interrupt Processing
 *
 * Soft interrupts are virtual interrupts which are defined, raised and
 * handled by software. They run at lower priority than hardware
 * interrupts, but higher priority than work queues.
 *
 * Applications which need to use soft interrupts must provide a header
 * file app/softirq.h providing a enum softirq_id with a unique value
 * for each soft interrupt ID in the system. The application must also
 * define SOFTIRQ_NR_IDS, usually as a member of this enum.
 *
 * @{
 */

/**
 * \brief Soft interrupt handler function.
 * \param data Data pointer that was passed to softirq_set_handler().
 */
typedef void (*softirq_handler_t)(void *data);

/**
 * \brief Soft interrupt descriptor
 *
 * Each soft interrupt descriptor contains a handler function along with
 * its associated data.
 */
struct softirq_desc {
	//! Function to be called when softirq is active
	softirq_handler_t	handler;
	//! Data pointer passed to the handler function
	void			*data;
};

#define SOFTIRQ_BITMAP_WORDS (div_ceil(SOFTIRQ_NR_IDS, 8 * sizeof(bit_word_t)))

extern bit_word_t softirq_priv_status[SOFTIRQ_BITMAP_WORDS];
extern struct softirq_desc softirq_priv_table[SOFTIRQ_NR_IDS];

extern void softirq_set_handler(enum softirq_id id,
		softirq_handler_t handler, void *data);

/**
 * \brief Raise the soft interrupt \a id
 *
 * Calling this function will flag soft interrupt \a id as active, which
 * will cause the associated handler to be run at the first opportunity.
 *
 * Note that if this function is called from a non-interrupt context
 * with softirqs enabled, it is undefined when the handler will actually
 * be run.
 */
static inline void softirq_raise(enum softirq_id id)
{
	atomic_set_bit(id, softirq_priv_status);
}

#include <arch/softirq.h>

/* Documentation for app/softirq.h follows */

/**
 * \enum softirq_id
 * \brief Enumeration of all soft interrupt IDs in the system.
 *
 * Application-provided enumeration of all the soft interrupt IDs in the
 * system. This enum also defines the priority of each soft interrupt --
 * the pending softirq with the lowest ID will be handled first.
 */
/**
 * \var softirq_id::SOFTIRQ_NR_IDS
 * \brief The total number of soft interrupts in the system.
 *
 * This constant is provided by the application, usually as a part of
 * the softirq_id enum, and determines the size of some internal data
 * structures used by the soft interrupt system.
 */

/* The rest of the file is common documentation for arch/softirq.h */

/**
 * \fn void softirq_poll(void)
 * \brief Check for pending soft interrupts and handle them
 *
 * This function should be called from the main loop in order to process
 * any pending soft interrupts on architectures which don't run soft
 * interrupt handlers automatically after an interrupt. On architectures
 * which do run soft interrupts handlers automatically, this function
 * does nothing.
 */
/**
 * \fn void softirq_disable(void)
 * \brief Disable softirq processing
 *
 * No pending softirqs will be processed until softirq_enable() is
 * called.
 */
/**
 * \fn void softirq_enable(void)
 * \brief Enable softirq processing
 *
 * This function will also process any softirqs raised while softirq
 * processing was disabled.
 *
 * \pre Interrupts are not masked
 */
/**
 * \fn bool softirq_is_enabled_flags(softirq_flags_t flags)
 * \brief Test if soft interrupts are enabled in \a flags
 * \param flags Saved soft interrupt state
 * \retval true If \a flags indicates soft interrups are enabled
 * \retval false If \a flags indicates soft interrupts are disabled
 */
/**
 * \fn bool softirq_is_enabled(void)
 * \brief Test if soft interrupts are enabled
 * \retval true If soft interrups are enabled
 * \retval false If soft interrupts are disabled
 */
/**
 * \fn softirq_flags_t softirq_save(void)
 * \brief Save the current soft interrupt sate and disable soft
 * interrupts.
 *
 * \return A cookie indicating the soft interrupt state before
 * disabling.
 */
/**
 * \fn void softirq_restore(softirq_flags_t flags)
 * \brief Restore saved soft interrupt state
 * \param flags A cookie returned by softirq_save()
 */

//! @}

#endif /* SOFTIRQ_H_INCLUDED */

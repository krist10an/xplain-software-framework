/**
 * \file
 *
 * \brief Main loop processing
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
#ifndef MAINLOOP_H_INCLUDED
#define MAINLOOP_H_INCLUDED

#include <slist.h>
#include <sleep.h>
#include <workqueue.h>

#ifdef CONFIG_SOFTIRQ
#include <softirq.h>
#endif

/**
 * \defgroup mainloop_group Main Loop Processing
 * @{
 */

/**
 * \brief Run the main loop
 *
 * This function will loop forever processing work queues and soft
 * interrupts.
 *
 * \note This function will enable interrupts unconditionally before
 * processing any work queue tasks.
 */
__noreturn static inline void mainloop_run(struct workqueue *queue)
{
	while (1) {
		struct workqueue_task *task;

#ifdef CONFIG_SOFTIRQ
		assert(softirq_is_enabled());
#endif

		cpu_irq_disable();
#ifdef CONFIG_SOFTIRQ
		softirq_poll();
#endif
		task = workqueue_pop_task(queue);
		if (task) {
			cpu_irq_enable();
			workqueue_run_task(task);
		} else {
			cpu_enter_sleep();
		}
	}
}

//! @}

#endif /* MAINLOOP_H_INCLUDED */

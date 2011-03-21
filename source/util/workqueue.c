/**
 * \file
 *
 * \brief Workqueue implementation
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
#include <interrupt.h>
#include <types.h>
#include <workqueue.h>

/**
 * \weakgroup workqueue_group
 * @{
 */

struct workqueue main_workqueue;

/**
 * \brief Add task to work queue
 *
 * This function adds a task to a work queue. The task structure must be
 * initialized by the caller, and only a pointer to it is stored in the
 * queue. The caller must make sure the task struct is kept intact while
 * in the queue and if necessary freed after running the task. The task
 * is removed from the queue before the worker function is called, so it
 * is safe to free or re-queue the task from the worker function.
 *
 * If \a task has already been added to some work queue, or \a task is
 * NULL, this function does nothing.
 *
 * \param queue Work queue
 * \param task Task to be added to \a queue
 *
 * \retval true The task was successfully queued
 * \retval false The task has already been queued, or is NULL, so
 * nothing was done
 */
bool workqueue_add_task(struct workqueue *queue, struct workqueue_task *task)
{
	irqflags_t	iflags;
	bool		was_queued = false;

	/* Sanity check on parameters. */
	assert(queue);

	/*
	 * Allowing callers to pass a NULL task will eliminate a few
	 * tests elsewhere and thus reduce the code size.
	 */
	if (!task)
		return false;

	/* We will run this later, so check that it is valid */
	assert(task->worker);

	iflags = cpu_irq_save();
	if (!workqueue_task_is_queued(task)) {
		slist_insert_tail(&queue->task_list, &task->node);
		was_queued = true;
	}
	cpu_irq_restore(iflags);

	return was_queued;
}

/**
 * \brief Add task to nested work queue
 *
 * This adds \a task to the nested work queue \a nwq. If there is no
 * task currently active, i.e. nwq->current is NULL, the new
 * task is immediately made active by moving it to the main workqueue
 * and assigning it to nwq->current.
 *
 * \param nwq A nested workqueue
 * \param task Task to be added to the nested work queue
 *
 * \retval true The task was successfully queued
 * \retval false The task has already been queued, so nothing was done
 */
bool nested_workqueue_add_task(struct nested_workqueue *nwq,
		struct workqueue_task *task)
{
	irqflags_t	iflags;
	bool		was_queued;

	iflags = cpu_irq_save();
	if (nwq->current) {
		was_queued = workqueue_add_task(&nwq->wq, task);
	} else {
		nwq->current = task;
		was_queued = workqueue_add_task(&main_workqueue, task);
	}
	cpu_irq_restore(iflags);

	return was_queued;
}

/**
 * \brief Switch to the next task in a nested work queue
 *
 * This removes the task at the head of the nested work queue \a nwq, if
 * any, and makes it current by adding it to the main workqueue and
 * assigning it to nwq->current. If \a nwq is empty, nwq->current is set
 * to NULL.
 *
 * \param nwq A nested workqueue
 */
void nested_workqueue_next_task(struct nested_workqueue *nwq)
{
	struct workqueue_task	*task;
	irqflags_t		iflags;

	iflags = cpu_irq_save();
	task = workqueue_pop_task(&nwq->wq);
	if (task)
		workqueue_add_task(&main_workqueue, task);
	nwq->current = task;
	cpu_irq_restore(iflags);
}

//! @}

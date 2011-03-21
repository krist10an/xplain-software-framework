/**
 * \file
 *
 * \brief Workqueue interface
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
#ifndef WORKQUEUE_H_INCLUDED
#define WORKQUEUE_H_INCLUDED

#include <types.h>
#include <slist.h>
#include <util.h>
#include <interrupt.h>

/**
 * \ingroup mainloop_group
 * \defgroup workqueue_group Work Queue Processing
 *
 * This is a workqueue designed to simplify and formalize sequential
 * execution of tasks. It provides a low overhead structure that can
 * replace or extend the use of threads in simple applications.
 *
 * @{
 */

struct workqueue_task;

/**
 * \brief Work queue worker function
 * \param task The work queue task which is currently being executed
 */
typedef void (*workqueue_func_t)(struct workqueue_task *task);

/**
 * \brief Task to be run from a work queue
 *
 * This structure may be embedded into another struct containing data
 * specific to the task. The container_of() macro is useful for
 * accessing the task-specific data.
 */
struct workqueue_task {
	workqueue_func_t	worker;	//!< Function implementing the task
	struct slist_node	node;	//!< Node in the work queue task list
};

/**
 * \brief Work queue
 *
 * This structure contains a queue of tasks to be performed whenever the
 * CPU becomes available.
 */
struct workqueue
{
	struct slist		task_list;	//!< Tasks to be executed
};


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief The main work queue
 *
 * This is the main work queue of the application. The main loop will
 * pull tasks from this and execute them one by one. It can be
 * considered a queue of tasks waiting to use the CPU resource; other
 * workqueues will typically contend for some other resource, e.g. a SPI
 * bus or flash device.
 */
extern struct workqueue main_workqueue;

/**
 * \brief Initialize a work queue
 *
 * \param queue The work queue to be initialized
 */
static inline void workqueue_init(struct workqueue *queue)
{
	/* Sanity check on parameters. */
	assert(queue);

	/* Initialize to an empty state, ready for data. */
	slist_init(&queue->task_list);
}

/**
 * \brief Change the worker function of a task
 *
 * It is safe to call this function on tasks that have already been
 * queued.
 *
 * \param task		Task to be updated
 * \param worker_func	New worker function for the task
 */
static inline void workqueue_task_set_work_func(struct workqueue_task *task,
		workqueue_func_t worker_func)
{
	task->worker = worker_func;
}

/**
 * \brief Initialize a work queue task
 *
 * This function initialize a work queue task. It should be used before
 * adding the task to any workqueue.
 *
 * \pre \a task is not queued on any work queue (not verified)
 *
 * \param task Work queue task to be initialized
 * \param worker_func Function implementing the task
 */
static inline void workqueue_task_init(struct workqueue_task *task,
		workqueue_func_t worker_func)
{
	/* Flag the item as ready for use by clearing next pointer */
	task->node.next = NULL;

	workqueue_task_set_work_func(task, worker_func);
}

/**
 * \brief Check if a work queue is empty
 *
 * \param queue Work queue
 *
 * \return true \a queue is empty (i.e. has no tasks ready to run)
 * \return false \a queue has at least one task ready to run
 */
static inline bool workqueue_is_empty(struct workqueue *queue)
{
	/* Sanity check on parameters. */
	assert(queue);

	return slist_is_empty(&queue->task_list);
}

/**
 * \brief Check if a work queue task has been queued
 *
 * \retval true \a task has already been added to a workqueue
 * \retval false \a task has not been added to any workqueue
 */
static inline bool workqueue_task_is_queued(struct workqueue_task *task)
{
	return task->node.next != NULL;
}

extern bool workqueue_add_task(struct workqueue *queue,
		struct workqueue_task *task);

/**
 * \brief Remove task from front of work queue
 *
 * This function removes one task from the front of a work queue and
 * returns a pointer to that item. The memory allocated to the item
 * struct will not be freed. If the queue is empty, a NULL pointer will
 * be returned.
 *
 * \param queue Work queue
 *
 * \return Pointer to the task that was removed, or NULL if \a queue is
 * empty.
 *
 * \pre Interrupts are disabled
 */
static inline struct workqueue_task *workqueue_pop_task(
		struct workqueue *queue)
{
	struct workqueue_task	*task = NULL;

	/* Sanity check on parameters. */
	assert(queue);

	assert(!cpu_irq_is_enabled());

	if (!workqueue_is_empty(queue)) {
		task = slist_pop_head(&queue->task_list,
				struct workqueue_task, node);
		/* Flag the item as ready for use by clearing next pointer */
		task->node.next = NULL;
	}

	return task;
}

/**
 * \brief Run a work queue task
 * \param task The work queue task to be run
 */
static inline void workqueue_run_task(struct workqueue_task *task)
{
	task->worker(task);
}

/**
 * \name Nested Workqueues
 * Nested workqueues are workqueues specific to certain shared
 * resources, for example a bus driver which can only handle one request
 * at a time. Such drivers may create a workqueue for keeping track of
 * tasks wanting to use the shared resource, and move them one by one
 * into the main workqueue.
 */
//@{

/**
 * \brief Nested Work Queue
 *
 * This structure represents a queue of tasks to be performed one at a
 * time, possibly through several iterations in the main work queue.
 */
struct nested_workqueue {
	//! The queue of tasks waiting to run
	struct workqueue	wq;
	//! The currently running task
	struct workqueue_task	*current;
};

/**
 * \brief Initialize a nested workqueue
 *
 * \param wq Nested workqueue to inititalize
 */
static inline void nested_workqueue_init(struct nested_workqueue *wq)
{
	wq->current = NULL;
	workqueue_init(&wq->wq);
}

extern bool nested_workqueue_add_task(struct nested_workqueue *wq,
		struct workqueue_task *task);
extern void nested_workqueue_next_task(struct nested_workqueue *wq);

//@}

#ifdef __cplusplus
} /* extern "C" */
#endif

//! @}

#endif /* WORKQUEUE_H_INCLUDED */

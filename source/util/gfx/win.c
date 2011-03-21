/**
 * \file
 *
 * \brief Graphical Window system
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

/**
 * \weakgroup gfx_win
 * @{
 */

#include <stdint.h>
#include <stdlib.h>
#include "assert.h"
#include "gfx/win.h"
#include <membag.h>
#include <ring.h>
#include <workqueue.h>

#ifdef CONFIG_GFX_WIN_USE_TOUCH
# include <touch/touch.h>
#endif

/**
 * \brief Window control data.
 *
 * In addition to the attribute struct defined public, it also includes
 * the required members for it to be a part of the parent's linked list
 * of children, and also the mapping status. If mapped, the window will
 * be visible if the parent is visible.
 */
struct win_window {
	//! Window attributes.
	struct win_attributes attributes;

	//! Parent window.
	struct win_window *parent;
	//! Previous sibling in parent's linked list.
	struct win_window *prev_sibling;
	//! Next sibling in parent's linked list.
	struct win_window *next_sibling;
	//! Top child of this window, start of list.
	struct win_window *top_child;

	//! True if this window should be visible if parent is.
	bool is_mapped;
};



/**
 * \internal
 * \brief Event queue control struct.
 *
 * This struct contains the control data for the circular event queue.
 * The event queue is used to store all input events and command events.
 * Other events, like DRAW and DESTROY, are sent directly to the window.
 */
struct win_event_queue {
	struct ring_head        ring;
	//! Workqueue task for handling event queue
	struct workqueue_task   task;
	struct win_event        buffer[WIN_EVENT_QUEUE_SIZE];
};



/*
 * Private prototypes are required due to circular references of the functions.
 */

//! Return true if this window and all parents and grand parents are visible.
static bool win_is_visible(const struct win_window *win);

//! Draw the parts of the window covered by dirty_area, and all covering windows.
static void win_draw(
		const struct win_window *win,
		const struct win_area *dirty_area);

//! Draw parent window, with all child windows, but limited to dirty area.
static void win_draw_parent(
		const struct win_window *child,
		const struct win_area *dirty_area);

//! Draw window background and contents, including children, limited to clip.
static void win_draw_contents(
		const struct win_window *win,
		const struct win_clip_region *clip);

//! Draw a child window, automatically translating clipping region from parent.
static void win_draw_child(
		const struct win_window *child,
		const struct win_clip_region *parentClip);

//! Translate an area to coordinates relative to parent's origin.
static bool win_translate_area_to_parent(
		struct win_area *area,
		const struct win_window *parent);

//! Destroy child windows, freeing allocated memory.
static void win_destroy_children(struct win_window *win);

//! Remove window from parent's linked list.
static void win_unlink(struct win_window *win);

//! Add child window at the start of parent's linked list.
static void win_add_top_child(
		struct win_window *parent,
		struct win_window *child);

//! Helper function to process pointer event, called from win_process_events().
static void win_process_pointer_event(struct win_pointer_event *event);

//! Ask window to process pointer event, or pass it on to correct child.
static bool win_handle_pointer_event(
		struct win_window *win,
		const struct win_pointer_event *event,
		const struct win_point *pos);

//! Call window event handler if any, return value from handler, or false.
static bool win_handle_event(
		struct win_window *win,
		enum win_event_type type,
		const void *data);

#ifdef CONFIG_GFX_WIN_USE_TOUCH
//! Create and queue a pointer event from a touch event.
static void win_queue_touch_event(const struct touch_event *event);
#endif

//! Add an event to the internal queue.
static void win_queue_event(const struct win_event *event);

//! Worker function to be added to main work queue, calls win_process_events().
static void win_event_worker(struct workqueue_task *task);




//! Root window, i.e. desktop window.
static struct win_window win_root;

//! Window event queue.
static struct win_event_queue win_event_queue;

//! Diagnostic value counting number of dropped events due to event queue full.
static uint32_t win_num_dropped_events;

//! Current pointer grabbing window, or NULL. Grabber gets all pointer events.
static struct win_window *win_pointer_grabber;
//! Current keyboard focus, or NULL. Keyboard focus gets all keyboard events.
static struct win_window *win_keyboard_focus;

//! Stores last pointer event position, in absolute coordinates.
static struct win_point win_last_pointer_pos;

//! Frame background bitmap
static struct gfx_bitmap win_root_background = {
		.type = BITMAP_SOLID,
		.data.color = GFX_COLOR(0, 0, 0),
};


/**
 * This function initializes the windowing system. Call this function before
 * any other window system operations, as this initializes the root window,
 * internal queues and other internals.
 */
void win_init(void)
{
	// Setup root window attributes to fill entire screen, no event handler.
	win_root.attributes.area.size.x = gfx_get_width();
	win_root.attributes.area.size.y = gfx_get_height();
	win_root.attributes.background = &win_root_background;

	// Initialize the event queue
	build_assert(!(WIN_EVENT_QUEUE_SIZE & (WIN_EVENT_QUEUE_SIZE - 1)));
	workqueue_task_init(&win_event_queue.task, win_event_worker);

	// Global states.
	win_keyboard_focus = &win_root;
	win_last_pointer_pos.x = gfx_get_width() / 2;
	win_last_pointer_pos.y = gfx_get_height() / 2;

#ifdef CONFIG_GFX_WIN_USE_TOUCH
	// Hook into touch driver.
	touch_set_event_handler(win_queue_touch_event);
#endif
}


/**
 * Return a pointer to the root window. Use this to change the attributes
 * of the root window, or use the root window as a parent for new windows.
 *
 * \return  Pointer to root window.
 */
struct win_window *win_get_root(void)
{
	return &win_root;
}


/**
 * Return pointer to parent window, or NULL for root window.
 *
 * \param  win  Child window.
 *
 * \return  Pointer to parent window.
 */
struct win_window *win_get_parent(const struct win_window *win)
{
	return win->parent;
}


/**
 * This function resets the root window size to fill the entire screen. Use
 * this function to reconfigure the root window after a change in screen
 * orientation.
 */
void win_reset_root_geometry(void)
{
	// Reset to new fullscreen size.
	win_root.attributes.area.size.x = gfx_get_width();
	win_root.attributes.area.size.y = gfx_get_height();

	// Send size change event.
	win_attribute_mask_t mask = WIN_ATTR_SIZE;
	win_handle_event(&win_root, WIN_EVENT_ATTRIBUTES, &mask);

	// Now, redraw everything, if it is visible.
	win_redraw(&win_root);
}


/*!
 * This function updates one or more of a window's attributes. Only the
 * the attributes corresponding to the flags of the attribute mask are
 * updated. The others are left untouched.
 *
 * \param  win            Pointer to window that will be updated.
 * \param  new_attributes  Pointer to new attribute values.
 * \param  attribute_mask  Attribute flags selecting which attributes to update.
 */
void win_set_attributes(struct win_window *win,
		const struct win_attributes *new_attributes,
		win_attribute_mask_t attribute_mask)
{
	// Don't need redraw until some attribute change calls for it.
	bool needs_redraw = false;
	struct win_area dirty_area;

	// Sanity check.
	assert(win != NULL);
	assert(new_attributes != NULL);

	// Keep track of which areas are updated. Start with an area equal to
	// to the window itself, possibly extending it on move or resize.
	dirty_area = win->attributes.area;

	// Change background if requested.
	if (attribute_mask & WIN_ATTR_BACKGROUND) {
		win->attributes.background = new_attributes->background;

		needs_redraw = true;
	}

	// Change event handler if requested.
	if (attribute_mask & WIN_ATTR_EVENTHANDLER)
		win->attributes.event_handler = new_attributes->event_handler;

	// Change behavor setting if requested.
	if (attribute_mask & WIN_ATTR_BEHAVIOR)
		win->attributes.behavior = new_attributes->behavior;

	// Change custom data if requested.
	if (attribute_mask & WIN_ATTR_CUSTOM)
		win->attributes.custom = new_attributes->custom;

	/*
	 * If position and/or size change is requested, hand over
	 * control to the win_set_area function, which will also take
	 * care of events and redraw.
	 */
	if (attribute_mask & (WIN_ATTR_POSITION | WIN_ATTR_SIZE)) {
		win_set_area(win, &(new_attributes->area), attribute_mask);
	} else {
		win_handle_event(win, WIN_EVENT_ATTRIBUTES, &attribute_mask);

		/*
		 * Redraw if one or more changes called for it, and it
		 * is visible.
		 */
		if (needs_redraw && win_is_visible(win))
			win_draw(win, &dirty_area);
	}
}


/**
 * This function updates the window position and/or size. The flags of
 * the attribute mask decides what to update.
 * Note that you cannot change the position or size of the root window,
 * since the root window is always fullscreen.
 *
 * \param  win             Pointer to window that will be updated.
 * \param  new_area        Pointer to new area attributes.
 * \param  attribute_mask  Attribute flags selecting which attributes to update.
 */
void win_set_area(struct win_window *win,
		const struct win_area *new_area,
		win_attribute_mask_t attribute_mask)
{
	// Nothing underneath is exposed unless move or resize to smaller.
	bool exposed_areas = false;
	struct win_area dirty_area;

	// Sanity check.
	assert(win != NULL);
	assert(win != &win_root);
	assert(new_area != NULL);

	/*
	 * Keep track of which areas are updated. Start with an area
	 * equal to to the window itself, extending it on move or
	 * resize.
	 */
	dirty_area = win->attributes.area;

	// Change position if requested.
	if (attribute_mask & WIN_ATTR_POSITION) {
		win->attributes.area.pos = new_area->pos;
		win_compute_union(&dirty_area, new_area);
		exposed_areas = true;
	}

	// Change size if requested.
	if (attribute_mask & WIN_ATTR_SIZE) {
		// Exposed areas only if we shrink the size.
		if ((new_area->size.x < win->attributes.area.size.x) ||
			(new_area->size.y < win->attributes.area.size.y)) {
			exposed_areas = true;
		}

		win->attributes.area.size = new_area->size;
		win_compute_union(&dirty_area, new_area);
	}

	// Notify attribute change and redraw if it is visible.
	win_handle_event(win, WIN_EVENT_ATTRIBUTES, &attribute_mask);

	if (win_is_visible(win)) {
		/*
		 * If areas are exposed, we need to draw all siblings as
		 * well, to make sure everything is updated, therefore
		 * we ask our parent to draw itself.
		 */
		if (exposed_areas)
			win_draw_parent(win, &dirty_area);
		else
			win_draw(win, &dirty_area);
	}
}


/**
 * This function returns a pointer to the internal attribute struct of
 * a window. It cannot be modified, but could be copied from.
 *
 * \param  win  Pointer to window.
 *
 * \return  Pointer to attribute struct.
 */
const struct win_attributes *win_get_attributes(const struct win_window *win)
{
	// Sanity check.
	assert(win != NULL);

	// Return reference.
	return &win->attributes;
}


/**
 * This function returns a pointer to the internal area struct of
 * a window. It cannot be modified, but could be copied from.
 *
 * \param  win  Pointer to window.
 *
 * \return  Pointer to area struct.
 */
const struct win_area *win_get_area(const struct win_window *win)
{
	// Sanity check.
	assert(win != NULL);

	// Return reference.
	return &win->attributes.area;
}


/**
 * This function returns the custom data from the window's attributes.
 * The custom data can be used for e.g. linking to associated widgets.
 *
 * \param  win  Pointer to window.
 *
 * \return  Copy of the custom data.
 */
void *win_get_custom_data(const struct win_window *win)
{
	return win->attributes.custom;
}


/**
 * This function attempts to allocate memory for a new window from the dynamic
 * memory pool. If successful, it initializes the window and attaches it to
 * the parent, raising it to the top of all siblings, and returns the
 * window pointer. If it fails, the function returns NULL.
 *
 * Use win_destroy() to properly free the allocated memory. Refer to the
 * win_destroy() function for more details on the destroy procedure.
 *
 * \param  parent      Pointer to parent window.
 * \param  attributes  Pointer to initial attributes.
 *
 * \return  Pointer to allocated window struct, or NULL.
 */
struct win_window *win_create(struct win_window *parent,
		const struct win_attributes *attributes)
{
	struct win_window *win;

	// Sanity check.
	assert(parent != NULL);
	assert(attributes != NULL);

	win = membag_alloc(sizeof(struct win_window));
	if (!win)
		return NULL;

	// Setup window attributes and start as invisible initially.
	win->attributes = *attributes;
	win->is_mapped = false;

	/*
	 * Setup window structure and add to parent window.  The parent
	 * window will set parent link and sibling pointers.
	 */
	win->top_child = NULL;

	win_add_top_child(parent, win);

	return win;
}


/**
 * This function hides a window, detaches it from its parent. It then
 * frees all memory allocated by all its children, grand children etc, and
 * itself. Before freeing the memory, a DESTROY event is sent to each window's
 * event handler. Use this event to free memory and resources allocated by e.g.
 * widget toolkits. Since each window is detached from its parent before
 * sending the DESTROY event, the event handling should never attempt to refer
 * to parent windows.
 *
 * \param  win  Pointer to window.
 */
void win_destroy(struct win_window *win)
{
	// Sanity check.
	assert(win != NULL);
	assert(win != &win_root);

	// Hide if still visible.
	if (win->is_mapped)
		win_hide(win);

	/*
	 * Unlink from parent and destroy entire substructure without
	 * bothering to hide and unlink each and every one on the way.
	 */
	win_unlink(win);
	win_destroy_children(win);
	win_handle_event(win, WIN_EVENT_DESTROY, NULL);
	membag_free(win);
}


/**
 * This function detaches a window from its parent and attaches it to a new
 * parent, as the top child window. This essentially moves a window from one
 * parent to another. If the window is not mapped, it will still be unmapped
 * at the new parent.
 *
 * \param  child      Pointer to child window to reparent.
 * \param  new_parent  Pointer to new parent.
 */
void win_reparent(struct win_window *child, struct win_window *new_parent)
{
	bool was_mapped;

	// Sanity check.
	assert(child != NULL);
	assert(child != &win_root);
	assert(new_parent != NULL);

	// Transfer to new parent and restore visibility.
	was_mapped = child->is_mapped;
	if (was_mapped)
		win_hide(child);

	win_unlink(child);
	win_add_top_child(new_parent, child);

	if (was_mapped)
		win_show(child);
}


/**
 * This function maps a window to its parent. Mapping means that the window
 * will be visible and drawn if the parent is visible, unless obscured by
 * other windows of course.
 *
 * \param  win  Pointer to window.
 */
void win_show(struct win_window *win)
{
	// Sanity check.
	assert(win != NULL);

	// Don't do anything if already visible.
	if (win->is_mapped)
		return;

	// Make visible and draw the window.
	win->is_mapped = true;
	win_redraw(win);
}


/**
 * This function redraws a window, if it is mapped and visible. Use this
 * function to refresh graphics if e.g. a widget's state is changed.
 *
 * \param  win  Pointer to window.
 */
void win_redraw(const struct win_window *win)
{
	if (win_is_visible(win)) {
		const struct win_area *dirty_area = &win->attributes.area;
		win_draw(win, dirty_area);
	}
}


/**
 * This function unmaps a window from its parent. If it was visible,
 * it will be removed from the screen. If the root window is hidden, the
 * screen background will be erased to the color WIN_SCREEN_COLOR.
 *
 * \param  win  Pointer to window.
 */
void win_hide(struct win_window *win)
{
	// Sanity check.
	assert(win != NULL);

	// Don't do anything if already hidden.
	if (!win->is_mapped)
		return;

	// If we want to hide the root window, clear the entire screen.
	if (win == &win_root) {
		win->is_mapped = false;
		gfx_set_clipping(0, 0, gfx_get_width(), gfx_get_height());
		gfx_draw_filled_rect(0, 0, gfx_get_width(), gfx_get_height(),
			WIN_SCREEN_COLOR);
	} else {
		// Make invisible and refresh exposed areas.
		if (win_is_visible(win)) {
			win->is_mapped = false;
			win_draw_parent(win, &win->attributes.area);
		} else {
			win->is_mapped = false;
		}
	}
}


/**
 * This function raises a window to the top of the Z-stack of child windows.
 * Proper redraw calls are issued to update graphics. Before rearranging
 * windows, the current top window is sent an UNRAISE event, and after putting
 * this window on top, this gets a RAISE event. Use these events to update
 * any focus and highlight related graphics.
 *
 * \param  win  Pointer to window.
 */
void win_raise(struct win_window *win)
{
	struct win_window *parent;

	// Sanity check.
	assert(win != NULL);
	assert(win != &win_root);

	// No need to raise if already on top.
	if (win->parent->top_child == win)
		return;

	// Move window to beginning of child list and redraw.
	win_handle_event(win->top_child, WIN_EVENT_UNRAISE, NULL);

	parent = win->parent;
	win_unlink(win);
	win_add_top_child(parent, win);

	win_handle_event(win, WIN_EVENT_RAISE, NULL);

	if (win_is_visible(win)) {
		const struct win_area *dirty_area = &win->attributes.area;
		win_draw(win, dirty_area);
	}
}


/**
 * This function lowers a window to the bottom of the Z-stack of child windows.
 * Proper redraw calls are issued to update graphics. Before rearranging
 * windows, this window is sent an UNRAISE event, and the window the gets to
 * be on top after rearranging is sent a RAISE event.
 * Use these events to update any focus and highlight related graphics.
 *
 * \param  win  Pointer to window.
 */
void win_lower(struct win_window *win)
{
	struct win_window *parent;

	// Sanity check.
	assert(win != NULL);
	assert(win != &win_root);

	// No need to lower if already on bottom.
	if (win->parent->top_child->prev_sibling == win)
		return;

	/*
	 * Move window to the end of child list and redraw exposed
	 * areas.  Since the list wraps at the last window, its next
	 * sibling is the top window.
	 */
	win_handle_event(win->top_child, WIN_EVENT_UNRAISE, NULL);

	parent = win->parent;
	win_unlink(win);
	win_add_top_child(parent, win);
	parent->top_child = win->next_sibling;

	win_handle_event(win->top_child, WIN_EVENT_RAISE, NULL);

	if (win_is_visible(win)) {
		const struct win_area *dirty_area = &win->attributes.area;
		win_draw(win, dirty_area);
	}
}


/**
 * This function creates a struct win_event from the pointer
 * event and adds it to the internal event queue. The event data
 * is copied, so there is no need to keep the event variable alive
 * after calling this function.
 *
 * \param  event  Pointer to pointer event data.
 */
void win_queue_pointer_event(const struct win_pointer_event *event)
{
	struct win_event w_event;

	w_event.type = WIN_EVENT_POINTER;
	w_event.pointer = *event;
	win_queue_event(&w_event);
}


/**
 * This function creates a struct win_event from the keyboard
 * event and adds it to the internal event queue. The event data
 * is copied, so there is no need to keep the event variable alive
 * after calling this function.
 *
 * \param  event  Pointer to keyboard event data.
 */
void win_queue_keyboard_event(const struct win_keyboard_event *event)
{
	struct win_event w_event;

	w_event.type = WIN_EVENT_KEYBOARD;
	w_event.keyboard = *event;
	win_queue_event(&w_event);
}


/**
 * This function creates a struct win_event from the command
 * event and adds it to the internal event queue. The event data
 * is copied, so there is no need to keep the event variable alive
 * after calling this function.
 *
 * \param  event  Pointer to command event data.
 */
void win_queue_command_event(const struct win_command_event *event)
{
	struct win_event w_event;

	w_event.type = WIN_EVENT_COMMAND;
	w_event.command = *event;
	win_queue_event(&w_event);
}

/**
 * \brief Process a keyboard event.
 *
 * This function is only called by win_process_events().
 * \param kevent The keyboard event being processed.
 */
static void win_process_keyboard_event(struct win_keyboard_event *kevent)
{
	/*
	 * Send event to current keyboard focus window, if any.  Ignore
	 * return value, since nobody wants the event anyway if not
	 * accepted by the focus window or its parents.
	 */
	if (win_keyboard_focus) {
		struct win_window *win = win_keyboard_focus;

		do {
			bool accepted = win_handle_event(win,
				WIN_EVENT_KEYBOARD, kevent);

			if (accepted)
				break;

			win = win->parent;
		} while (win != NULL);
	}
}

/**
 * \brief Process a command event.
 *
 * This function is only called by win_process_events().
 * \param cevent The command event being processed.
 */
static void win_process_command_event(struct win_command_event *cevent)
{
	struct win_window *win = cevent->recipient;

	assert(win != NULL);

	do {
		bool accepted = win_handle_event(win, WIN_EVENT_COMMAND,
			(win_command_t)cevent->data);

		if (accepted)
			break;

		win = win->parent;
	} while (win != NULL);
}

static bool win_event_queue_is_empty(void)
{
	return ring_is_empty(&win_event_queue.ring);
}

static bool win_event_queue_is_full(void)
{
	return ring_is_full(&win_event_queue.ring, WIN_EVENT_QUEUE_SIZE);
}

static struct win_event *win_event_queue_get_tail(void)
{
	unsigned int tail;

	tail = ring_get_tail(&win_event_queue.ring, WIN_EVENT_QUEUE_SIZE);
	return &win_event_queue.buffer[tail];
}

static void win_event_queue_push(const struct win_event *event,
		clock_jiffy_t timestamp)
{
	unsigned int index;
	struct win_event *head;

	index = ring_get_head(&win_event_queue.ring, WIN_EVENT_QUEUE_SIZE);
	head = &win_event_queue.buffer[index];
	*head = *event;
	head->timestamp = timestamp;
	ring_insert_entries(&win_event_queue.ring, 1);
}

static void win_event_queue_pop(void)
{
	ring_extract_entries(&win_event_queue.ring, 1);
}

/**
 * This function processes all pending events from the internal queue.
 * In order for the window system to work properly, this function
 * should be called as often as possible, to process new events.
 * In this implementation, whenever a new event is added to the queue,
 * the system automatically adds a work item to the main application
 * work queue. This work item refers to win_event_worker() which in turn
 * will call this function to process all pending events.
 *
 * \warning This function is recursive, so ensure that your application
 * has plenty of stack space before calling this function.
 */
void win_process_events(void)
{
	// Process until no more events waiting.
	while (!win_event_queue_is_empty()) {
		// Process event depending on type.
		struct win_event *event = win_event_queue_get_tail();

		switch (event->type) {
		case WIN_EVENT_POINTER:
			win_process_pointer_event(&event->pointer);
			break;

		case WIN_EVENT_KEYBOARD:
			win_process_keyboard_event(&event->keyboard);
			break;

		case WIN_EVENT_COMMAND:
			win_process_command_event(&event->command);
			break;

		default:
			// Error if any other events end up here.
			unhandled_case(event->type);
			break;
		}

		// We're done with the front event now, so remove it.
		win_event_queue_pop();
	}
}


/**
 * This function changes the keyboard focus window. The current keyboard focus
 * window is sent a LOSEFOCUS event, and the new focus window is sent a
 * GETFOCUS event. The keyboard focus window will receive all keyboard events.
 * Set the focus window to NULL to remove the focus.
 *
 * \param  win  Pointer to the new keyboard focus window, or NULL.
 */
void win_set_keyboard_focus(struct win_window *win)
{
	if (win_keyboard_focus != NULL)
		win_handle_event(win_keyboard_focus, WIN_EVENT_LOSEFOCUS, NULL);

	win_keyboard_focus = win;
	win_handle_event(win, WIN_EVENT_GETFOCUS, NULL);
}


/**
 * This function sets a pointer grabber window. The window that has grabbed the
 * pointer will receive all further pointer events until another window grabs
 * to pointer, or the pointer is ungrabbed by setting the grabbing window to
 * NULL. Use this function to grab the pointer to widgets that in certain
 * states needs to get all pointer events, e.g. dragging.
 *
 * \param  win  Pointer to grabbing window, or NULL.
 */
void win_grab_pointer(struct win_window *win)
{
	win_pointer_grabber = win;
}


/**
 * This function checks if a point is inside the clipping region. The origin
 * field of the clipping region is not relevant, only the corners of the
 * clipping region itself.
 *
 * \param  clip   Pointer to clipping region.
 * \param  point  Pointer to point to check.
 *
 * \return  True if point is inside clipping region.
 */
bool win_is_inside_clip(const struct win_clip_region *clip,
		const struct win_point *point)
{
	return (clip->NW.x <= point->x)
		&& (clip->NW.y <= point->y)
		&& (point->x <= clip->SE.x)
		&& (point->y <= clip->SE.y);
}


/**
 * This function checks if a point is inside a given area.
 *
 * \param  area   Pointer to area.
 * \param  point  Pointer to point to check.
 *
 * \return  True if point is inside area.
 */
bool win_is_inside_area(const struct win_area *area,
		const struct win_point *point)
{
	return (area->pos.x <= point->x)
		&& (area->pos.y <= point->y)
		&& (point->x < (area->pos.x + area->size.x))
		&& (point->y < (area->pos.y + area->size.y));
}


/**
 * This function checks if point given in global coordinates is inside
 * a given window.
 *
 * \param  win    Window to check.
 * \param  point  Point in global coordinates.
 *
 * \retval true if \a point is inside \a win.
 * \retval false if \a point is not inside \a win.
 */
bool win_is_inside_window(
		const struct win_window *win,
		const struct win_point *point)
{
	struct win_clip_region clip;

	win_compute_clipping(win, &win->attributes.area, &clip);

	return win_is_inside_clip(&clip, point);
}


/**
 * This function computes the smallest area the will fit both areas.
 * The new area is copied into the first parameter.
 *
 * \param  area   First area, and also where the result is stored.
 * \param  merge  Second area, which is "merged" with the first.
 */
void win_compute_union(struct win_area *area, const struct win_area *merge)
{
	gfx_coord_t expand_left;
	gfx_coord_t expand_top;
	gfx_coord_t expand_width;
	gfx_coord_t expand_height;

	// Expand leftwards if "merge" is left of "area".
	expand_left = area->pos.x - merge->pos.x;

	if (expand_left > 0) {
		area->pos.x -= expand_left;
		area->size.x += expand_left;
	}

	// Expand upwards if "merge" is above "area".
	expand_top = area->pos.y - merge->pos.y;
	if (expand_top > 0) {
		area->pos.y -= expand_top;
		area->size.y += expand_top;
	}

	// Expand width if right side of "merge" is to the right of "area"
	expand_width = (merge->pos.x + merge->size.x)
		- (area->pos.x + area->size.x);
	if (expand_width > 0)
		area->size.x += expand_width;

	// Expand height if bottom side of "merge" is below "area".
	expand_height = (merge->pos.y + merge->size.y)
		- (area->pos.y + area->size.y);
	if (expand_height > 0)
		area->size.y += expand_height;
}


/**
 * This function computes the intersection of a clipping region and an area.
 * The clipping region is updated with the result, so that the new region
 * only contains the area common to both the original region and the area.
 * If the clipping region and the area did not share any areas, i.e. the result
 * is empty set, then the function returns false.
 *
 * \param  clip  Original clipping region, and also where the result is stored.
 * \param  area  Area to compute intersection with.
 *
 * \retval true The resulting area is valid.
 * \retval false The resulting area is an empty set.
 */
bool win_compute_intersection(struct win_clip_region *clip,
		const struct win_area *area)
{
	// Shortcut calculation.
	const gfx_coord_t right_x = area->pos.x + area->size.x - 1;
	const gfx_coord_t bottom_y = area->pos.y + area->size.y - 1;

	/*
	 * Invalid region if top-left of area is outside right and/or
	 * bottom edge.
	 */
	if (clip->SE.x < area->pos.x)
		return false;
	if (clip->SE.y < area->pos.y)
		return false;

	/*
	 * Invalid region if bottom-right of area is outside left and/or
	 * top edge.
	 */
	if (clip->NW.x > right_x)
		return false;
	if (clip->NW.y > bottom_y)
		return false;

	// Shrink region if top-left of area is inside region.
	if (clip->NW.x < area->pos.x)
		clip->NW.x = area->pos.x;
	if (clip->NW.y < area->pos.y)
		clip->NW.y = area->pos.y;

	// Shrink region if bottom-right of area is inside region.
	if (clip->SE.x > right_x)
		clip->SE.x = right_x;
	if (clip->SE.y > bottom_y)
		clip->SE.y = bottom_y;

	// If we arrive here, the region is valid.
	return true;
}


/**
 * This function computes a clipping region given a window and an area given in
 * that window's coordinate system. The area is translated to screen coordinates
 * by translating from parent to parent until the root window is reached. Also,
 * the area is clipped to fit, inside the window, and its parents.
 * Use this function to compute a clipping region for updating parts of
 * a window. If the resulting region is an empty set, the function returns
 * false.
 *
 * The dirty_area parameter is given in the same coordinate system as the
 * window's area struct. This means that if you provide a pointer to the
 * window's area as the dirty_area parameter, you get a clipping region covering
 * the entire window.
 *
 * \param  win        The window to start from.
 * \param  dirty_area  Area to be clipped, same coordinate system as window.
 * \param  clip       Resulting clipping region, global coordinates.
 *
 * \retval true The resulting region is valid.
 * \retval false The resulting region is an empty set.
 */
bool win_compute_clipping(const struct win_window *win,
		const struct win_area *dirty_area,
		struct win_clip_region *clip)
{
	/*
	 * Start with a clipping region equal to own window area. Values
	 * given in parent window's coordinate system, which means (0,0)
	 * is parent window's top-left corner.
	 */
	clip->origin = win->attributes.area.pos;
	clip->NW = win->attributes.area.pos;
	clip->SE.x = win->attributes.area.pos.x
		+ win->attributes.area.size.x - 1;
	clip->SE.y = win->attributes.area.pos.y
		+ win->attributes.area.size.y - 1;

	// Intersect with dirty area, and return false if no intersection.
	if (!win_compute_intersection(clip, dirty_area))
		return false;

	/*
	 * Translate origin and clip region using parent areas, until
	 * root window.
	 */
	while (win != &win_root) {
		// Move to parent window first.
		win = win->parent;

		// Clip region inside parent window extents.
		if (clip->NW.x < 0)
			clip->NW.x = 0;
		if (clip->NW.y < 0)
			clip->NW.y = 0;
		if (clip->SE.x >= win->attributes.area.size.x)
			clip->SE.x = win->attributes.area.size.x - 1;
		if (clip->SE.y >= win->attributes.area.size.y)
			clip->SE.y = win->attributes.area.size.y - 1;

		/*
		 * If we got clipped away altogether, return with false.
		 * This is the case if we have a negative area.
		 */
		if ((clip->NW.x > clip->SE.x) || (clip->NW.y > clip->SE.y))
			return false;

		/*
		 * Translate to parent coordinate system, which means we
		 * will be in the same coordinate system _as_ the
		 * parent, not _in_ it.
		 */
		clip->origin.x += win->attributes.area.pos.x;
		clip->NW.x += win->attributes.area.pos.x;
		clip->SE.x += win->attributes.area.pos.x;

		clip->origin.y += win->attributes.area.pos.y;
		clip->NW.y += win->attributes.area.pos.y;
		clip->SE.y += win->attributes.area.pos.y;
	}

	/*
	 * If we arrived here, the clipping region is still valid, ie.
	 * positive.  The root window is always fullscreen, so there is
	 * no reason to check that it is valid.
	 */
	return true;
}


/**
 * This function checks if the window should be visible on screen, or
 * at least drawn on screen but maybe obscured by other windows. If a window's
 * parent is visible, all mapped children are also visible. If any parent on the
 * path up to, and including, the root window is unmapped, the window is not
 * visible. Once again, note that even if a window is "visible", i.e. this
 * function returns true, it might be obscured by other windows on top.
 *
 * \param  win  Window to check.
 *
 * \retval true \a win is visible.
 * \retval false \a win is not visible.
 */
static bool win_is_visible(const struct win_window *win)
{
	// Move up the window tree, search for unmapped windows.
	do {
		// Return false once an unmapped window is found.
		if (!win->is_mapped)
			return false;

		win = win->parent;
	} while (win != NULL);

	/*
	 * If we arrived here, all windows were mapped, ergo visible on
	 * screen, although maybe covered by other windows.
	 */
	return true;
}


/**
 * This function takes care of the actual drawing of all or parts of a
 * window.  If the window is not the top window, i.e. obscured by other
 * windows, these other windows are also redrawn in the proper order.
 * Only the parts of the windows that need to will be redrawn.
 *
 * The dirty_area parameter dictates which areas of the window to draw
 * or redraw.  The area is given in the same coordinate system as the
 * window itself, which means that providing the window's own area
 * struct will result in drawing the entire window. If only smaller
 * parts of the window needs to be drawn, make a copy of the window's
 * area struct and update to your liking before passing to this
 * function.
 *
 * For windows that are partly transparent, e.g. drop shadows or round
 * corners, the parent is asked to redraw all its child windows, in
 * order to provide a fresh background for the window to draw upon. This
 * applies to windows with the behavior flag REDRAW_PARENT set.
 *
 * \param  win        Window to draw or redraw.
 * \param  dirty_area Area dictating which parts to draw.
 */
static void win_draw(const struct win_window *win,
		const struct win_area *dirty_area)
{
	struct win_clip_region clip;

	/*
	 * Compute screen global origin and clipping region for this
	 * window, using the provided dirty_area. Return if we are
	 * clipped away altogether.
	 */
	if (!win_compute_clipping(win, dirty_area, &clip))
		return;

	/*
	 * If this window requires parent to redraw first, defer drawing
	 * to parent.
	 */
	if (win->parent && (win->attributes.behavior
				& WIN_BEHAVIOR_REDRAW_PARENT)) {
		win_draw_parent(win, dirty_area);
		return;
	}

	// Draw this window first.
	win_draw_contents(win, &clip);

	/*
	 * Move up the window tree, drawing all visible covering sibling
	 * windows on the way. Stop when root window is reached.
	 */
	while (win != &win_root) {
		/*
		 * We need the origin of our parent in order to provide
		 * proper clipping info to our covering siblings.
		 */
		clip.origin.x -= win->attributes.area.pos.x;
		clip.origin.y -= win->attributes.area.pos.y;

		// Draw all covering siblings.
		while (win != win->parent->top_child) {
			win = win->prev_sibling;
			if (win->is_mapped)
				win_draw_child(win, &clip);
		}

		// Move to parent window, break when root window is reached.
		win = win->parent;
	}
}


/**
 * This function is a shortcut to ask a window's parent to draw all its
 * children. However, the area to draw is given in the same coordinate system
 * as the child window, i.e. point 0,0 is top left corner of the parent window.
 * This function is used e.g. when rearranging child windows or to draw partly
 * transparent windows.
 *
 * \param  child      Child window.
 * \param  dirty_area  The area, given in same coordinate system as the child.
 */
static void win_draw_parent(const struct win_window *child,
		const struct win_area *dirty_area)
{
	struct win_area area = *dirty_area;

	if (win_translate_area_to_parent(&area, child->parent))
		win_draw(child->parent, &area);
}


/**
 * This function is a helper function for the win_draw() function. It draws the
 * actual contents of a window, given a clipping region. First, it draws the
 * window background itself, depending on the type. Then it sends a DRAW event
 * to the window, so that e.g. widgets or other handlers can draw the rest of
 * the window contents. Finally, it asks all mapped children, if any, to draw
 * themselves.
 *
 * Note that when the DRAW event is sent to the window, the TFT clipping region
 * is already set, so all TFT graphics functions called from the event handler
 * will be subject to proper clipping automatically.
 *
 * \param  win   The window to draw.
 * \param  clip  Clipping region, in global coordinates.
 *
 * \todo Either de-constify \a win or constify the parameter to
 * win_handle_event(). The code below looks dangerous.
 */
static void win_draw_contents(const struct win_window *win,
		const struct win_clip_region *clip)
{
	const struct win_window *child;

	// Set screen clipping limits and draw background.
	gfx_set_clipping(clip->NW.x, clip->NW.y, clip->SE.x, clip->SE.y);

	if (win->attributes.background) {
		gfx_draw_bitmap_tiled(win->attributes.background, clip->NW.x,
				clip->NW.y, clip->SE.x, clip->SE.y, clip->origin.x,
				clip->origin.y);
	}

	win_handle_event((struct win_window *)win, WIN_EVENT_DRAW, clip);

	// Draw all visible children, if any.
	child = win->top_child;
	if (child != NULL) {
		do {
			/*
			 * Move to previous first, which means the
			 * bottom child first, then the one above, and
			 * so on.
			 */
			child = child->prev_sibling;

			if (child->is_mapped)
				win_draw_child(child, clip);
		} while (child != win->top_child);
	}
}

/**
 * This function is a helper function for the win_draw() and win_draw_contents()
 * functions. It takes care of updating the clipping region, intersecting it
 * with the child window area. Then it draws the child window's contents.
 *
 * \param  child       Child window to draw.
 * \param  parent_clip  Clipping region, in global coordinates.
 */
static void win_draw_child(const struct win_window *child,
		const struct win_clip_region *parent_clip)
{
	struct win_clip_region clip;

	/*
	 * Start with a clipping region relative to parent window
	 * coordinates, which means point (0,0) is parent window's
	 * top-left corner.
	 */
	clip.origin = child->attributes.area.pos;
	clip.NW = child->attributes.area.pos;
	clip.SE.x = child->attributes.area.pos.x
		+ child->attributes.area.size.x - 1;
	clip.SE.y = child->attributes.area.pos.y
		+ child->attributes.area.size.y - 1;

	/*
	 * Translate to global coordinates using origin field of parent
	 * clipping region, which equal the parent window's top left
	 * corner in global coordinates.
	 */
	clip.origin.x += parent_clip->origin.x;
	clip.NW.x += parent_clip->origin.x;
	clip.SE.x += parent_clip->origin.x;

	clip.origin.y += parent_clip->origin.y;
	clip.NW.y += parent_clip->origin.y;
	clip.SE.y += parent_clip->origin.y;

	// Clip child region using clipping region from parent.
	if (clip.NW.x < parent_clip->NW.x)
		clip.NW.x = parent_clip->NW.x;
	if (clip.NW.y < parent_clip->NW.y)
		clip.NW.y = parent_clip->NW.y;
	if (clip.SE.x > parent_clip->SE.x)
		clip.SE.x = parent_clip->SE.x;
	if (clip.SE.y > parent_clip->SE.y)
		clip.SE.y = parent_clip->SE.y;

	/*
	 * Check if we got clipped away altogether, and return if so.
	 * This is the case if we have a negative area.
	 */
	if ((clip.NW.x > clip.SE.x) || (clip.NW.y > clip.SE.y))
		return;

	// Draw contents and children, if any.
	win_draw_contents(child, &clip);
}


/**
 * This function calculates the position of a window on the root window.
 * It traverses all parent windows and adds their position to get the
 * position of the given window.
 *
 * \param   start_win    The window of which to find absolute position.
 * \retval  return_pos   The absolute position of the window
 */
void win_translate_win_to_root(struct win_window const *start_win,
		struct win_point *return_pos)
{
	struct win_window *win;

	assert(start_win);
	assert(return_pos);

	return_pos->x = start_win->attributes.area.pos.x;
	return_pos->y = start_win->attributes.area.pos.y;

	win = start_win->parent;
	while (win) {
		return_pos->x += win->attributes.area.pos.x;
		return_pos->y += win->attributes.area.pos.y;
		win = win->parent;
	}
}

/**
 * This function is a helper function for the win_draw_parent() function. It
 * translates an area given in the same coordinate system as the child window,
 * to the same system as the parent window. The area is clipped inside the
 * parent extents if required.
 * If the resulting area is en empty set, the function returns false.
 *
 * \param  area    The area to translate.
 * \param  parent  Parent window.
 *
 * \retval true The resulting area is valid.
 * \retval false The resulting area is an empty set.
 */
static bool win_translate_area_to_parent(struct win_area *area,
		const struct win_window *parent)
{
	const struct win_area *parent_area = &parent->attributes.area;

	// Clip area inside parent window extents.
	if (area->pos.x < 0) {
		// pos.x is negative, so size shrinks.
		area->size.x += area->pos.x;
		area->pos.x = 0;
	}
	if (area->pos.y < 0) {
		// pos.y is negative, so size shrinks.
		area->size.y += area->pos.y;
		area->pos.y = 0;
	}

	if ((area->pos.x + area->size.x) > parent_area->size.x)
		area->size.x = parent_area->size.x - area->pos.x;
	if ((area->pos.y + area->size.y) > parent_area->size.y)
		area->size.y = parent_area->size.y - area->pos.y;

	// Check if area is still valid, i.e. not negative.
	if ((area->size.x <= 0) || (area->size.y <= 0))
		return false;

	// Translate to parent's coordinate system.
	area->pos.x += parent_area->pos.x;
	area->pos.y += parent_area->pos.y;

	return true;
}


/**
 * This function is a helper function for the win_destroy() function. For every
 * child window, it asks it to destroy its children, then it send a DESTROY
 * event to the child, and finally frees the allocated memory. The window's
 * DESTROY event handler should free any other memory or resources allocated
 * by e.g. widgets.
 *
 * \warning This function is recursive, so ensure that your application
 * has plenty of stack space before calling this function.
 *
 * \param  win  Window whose children should be destroyed.
 */
static void win_destroy_children(struct win_window *win)
{
	struct win_window *next_sibling;
	struct win_window *child = win->top_child;

	if (child != NULL) {
		do {
			win_destroy_children(child);
			win_handle_event(child, WIN_EVENT_DESTROY, NULL);
			next_sibling = child->next_sibling;
			membag_free(child);

			child = next_sibling;
		} while (child != win->top_child);

		win->top_child = NULL;
	}
}


/**
 * This function pulls out a window from the parent window's linked list of
 * of child windows. This function is used when rearranging child windows.
 *
 * \param  win  Window to be unlinked from parent.
 */
static void win_unlink(struct win_window *win)
{
	// If we are the only child, slightly easier handling.
	if (win->next_sibling == win) {
		win->parent->top_child = NULL;
	} else {
		// If we are the top child, set the next child as new top.
		if (win->parent->top_child == win)
			win->parent->top_child = win->next_sibling;

		// Remove ourselves from the linked list of children.
		win->next_sibling->prev_sibling = win->prev_sibling;
		win->prev_sibling->next_sibling = win->next_sibling;
	}

	// Clean up, for consistency.
	win->parent = NULL;
	win->next_sibling = NULL;
	win->prev_sibling = NULL;
}


/**
 * This function adds a child window as the top window of a parent.
 *
 * \param  parent  Parent window for this child.
 * \param  child   Child window to add to parent.
 */
static void win_add_top_child(struct win_window *parent,
		struct win_window *child)
{
	// If we are the first child, slightly easier handling.
	if (parent->top_child == NULL) {
		// We are the only child, so point to self.
		child->next_sibling = child;
		child->prev_sibling = child;
	} else {
		// Insert ourselves into the linked list at the top position.
		child->next_sibling = parent->top_child;
		child->prev_sibling = parent->top_child->prev_sibling;

		child->next_sibling->prev_sibling = child;
		child->prev_sibling->next_sibling = child;
	}

	// Parent-child link, putting child at top of Z stack.
	parent->top_child = child;
	child->parent = parent;
}


/**
 * This function is a helper function for the win_process_events() function.
 * It processes a pointer event by sending it to either the current pointer
 * grabbing window or into the window tree to find the window that is directly
 * beneath the pointer. The function also converts any relative coordinates
 * to absolute screen coordinates.
 *
 * \param  event  Pointer event data to process.
 */
static void win_process_pointer_event(struct win_pointer_event *event)
{
	// Store last absolute pointer pos in event struct.
	event->last_pos = win_last_pointer_pos;

	// Compute new last pos from this event.
	if (event->is_relative) {
		event->pos.x += win_last_pointer_pos.x;
		event->pos.y += win_last_pointer_pos.y;

		event->is_relative = false;
	}

	// Track last absolute pointer pos.
	win_last_pointer_pos = event->pos;

	/*
	 * If a window has grabbed the pointer, pass event to it, ignore
	 * return value.  If no grabbing window, find window under
	 * pointer and send event to it. Ignore return value, since root
	 * window has no parent.
	 */
	if (win_pointer_grabber != NULL) {
		win_handle_event(win_pointer_grabber,
			WIN_EVENT_POINTER, event);
	} else {
		win_handle_pointer_event(&win_root,
			event, &win_last_pointer_pos);
	}
}


/**
 * This function is a helper function for the win_process_pointer_event()
 * function. The pointer event is already processed, so this function searches
 * through its child windows to find the one beneath the pointer. If no suitable
 * child windows are found, the window handles the pointer event itself.
 * If either a child or this window accepts the event, this function returns
 * true.
 *
 * Any window encountered with the RAISE_ON_BUTTON behavior flag set will be
 * raised to the top inside it's parent, if the event was a POINTER_PRESS.
 *
 * \warning This function is recursive, so ensure that your application
 * has plenty of stack space before calling this function.
 *
 * \param  win    Window to handle the event itself, or pass on to children.
 * \param  event  Pointer event to handle.
 * \param  pos    Absolute position, translated to window's own coordinates.
 *
 * \return  True if child or window itself accepted the event.
 */
static bool win_handle_pointer_event(struct win_window *win,
		const struct win_pointer_event *event,
		const struct win_point *pos)
{
	// Search from top down, find window under pointer.
	struct win_window *child = win->top_child;

	if (child != NULL) {
		// Translate pointer position relative to this window.
		struct win_point local_pos = *pos;

		local_pos.x -= win->attributes.area.pos.x;
		local_pos.y -= win->attributes.area.pos.y;

		do {
			struct win_area *area;

			/*
			 * Hand event to first child that is mapped and
			 * contains pointer.  If child accepts, return
			 * true, if not, break search and hope this
			 * parent window has an event handler.
			 */
			area = &child->attributes.area;
			if (child->is_mapped && win_is_inside_area(area, &local_pos)) {
				bool accepted;

				/*
				 * Raise window if button pressed and
				 * window wants to be raised on button
				 * press. This is the mechanism that
				 * raises windows to the top when
				 * something inside them is clicked.
				 */
				if ((event->type == WIN_POINTER_PRESS)
						&& (child->attributes.behavior
							& WIN_BEHAVIOR_RAISE_ON_PRESS)) {
					win_raise(child);
				}

				// Now, hand event to child.
				accepted = win_handle_pointer_event(child,
						event, &local_pos);

				if (accepted)
					return true;
				else
					break;
			}

			/*
			 * Move on to the next sibling, one step lower
			 * in the Z stack.
			 */
			child = child->next_sibling;
		} while (child != win->top_child);
	}

	// If no child accepted the event, try our own handler.
	return win_handle_event(win, WIN_EVENT_POINTER, event);
}


/**
 * This function is a small helper function that checks if a window has an
 * event handler at all, and in that case, sends it an event for handling.
 * If an event handler is present, and accepts the event, this function returns
 * true. If the event handler rejects it, or there's no event handler present,
 * this function returns false.
 *
 * \param  win   Window to handle event.
 * \param  type  Event type.
 * \param  data  Custom data, dependent on event type.
 *
 * \retval true Event handler present and accepts event
 * \retval false No event handler present or event handler doesn't
 *      accept event.
 */
static bool win_handle_event(struct win_window *win,
		enum win_event_type type, const void *data)
{
	win_event_handler_t handler = win->attributes.event_handler;

	if (handler)
		return handler(win, type, data);
	else
		return false;
}


#ifdef CONFIG_GFX_WIN_USE_TOUCH
/**
 * This function serves as the touch driver event handler. If touch events
 * are supported, this function will be installed as the touch event handler
 * and converts touch events to pointer events and queues it in the internal
 * event queue.
 *
 * \param  event  Touch event from touch driver.
 */
static void win_queue_touch_event(const struct touch_event *event)
{
	struct win_pointer_event pointer_event;

	/*
	 * A touch event is always the same button, either pressed,
	 * released or moved/dragged. The only difference is the type,
	 * set in the switch-case below.
	 */
	pointer_event.buttons = WIN_TOUCH_BUTTON;
	pointer_event.pos.x = event->point.panel_x;
	pointer_event.pos.y = event->point.panel_y;
	pointer_event.is_relative = false;

	switch (event->type) {
	case TOUCH_PRESS:
		pointer_event.type = WIN_POINTER_PRESS;
		break;

	case TOUCH_MOVE:
		pointer_event.type = WIN_POINTER_MOVE;
		break;

	case TOUCH_RELEASE:
		pointer_event.type = WIN_POINTER_RELEASE;
		break;

	case TOUCH_NO_EVENT:
		// FIXME: pointer_event.type is not initialized
		break;
	}

	// Queue the event.
	win_queue_pointer_event(&pointer_event);
}
#endif


/**
 * This function adds a window event to the internal queue, taking care of
 * the internals of updating the circular queue. This function is reentrant
 * and protected from interrupts, so it can be called at any time from anywhere
 * without concern for concurrency.
 * The event data is copied into the queue, so there is no need to keep the
 * event variable alive after calling this function.
 * This function also takes care of adding a work item to the main work queue
 * of the application once new events arrive. If there are already events in
 * the queue, no extra work items will be added.
 *
 * \todo   Set a timestamp for each event.
 * \param  event  Event to add to the queue.
 */
static void win_queue_event(const struct win_event *event)
{
	irqflags_t iflags;

	workqueue_add_task(&main_workqueue, &win_event_queue.task);

	/*
	 * This is only necessary if this function may be called from
	 * different contexts.
	 */
	iflags = cpu_irq_save();

	// Drop event if queue is full.
	if (win_event_queue_is_full()) {
		++win_num_dropped_events;
	} else {
		// Put element at the back of the queue and timestamp it.
		win_event_queue_push(event, 0);
	}

	cpu_irq_restore(iflags);
}


/**
 * This function will be used as the work item callback when new events are
 * added to the queue.
 *
 * \param  task Pointer to the task being run, not used in function.
 */
static void win_event_worker(struct workqueue_task *task)
{
	win_process_events();
}

//! @}

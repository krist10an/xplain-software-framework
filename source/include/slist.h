/**
 * \file
 *
 * \brief Singly linked list implementation
 *
 * This is a generic implementation of singly linked lists. Each list
 * is represented by a struct slist, which is typically embedded in
 * another struct. Each list item is represented by a struct
 * slist_node, which is also typically embedded in another struct.
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
#ifndef SLIST_H_INCLUDED
#define SLIST_H_INCLUDED

#include <assert.h>
#include <types.h>

/**
 * \brief A node in a singly linked list
 */
struct slist_node {
	struct slist_node	*next;	//!< Next node in the list
};

/**
 * \brief A singly linked list
 */
struct slist {
	/**
	 * Sentinel node which is always at the head of the list. The
	 * last node in the list points back at this node.
	 */
	struct slist_node	first;
	/**
	 * Pointer to the last node in the list. Initially points at
	 * slist::first.
	 */
	struct slist_node	*last;
};

/**
 * \brief Initialize a singly linked list
 */
static inline void slist_init(struct slist *list)
{
	list->first.next = &list->first;
	list->last = &list->first;
}

/**
 * \brief Determine if \a list is empty
 * \retval true \a list is empty
 * \retval false \a list contains at least one node besides the
 * sentinel node.
 */
static inline bool slist_is_empty(struct slist *list)
{
	return list->first.next == &list->first;
}

/**
 * \brief Determine if \a node is the last node in \a list
 * \retval true \a node is the last node in \a list
 * \retval false \a node is not the last node in \a list
 */
static inline bool slist_node_is_last(struct slist *list,
		struct slist_node *node)
{
	return node == list->last;
}

/**
 * \brief Determine if \a node represents an item in \a list (i.e.
 * is not the sentinel node).
 * \retval true \a node represents an actual item
 * \retval false \a node is the sentinel node (i.e. one past the end
 * of the list)
 */
static inline bool slist_node_is_valid(struct slist *list,
		struct slist_node *node)
{
	return node != &list->first;
}

/**
 * \brief Access the containing structure of \a node
 * \param node A singly-linked list node
 * \param type The type of the containing structure
 * \param member The member of the containing struct that holds \a
 * node
 * \return A pointer to the containing structure
 */
#define slist_entry(node, type, member)				\
	container_of(node, type, member)

/**
 * \brief Insert \a node as the first node in \a list
 */
static inline void slist_insert_head(struct slist *list,
		struct slist_node *node)
{
	node->next = list->first.next;
	if (list->last == &list->first)
		list->last = node;
	list->first.next = node;
}

/**
 * \brief Insert \a node as the last node in \a list
 */
static inline void slist_insert_tail(struct slist *list,
		struct slist_node *node)
{
	node->next = &list->first;
	list->last->next = node;
	list->last = node;
}

/**
 * \brief Borrow the list \a from appending it to the tail of \a to
 *
 * This appends all the nodes in \a from to the end of the list \a to.
 * The nodes are still reachable from \a from, but there may be
 * additional nodes at the end. The list \a from must not be modified
 * or iterated over until the nodes are handed back using
 * slist_give_back_head().
 *
 * \pre \a from must contain at least one node
 * \post The last node in \a from does not point to \a from's sentinel
 * node.
 * \post The tail of \a from points to the last element in \a from
 */
static inline void slist_borrow_to_tail(struct slist *to, struct slist *from)
{
	assert(!slist_is_empty(from));

	from->last->next = &to->first;
	to->last->next = from->first.next;
	to->last = from->last;
}

/**
 * \brief Give back nodes borrowed from \a to currently at the head of
 * \a from.
 *
 * This restores the state of the list \a to as it was before \a from
 * borrowed nodes from it. After this, \a from will contain any nodes
 * following the ones borrowed from \a to.
 *
 * \pre The head of \a from must point to any of the nodes that were
 * borrowed from \a to.
 * \post The last node in \a to points to \a to's sentinel node
 */
static inline void slist_give_back_head(struct slist *to, struct slist *from)
{
	from->first.next = to->last->next;
	if (from->first.next == &from->first)
		from->last = &from->first;
	to->last->next = &to->first;
}

/**
 * \brief Move all the nodes in \a from to the tail of the list \a to.
 * \pre \a from must contain at least one node
 * \post \a from is empty
 */
static inline void slist_move_to_tail(struct slist *to, struct slist *from)
{
	slist_borrow_to_tail(to, from);
	slist_init(from);
}

/**
 * \brief Move all the nodes in \a from to the head of the list \a to.
 * \pre \a from must contain at least one node
 * \post \a from is empty
 */
static inline void slist_move_to_head(struct slist *to, struct slist *from)
{
	assert(!slist_is_empty(from));

	from->last->next = to->first.next;
	to->first.next = from->first.next;
	if (to->last == &to->first)
		to->last = from->last;
	slist_init(from);
}

/**
 * \brief Return the first node in \a list.
 */
static inline struct slist_node *slist_peek_head_node(struct slist *list)
{
	assert(list);
	return list->first.next;
}

/**
 * \brief Return the first item in \a list.
 * \param list The list
 * \param type The type of the item stored in \a list
 * \param member The member of the item struct holding the list node
 */
#define slist_peek_head(list, type, member)			\
	slist_entry(slist_peek_head_node(list), type, member)

/**
 * \brief Return the last node in \a list.
 */
static inline struct slist_node *slist_peek_tail_node(struct slist *list)
{
	assert(list);
	return list->last;
}

/**
 * \brief Return the last item in \a list.
 * \param list The list
 * \param type The type of the item stored in \a list
 * \param member The member of the item struct holding the list node
 */
#define slist_peek_tail(list, type, member)			\
	slist_entry(slist_peek_tail_node(list), type, member)

/**
 * \brief Return the node following \a node in the list
 */
static inline struct slist_node *slist_peek_next_node(struct slist_node *node)
{
	assert(node);
	return node->next;
}

/**
 * \brief Return the node following \a node in \a list.
 * \param node A list node
 * \param type The type of the item stored in \a list
 * \param member The member of the item struct holding the list node
 */
#define slist_peek_next(node, type, member)			\
	slist_entry(slist_peek_next_node(node), type, member)

/**
 * \brief Return the first node in \a list and remove it.
 * \pre \a list is not empty
 */
static inline struct slist_node *slist_pop_head_node(struct slist *list)
{
	struct slist_node *node;

	assert(list);
	assert(list->first.next != &list->first);

	node = list->first.next;
	list->first.next = node->next;
	if (list->last == node)
		list->last = &list->first;

	return node;
}

/**
 * \brief Return the first item in \a list and remove it.
 * \param list The list
 * \param type The type of the item stored in \a list
 * \param member The member of the item struct holding the list node
 * \pre \a list is not empty
 */
#define slist_pop_head(list, type, member)				\
	container_of(slist_pop_head_node(list), type, member)

#endif /* SLIST_H_INCLUDED */

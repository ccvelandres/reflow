// SPDX-License-Identifier: MIT

#ifndef __YUCK_LIST_H_
#define __YUCK_LIST_H_

/**
 * @file list.h
 * @author Cedric Velandres (ccvelandres@gmail.com)
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct list_node {
    struct list_node *next, *prev;
};

/**
 * Performs node delete to be:
 *   from: prev -> node -> next
 *   to:   prev -> next
 */
static inline void __list_del(struct list_node *prev, struct list_node *next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * Performs node insert to be:
 *   from: prev -> next
 *   to:   prev -> node -> next
 */
static inline void __list_add(struct list_node *node, struct list_node *prev, struct list_node *next)
{
    node->next = next;
    node->prev = prev;
    next->prev = node;
    prev->next = node;
}

static inline void list_init(struct list_node *head)
{
    head->next = head;
    head->prev = head;
}

static inline void list_add(struct list_node *head, struct list_node *node)
{
    __list_add(node, head, head->next);
}

static inline void list_add_tail(struct list_node *head, struct list_node *node)
{
    __list_add(node, head->prev, head);
}

static inline void list_del(struct list_node *node)
{
    __list_del(node->prev, node->next);
}

static inline bool list_empty(struct list_node *head)
{
    return head->next == head;
}

#ifdef __cplusplus
}
#endif

#endif
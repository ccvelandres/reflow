// SPDX-License-Identifier: MIT

#ifndef __YUCK_SCHEDULER_H__
#define __YUCK_SCHEDULER_H__

/**
 * @file scheduler.h
 * @author Cedric Velandres (ccvelandres@gmail.com)
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include "list.h"

typedef struct scheduler_s scheduler_scb_t;
typedef void* scheduler_handle_t;

typedef struct scheduler_task_s scheduler_task_t;
typedef int(*scheduler_fn_t)(scheduler_task_t *);

struct scheduler_s
{
    struct list_node task_list_head;

    // API
    yuck_mutex_api_t mutex_api;
};

struct scheduler_task_s {
    struct list_node list;
    scheduler_fn_t task_fn;
    uint32_t waketime_ns;
};

scheduler_handle_t scheduler_init(scheduler_scb_t *scb);
// int scheduler_queue(scheduler_t *scb, )

void scheduler_timer_handler(scheduler_scb_t *scb);

/**
 * likely usage 
 * 
 * handle = scheduler_init(&cfg);
 * 
 * scheduler_task_add(handle, &task);
 */

#ifdef __cplusplus
}
#endif

#endif
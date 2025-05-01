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

typedef struct scheduler_task scheduler_task_t;

typedef int(*scheduler_task_fn_t)(scheduler_task_t *);

struct scheduler_task {
    struct list_node list;
    scheduler_task_fn_t task_fn;
    uint32_t waketime_ns;
};

struct scheduler_scb
{

};

int scheduler_init(scheduler_task_t *scb);
// int scheduler_queue(scheduler_task_t *scb, )

#ifdef __cplusplus
}
#endif

#endif
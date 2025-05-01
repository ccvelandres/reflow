// SPDX-License-Identifier: MIT

#ifndef __YUCK_H__
#define __YUCK_H__

/**
 * @file error.h
 * @author Cedric Velandres (ccvelandres@gmail.com)
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

/*! @name Common API Error Codes */
#define YUCK_E_OK                     (0x00)
#define YUCK_E_NULLPTR                (0x01)
#define YUCK_E_COMM_FAIL              (0x02)
#define YUCK_E_INVALID_ARG            (0x03)
#define YUCK_E_OUT_OF_BOUNDS          (0x04)

/*! @name Common Argument Check Macros */
#define YUCK_CHECK_ARG(cond, rval)   do { if(cond) return rval; } while(0)
#define YUCK_CHECK_NULL(arg)         YUCK_CHECK_ARG(NULL == arg, YUCK_E_NULLPTR)

/*! @name Common Utility Macros */
#define YUCK_MEMBER_SIZEOF(type, member) (sizeof( ((type *)0)->member ))
#define YUCK_ARRAY_SIZEOF(array) (sizeof((array)) / sizeof((array)[0]))
#define YUCK_CONTAINEROF(ptr, type, member) ({                \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)((char *)__mptr - offsetof(type,member));})


#ifdef __cplusplus
}
#endif
#endif
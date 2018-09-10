/******************************************************************************

    @file    StateOS: osonceflag.h
    @author  Rajmund Szymanski
    @date    10.09.2018
    @brief   This file contains definitions for StateOS.

 ******************************************************************************

   Copyright (c) 2018 Rajmund Szymanski. All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

 ******************************************************************************/

#ifndef __STATEOS_ONE_H
#define __STATEOS_ONE_H

#include "oskernel.h"
#include "oscriticalsection.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : once flag
 *                     like a POSIX pthread_once_t
 *
 ******************************************************************************/

typedef uint_fast8_t one_t, * const one_id;

/******************************************************************************
 *
 * Name              : _ONE_INIT
 *
 * Description       : create and initialize a once flag object
 *
 * Parameters        : none
 *
 * Return            : once flag object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _ONE_INIT()   0

/******************************************************************************
 *
 * Name              : OS_ONE
 *
 * Description       : define and initialize a once flag object
 *
 * Parameters
 *   one             : name of a pointer to once flag object
 *
 ******************************************************************************/

#define             OS_ONE( one )                     \
                       one_t one##__one = _ONE_INIT(); \
                       one_id one = & one##__one

/******************************************************************************
 *
 * Name              : static_ONE
 *
 * Description       : define and initialize a static once flag object
 *
 * Parameters
 *   one             : name of a pointer to once flag object
 *
 ******************************************************************************/

#define         static_ONE( one )                     \
                static one_t one##__one = _ONE_INIT(); \
                static one_id one = & one##__one

/******************************************************************************
 *
 * Name              : ONE_INIT
 *
 * Description       : create and initialize a once flag object
 *
 * Parameters        : none
 *
 * Return            : once flag object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                ONE_INIT() \
                      _ONE_INIT()
#endif

/******************************************************************************
 *
 * Name              : ONE_CREATE
 * Alias             : ONE_NEW
 *
 * Description       : create and initialize a once flag object
 *
 * Parameters        : none
 *
 * Return            : pointer to once flag object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                ONE_CREATE() \
           (one_t[]) { ONE_INIT  () }
#define                ONE_NEW \
                       ONE_CREATE
#endif

/******************************************************************************
 *
 * Name              : one_init
 *
 * Description       : initialize a once flag object
 *
 * Parameters
 *   one             : pointer to once flag object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
void one_init( one_t *one ) { *one = 0; }

/******************************************************************************
 *
 * Name              : one_call
 *
 * Description       : try to execute given function
 *                     if the once flag has been previously set, the function is not executed
 *
 * Parameters
 *   one             : pointer to once flag object
 *   fun             : pointer to once function
 *
 * Return            : none
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

void one_call( one_t *one, fun_t *fun );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : OnceFlag
 *
 * Description       : use a once flag object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct OnceFlag
{
	OnceFlag( void ): flg_(_ONE_INIT()) {}
#if OS_FUNCTIONAL
	void call( FUN_t  fun ) { one_t flag; sys_lock(); flag = flg_; flg_ = 1; sys_unlock(); if (flag == 0) fun(); }
#else
	void call( fun_t *fun ) { one_call(&flg_, fun); }
#endif
private:
	one_t flg_;
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_ONE_H

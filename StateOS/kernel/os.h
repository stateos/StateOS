/******************************************************************************

    @file    StateOS: os.h
    @author  Rajmund Szymanski
    @date    28.11.2018
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

#ifndef __STATEOS

#define __STATEOS_MAJOR       6
#define __STATEOS_MINOR       6
#define __STATEOS_BUILD       0

#define __STATEOS       ((((__STATEOS_MAJOR)&0xFFUL)<<24)|(((__STATEOS_MINOR)&0xFFUL)<<16)|((__STATEOS_BUILD)&0xFFFFUL))

#define __STATEOS__          "StateOS v" STRINGIZE(__STATEOS_MAJOR) "." STRINGIZE(__STATEOS_MINOR) "." STRINGIZE(__STATEOS_BUILD)

#define STRINGIZE(n) STRINGIZE_HELPER(n)
#define STRINGIZE_HELPER(n) #n

/* -------------------------------------------------------------------------- */

#include "oskernel.h"
#include "osalloc.h"
#include "inc/oscriticalsection.h"
#include "inc/osspinlock.h"
#include "inc/osonceflag.h"
#include "inc/osevent.h"
#include "inc/ossignal.h"
#include "inc/osflag.h"
#include "inc/osbarrier.h"
#include "inc/ossemaphore.h"
#include "inc/osmutex.h"
#include "inc/osfastmutex.h"
#include "inc/osconditionvariable.h"
#include "inc/oslist.h"
#include "inc/osmemorypool.h"
#include "inc/osstreambuffer.h"
#include "inc/osmessagebuffer.h"
#include "inc/osmailboxqueue.h"
#include "inc/oseventqueue.h"
#include "inc/osjobqueue.h"
#include "inc/ostimer.h"
#include "inc/ostask.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : sys_init
 *
 * Description       : initialize system timer and enable services
 *
 * Parameters        : none
 *
 * Return            : none
 *
 * Note              : function port_sys_init should be invoked as a constructor
 *                   : otherwise, call sys_init as the first instruction in function main
 *
 ******************************************************************************/

__STATIC_INLINE
void sys_init( void ) { port_sys_init(); }

/******************************************************************************
 *
 * Name              : sys_time
 * ISR alias         : sys_timeISR
 *
 * Description       : return current value of system counter
 *
 * Parameters        : none
 *
 * Return            : current value of system counter
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

cnt_t sys_time( void );

__STATIC_INLINE
cnt_t sys_timeISR( void ) { return sys_time(); }

#ifdef __cplusplus
}
#endif

#endif//__STATEOS

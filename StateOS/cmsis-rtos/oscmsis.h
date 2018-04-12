/******************************************************************************

    @file    StateOS: oscmsis.h
    @author  Rajmund Szymanski
    @date    12.04.2018
    @brief   CMSIS-RTOS2 API implementation for StateOS.

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

#ifndef __STATEOSCMSIS_H
#define __STATEOSCMSIS_H

#include <cmsis_os2.h>
#include <os.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

/// Kernel Information
#define osVersionAPI         20010002   ///< API version (2.1.2)
#define osVersionKernel      50080000   ///< Kernel version (5.8.0)
#define osKernelId      "StateOS v5.8"  ///< Kernel identification string

/*---------------------------------------------------------------------------*/

#define IS_IRQ_MODE()    port_isr_inside()
#define IS_IRQ_MASKED()  port_isr_masked()

/*---------------------------------------------------------------------------*/

struct __Thread
{
	tsk_t          tsk;   // StateOS task object
	flg_t          flg;   // StateOS flag object
	uint32_t       flags; // attribute bits
	const char   * name;  // task name
	osThreadFunc_t func;  // task function
	void         * arg;   // task function argument
};

typedef struct __Thread osThread_t;

#define osThreadCbSize sizeof(osThread_t)
#define osThreadStackSize(size) (((((size)?(size):(OS_STACK_SIZE))+7)/8)*8)

/*---------------------------------------------------------------------------*/

struct __Timer
{
	tmr_t         tmr;   // StateOS timer object
	uint32_t      flags; // attribute bits
	const char  * name;  // timer name
	osTimerFunc_t func;  // timer function
	void        * arg;   // timer function argument
};

typedef struct __Timer osTimer_t;

#define osTimerCbSize sizeof(osTimer_t)

/*---------------------------------------------------------------------------*/

struct __EventFlags
{
	flg_t        flg;   // StateOS flag object
	uint32_t     flags; // attribute bits
	const char * name;  // flag name
};

typedef struct __EventFlags osEventFlags_t;

#define osEventFlagsCbSize sizeof(osEventFlags_t)

/*---------------------------------------------------------------------------*/

struct __Mutex
{
	mtx_t        mtx;   // StateOS mutex object
	uint32_t     flags; // attribute bits
	const char * name;  // mutex name
};

typedef struct __Mutex osMutex_t;

#define osMutexCbSize sizeof(osMutex_t)

/*---------------------------------------------------------------------------*/

struct __Semaphore
{
	sem_t        sem;   // StateOS semaphore object
	uint32_t     flags; // attribute bits
	const char * name;  // semaphore name
};

typedef struct __Semaphore osSemaphore_t;

#define osSemaphoreCbSize sizeof(osSemaphore_t)

/*---------------------------------------------------------------------------*/

struct __MemoryPool
{
	mem_t        mem;   // StateOS memory pool object
	uint32_t     flags; // attribute bits
	const char * name;  // memory pool name
};

typedef struct __MemoryPool osMemoryPool_t;

#define osMemoryPoolCbSize sizeof(osMemoryPool_t)
#define osMemoryPoolMemSize(count, size) ((((((size)+3)/4)+1)*4)*count)

/*---------------------------------------------------------------------------*/

struct __MessageQueue
{
	box_t        box;   // StateOS mail box object
	uint32_t     flags; // attribute bits
	const char * name;  // mail box name
};

typedef struct __MessageQueue osMessageQueue_t;

#define osMessageQueueCbSize sizeof(osMessageQueue_t)
#define osMessageQueueMemSize(count, size) (((((size)+3)/4)*4)*count)

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOSCMSIS_H

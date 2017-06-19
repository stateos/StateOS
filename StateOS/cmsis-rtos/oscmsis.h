/******************************************************************************

    @file    StateOS: oscmsis.h
    @author  Rajmund Szymanski
    @date    19.06.2017
    @brief   CMSIS-RTOS2 API implementation for StateOS.

 ******************************************************************************

    StateOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of StateOS distribution.

    StateOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    StateOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#ifndef __STATEOSCMSIS_H
#define __STATEOSCMSIS_H

#include <os.h>
#include <cmsis_os2.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

/// Kernel Information
#define osVersionAPI         20010001   ///< API version (2.1.1)
#define osVersionKernel      50000000   ///< Kernel version (5.0.0)
#define osKernelId      "StateOS v5.0"  ///< Kernel identification string

/*---------------------------------------------------------------------------*/

/// Object Flags definitions
#define osFlagSystemObject    0x0100U   ///< Object allocated by system
#define osFlagSystemMemory    0x0200U   ///< Memory allocated by ststem

/*---------------------------------------------------------------------------*/

#define IS_IRQ_MODE()    port_isr_inside()

/*---------------------------------------------------------------------------*/

struct __Thread
{
	tsk_t          tsk;   // StateOS task object
	flg_t          flg;   // StateOS flag object
	uint32_t       flags; // attribute bits
	const char   * name;  // task name
	osThreadFunc_t func;  // task function
	void         * arg;   // task function argument
	void         * stack; // task stack
	uint32_t       size;  // task stack size
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

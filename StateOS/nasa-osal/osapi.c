/*
** File: osapi.c
**
**      Copyright (c) 2004-2006, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software was created at NASAs Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** Author:  Alan Cudmore - Code 582
**
** Purpose: Contains functions prototype definitions and variables declarations
**          for the OS Abstraction Layer, Core OS module
**
** $Revision: 1.10 $
**
** $Date: 2013/07/25 10:01:32GMT-05:00 $
**
** $Log: osapi.h  $
** Revision 1.10 2013/07/25 10:01:32GMT-05:00 acudmore
** Added C++ support
** Revision 1.9 2010/11/12 12:00:17GMT-05:00 acudmore
** replaced copyright character with (c) and added open source notice where needed.
** Revision 1.8 2010/03/08 15:57:20EST acudmore
** include new OSAL version header file
** Revision 1.7 2009/08/10 14:01:10EDT acudmore
** Reset OSAL version for trunk
** Revision 1.6 2009/08/10 13:55:49EDT acudmore
** Updated OSAL version defines to 3.0
** Revision 1.5 2009/06/10 14:15:55EDT acudmore
** Removed HAL include files. HAL code was removed from OSAL.
** Revision 1.4 2008/08/20 16:12:51EDT apcudmore
** Updated timer error codes
** Revision 1.3 2008/08/20 15:46:27EDT apcudmore
** Add support for timer API
** Revision 1.2 2008/06/20 15:13:43EDT apcudmore
** Checked in new Module loader/symbol table functionality
** Revision 1.1 2008/04/20 22:36:02EDT ruperera
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-OSAL-REPOSITORY/src/os/inc/project.pj
** Revision 1.6 2008/02/14 11:29:10EST apcudmore
** Updated version define ( 2.11 )
** Revision 1.5 2008/02/07 11:31:58EST apcudmore
** Fixed merge problem
** Revision 1.4 2008/02/07 11:07:29EST apcudmore
** Added dynamic loader / Symbol lookup API
**   -- API only, next release will have functionality
** Revision 1.2 2008/01/29 14:30:49EST njyanchik
** I added code to all the ports that allow the values of both binary and counting semaphores to be
** gotten through the OS_*SemGetInfo API.
** Revision 1.1 2007/10/16 16:14:52EDT apcudmore
** Initial revision
** Member added to project d:/mksdata/MKS-OSAL-REPOSITORY/src/os/inc/project.pj
** Revision 1.2 2007/09/28 15:46:49EDT rjmcgraw
** Updated version numbers to 5.0
** Revision 1.1 2007/08/24 13:43:25EDT apcudmore
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/fsw/cfe-core/os/inc/project.pj
** Revision 1.9.1.1 2007/05/21 08:58:51EDT njyanchik
** The trunk version number has been updated to version 0.0
** Revision 1.9 2006/06/12 10:20:07EDT rjmcgraw
** Updated OS_MINOR_VERSION from 3 to 4
** Revision 1.8 2006/02/03 09:30:45EST njyanchik
** Changed version number to 2.3
** Revision 1.7 2006/01/20 11:56:16EST njyanchik
** Fixed header file information to match api document
** Revision 1.15  2005/11/09 13:35:49  nyanchik
** Revisions for 2.2 include:
** a new scheduler mapper for Linux and OS X
** addition of OS_printf function
** fixed issues that would cause warnings at compile time
**
**
*/

/******************************************************************************

    @file    StateOS: osapi.c
    @author  Rajmund Szymanski
    @date    18.01.2017
    @brief   NASA OSAPI implementation for StateOS.

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

#include <stdarg.h>
#include <stdio.h>
#include <osapi.h>
#include <os.h>

/*---------------------------------------------------------------------------*/
/*
** OSAL data
*/

typedef struct
{
	OS_time_t localtime;
	boolean   printf;
}	osal_t;

/*---------------------------------------------------------------------------*/
/*
** OSAL data instance
*/

static osal_t osal = {{ 0, 0 }, 0 };

/*---------------------------------------------------------------------------*/
/*
** OSAL local timer handler
*/

static void local_timer_handler( void )
{
	sys_lockISR();

	osal.localtime.microsecs += 1000;
	if (osal.localtime.microsecs >= 1000000)
	{
		osal.localtime.microsecs = 0;
		osal.localtime.seconds++;
	}

	sys_unlockISR();
}

/*---------------------------------------------------------------------------*/
/*
** OSAL local timer instance
*/

static tmr_t local_timer = TMR_INIT();

/*---------------------------------------------------------------------------*/
/*
** Initialization of API
*/

int32 OS_API_Init(void)
{
#if defined(__CSMC__)
	sys_init();
#endif
	tmr_startPeriodic(&local_timer, MSEC, local_timer_handler);
	
	return OS_SUCCESS;
}

/*---------------------------------------------------------------------------*/
/*
** Task API
*/

int32 OS_TaskCreate(uint32 *task_id, const char *task_name, osal_task_entry function_pointer,
                    const uint32 *stack_pointer, uint32 stack_size, uint32 priority, uint32 flags)
{
	(void) task_name;
	(void) flags;

	sys_lock();

	if (!stack_pointer && !stack_size) stack_size = OS_STACK_SIZE;

	*task_id = (uint32) tsk_create(~priority, function_pointer, stack_pointer ? 0 : stack_size);

	if (*task_id && stack_pointer)
		((tsk_t*)*task_id)->top = (void*) &stack_pointer[stack_size / sizeof(uint32)];

	sys_unlock();

	return *task_id ? OS_SUCCESS : OS_ERROR;
}

int32 OS_TaskDelete(uint32 task_id)
{
	if (task_id == 0)
		task_id = OS_TaskGetId();

	tsk_kill((tsk_t*)task_id);
	sys_free((void *)task_id);

	return OS_SUCCESS;
}

void OS_TaskExit(void)
{
	tsk_stop();
}

int32 OS_TaskInstallDeleteHandler(void *function_pointer)
{
	(void) function_pointer;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_TaskDelay(uint32 millisecond)
{
	switch (tsk_delay(millisecond*MSEC))
	{
	case E_TIMEOUT: return OS_SUCCESS;
	default:        return OS_ERROR;
	}
}

int32 OS_TaskSetPriority(uint32 task_id, uint32 new_priority)
{
	if (task_id == 0)
		task_id = OS_TaskGetId();

	sys_lock();

	new_priority = ~new_priority;
	((tsk_t*)task_id)->basic = new_priority;
	core_tsk_prio(((tsk_t*)task_id), new_priority);

	sys_unlock();

	return OS_SUCCESS;
}

int32 OS_TaskRegister(void)
{
	return OS_SUCCESS;
}

uint32 OS_TaskGetId(void)
{
	return (uint32) Current;
}

int32 OS_TaskGetIdByName(uint32 *task_id, const char *task_name)
{
	(void) task_id;
	(void) task_name;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_TaskGetInfo(uint32 task_id, OS_task_prop_t *task_prop)
{
	(void) task_id;
	(void) task_prop;

	return OS_ERR_NOT_IMPLEMENTED;
}

/*---------------------------------------------------------------------------*/
/*
** Message Queue API
*/

int32 OS_QueueCreate(uint32 *queue_id, const char *queue_name, uint32 queue_depth, uint32 data_size, uint32 flags)
{
	(void) queue_name;
	(void) flags;

	*queue_id = (uint32) box_create(queue_depth, data_size);

	return *queue_id ? OS_SUCCESS : OS_ERROR;
}

int32 OS_QueueDelete(uint32 queue_id)
{
	box_kill((box_t*)queue_id);
	sys_free((void *)queue_id);

	return OS_SUCCESS;
}

int32 OS_QueueGet(uint32 queue_id, void *data, uint32 size, uint32 *size_copied, int32 timeout)
{
	(void) size;

	switch (box_waitFor((box_t*)queue_id, data, timeout))
	{
	case E_SUCCESS: *size_copied = ((box_t*)queue_id)->size; return OS_SUCCESS;
	case E_TIMEOUT: *size_copied = 0;                        return OS_QUEUE_TIMEOUT;
	default:        *size_copied = 0;                        return OS_ERROR;
	}
}

int32 OS_QueuePut(uint32 queue_id, void *data, uint32 size, uint32 flags)
{
	(void) size;
	(void) flags;

	switch (box_give((box_t*)queue_id, data))
	{
	case E_SUCCESS: return OS_SUCCESS;
	case E_TIMEOUT: return OS_QUEUE_FULL;
	default:        return OS_ERROR;
	}
}

int32 OS_QueueGetIdByName(uint32 *queue_id, const char *queue_name)
{
	(void) queue_id;
	(void) queue_name;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_QueueGetInfo(uint32 queue_id, OS_queue_prop_t *queue_prop)
{
	(void) queue_id;
	(void) queue_prop;

	return OS_ERR_NOT_IMPLEMENTED;
}

/*---------------------------------------------------------------------------*/
/*
** Semaphore API
*/

int32 OS_BinSemCreate(uint32 *sem_id, const char *sem_name, uint32 sem_initial_value, uint32 options)
{
	(void) sem_name;
	(void) options;

	*sem_id = (uint32) sem_create(sem_initial_value, semBinary);

	return *sem_id ? OS_SUCCESS : OS_ERROR;
}

int32 OS_BinSemFlush(uint32 sem_id)
{
	sem_kill((sem_t*)sem_id);

	return OS_SUCCESS;
}

int32 OS_BinSemGive(uint32 sem_id)
{
	switch (sem_give((sem_t*)sem_id))
	{
	case E_SUCCESS: return OS_SUCCESS;
	case E_TIMEOUT: return OS_SUCCESS;
	default:        return OS_SEM_FAILURE;
	}
}

int32 OS_BinSemTake(uint32 sem_id)
{
	switch (sem_wait((sem_t*)sem_id))
	{
	case E_SUCCESS: return OS_SUCCESS;
	case E_TIMEOUT: return OS_SEM_TIMEOUT;
	default:        return OS_SEM_FAILURE;
	}
}

int32 OS_BinSemTimedWait(uint32 sem_id, uint32 msecs)
{
	switch (sem_waitFor((sem_t*)sem_id, msecs*MSEC))
	{
	case E_SUCCESS: return OS_SUCCESS;
	case E_TIMEOUT: return OS_SEM_TIMEOUT;
	default:        return OS_SEM_FAILURE;
	}
}

int32 OS_BinSemDelete(uint32 sem_id)
{
	sem_kill((sem_t*)sem_id);
	sys_free((void *)sem_id);

	return OS_SUCCESS;
}

int32 OS_BinSemGetIdByName(uint32 *sem_id, const char *sem_name)
{
	(void) sem_id;
	(void) sem_name;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_BinSemGetInfo(uint32 sem_id, OS_bin_sem_prop_t *bin_prop)
{
	(void) sem_id;
	(void) bin_prop;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_CountSemCreate(uint32 *sem_id, const char *sem_name, uint32 sem_initial_value, uint32 options)
{
	(void) sem_name;
	(void) options;

	*sem_id = (uint32) sem_create(sem_initial_value, semCounting);

	return *sem_id ? OS_SUCCESS : OS_ERROR;
}

int32 OS_CountSemGive(uint32 sem_id)
{
	switch (sem_give((sem_t*)sem_id))
	{
	case E_SUCCESS: return OS_SUCCESS;
	case E_TIMEOUT: return OS_SUCCESS;
	default:        return OS_SEM_FAILURE;
	}
}

int32 OS_CountSemTake(uint32 sem_id)
{
	switch (sem_wait((sem_t*)sem_id))
	{
	case E_SUCCESS: return OS_SUCCESS;
	case E_TIMEOUT: return OS_SEM_TIMEOUT;
	default:        return OS_SEM_FAILURE;
	}
}

int32 OS_CountSemTimedWait(uint32 sem_id, uint32 msecs)
{
	switch (sem_waitFor((sem_t*)sem_id, msecs*MSEC))
	{
	case E_SUCCESS: return OS_SUCCESS;
	case E_TIMEOUT: return OS_SEM_TIMEOUT;
	default:        return OS_SEM_FAILURE;
	}
}

int32 OS_CountSemDelete(uint32 sem_id)
{
	sem_kill((sem_t*)sem_id);
	sys_free((void *)sem_id);

	return OS_SUCCESS;
}

int32 OS_CountSemGetIdByName(uint32 *sem_id, const char *sem_name)
{
	(void) sem_id;
	(void) sem_name;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_CountSemGetInfo(uint32 sem_id, OS_count_sem_prop_t *count_prop)
{
	(void) sem_id;
	(void) count_prop;

	return OS_ERR_NOT_IMPLEMENTED;
}

/*---------------------------------------------------------------------------*/
/*
** Mutex API
*/

int32 OS_MutSemCreate(uint32 *sem_id, const char *sem_name, uint32 options)
{
	(void) sem_name;
	(void) options;

	*sem_id = (uint32) mtx_create();

	return *sem_id ? OS_SUCCESS : OS_ERROR;
}

int32 OS_MutSemGive(uint32 sem_id)
{
	switch (mtx_give((mtx_t*)sem_id))
	{
	case E_SUCCESS: return OS_SUCCESS;
	default:        return OS_SEM_FAILURE;
	}
}

int32 OS_MutSemTake(uint32 sem_id)
{
	switch (mtx_wait((mtx_t*)sem_id))
	{
	case E_SUCCESS: return OS_SUCCESS;
	case E_TIMEOUT: return OS_SEM_TIMEOUT;
	default:        return OS_SEM_FAILURE;
	}
}

int32 OS_MutSemDelete(uint32 sem_id)
{
	mtx_kill((mtx_t*)sem_id);
	sys_free((void *)sem_id);

	return OS_SUCCESS;
}

int32 OS_MutSemGetIdByName(uint32 *sem_id, const char *sem_name)
{
	(void) sem_id;
	(void) sem_name;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_MutSemGetInfo(uint32 sem_id, OS_mut_sem_prop_t *mut_prop)
{
	(void) sem_id;
	(void) mut_prop;

	return OS_ERR_NOT_IMPLEMENTED;
}

/*---------------------------------------------------------------------------*/
/*
** OS Time/Tick related API
*/

int32 OS_Milli2Ticks(uint32 milli_seconds)
{
	return milli_seconds * MSEC;
}

int32 OS_Tick2Micros(void)
{
	return 1000000 / OS_FREQUENCY;
}

int32 OS_GetLocalTime(OS_time_t *time_struct)
{
	sys_lock();

	*time_struct = osal.localtime;

	sys_unlock();

	return OS_SUCCESS;
}

int32 OS_SetLocalTime(OS_time_t *time_struct)
{
	sys_lock();

	osal.localtime = *time_struct;

	sys_unlock();

	return OS_SUCCESS;
}

/*---------------------------------------------------------------------------*/
/*
** Exception API
*/

int32 OS_ExcAttachHandler(uint32 ExceptionNumber, void (*ExceptionHandler)(uint32, uint32 *,uint32), int32 parameter)
{
	(void) ExceptionNumber;
	(void) ExceptionHandler;
	(void) parameter;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_ExcEnable(int32 ExceptionNumber)
{
	(void) ExceptionNumber;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_ExcDisable(int32 ExceptionNumber)
{
	(void) ExceptionNumber;

	return OS_ERR_NOT_IMPLEMENTED;
}

/*---------------------------------------------------------------------------*/
/*
** Floating Point Unit API
*/

int32 OS_FPUExcAttachHandler(uint32 ExceptionNumber, void *ExceptionHandler, int32 parameter)
{
	(void) ExceptionNumber;
	(void) ExceptionHandler;
	(void) parameter;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_FPUExcEnable(int32 ExceptionNumber)
{
	(void) ExceptionNumber;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_FPUExcDisable(int32 ExceptionNumber)
{
	(void) ExceptionNumber;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_FPUExcSetMask(uint32 mask)
{
	(void) mask;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_FPUExcGetMask(uint32 *mask)
{
	(void) mask;

	return OS_ERR_NOT_IMPLEMENTED;
}

/*---------------------------------------------------------------------------*/
/*
** Interrupt API
*/

int32 OS_IntAttachHandler(uint32 InterruptNumber, osal_task_entry InterruptHandler, int32 parameter)
{
	(void) InterruptNumber;
	(void) InterruptHandler;
	(void) parameter;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_IntUnlock(int32 IntLevel)
{
	port_put_lock(IntLevel);

	return OS_SUCCESS;
}

int32 OS_IntLock(void)
{
	uint32 IntLevel = port_get_lock();
	port_set_lock();
	return IntLevel;
}

int32 OS_IntEnable(int32 Level)
{
	NVIC_EnableIRQ((IRQn_Type)Level);

	return OS_SUCCESS;
}

int32 OS_IntDisable(int32 Level)
{
	NVIC_DisableIRQ((IRQn_Type)Level);

	return OS_SUCCESS;
}

int32 OS_IntSetMask(uint32 mask)
{
	(void) mask;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_IntGetMask(uint32 *mask)
{
	(void) mask;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_IntAck(int32 InterruptNumber)
{
	NVIC_ClearPendingIRQ((IRQn_Type)InterruptNumber);

	return OS_SUCCESS;
}

/*---------------------------------------------------------------------------*/
/*
** Shared memory API
*/
int32 OS_ShMemInit(void)
{
	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_ShMemCreate(uint32 *Id, uint32 NBytes, char *SegName)
{
	(void) Id;
	(void) NBytes;
	(void) SegName;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_ShMemSemTake(uint32 Id)
{
	(void) Id;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_ShMemSemGive(uint32 Id)
{
	(void) Id;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_ShMemAttach(uint32 *Address, uint32 Id)
{
	(void) Address;
	(void) Id;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_ShMemGetIdByName(uint32 *ShMemId, const char *SegName)
{
	(void) ShMemId;
	(void) SegName;

	return OS_ERR_NOT_IMPLEMENTED;
}

/*---------------------------------------------------------------------------*/
/*
** Heap API
*/
int32 OS_HeapGetInfo(OS_heap_prop_t *heap_prop)
{
	(void) heap_prop;

	return OS_ERR_NOT_IMPLEMENTED;
}

/*---------------------------------------------------------------------------*/
/*
** API for useful debugging function
*/
int32 OS_GetErrorName(int32 error_num, os_err_name_t *err_name)
{
	(void) error_num;
	(void) err_name;

	return OS_ERR_NOT_IMPLEMENTED;
}

/*---------------------------------------------------------------------------*/
/*
** Abstraction for printf statements
*/
void OS_printf(const char *fmt, ...)
{
	if (osal.printf)
	{
		va_list arp;
		va_start(arp, fmt);
		printf(fmt, arp);
		va_end(arp);
	}
}

void OS_printf_disable(void)
{
	osal.printf = FALSE;
}

void OS_printf_enable(void)
{
	osal.printf = TRUE;
}

/*---------------------------------------------------------------------------*/
/*
** Timer API
*/
int32 OS_TimerAPIInit(void)
{
	return OS_SUCCESS;
}

int32 OS_TimerCreate(uint32 *timer_id, const char *timer_name, uint32 *clock_accuracy, OS_TimerCallback_t callback_ptr)
{
	(void) timer_name;

	sys_lock();

	*clock_accuracy = 1000000 / OS_FREQUENCY;

	*timer_id = (uint32) tmr_create();

	if (*timer_id)
		((tmr_t*)*timer_id)->state = (fun_t*) callback_ptr;

	sys_unlock();

	return *timer_id ? OS_SUCCESS : OS_ERROR;
}

int32 OS_TimerSet(uint32 timer_id, uint32 start_msec, uint32 interval_msec)
{
	tmr_start((tmr_t*)timer_id, start_msec * MSEC, interval_msec * MSEC, ((tmr_t*)timer_id)->state);
	
	return OS_SUCCESS;
}

int32 OS_TimerDelete(uint32 timer_id)
{
	tmr_kill((tmr_t*)timer_id);
	sys_free((void *)timer_id);

	return OS_SUCCESS;
}

int32 OS_TimerGetIdByName(uint32 *timer_id, const char *timer_name)
{
	(void) timer_id;
	(void) timer_name;

	return OS_ERR_NOT_IMPLEMENTED;
}

int32 OS_TimerGetInfo(uint32 timer_id, OS_timer_prop_t *timer_prop)
{
	(void) timer_id;
	(void) timer_prop;

	return OS_ERR_NOT_IMPLEMENTED;
}

/*---------------------------------------------------------------------------*/

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
*/

/******************************************************************************

    @file    StateOS: osapi.c
    @author  Rajmund Szymanski
    @date    27.08.2018
    @brief   NASA OSAPI implementation for StateOS.

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

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <osnasa.h>

/* -------------------------------------------------------------------------- */
/*
** OSAL internal data
*/

static OS_queue_record_t     OS_queue_table    [OS_MAX_QUEUES];
static OS_bin_sem_record_t   OS_bin_sem_table  [OS_MAX_BIN_SEMAPHORES];
static OS_count_sem_record_t OS_count_sem_table[OS_MAX_COUNT_SEMAPHORES];
static OS_mut_sem_record_t   OS_mut_sem_table  [OS_MAX_MUTEXES];
static OS_task_record_t      OS_task_table     [OS_MAX_TASKS];
static OS_timer_record_t     OS_timer_table    [OS_MAX_TIMERS];

static OS_time_t             localtime        = { 0, 0 };
static tmr_t                 local_timer      = TMR_INIT(0);
static bool                  printf_enabled   = FALSE;

/* -------------------------------------------------------------------------- */
/*
** OSAL local timer handler
*/

static void local_timer_handler( void )
{
	sys_lockISR();
	{
		localtime.microsecs += 1000;

		if (localtime.microsecs >= 1000000)
		{
			localtime.microsecs = 0;
			localtime.seconds++;
		}
	}
	sys_unlockISR();
}

/* -------------------------------------------------------------------------- */
/*
** Initialization of API
*/

int32 OS_API_Init(void)
{
	tmr_startFrom(&local_timer, MSEC, MSEC, local_timer_handler);

	return OS_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/*
** Abstraction for printf statements
*/

void OS_printf(const char *fmt, ...)
{
	if (printf_enabled)
	{
		va_list arp;
		va_start(arp, fmt);
		printf(fmt, arp);
		va_end(arp);
	}
}

void OS_printf_disable(void)
{
	printf_enabled = FALSE;
}

void OS_printf_enable(void)
{
	printf_enabled = TRUE;
}

/* -------------------------------------------------------------------------- */
/*
** OS Time/Tick related API
*/

int32 OS_Milli2Ticks(uint32 milli_seconds)
{
	return milli_seconds * MSEC;
}

int32 OS_Tick2Micros(void)
{
	return 1000000 / (OS_FREQUENCY);
}

int32 OS_GetLocalTime(OS_time_t *time_struct)
{
	sys_lock();
	{
		*time_struct = localtime;
	}
	sys_unlock();

	return OS_SUCCESS;
}

int32 OS_SetLocalTime(OS_time_t *time_struct)
{
	sys_lock();
	{
		localtime = *time_struct;
	}
	sys_unlock();

	return OS_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/*
** Message Queue API
*/

int32 OS_QueueCreate(uint32 *queue_id, const char *queue_name, uint32 queue_depth, uint32 data_size, uint32 flags)
{
	OS_queue_record_t *rec;
	int32 status;
	void *data;

	(void) flags;

	sys_lock();
	{
		if (!queue_id || !queue_name)
			status = OS_INVALID_POINTER;
		else if (strlen(queue_name) >= OS_MAX_API_NAME)
			status = OS_ERR_NAME_TOO_LONG;
		else
		{
			for (rec = OS_queue_table; rec < OS_queue_table + OS_MAX_QUEUES; rec++)
				if (rec->used)
					if (strcmp(rec->name, queue_name) == 0)
						break;

			if (rec < OS_queue_table + OS_MAX_QUEUES)
				status = OS_ERR_NAME_TAKEN;
			else
			{
				for (rec = OS_queue_table; rec < OS_queue_table + OS_MAX_QUEUES; rec++)
					if (rec->used == 0)
						break;

				if (rec >= OS_queue_table + OS_MAX_QUEUES)
					status = OS_ERR_NO_FREE_IDS;
				else
				{
					data = sys_alloc(queue_depth * data_size);

					if (!data)
						status = OS_ERROR;
					else
					{
						*queue_id = rec - OS_queue_table;
						box_init(&rec->box, queue_depth, data, data_size);
						rec->box.res = data;
						strcpy(rec->name, queue_name);
						rec->creator = OS_TaskGetId();
						rec->used = 1;
						status = OS_SUCCESS;
					}
				}
			}
		}
	}
	sys_unlock();

	return status;
}

int32 OS_QueueDelete(uint32 queue_id)
{
	OS_queue_record_t *rec = &OS_queue_table[queue_id];
	int32 status;

	sys_lock();
	{
		if (queue_id >= OS_MAX_QUEUES)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else
		{
			box_delete(&rec->box);
			rec->used = 0;
			status = OS_SUCCESS;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_QueueGet(uint32 queue_id, void *data, uint32 size, uint32 *size_copied, int32 timeout)
{
	OS_queue_record_t *rec = &OS_queue_table[queue_id];
	int32 status;

	sys_lock();
	{
		if (queue_id >= OS_MAX_QUEUES)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else if (size < rec->box.size)
			status = OS_QUEUE_INVALID_SIZE;
		else
		{
			timeout = (timeout == OS_PEND)  ? (int32) INFINITE  :
			          (timeout == OS_CHECK) ? (int32) IMMEDIATE :
			          /* else */              (int32)(timeout * MSEC);

			switch (box_waitFor(&rec->box, data, timeout))
			{
				case E_SUCCESS: *size_copied = rec->box.size; status = OS_SUCCESS; break;
				case E_TIMEOUT: status = timeout ? OS_QUEUE_TIMEOUT : OS_QUEUE_EMPTY; break;
				default:        status = OS_ERROR; break;
			}
		}
	}
	sys_unlock();

	return status;
}

int32 OS_QueuePut(uint32 queue_id, void *data, uint32 size, uint32 flags)
{
	OS_queue_record_t *rec = &OS_queue_table[queue_id];
	int32 status;

	(void) flags;

	sys_lock();
	{
		if (queue_id >= OS_MAX_QUEUES)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else if (size > rec->box.size)
			status = OS_QUEUE_INVALID_SIZE;
		else switch (box_give(&rec->box, data))
		{
			case E_SUCCESS: status = OS_SUCCESS;    break;
			case E_TIMEOUT: status = OS_QUEUE_FULL; break;
			default:        status = OS_ERROR;      break;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_QueueGetIdByName(uint32 *queue_id, const char *queue_name)
{
	OS_queue_record_t *rec;
	int32 status;

	sys_lock();
	{
		if (!queue_id || !queue_name)
			status = OS_INVALID_POINTER;
		else if (strlen(queue_name) >= OS_MAX_API_NAME)
			status = OS_ERR_NAME_TOO_LONG;
		else
		{
			for (rec = OS_queue_table; rec < OS_queue_table + OS_MAX_QUEUES; rec++)
				if (rec->used)
					if (strcmp(rec->name, queue_name) == 0)
						break;

			if (rec >= OS_queue_table + OS_MAX_QUEUES)
				status = OS_ERR_NAME_NOT_FOUND;
			else
			{
				*queue_id = rec - OS_queue_table;
				status = OS_SUCCESS;
			}
		}
	}
	sys_unlock();

	return status;
}

int32 OS_QueueGetInfo(uint32 queue_id, OS_queue_prop_t *queue_prop)
{
	OS_queue_record_t *rec = &OS_queue_table[queue_id];
	int32 status;

	sys_lock();
	{
		if (queue_id >= OS_MAX_QUEUES)
			status = OS_ERR_INVALID_ID;
		else if (!queue_prop || !rec->used)
			status = OS_INVALID_POINTER;
		else
		{
			strcpy(queue_prop->name, rec->name);
			queue_prop->creator = rec->creator;
			status = OS_SUCCESS;
		}
	}
	sys_unlock();

	return status;
}

/* -------------------------------------------------------------------------- */
/*
** Semaphore API
*/

int32 OS_BinSemCreate(uint32 *semaphore_id, const char *sem_name, uint32 sem_initial_value, uint32 options)
{
	OS_bin_sem_record_t *rec;
	int32 status;

	(void) options;

	sys_lock();
	{
		if (!semaphore_id || !sem_name)
			status = OS_INVALID_POINTER;
		else if (strlen(sem_name) >= OS_MAX_API_NAME)
			status = OS_ERR_NAME_TOO_LONG;
		else
		{
			for (rec = OS_bin_sem_table; rec < OS_bin_sem_table + OS_MAX_BIN_SEMAPHORES; rec++)
				if (rec->used)
					if (strcmp(rec->name, sem_name) == 0)
						break;

			if (rec < OS_bin_sem_table + OS_MAX_BIN_SEMAPHORES)
				status = OS_ERR_NAME_TAKEN;
			else
			{
				for (rec = OS_bin_sem_table; rec < OS_bin_sem_table + OS_MAX_BIN_SEMAPHORES; rec++)
					if (rec->used == 0)
						break;

				if (rec >= OS_bin_sem_table + OS_MAX_BIN_SEMAPHORES)
					status = OS_ERR_NO_FREE_IDS;
				else
				{
					*semaphore_id = rec - OS_bin_sem_table;
					sem_init(&rec->sem, sem_initial_value, semBinary);
					strcpy(rec->name, sem_name);
					rec->creator = OS_TaskGetId();
					rec->used = 1;
					status = OS_SUCCESS;
				}
			}
		}
	}
	sys_unlock();

	return status;
}

int32 OS_BinSemDelete(uint32 semaphore_id)
{
	OS_bin_sem_record_t *rec = &OS_bin_sem_table[semaphore_id];
	int32 status;

	sys_lock();
	{
		if (semaphore_id >= OS_MAX_BIN_SEMAPHORES)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else
		{
			sem_delete(&rec->sem);
			rec->used = 0;
			status = OS_SUCCESS;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_BinSemFlush(uint32 semaphore_id)
{
	OS_bin_sem_record_t *rec = &OS_bin_sem_table[semaphore_id];
	int32 status;

	sys_lock();
	{
		if (semaphore_id >= OS_MAX_BIN_SEMAPHORES)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else
		{
			sem_kill(&rec->sem);
			status = OS_SUCCESS;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_BinSemGive(uint32 semaphore_id)
{
	OS_bin_sem_record_t *rec = &OS_bin_sem_table[semaphore_id];
	int32 status;

	sys_lock();
	{
		if (semaphore_id >= OS_MAX_BIN_SEMAPHORES)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else switch (sem_give(&rec->sem))
		{
			case E_SUCCESS: status = OS_SUCCESS;     break;
			default:        status = OS_SEM_FAILURE; break;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_BinSemTake(uint32 semaphore_id)
{
	OS_bin_sem_record_t *rec = &OS_bin_sem_table[semaphore_id];
	int32 status;

	sys_lock();
	{
		if (semaphore_id >= OS_MAX_BIN_SEMAPHORES)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else switch (sem_wait(&rec->sem))
		{
			case E_SUCCESS: status = OS_SUCCESS;     break;
			default:        status = OS_SEM_FAILURE; break;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_BinSemTimedWait(uint32 semaphore_id, uint32 msecs)
{
	OS_bin_sem_record_t *rec = &OS_bin_sem_table[semaphore_id];
	int32 status;

	sys_lock();
	{
		if (semaphore_id >= OS_MAX_BIN_SEMAPHORES)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else switch (sem_waitFor(&rec->sem, msecs*MSEC))
		{
			case E_SUCCESS: status = OS_SUCCESS;     break;
			case E_TIMEOUT: status = OS_SEM_TIMEOUT; break;
			default:        status = OS_SEM_FAILURE; break;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_BinSemGetIdByName(uint32 *semaphore_id, const char *sem_name)
{
	OS_bin_sem_record_t *rec;
	int32 status;

	sys_lock();
	{
		if (!semaphore_id || !sem_name)
			status = OS_INVALID_POINTER;
		else if (strlen(sem_name) >= OS_MAX_API_NAME)
			status = OS_ERR_NAME_TOO_LONG;
		else
		{
			for (rec = OS_bin_sem_table; rec < OS_bin_sem_table + OS_MAX_BIN_SEMAPHORES; rec++)
				if (rec->used)
					if (strcmp(rec->name, sem_name) == 0)
						break;

			if (rec >= OS_bin_sem_table + OS_MAX_BIN_SEMAPHORES)
				status = OS_ERR_NAME_NOT_FOUND;
			else
			{
				*semaphore_id = rec - OS_bin_sem_table;
				status = OS_SUCCESS;
			}
		}
	}
	sys_unlock();

	return status;
}

int32 OS_BinSemGetInfo(uint32 semaphore_id, OS_bin_sem_prop_t *bin_prop)
{
	OS_bin_sem_record_t *rec = &OS_bin_sem_table[semaphore_id];
	int32 status;

	sys_lock();
	{
		if (semaphore_id >= OS_MAX_BIN_SEMAPHORES)
			status = OS_ERR_INVALID_ID;
		else if (!bin_prop || !rec->used)
			status = OS_INVALID_POINTER;
		else
		{
			strcpy(bin_prop->name, rec->name);
			bin_prop->creator = rec->creator;
			bin_prop->value = rec->sem.count;
			status = OS_SUCCESS;
		}
	}
	sys_unlock();

	return status;
}

/* -------------------------------------------------------------------------- */

int32 OS_CountSemCreate(uint32 *semaphore_id, const char *sem_name, uint32 sem_initial_value, uint32 options)
{
	OS_count_sem_record_t *rec;
	int32 status;

	(void) options;

	sys_lock();
	{
		if (!semaphore_id || !sem_name)
			status = OS_INVALID_POINTER;
		else if (strlen(sem_name) >= OS_MAX_API_NAME)
			status = OS_ERR_NAME_TOO_LONG;
		else
		{
			for (rec = OS_count_sem_table; rec < OS_count_sem_table + OS_MAX_COUNT_SEMAPHORES; rec++)
				if (rec->used)
					if (strcmp(rec->name, sem_name) == 0)
						break;

			if (rec < OS_count_sem_table + OS_MAX_COUNT_SEMAPHORES)
				status = OS_ERR_NAME_TAKEN;
			else
			{
				for (rec = OS_count_sem_table; rec < OS_count_sem_table + OS_MAX_COUNT_SEMAPHORES; rec++)
					if (rec->used == 0)
						break;

				if (rec >= OS_count_sem_table + OS_MAX_COUNT_SEMAPHORES)
					status = OS_ERR_NO_FREE_IDS;
				else
				{
					*semaphore_id = rec - OS_count_sem_table;
					sem_init(&rec->sem, sem_initial_value, semCounting);
					strcpy(rec->name, sem_name);
					rec->creator = OS_TaskGetId();
					rec->used = 1;
					status = OS_SUCCESS;
				}
			}
		}
	}
	sys_unlock();

	return status;
}

int32 OS_CountSemDelete(uint32 semaphore_id)
{
	OS_count_sem_record_t *rec = &OS_count_sem_table[semaphore_id];
	int32 status;

	sys_lock();
	{
		if (semaphore_id >= OS_MAX_COUNT_SEMAPHORES)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else
		{
			sem_delete(&rec->sem);
			rec->used = 0;
			status = OS_SUCCESS;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_CountSemGive(uint32 semaphore_id)
{
	OS_count_sem_record_t *rec = &OS_count_sem_table[semaphore_id];
	int32 status;

	sys_lock();
	{
		if (semaphore_id >= OS_MAX_COUNT_SEMAPHORES)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else switch (sem_give(&rec->sem))
		{
			case E_SUCCESS: status = OS_SUCCESS;     break;
			default:        status = OS_SEM_FAILURE; break;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_CountSemTake(uint32 semaphore_id)
{
	OS_count_sem_record_t *rec = &OS_count_sem_table[semaphore_id];
	int32 status;

	sys_lock();
	{
		if (semaphore_id >= OS_MAX_COUNT_SEMAPHORES)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else switch (sem_wait(&rec->sem))
		{
			case E_SUCCESS: status = OS_SUCCESS;     break;
			default:        status = OS_SEM_FAILURE; break;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_CountSemTimedWait(uint32 semaphore_id, uint32 msecs)
{
	OS_count_sem_record_t *rec = &OS_count_sem_table[semaphore_id];
	int32 status;

	sys_lock();
	{
		if (semaphore_id >= OS_MAX_COUNT_SEMAPHORES)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else switch (sem_waitFor(&rec->sem, msecs*MSEC))
		{
			case E_SUCCESS: status = OS_SUCCESS;     break;
			case E_TIMEOUT: status = OS_SEM_TIMEOUT; break;
			default:        status = OS_SEM_FAILURE; break;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_CountSemGetIdByName(uint32 *semaphore_id, const char *sem_name)
{
	OS_count_sem_record_t *rec;
	int32 status;

	sys_lock();
	{
		if (!semaphore_id || !sem_name)
			status = OS_INVALID_POINTER;
		else if (strlen(sem_name) >= OS_MAX_API_NAME)
			status = OS_ERR_NAME_TOO_LONG;
		else
		{
			for (rec = OS_count_sem_table; rec < OS_count_sem_table + OS_MAX_COUNT_SEMAPHORES; rec++)
				if (rec->used)
					if (strcmp(rec->name, sem_name) == 0)
						break;

			if (rec >= OS_count_sem_table + OS_MAX_COUNT_SEMAPHORES)
				status = OS_ERR_NAME_NOT_FOUND;
			else
			{
				*semaphore_id = rec - OS_count_sem_table;
				status = OS_SUCCESS;
			}
		}
	}
	sys_unlock();

	return status;
}

int32 OS_CountSemGetInfo(uint32 semaphore_id, OS_count_sem_prop_t *count_prop)
{
	OS_count_sem_record_t *rec = &OS_count_sem_table[semaphore_id];
	int32 status;

	sys_lock();
	{
		if (semaphore_id >= OS_MAX_COUNT_SEMAPHORES)
			status = OS_ERR_INVALID_ID;
		else if (!count_prop || !rec->used)
			status = OS_INVALID_POINTER;
		else
		{
			strcpy(count_prop->name, rec->name);
			count_prop->creator = rec->creator;
			count_prop->value = rec->sem.count;
			status = OS_SUCCESS;
		}
	}
	sys_unlock();

	return status;
}

/* -------------------------------------------------------------------------- */
/*
** Mutex API
*/

int32 OS_MutSemCreate(uint32 *semaphore_id, const char *sem_name, uint32 options)
{
	OS_mut_sem_record_t *rec;
	int32 status;

	(void) options;

	sys_lock();
	{
		if (!semaphore_id || !sem_name)
			status = OS_INVALID_POINTER;
		else if (strlen(sem_name) >= OS_MAX_API_NAME)
			status = OS_ERR_NAME_TOO_LONG;
		else
		{
			for (rec = OS_mut_sem_table; rec < OS_mut_sem_table + OS_MAX_MUTEXES; rec++)
				if (rec->used)
					if (strcmp(rec->name, sem_name) == 0)
						break;

			if (rec < OS_mut_sem_table + OS_MAX_MUTEXES)
				status = OS_ERR_NAME_TAKEN;
			else
			{
				for (rec = OS_mut_sem_table; rec < OS_mut_sem_table + OS_MAX_MUTEXES; rec++)
					if (rec->used == 0)
						break;

				if (rec >= OS_mut_sem_table + OS_MAX_MUTEXES)
					status = OS_ERR_NO_FREE_IDS;
				else
				{
					*semaphore_id = rec - OS_mut_sem_table;
					mtx_init(&rec->mtx);
					strcpy(rec->name, sem_name);
					rec->creator = OS_TaskGetId();
					rec->used = 1;
					status = OS_SUCCESS;
				}
			}
		}
	}
	sys_unlock();

	return status;
}

int32 OS_MutSemDelete(uint32 semaphore_id)
{
	OS_mut_sem_record_t *rec = &OS_mut_sem_table[semaphore_id];
	int32 status;

	sys_lock();
	{
		if (semaphore_id >= OS_MAX_MUTEXES)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else
		{
			mtx_delete(&rec->mtx);
			rec->used = 0;
			status = OS_SUCCESS;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_MutSemGive(uint32 semaphore_id)
{
	OS_mut_sem_record_t *rec = &OS_mut_sem_table[semaphore_id];
	int32 status;

	sys_lock();
	{
		if (semaphore_id >= OS_MAX_MUTEXES)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else switch (mtx_give(&rec->mtx))
		{
			case E_SUCCESS: status = OS_SUCCESS;     break;
			default:        status = OS_SEM_FAILURE; break;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_MutSemTake(uint32 semaphore_id)
{
	OS_mut_sem_record_t *rec = &OS_mut_sem_table[semaphore_id];
	int32 status;

	sys_lock();
	{
		if (semaphore_id >= OS_MAX_MUTEXES)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else switch (mtx_wait(&rec->mtx))
		{
			case E_SUCCESS: status = OS_SUCCESS;     break;
			default:        status = OS_SEM_FAILURE; break;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_MutSemGetIdByName(uint32 *semaphore_id, const char *sem_name)
{
	OS_mut_sem_record_t *rec;
	int32 status;

	sys_lock();
	{
		if (!semaphore_id || !sem_name)
			status = OS_INVALID_POINTER;
		else if (strlen(sem_name) >= OS_MAX_API_NAME)
			status = OS_ERR_NAME_TOO_LONG;
		else
		{
			for (rec = OS_mut_sem_table; rec < OS_mut_sem_table + OS_MAX_MUTEXES; rec++)
				if (rec->used)
					if (strcmp(rec->name, sem_name) == 0)
						break;

			if (rec >= OS_mut_sem_table + OS_MAX_MUTEXES)
				status = OS_ERR_NAME_NOT_FOUND;
			else
			{
				*semaphore_id = rec - OS_mut_sem_table;
				status = OS_SUCCESS;
			}
		}
	}
	sys_unlock();

	return status;
}

int32 OS_MutSemGetInfo(uint32 semaphore_id, OS_mut_sem_prop_t *mut_prop)
{
	OS_mut_sem_record_t *rec = &OS_mut_sem_table[semaphore_id];
	int32 status;

	sys_lock();
	{
		if (semaphore_id >= OS_MAX_MUTEXES)
			status = OS_ERR_INVALID_ID;
		else if (!mut_prop || !rec->used)
			status = OS_INVALID_POINTER;
		else
		{
			strcpy(mut_prop->name, rec->name);
			mut_prop->creator = rec->creator;
			status = OS_SUCCESS;
		}
	}
	sys_unlock();

	return status;
}

/* -------------------------------------------------------------------------- */
/*
** Task API
*/

static void task_handler(void)
{
	void *tmp = tsk_this(); // because of COSMIC compiler
	OS_task_record_t *rec = tmp;

	rec->handler();

	tsk_stop();
}

int32 OS_TaskCreate(uint32 *task_id, const char *task_name, osal_task_entry function_pointer,
                    const uint32 *stack_pointer, uint32 stack_size, uint32 priority, uint32 flags)
{
	OS_task_record_t *rec;
	int32 status;
	void *stack = (void *) stack_pointer;

	(void) flags;

	sys_lock();
	{
		if (!task_id || !task_name || !function_pointer)
			status = OS_INVALID_POINTER;
		else if (strlen(task_name) >= OS_MAX_API_NAME)
			status = OS_ERR_NAME_TOO_LONG;
		else if ((priority < 1) || (priority > 255))
			status = OS_ERR_INVALID_PRIORITY;
		else if (stack_pointer && !stack_size)
			status = OS_ERROR;
		else
		{
			for (rec = OS_task_table; rec < OS_task_table + OS_MAX_TASKS; rec++)
				if (rec->used)
					if (strcmp(rec->name, task_name) == 0)
						break;

			if (rec < OS_task_table + OS_MAX_TASKS)
				status = OS_ERR_NAME_TAKEN;
			else
			{
				for (rec = OS_task_table; rec < OS_task_table + OS_MAX_TASKS; rec++)
					if (rec->used == 0)
						break;

				if (rec >= OS_task_table + OS_MAX_TASKS)
					status = OS_ERR_NO_FREE_IDS;
				else
				{
					if (!stack)
					{
						if (!stack_size) stack_size = OS_STACK_SIZE;
						stack = sys_alloc(stack_size);
					}
					if (!stack)
						status = OS_ERROR;
					else
					{
						*task_id = rec - OS_task_table;
						tsk_init(&rec->tsk, ~priority, task_handler, stack, stack_size);
						if (stack_pointer == 0) rec->tsk.obj.res = stack;
						strcpy(rec->name, task_name);
						rec->creator = OS_TaskGetId();
						rec->used = 1;
						rec->handler = function_pointer;
						rec->delete_handler = NULL;
						status = OS_SUCCESS;
					}
				}
			}
		}
	}
	sys_unlock();

	return status;
}

int32 OS_TaskDelete(uint32 task_id)
{
	OS_task_record_t *rec = &OS_task_table[task_id];
	int32 status;

	sys_lock();
	{
		if (task_id >= OS_MAX_TASKS)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else
		{
			if (rec->delete_handler)
				rec->delete_handler();
			tsk_delete(&rec->tsk);
			rec->used = 0;
			status = OS_SUCCESS;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_TaskInstallDeleteHandler(void *function_pointer)
{
	uint32 task_id = OS_TaskGetId();
	OS_task_record_t *rec = &OS_task_table[task_id];
	int32 status;

	sys_lock();
	{
		if (task_id >= OS_MAX_TASKS)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else
		{
			rec->delete_handler = (void(*)(void)) function_pointer;
			status = OS_SUCCESS;
		}
	}
	sys_unlock();

	return status;
}

void OS_TaskExit(void)
{
	tsk_stop();
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
	OS_task_record_t *rec = &OS_task_table[task_id];
	int32 status;

	sys_lock();
	{
		if (task_id >= OS_MAX_TASKS)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else if ((new_priority < 1) || (new_priority > 255))
			status = OS_ERR_INVALID_PRIORITY;
		else
		{
			core_tsk_prio(&rec->tsk, rec->tsk.basic = ~new_priority);
			status =  OS_SUCCESS;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_TaskRegister(void)
{
	return OS_SUCCESS;
}

uint32 OS_TaskGetId(void)
{
	void *tmp = tsk_this(); // because of COSMIC compiler
	OS_task_record_t *rec = tmp;
	uint32 task_id = rec - OS_task_table;

	if (task_id >= OS_MAX_TASKS)
		return (uint32) OS_ERR_INVALID_ID;

	return task_id;
}

int32 OS_TaskGetIdByName(uint32 *task_id, const char *task_name)
{
	OS_task_record_t *rec;
	int32 status;

	sys_lock();
	{
		if (!task_id || !task_name)
			status = OS_INVALID_POINTER;
		else if (strlen(task_name) >= OS_MAX_API_NAME)
			status = OS_ERR_NAME_TOO_LONG;
		else
		{
			for (rec = OS_task_table; rec < OS_task_table + OS_MAX_TASKS; rec++)
				if (rec->used)
					if (strcmp(rec->name, task_name) == 0)
						break;

			if (rec >= OS_task_table + OS_MAX_TASKS)
				status = OS_ERR_NAME_NOT_FOUND;
			else
			{
				*task_id = rec - OS_task_table;
				status = OS_SUCCESS;
			}
		}
	}
	sys_unlock();

	return status;
}

int32 OS_TaskGetInfo(uint32 task_id, OS_task_prop_t *task_prop)
{
	OS_task_record_t *rec = &OS_task_table[task_id];
	int32 status;

	sys_lock();
	{
		if (task_id >= OS_MAX_TASKS)
			status = OS_ERR_INVALID_ID;
		else if (!task_prop || !rec->used)
			status = OS_INVALID_POINTER;
		else
		{
			strcpy(task_prop->name, rec->name);
			task_prop->creator = rec->creator;
			task_prop->stack_size = (uint32_t) rec->tsk.size;
			task_prop->priority = ~rec->tsk.basic;
			task_prop->OStask_id = (uint32) &rec->tsk;
			status = OS_SUCCESS;
		}
	}
	sys_unlock();

	return status;
}

/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
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
	uint32 lock = port_get_lock();
	port_put_lock(IntLevel);
	return lock;
}

int32 OS_IntLock(void)
{
	uint32 lock = port_get_lock();
	port_set_lock();
	return lock;
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

/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/*
** Heap API
*/

int32 OS_HeapGetInfo(OS_heap_prop_t *heap_prop)
{
	(void) heap_prop;
	return OS_ERR_NOT_IMPLEMENTED;
}

/* -------------------------------------------------------------------------- */
/*
** API for useful debugging function
*/

int32 OS_GetErrorName(int32 error_num, os_err_name_t *err_name)
{
	char *error;

    switch (error_num)
    {
		case OS_SUCCESS:                  error = "OS_SUCCESS";                  break;
		case OS_ERROR:                    error = "OS_ERROR";                    break;
		case OS_INVALID_POINTER:          error = "OS_INVALID_POINTER";          break;
		case OS_ERROR_ADDRESS_MISALIGNED: error = "OS_ERROR_ADDRESS_MISALIGNED"; break;
		case OS_ERROR_TIMEOUT:            error = "OS_ERROR_TIMEOUT";            break;
		case OS_INVALID_INT_NUM:          error = "OS_INVALID_INT_NUM";          break;
		case OS_SEM_FAILURE:              error = "OS_SEM_FAILURE";              break;
		case OS_SEM_TIMEOUT:              error = "OS_SEM_TIMEOUT";              break;
		case OS_QUEUE_EMPTY:              error = "OS_QUEUE_EMPTY";              break;
		case OS_QUEUE_FULL:               error = "OS_QUEUE_FULL";               break;
		case OS_QUEUE_TIMEOUT:            error = "OS_QUEUE_TIMEOUT";            break;
		case OS_QUEUE_INVALID_SIZE:       error = "OS_QUEUE_INVALID_SIZE";       break;
		case OS_QUEUE_ID_ERROR:           error = "OS_QUEUE_ID_ERROR";           break;
		case OS_ERR_NAME_TOO_LONG:        error = "OS_ERR_NAME_TOO_LONG";        break;
		case OS_ERR_NO_FREE_IDS:          error = "OS_ERR_NO_FREE_IDS";          break;
		case OS_ERR_NAME_TAKEN:           error = "OS_ERR_NAME_TAKEN";           break;
		case OS_ERR_INVALID_ID:           error = "OS_ERR_INVALID_ID";           break;
		case OS_ERR_NAME_NOT_FOUND:       error = "OS_ERR_NAME_NOT_FOUND";       break;
		case OS_ERR_SEM_NOT_FULL:         error = "OS_ERR_SEM_NOT_FULL";         break;
		case OS_ERR_INVALID_PRIORITY:     error = "OS_ERR_INVALID_PRIORITY";     break;
		case OS_INVALID_SEM_VALUE:        error = "OS_INVALID_SEM_VALUE";        break;
		case OS_ERR_FILE:                 error = "OS_ERR_FILE";                 break;
		case OS_ERR_NOT_IMPLEMENTED:      error = "OS_ERR_NOT_IMPLEMENTED";      break;
		case OS_TIMER_ERR_INVALID_ARGS:   error = "OS_TIMER_ERR_INVALID_ARGS";   break;
		case OS_TIMER_ERR_TIMER_ID:       error = "OS_TIMER_ERR_TIMER_ID";       break;
		case OS_TIMER_ERR_UNAVAILABLE:    error = "OS_TIMER_ERR_UNAVAILABLE";    break;
		case OS_TIMER_ERR_INTERNAL:       error = "OS_TIMER_ERR_INTERNAL";       break;

        default: return OS_ERROR;
    }

	sys_lock();
	{
	    strcpy((char *) err_name, error);
	}
	sys_unlock();

    return OS_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/*
** Timer API
*/

static void timer_handler(void)
{
	void *tmp = tmr_thisISR(); // because of COSMIC compiler
	OS_timer_record_t *rec = tmp;
	uint32 timer_id = rec - OS_timer_table;

	rec->handler(timer_id);
}

int32 OS_TimerAPIInit(void)
{
	return OS_SUCCESS;
}

int32 OS_TimerCreate(uint32 *timer_id, const char *timer_name, uint32 *clock_accuracy, OS_TimerCallback_t callback_ptr)
{
	OS_timer_record_t *rec;
	int32 status;

	sys_lock();
	{
		if (!timer_id || !timer_name || !callback_ptr)
			status = OS_INVALID_POINTER;
		else if (strlen(timer_name) >= OS_MAX_API_NAME)
			status = OS_ERR_NAME_TOO_LONG;
		else
		{
			for (rec = OS_timer_table; rec < OS_timer_table + OS_MAX_TIMERS; rec++)
				if (rec->used)
					if (strcmp(rec->name, timer_name) == 0)
						break;

			if (rec < OS_timer_table + OS_MAX_TIMERS)
				status = OS_ERR_NAME_TAKEN;
			else
			{
				for (rec = OS_timer_table; rec < OS_timer_table + OS_MAX_TIMERS; rec++)
					if (rec->used == 0)
						break;

				if (rec >= OS_timer_table + OS_MAX_TIMERS)
					status = OS_ERR_NO_FREE_IDS;
				else
				{
					if (clock_accuracy)
						*clock_accuracy = 1000000 / (OS_FREQUENCY);

					*timer_id = rec - OS_timer_table;
					tmr_init(&rec->tmr, timer_handler);
					strcpy(rec->name, timer_name);
					rec->creator = OS_TaskGetId();
					rec->used = 1;
					rec->handler = callback_ptr;
					status = OS_SUCCESS;
				}
			}
		}
	}
	sys_unlock();

	return status;
}

int32 OS_TimerSet(uint32 timer_id, uint32 start_msec, uint32 interval_msec)
{
	OS_timer_record_t *rec = &OS_timer_table[timer_id];
	int32 status;

	sys_lock();
	{
		if (timer_id >= OS_MAX_TIMERS)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else
		{
			tmr_start(&rec->tmr, start_msec * MSEC, interval_msec * MSEC);
			status = OS_SUCCESS;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_TimerDelete(uint32 timer_id)
{
	OS_timer_record_t *rec = &OS_timer_table[timer_id];
	int32 status;

	sys_lock();
	{
		if (timer_id >= OS_MAX_TIMERS)
			status = OS_ERR_INVALID_ID;
		else if (rec->used == 0)
			status = OS_INVALID_POINTER;
		else
		{
			tmr_delete(&rec->tmr);
			rec->used = 0;
			status = OS_SUCCESS;
		}
	}
	sys_unlock();

	return status;
}

int32 OS_TimerGetIdByName(uint32 *timer_id, const char *timer_name)
{
	OS_timer_record_t *rec;
	int32 status;

	sys_lock();
	{
		if (!timer_id || !timer_name)
			status = OS_INVALID_POINTER;
		else if (strlen(timer_name) >= OS_MAX_API_NAME)
			status = OS_ERR_NAME_TOO_LONG;
		else
		{
			for (rec = OS_timer_table; rec < OS_timer_table + OS_MAX_TIMERS; rec++)
				if (rec->used)
					if (strcmp(rec->name, timer_name) == 0)
						break;

			if (rec >= OS_timer_table + OS_MAX_TIMERS)
				status = OS_ERR_NAME_NOT_FOUND;
			else
			{
				*timer_id = rec - OS_timer_table;
				status = OS_SUCCESS;
			}
		}
	}
	sys_unlock();

	return status;
}

int32 OS_TimerGetInfo(uint32 timer_id, OS_timer_prop_t *timer_prop)
{
	OS_timer_record_t *rec = &OS_timer_table[timer_id];
	int32 status;

	sys_lock();
	{
		if (timer_id >= OS_MAX_TIMERS)
			status = OS_ERR_INVALID_ID;
		else if (!timer_prop || !rec->used)
			status = OS_INVALID_POINTER;
		else
		{
			strcpy(timer_prop->name, rec->name);
			timer_prop->creator = rec->creator;
			timer_prop->start_time = rec->tmr.start;
			timer_prop->interval_time = rec->tmr.period;
			timer_prop->accuracy = 1000000 / (OS_FREQUENCY);
			status = OS_SUCCESS;
		}
	}
	sys_unlock();

	return status;
}

/* -------------------------------------------------------------------------- */

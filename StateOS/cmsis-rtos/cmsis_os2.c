/* --------------------------------------------------------------------------
 * Copyright (c) 2013-2017 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *      Name:    cmsis_os2.c
 *      Purpose: CMSIS RTOS2 wrapper for StateOS
 *
 * -------------------------------------------------------------------------- */

/******************************************************************************

    @file    StateOS: cmsis_os2.c
    @author  Rajmund Szymanski
    @date    27.08.2018
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

#include <string.h>
#include "oscmsis.h"

/* -------------------------------------------------------------------------- */

osStatus_t osKernelInitialize (void)
{
	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;

	tsk_prio(osPriorityISR);
	return osOK;
}

osStatus_t osKernelGetInfo (osVersion_t *version, char *id_buf, uint32_t id_size)
{
	if (version == NULL)
		return osError;

	version->api    = osVersionAPI;
	version->kernel = osVersionKernel;

	if (id_buf != NULL)
		strncpy(id_buf, osKernelId, id_size);

	return osOK;
}

osKernelState_t osKernelGetState (void)
{
	return osKernelRunning;
}

osStatus_t osKernelStart (void)
{
	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;

	tsk_prio(osPriorityNormal);
	return osOK;
}

int32_t osKernelLock (void)
{
	int32_t lock;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return (int32_t)osErrorISR;

	lock = port_get_lock();
	port_set_lock();
	return lock;
}

int32_t osKernelUnlock (void)
{
	int32_t lock;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return (int32_t)osErrorISR;

	lock = port_get_lock();
	port_clr_lock();
	return lock;
}

int32_t osKernelRestoreLock (int32_t lock)
{
	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return (int32_t)osErrorISR;

	port_put_lock(lock);
	return lock;
}

uint32_t osKernelSuspend (void)
{
	return 0U;
}

void osKernelResume (uint32_t sleep_ticks)
{
	(void) sleep_ticks;
}

uint32_t osKernelGetTickCount (void)
{
	return sys_time();
}

uint32_t osKernelGetTickFreq (void)
{
	return OS_FREQUENCY;
}

uint32_t osKernelGetSysTimerCount (void)
{
#if HW_TIMER_SIZE
	return sys_time();
#else
	uint32_t cnt;
	uint32_t tck;

	sys_lock();
	{
		cnt = sys_time();
		tck = SysTick->VAL;

		if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
		{
			tck = SysTick->VAL;
			cnt++;
		}

		cnt = (cnt + 1U) * (SysTick->LOAD + 1U) - tck;
	}
	sys_unlock();

	return cnt;
#endif
}

uint32_t osKernelGetSysTimerFreq (void)
{
#if HW_TIMER_SIZE
	return  OS_FREQUENCY;
#elif (CPU_FREQUENCY)/(OS_FREQUENCY)-1 <= SysTick_LOAD_RELOAD_Msk
	return CPU_FREQUENCY;
#else
	return  ST_FREQUENCY;
#endif
}

/* -------------------------------------------------------------------------- */

static void thread_handler (void)
{
	void *tmp = tsk_this(); // because of COSMIC compiler
	osThread_t *thread = tmp;

	thread->func(thread->arg);

	tsk_stop();
}

/* -------------------------------------------------------------------------- */

osThreadId_t osThreadNew (osThreadFunc_t func, void *argument, const osThreadAttr_t *attr)
{
	osThread_t *thread     = NULL;
	uint32_t    flags      = osThreadJoinable;
	void       *stack_mem  = NULL;
	uint32_t    stack_size = 0;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED() || (func == NULL))
		return NULL;

	if (attr != NULL)
	{
		if ((attr->priority < osPriorityIdle) || (attr->priority > osPriorityISR))
			return NULL;

		flags = attr->attr_bits;

		if (attr->cb_size != 0U)
		{
			thread = attr->cb_mem;
			if (attr->cb_size != osThreadCbSize)
				return NULL;
		}

		if (attr->stack_size != 0U)
		{
			stack_size = attr->stack_size;
			stack_mem  = attr->stack_mem;
		}
	}

	if (thread == NULL && stack_mem == NULL)
	{
		stack_size = osThreadStackSize(stack_size);
		thread = sys_alloc(ABOVE(osThreadCbSize) + stack_size);
		stack_mem = (void *)((size_t)thread + ABOVE(osThreadCbSize));
		if (thread == NULL)
			return NULL;
	}
	else
	if (thread == NULL)
	{
		thread = sys_alloc(osThreadCbSize);
		if (thread == NULL)
			return NULL;
	}
	else
	if (stack_mem == NULL)
	{
		stack_size = osThreadStackSize(stack_size);
		stack_mem = sys_alloc(stack_size);
		if (stack_mem == NULL)
			return NULL;
	}

	sys_lock();
	{
		tsk_init(&thread->tsk, (attr == NULL) ? osPriorityNormal : attr->priority, thread_handler, stack_mem, stack_size);
		if (attr->cb_mem    == NULL || attr->cb_size    == 0U) thread->tsk.obj.res = thread;
		else
		if (attr->stack_mem == NULL || attr->stack_size == 0U) thread->tsk.obj.res = stack_mem;
		thread->tsk.join = (flags & osThreadJoinable) ? JOINABLE : DETACHED;
		flg_init(&thread->flg, 0);
		thread->flags = flags;
		thread->name = (attr == NULL) ? NULL : attr->name;
		thread->func = func;
		thread->arg = argument;
	}
	sys_unlock();

	return thread;
}

const char *osThreadGetName (osThreadId_t thread_id)
{
	osThread_t *thread = thread_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED() || (thread_id == NULL))
		return NULL;

	return thread->name;
}

osThreadId_t osThreadGetId (void)
{
	return (osThreadId_t) tsk_this();
}

osThreadState_t osThreadGetState (osThreadId_t thread_id)
{
	osThread_t *thread = thread_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED() || (thread_id == NULL))
		return osThreadError;

	switch (thread->tsk.id)
	{
		case ID_STOPPED: return osThreadTerminated;
		case ID_READY:   return osThreadReady;
		case ID_DELAYED: return osThreadBlocked;
		default:         return osThreadError;
	}
}

osStatus_t osThreadSetPriority (osThreadId_t thread_id, osPriority_t priority)
{
	osThread_t *thread = thread_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if ((thread_id == NULL) || (priority < osPriorityIdle) || (priority > osPriorityISR))
		return osErrorParameter;

	sys_lock();
	{
		core_tsk_prio(&thread->tsk, thread->tsk.basic = priority);
	}
	sys_unlock();

	return osOK;
}

osPriority_t osThreadGetPriority (osThreadId_t thread_id)
{
	osThread_t *thread = thread_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED() || (thread_id == NULL))
		return osPriorityError;

	return (osPriority_t)thread->tsk.basic;
}

osStatus_t osThreadYield (void)
{
	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;

	tsk_yield();

	return osOK;
}

osStatus_t osThreadSuspend (osThreadId_t thread_id)
{
	osThread_t *thread = thread_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if (thread_id == NULL)
		return osErrorParameter;

	switch (tsk_suspend(&thread->tsk))
	{
		case E_SUCCESS: return osOK;
		default:        return osErrorResource;
	}
}

osStatus_t osThreadResume (osThreadId_t thread_id)
{
	osThread_t *thread = thread_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if (thread_id == NULL)
		return osErrorParameter;

	switch (tsk_resume(&thread->tsk))
	{
		case E_SUCCESS: return osOK;
		default:        return osErrorResource;
	}
}

osStatus_t osThreadDetach (osThreadId_t thread_id)
{
	osThread_t *thread = thread_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if (thread_id == NULL)
		return osErrorParameter;

	switch (tsk_detach(&thread->tsk))
	{
		case E_SUCCESS: return osOK;
		default:        return osErrorResource;
	}
}

osStatus_t osThreadJoin (osThreadId_t thread_id)
{
	osThread_t *thread = thread_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if (thread_id == NULL)
		return osErrorParameter;

	switch (tsk_join(&thread->tsk))
	{
		case E_SUCCESS:
		case E_STOPPED: return osOK;
		default:        return osErrorResource;
	}
}

__NO_RETURN
void osThreadExit (void)
{
	tsk_stop();
}

osStatus_t osThreadTerminate (osThreadId_t thread_id)
{
	osThread_t *thread = thread_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if (thread_id == NULL)
		return osErrorParameter;

	tsk_delete(&thread->tsk);

	return osOK;
}

uint32_t osThreadGetStackSize (osThreadId_t thread_id)
{
	osThread_t *thread = thread_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED() || (thread_id == NULL))
		return 0U;

	return (uint32_t) thread->tsk.size;
}

uint32_t osThreadGetStackSpace (osThreadId_t thread_id)
{
	osThread_t *thread = thread_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED() || (thread_id == NULL))
		return 0U;

	if (&thread->tsk == &MAIN)
		return 0U;

	if (&thread->tsk != tsk_this())
		return (uint32_t) thread->tsk.sp - (uint32_t) thread->tsk.stack;

	return (uint32_t) port_get_sp() - (uint32_t) thread->tsk.stack;
}

uint32_t osThreadGetCount (void)
{
	tsk_t   *tsk;
	tmr_t   *tmr;
	uint32_t count = 0;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return 0U;

	sys_lock();
	{
		count++;

		for (tsk = IDLE.obj.next; tsk != &IDLE; tsk = tsk->obj.next)
			count++;

		for (tmr = WAIT.obj.next; tmr != &WAIT; tmr = tmr->obj.next)
			if (tmr->id == ID_DELAYED)
				count++;
	}
	sys_unlock();

	return count;
}

uint32_t osThreadEnumerate (osThreadId_t *thread_array, uint32_t array_items)
{
	tsk_t   *tsk;
	tmr_t   *tmr;
	uint32_t count = 0;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED() || (thread_array == NULL) || (array_items == 0U))
		return 0U;

	sys_lock();
	{
		thread_array[count++] = &IDLE;

		for (tsk = IDLE.obj.next; (tsk != &IDLE) && (count < array_items); tsk = tsk->obj.next)
			thread_array[count++] = tsk;

		for (tmr = WAIT.obj.next; (tmr != &WAIT) && (count < array_items); tmr = tmr->obj.next)
			if (tmr->id == ID_DELAYED)
				thread_array[count++] = tmr;
	}
	sys_unlock();

	return count;
}

/* -------------------------------------------------------------------------- */

uint32_t osThreadFlagsSet (osThreadId_t thread_id, uint32_t flags)
{
	osThread_t *thread = thread_id;

	if ((thread_id == NULL) || ((flags & osFlagsError) != 0U))
		return osFlagsErrorParameter;

	return flg_give(&thread->flg, flags);
}

uint32_t osThreadFlagsClear (uint32_t flags)
{
	void *tmp = tsk_this(); // because of COSMIC compiler
	osThread_t *thread = tmp;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osFlagsErrorISR;

	return flg_clear(&thread->flg, flags);
}

uint32_t osThreadFlagsGet (void)
{
	void *tmp = tsk_this(); // because of COSMIC compiler
	osThread_t *thread = tmp;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osFlagsErrorISR;

	return thread->flg.flags;
}

uint32_t osThreadFlagsWait (uint32_t flags, uint32_t options, uint32_t timeout)
{
	void *tmp = tsk_this(); // because of COSMIC compiler
	osThread_t *thread = tmp;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osFlagsErrorISR;
	if ((flags & osFlagsError) != 0U)
		return osFlagsErrorParameter;

	switch (flg_waitFor(&thread->flg, flags, options, timeout))
	{
		case E_SUCCESS: return flags;
		case E_TIMEOUT: return osFlagsErrorTimeout;
		default:        return osFlagsErrorResource;
	}
}

/* -------------------------------------------------------------------------- */

osStatus_t osDelay (uint32_t ticks)
{
	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;

	switch (tsk_sleepFor(ticks))
	{
		case E_TIMEOUT: return osOK;
		default:        return osError;
	}
}

osStatus_t osDelayUntil (uint32_t ticks)
{
	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;

	switch (tsk_sleepUntil(ticks))
	{
		case E_TIMEOUT: return osOK;
		default:        return osError;
	}
}

/* -------------------------------------------------------------------------- */

static void timer_handler (void)
{
	void *tmp = tmr_thisISR(); // because of COSMIC compiler
	osTimer_t *timer = tmp;

	timer->func(timer->arg);
}

/* -------------------------------------------------------------------------- */

osTimerId_t osTimerNew (osTimerFunc_t func, osTimerType_t type, void *argument, const osTimerAttr_t *attr)
{
	osTimer_t *timer = NULL;
	uint32_t   flags = type;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED() || (func == NULL))
		return NULL;

	if (attr != NULL)
	{
		if (attr->cb_size != 0U)
		{
			timer = attr->cb_mem;
			if (attr->cb_size != osTimerCbSize)
				return NULL;
		}
	}

	if (timer == NULL)
	{
		timer = sys_alloc(osTimerCbSize);
		if (timer == NULL)
			return NULL;
	}

	sys_lock();
	{
		tmr_init(&timer->tmr, timer_handler);
		if (attr->cb_mem == NULL || attr->cb_size == 0U) timer->tmr.obj.res = timer;
		timer->flags = flags;
		timer->name = (attr == NULL) ? NULL : attr->name;
		timer->func = func;
		timer->arg = argument;
	}
	sys_unlock();

	return timer;
}

const char *osTimerGetName (osTimerId_t timer_id)
{
	osTimer_t *timer = timer_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED() || (timer_id == NULL))
		return NULL;

	return timer->name;
}

osStatus_t osTimerStart (osTimerId_t timer_id, uint32_t ticks)
{
	osTimer_t *timer = timer_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if (timer_id == NULL)
		return osErrorParameter;

	tmr_start(&timer->tmr, ticks, (timer->flags & osTimerPeriodic) ? ticks : 0);

	return osOK;
}

osStatus_t osTimerStop (osTimerId_t timer_id)
{
	osTimer_t *timer = timer_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if (timer_id == NULL)
		return osErrorParameter;

	tmr_kill(&timer->tmr);

	return osOK;
}

uint32_t osTimerIsRunning (osTimerId_t timer_id)
{
	osTimer_t *timer = timer_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED() || (timer_id == NULL))
		return 0U;

	return (timer->tmr.id != ID_STOPPED);
}

osStatus_t osTimerDelete (osTimerId_t timer_id)
{
	osTimer_t *timer = timer_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if (timer_id == NULL)
		return osErrorParameter;

	tmr_delete(&timer->tmr);

	return osOK;
}

/* -------------------------------------------------------------------------- */

osEventFlagsId_t osEventFlagsNew (const osEventFlagsAttr_t *attr)
{
	osEventFlags_t *ef    = NULL;
	uint32_t        flags = 0U;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return NULL;

	if (attr != NULL)
	{
		if (attr->cb_size != 0U)
		{
			ef = attr->cb_mem;
			if (attr->cb_size != osEventFlagsCbSize)
				return NULL;
		}
	}

	if (ef == NULL)
	{
		ef = sys_alloc(osEventFlagsCbSize);
		if (ef == NULL)
			return NULL;
	}

	sys_lock();
	{
		flg_init(&ef->flg, 0);
		if (attr->cb_mem == NULL || attr->cb_size == 0U) ef->flg.res = ef;
		ef->flags = flags;
		ef->name = (attr == NULL) ? NULL : attr->name;
	}
	sys_unlock();

	return ef;
}

uint32_t osEventFlagsSet (osEventFlagsId_t ef_id, uint32_t flags)
{
	osEventFlags_t *ef = ef_id;

	if ((ef_id == NULL) || ((flags & osFlagsError) != 0U))
		return osFlagsErrorParameter;

	return flg_give(&ef->flg, flags);
}

uint32_t osEventFlagsClear (osEventFlagsId_t ef_id, uint32_t flags)
{
	osEventFlags_t *ef = ef_id;

	if ((ef_id == NULL) || ((flags & osFlagsError) != 0U))
		return osFlagsErrorParameter;

	return flg_clear(&ef->flg, flags);
}

uint32_t osEventFlagsGet (osEventFlagsId_t ef_id)
{
	osEventFlags_t *ef = ef_id;

	if (ef_id == NULL)
		return 0U;

	return ef->flg.flags;
}

uint32_t osEventFlagsWait (osEventFlagsId_t ef_id, uint32_t flags, uint32_t options, uint32_t timeout)
{
	osEventFlags_t *ef = ef_id;

	if ((ef_id == NULL) || ((flags & osFlagsError) != 0U))
		return osFlagsErrorParameter;
	if ((IS_IRQ_MODE() || IS_IRQ_MASKED()) && (timeout != 0U))
		return osFlagsErrorParameter;

	switch (flg_waitFor(&ef->flg, flags, options, timeout))
	{
		case E_SUCCESS: return flags;
		case E_TIMEOUT: return osFlagsErrorTimeout;
		default:        return osFlagsErrorResource;
	}
}

osStatus_t osEventFlagsDelete (osEventFlagsId_t ef_id)
{
	osEventFlags_t *ef = ef_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if (ef_id == NULL)
		return osErrorParameter;

	flg_delete(&ef->flg);

	return osOK;
}

const char *osEventFlagsGetName (osEventFlagsId_t ef_id)
{
	osEventFlags_t *ef = ef_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED() || (ef_id == NULL))
		return NULL;

	return ef->name;
}

/* -------------------------------------------------------------------------- */

osMutexId_t osMutexNew (const osMutexAttr_t *attr)
{
	osMutex_t *mutex = NULL;
	uint32_t   flags = 0U;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return NULL;

	if (attr != NULL)
	{
		flags = attr->attr_bits;

		if (attr->cb_size != 0U)
		{
			mutex = attr->cb_mem;
			if (attr->cb_size != osMutexCbSize)
				return NULL;
		}
	}

	if (mutex == NULL)
	{
		mutex = sys_alloc(osMutexCbSize);
		if (mutex == NULL)
			return NULL;
	}

	sys_lock();
	{
		mtx_init(&mutex->mtx);
		if (attr->cb_mem == NULL || attr->cb_size == 0U) mutex->mtx.res = mutex;
		mutex->flags = flags;
		mutex->name = (attr == NULL) ? NULL : attr->name;
	}
	sys_unlock();

	return mutex;
}

const char *osMutexGetName (osMutexId_t mutex_id)
{
	osMutex_t *mutex = mutex_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED() || (mutex_id == NULL))
		return NULL;

	return mutex->name;
}

osStatus_t osMutexAcquire (osMutexId_t mutex_id, uint32_t timeout)
{
	osMutex_t *mutex = mutex_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if (mutex_id == NULL)
		return osErrorParameter;

	switch (mtx_waitFor(&mutex->mtx, timeout))
	{
		case E_SUCCESS: return osOK;
		case E_TIMEOUT: return osErrorTimeout;
		default:        return osErrorResource;
	}
}

osStatus_t osMutexRelease (osMutexId_t mutex_id)
{
	osMutex_t *mutex = mutex_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if (mutex_id == NULL)
		return osErrorParameter;

	switch (mtx_give(&mutex->mtx))
	{
		case E_SUCCESS: return osOK;
		default:        return osErrorResource;
	}
}

osThreadId_t osMutexGetOwner (osMutexId_t mutex_id)
{
	osMutex_t *mutex = mutex_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED() || (mutex_id == NULL))
		return NULL;

	return mutex->mtx.owner;
}

osStatus_t osMutexDelete (osMutexId_t mutex_id)
{
	osMutex_t *mutex = mutex_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if (mutex_id == NULL)
		return osErrorParameter;

	mtx_delete(&mutex->mtx);

	return osOK;
}

/* -------------------------------------------------------------------------- */

osSemaphoreId_t osSemaphoreNew (uint32_t max_count, uint32_t initial_count, const osSemaphoreAttr_t *attr)
{
	osSemaphore_t *semaphore = NULL;
	uint32_t    flags = 0U;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return NULL;

	if (attr != NULL)
	{
		if (attr->cb_size != 0U)
		{
			semaphore = attr->cb_mem;
			if (attr->cb_size != osSemaphoreCbSize)
				return NULL;
		}
	}

	if (semaphore == NULL)
	{
		semaphore = sys_alloc(osSemaphoreCbSize);
		if (semaphore == NULL)
			return NULL;
	}

	sys_lock();
	{
		sem_init(&semaphore->sem, initial_count, max_count);
		if (attr->cb_mem == NULL || attr->cb_size == 0U) semaphore->sem.res = semaphore;
		semaphore->flags = flags;
		semaphore->name = (attr == NULL) ? NULL : attr->name;
	}
	sys_unlock();

	return semaphore;
}

const char *osSemaphoreGetName (osSemaphoreId_t semaphore_id)
{
	osSemaphore_t *semaphore = semaphore_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED() || (semaphore_id == NULL))
		return NULL;

	return semaphore->name;
}

osStatus_t osSemaphoreAcquire (osSemaphoreId_t semaphore_id, uint32_t timeout)
{
	osSemaphore_t *semaphore = semaphore_id;

	if (semaphore_id == NULL)
		return osErrorParameter;
	if ((IS_IRQ_MODE() || IS_IRQ_MASKED()) && (timeout != 0U))
		return osErrorParameter;

	switch (sem_waitFor(&semaphore->sem, timeout))
	{
		case E_SUCCESS: return osOK;
		case E_TIMEOUT: return osErrorTimeout;
		default:        return osErrorResource;
	}
}

osStatus_t osSemaphoreRelease (osSemaphoreId_t semaphore_id)
{
	osSemaphore_t *semaphore = semaphore_id;

	if (semaphore_id == NULL)
		return osErrorParameter;

	switch (sem_give(&semaphore->sem))
	{
		case E_SUCCESS: return osOK;
		default:        return osErrorResource;
	}
}

uint32_t osSemaphoreGetCount (osSemaphoreId_t semaphore_id)
{
	osSemaphore_t *semaphore = semaphore_id;

	if (semaphore_id == NULL)
		return 0U;

	return semaphore->sem.count;
}

osStatus_t osSemaphoreDelete (osSemaphoreId_t semaphore_id)
{
	osSemaphore_t *semaphore = semaphore_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if (semaphore_id == NULL)
		return osErrorParameter;

	sem_delete(&semaphore->sem);

	return osOK;
}

/* -------------------------------------------------------------------------- */

osMemoryPoolId_t osMemoryPoolNew (uint32_t block_count, uint32_t block_size, const osMemoryPoolAttr_t *attr)
{
	osMemoryPool_t *mp    = NULL;
	uint32_t        flags = 0U;
	void           *data  = NULL;
	uint32_t        size  = osMemoryPoolMemSize(block_count, block_size);

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return NULL;

	if (attr != NULL)
	{
		if (attr->cb_size != 0U)
		{
			mp = attr->cb_mem;
			if (attr->cb_size != osMemoryPoolCbSize)
				return NULL;
		}

		if (attr->mp_size != 0U)
		{
			data = attr->mp_mem;
			if (attr->mp_size != size)
				return NULL;
		}
	}

	if (mp == NULL && data == NULL)
	{
		mp = sys_alloc(ABOVE(osMemoryPoolCbSize) + size);
		data = (void *)((size_t)mp + ABOVE(osMemoryPoolCbSize));
		if (mp == NULL)
			return NULL;
	}
	else
	if (mp == NULL)
	{
		mp = sys_alloc(osMemoryPoolCbSize);
		if (mp == NULL)
			return NULL;
	}
	else
	if (data == NULL)
	{
		data = sys_alloc(size);
		if (data == NULL)
			return NULL;
	}

	sys_lock();
	{
		mem_init(&mp->mem, block_size, data, size);
		if (attr->cb_mem == NULL || attr->cb_size == 0U) mp->mem.res = mp;
		else
		if (attr->mp_mem == NULL || attr->mp_size == 0U) mp->mem.res = data;
		mp->flags = flags;
		mp->name = (attr == NULL) ? NULL : attr->name;
	}
	sys_unlock();

	return mp;
}

const char *osMemoryPoolGetName (osMemoryPoolId_t mp_id)
{
	osMemoryPool_t *mp = mp_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED() || (mp_id == NULL))
		return NULL;

	return mp->name;
}

void *osMemoryPoolAlloc (osMemoryPoolId_t mp_id, uint32_t timeout)
{
	osMemoryPool_t *mp = mp_id;
	void           *block;

	if (mp_id == NULL)
		return NULL;
	if ((IS_IRQ_MODE() || IS_IRQ_MASKED()) && (timeout != 0U))
		return NULL;

	switch (mem_waitFor(&mp->mem, &block, timeout))
	{
		case E_SUCCESS: return block;
		default:        return NULL;
	}
}

osStatus_t osMemoryPoolFree (osMemoryPoolId_t mp_id, void *block)
{
	osMemoryPool_t *mp = mp_id;

	if (mp_id == NULL)
		return osErrorParameter;

	mem_give(&mp->mem, block);

	return osOK;
}

uint32_t osMemoryPoolGetCapacity (osMemoryPoolId_t mp_id)
{
	osMemoryPool_t *mp = mp_id;

	if (mp_id == NULL)
		return 0U;

	return mp->mem.limit;
}

uint32_t osMemoryPoolGetBlockSize (osMemoryPoolId_t mp_id)
{
	osMemoryPool_t *mp = mp_id;

	if (mp_id == NULL)
		return 0U;

	return mp->mem.size;
}

uint32_t osMemoryPoolGetCount (osMemoryPoolId_t mp_id)
{
	osMemoryPool_t *mp = mp_id;
	uint32_t     count = mp->mem.limit;
	que_t         *que;

	if (mp_id == NULL)
		return 0U;

	sys_lock();
	{
		for (que = mp->mem.head.next; que != NULL; que = que->next) count--;
	}
	sys_unlock();

	return count;
}

uint32_t osMemoryPoolGetSpace (osMemoryPoolId_t mp_id)
{
	osMemoryPool_t *mp = mp_id;
	uint32_t     count = 0;
	que_t        *que;

	if (mp_id == NULL)
		return 0U;

	sys_lock();
	{
		for (que = mp->mem.head.next; que != NULL; que = que->next) count++;
	}
	sys_unlock();

	return count;
}

osStatus_t osMemoryPoolDelete (osMemoryPoolId_t mp_id)
{
	osMemoryPool_t *mp = mp_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if (mp_id == NULL)
		return osErrorParameter;

	mem_delete(&mp->mem);

	return osOK;
}

/* -------------------------------------------------------------------------- */

osMessageQueueId_t osMessageQueueNew (uint32_t msg_count, uint32_t msg_size, const osMessageQueueAttr_t *attr)
{
	osMessageQueue_t *mq    = NULL;
	uint32_t          flags = 0U;
	void             *data  = NULL;
	uint32_t          size  = osMessageQueueMemSize(msg_count, msg_size);

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return NULL;

	if (attr != NULL)
	{
		if (attr->cb_size != 0U)
		{
			mq = attr->cb_mem;
			if (attr->cb_size != osMessageQueueCbSize)
				return NULL;
		}

		if (attr->mq_size != 0U)
		{
			data = attr->mq_mem;
			if (attr->mq_size != size)
				return NULL;
		}
	}

	if (mq == NULL && data == NULL)
	{
		mq = sys_alloc(ABOVE(osMessageQueueCbSize) + size);
		data = (void *)((size_t)mq + ABOVE(osMessageQueueCbSize));
		if (mq == NULL)
			return NULL;
	}
	else
	if (mq == NULL)
	{
		mq = sys_alloc(osMessageQueueCbSize);
		if (mq == NULL)
			return NULL;
	}
	else
	if (data == NULL)
	{
		data = sys_alloc(size);
		if (data == NULL)
			return NULL;
	}

	sys_lock();
	{
		box_init(&mq->box, msg_count, data, msg_size);
		if (attr->cb_mem == NULL || attr->cb_size == 0U) mq->box.res = mq;
		else
		if (attr->mq_mem == NULL || attr->mq_size == 0U) mq->box.res = data;
		mq->flags = flags;
		mq->name = (attr == NULL) ? NULL : attr->name;
	}
	sys_unlock();

	return mq;
}

const char *osMessageQueueGetName (osMessageQueueId_t mq_id)
{
	osMessageQueue_t *mq = mq_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED() || (mq_id == NULL))
		return NULL;

	return mq->name;
}

osStatus_t osMessageQueuePut (osMessageQueueId_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout)
{
	osMessageQueue_t *mq = mq_id;

	(void)msg_prio; /* message priority is ignored */

	if ((mq_id == NULL) || (msg_ptr == NULL))
		return osErrorParameter;

	if ((IS_IRQ_MODE() || IS_IRQ_MASKED()) && (timeout != 0U))
		return osErrorParameter;

	switch (box_sendFor(&mq->box, msg_ptr, timeout))
	{
		case E_SUCCESS: return osOK;
		case E_TIMEOUT: return osErrorTimeout;
		default:        return osErrorResource;
	}
}

osStatus_t osMessageQueueGet (osMessageQueueId_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout)
{
	osMessageQueue_t *mq = mq_id;

	(void)msg_prio; /* message priority is ignored */

	if ((mq_id == NULL) || (msg_ptr == NULL))
		return osErrorParameter;

	if ((IS_IRQ_MODE() || IS_IRQ_MASKED()) && (timeout != 0U))
		return osErrorParameter;

	switch (box_waitFor(&mq->box, msg_ptr, timeout))
	{
		case E_SUCCESS: return osOK;
		case E_TIMEOUT: return osErrorTimeout;
		default:        return osErrorResource;
	}
}

uint32_t osMessageQueueGetCapacity (osMessageQueueId_t mq_id)
{
	osMessageQueue_t *mq = mq_id;

	if (mq_id == NULL)
		return 0U;

	return mq->box.limit;
}

uint32_t osMessageQueueGetMsgSize (osMessageQueueId_t mq_id)
{
	osMessageQueue_t *mq = mq_id;

	if (mq_id == NULL)
		return 0U;

	return mq->box.size;
}

uint32_t osMessageQueueGetCount (osMessageQueueId_t mq_id)
{
	osMessageQueue_t *mq = mq_id;

	if (mq_id == NULL)
		return 0U;

	return mq->box.count;
}

uint32_t osMessageQueueGetSpace (osMessageQueueId_t mq_id)
{
	osMessageQueue_t *mq = mq_id;
	uint32_t       count;

	if (mq_id == NULL)
		return 0U;

	sys_lock();
	{
		count = mq->box.limit - mq->box.count;
	}
	sys_unlock();

	return count;
}

osStatus_t osMessageQueueReset (osMessageQueueId_t mq_id)
{
	osMessageQueue_t *mq = mq_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if (mq_id == NULL)
		return osErrorParameter;

	box_kill(&mq->box);

	return osOK;
}

osStatus_t osMessageQueueDelete (osMessageQueueId_t mq_id)
{
	osMessageQueue_t *mq = mq_id;

	if (IS_IRQ_MODE() || IS_IRQ_MASKED())
		return osErrorISR;
	if (mq_id == NULL)
		return osErrorParameter;

	box_delete(&mq->box);

	return osOK;
}

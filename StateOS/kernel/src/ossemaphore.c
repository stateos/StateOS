/******************************************************************************

    @file    StateOS: ossemaphore.c
    @author  Rajmund Szymanski
    @date    13.05.2018
    @brief   This file provides set of functions for StateOS.

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

#include "inc/ossemaphore.h"

/* -------------------------------------------------------------------------- */
void sem_init( sem_t *sem, unsigned init, unsigned limit )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(sem);
	assert(init<=limit);

	port_sys_lock();

	memset(sem, 0, sizeof(sem_t));

	sem->count = init;
	sem->limit = limit;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
sem_t *sem_create( unsigned init, unsigned limit )
/* -------------------------------------------------------------------------- */
{
	sem_t *sem;

	assert(!port_isr_inside());

	port_sys_lock();

	sem = core_sys_alloc(sizeof(sem_t));
	sem_init(sem, init, limit);
	sem->res = sem;

	port_sys_unlock();

	return sem;
}

/* -------------------------------------------------------------------------- */
void sem_kill( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(sem);

	port_sys_lock();

	sem->count = 0;

	core_all_wakeup(sem, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void sem_delete( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	sem_kill(sem);
	core_sys_free(sem->res);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned sem_take( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(sem);
	assert(sem->limit);

	port_sys_lock();

	if (sem->count > 0)
	{
		if (core_one_wakeup(sem, E_SUCCESS) == 0)
			sem->count--;
		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_sem_wait( sem_t *sem, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_inside());
	assert(sem);
	assert(sem->limit);

	port_sys_lock();

	if (sem->count > 0)
	{
		if (core_one_wakeup(sem, E_SUCCESS) == 0)
			sem->count--;
		event = E_SUCCESS;
	}
	else
	{
		event = wait(sem, time);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned sem_waitUntil( sem_t *sem, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_sem_wait(sem, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned sem_waitFor( sem_t *sem, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_sem_wait(sem, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned sem_give( sem_t *sem )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(sem);
	assert(sem->limit);

	port_sys_lock();

	if (sem->count < sem->limit)
	{
		if (core_one_wakeup(sem, E_SUCCESS) == 0)
			sem->count++;
		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_sem_send( sem_t *sem, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_inside());
	assert(sem);
	assert(sem->limit);

	port_sys_lock();

	if (sem->count < sem->limit)
	{
		if (core_one_wakeup(sem, E_SUCCESS) == 0)
			sem->count++;
		event = E_SUCCESS;
	}
	else
	{
		event = wait(sem, time);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned sem_sendUntil( sem_t *sem, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_sem_send(sem, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned sem_sendFor( sem_t *sem, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_sem_send(sem, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */

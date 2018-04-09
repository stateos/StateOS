/******************************************************************************

    @file    StateOS: os_job.c
    @author  Rajmund Szymanski
    @date    09.04.2018
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

#include <os.h>

/* -------------------------------------------------------------------------- */
void job_init( job_t *job, unsigned limit, fun_t **data )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(job);
	assert(limit);
	assert(data);

	port_sys_lock();

	memset(job, 0, sizeof(job_t));

	job->limit = limit;
	job->data  = data;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
job_t *job_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	job_t *job;

	assert(!port_isr_inside());
	assert(limit);

	port_sys_lock();

	job = core_sys_alloc(ABOVE(sizeof(job_t)) + limit * sizeof(fun_t *));
	job_init(job, limit, (void *)((size_t)job + ABOVE(sizeof(job_t))));
	job->res = job;

	port_sys_unlock();

	return job;
}

/* -------------------------------------------------------------------------- */
void job_kill( job_t *job )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(job);

	port_sys_lock();

	job->count = 0;
	job->first = 0;
	job->next  = 0;

	core_all_wakeup(job, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void job_delete( job_t *job )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	job_kill(job);
	core_sys_free(job->res);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_job_get( job_t *job, fun_t **fun )
/* -------------------------------------------------------------------------- */
{
	*fun = job->data[job->first];

	job->first = (job->first + 1) % job->limit;
	job->count--;
}

/* -------------------------------------------------------------------------- */
static
void priv_job_put( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	job->data[job->next] = fun;

	job->next = (job->next + 1) % job->limit;
	job->count++;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_job_wait( job_t *job, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned event = E_SUCCESS;

	assert(job);

	port_sys_lock();

	if (job->count == 0)
	{
		event = wait(job, time);
	}
	else
	{
		priv_job_get(job, &System.cur->tmp.fun);

		tsk = core_one_wakeup(job, E_SUCCESS);

		if (tsk) priv_job_put(job, tsk->tmp.fun);
	}

	if (event == E_SUCCESS)
	{
		System.cur->tmp.fun();
	}
	
	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned job_waitUntil( job_t *job, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	return priv_job_wait(job, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned job_waitFor( job_t *job, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside() || !delay);

	return priv_job_wait(job, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_job_send( job_t *job, fun_t *fun, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned event = E_SUCCESS;

	assert(job);
	assert(fun);

	port_sys_lock();

	if (job->count >= job->limit)
	{
		System.cur->tmp.fun = fun;

		event = wait(job, time);
	}
	else
	{
		priv_job_put(job, fun);

		tsk = core_one_wakeup(job, E_SUCCESS);

		if (tsk) priv_job_get(job, &tsk->tmp.fun);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned job_sendUntil( job_t *job, fun_t *fun, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	return priv_job_send(job, fun, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned job_sendFor( job_t *job, fun_t *fun, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside() || !delay);

	return priv_job_send(job, fun, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void job_push( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	assert(job);
	assert(fun);

	port_sys_lock();

	while (job->count >= job->limit)
	{
		job->first = (job->first + 1) % job->limit;
		job->count--;
	}

	priv_job_put(job, fun);

	tsk = core_one_wakeup(job, E_SUCCESS);

	if (tsk) priv_job_get(job, &tsk->tmp.fun);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */

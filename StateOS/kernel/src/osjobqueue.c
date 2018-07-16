/******************************************************************************

    @file    StateOS: osjobqueue.c
    @author  Rajmund Szymanski
    @date    16.07.2018
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

#include "inc/osjobqueue.h"
#include "inc/ostask.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void job_init( job_t *job, unsigned limit, fun_t **data )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(job);
	assert(limit);
	assert(data);

	sys_lock();
	{
		memset(job, 0, sizeof(job_t));

		job->limit = limit;
		job->data  = data;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
job_t *job_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	job_t *job;

	assert(!port_isr_inside());
	assert(limit);

	sys_lock();
	{
		job = core_sys_alloc(ABOVE(sizeof(job_t)) + limit * sizeof(fun_t *));
		job_init(job, limit, (void *)((size_t)job + ABOVE(sizeof(job_t))));
		job->res = job;
	}
	sys_unlock();

	return job;
}

/* -------------------------------------------------------------------------- */
void job_kill( job_t *job )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(job);

	sys_lock();
	{
		job->count = 0;
		job->head  = 0;
		job->tail  = 0;

		core_all_wakeup(job, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void job_delete( job_t *job )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		job_kill(job);
		core_sys_free(job->res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
fun_t *priv_job_get( job_t *job )
/* -------------------------------------------------------------------------- */
{
	fun_t  * fun;
	unsigned i = job->head;

	fun = job->data[i++];
	job->head = (i < job->limit) ? i : 0;
	job->count--;

	return fun;
}

/* -------------------------------------------------------------------------- */
static
void priv_job_put( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	unsigned i = job->tail;

	job->data[i++] = fun;

	job->tail = (i < job->limit) ? i : 0;
	job->count++;
}

/* -------------------------------------------------------------------------- */
unsigned job_take( job_t *job )
/* -------------------------------------------------------------------------- */
{
	fun_t  * fun;
	tsk_t  * tsk;
	unsigned event = E_TIMEOUT;

	assert(job);

	sys_lock();
	{
		if (job->count > 0)
		{
			fun = priv_job_get(job);
			tsk = core_one_wakeup(job, E_SUCCESS);
			if (tsk) priv_job_put(job, tsk->tmp.job.fun);
			fun();
			event = E_SUCCESS;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_job_wait( job_t *job, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned event;

	assert(!port_isr_inside());
	assert(job);

	sys_lock();
	{
		if (job->count > 0)
		{
			System.cur->tmp.job.fun = priv_job_get(job);
			tsk = core_one_wakeup(job, E_SUCCESS);
			if (tsk) priv_job_put(job, tsk->tmp.job.fun);
			event = E_SUCCESS;
		}
		else
		{
			event = wait(job, time);
		}

		if (event == E_SUCCESS)
			System.cur->tmp.job.fun();
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned job_waitUntil( job_t *job, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_job_wait(job, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned job_waitFor( job_t *job, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_job_wait(job, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned job_give( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned event = E_TIMEOUT;

	assert(job);
	assert(fun);

	sys_lock();
	{
		if (job->count < job->limit)
		{
			priv_job_put(job, fun);
			tsk = core_one_wakeup(job, E_SUCCESS);
			if (tsk) tsk->tmp.job.fun = priv_job_get(job);
			event = E_SUCCESS;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_job_send( job_t *job, fun_t *fun, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned event;

	assert(!port_isr_inside());
	assert(job);
	assert(fun);

	sys_lock();
	{
		if (job->count < job->limit)
		{
			priv_job_put(job, fun);
			tsk = core_one_wakeup(job, E_SUCCESS);
			if (tsk) tsk->tmp.job.fun = priv_job_get(job);
			event = E_SUCCESS;
		}
		else
		{
			System.cur->tmp.job.fun = fun;
			event = wait(job, time);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned job_sendUntil( job_t *job, fun_t *fun, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_job_send(job, fun, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned job_sendFor( job_t *job, fun_t *fun, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_job_send(job, fun, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned job_push( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned event = E_TIMEOUT;

	assert(job);
	assert(fun);

	sys_lock();
	{
		if (job->count == 0 || job->queue == 0)
		{
			priv_job_put(job, fun);
			if (job->count > job->limit)
			{
				job->count = job->limit;
				job->head = job->tail;
			}
			tsk = core_one_wakeup(job, E_SUCCESS);
			if (tsk) tsk->tmp.job.fun = priv_job_get(job);
			event = E_SUCCESS;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */

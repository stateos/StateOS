/******************************************************************************

    @file    StateOS: osjobqueue.c
    @author  Rajmund Szymanski
    @date    21.09.2018
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
#include "osalloc.h"

/* -------------------------------------------------------------------------- */
void job_init( job_t *job, fun_t **data, unsigned bufsize )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(job);
	assert(data);
	assert(bufsize);

	sys_lock();
	{
		memset(job, 0, sizeof(job_t));

		core_obj_init(&job->obj);

		job->limit = bufsize / sizeof(fun_t *);
		job->data  = data;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
job_t *job_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	job_t  * job;
	unsigned bufsize;

	assert_tsk_context();
	assert(limit);

	sys_lock();
	{
		bufsize = limit * sizeof(fun_t *);
		job = sys_alloc(SEG_OVER(sizeof(job_t)) + bufsize);
		job_init(job, (void *)((size_t)job + SEG_OVER(sizeof(job_t))), bufsize);
		job->obj.res = job;
	}
	sys_unlock();

	return job;
}

/* -------------------------------------------------------------------------- */
void job_kill( job_t *job )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(job);

	sys_lock();
	{
		job->count = 0;
		job->head  = 0;
		job->tail  = 0;

		core_all_wakeup(&job->obj.queue, E_STOPPED);
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
		sys_free(job->obj.res);
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
static
void priv_job_skip( job_t *job )
/* -------------------------------------------------------------------------- */
{
	job->count--;
	job->head++;
	if (job->head == job->limit) job->head = 0;
}

/* -------------------------------------------------------------------------- */
static
fun_t *priv_job_getUpdate( job_t *job )
/* -------------------------------------------------------------------------- */
{
	fun_t *fun;
	tsk_t *tsk;

	fun = priv_job_get(job);
	tsk = core_tsk_wakeup(job->obj.queue, E_SUCCESS);
	if (tsk) priv_job_put(job, tsk->tmp.job.fun);

	return fun;
}

/* -------------------------------------------------------------------------- */
static
void priv_job_putUpdate( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	priv_job_put(job, fun);
	tsk = core_tsk_wakeup(job->obj.queue, E_SUCCESS);
	if (tsk) tsk->tmp.job.fun = priv_job_get(job);
}

/* -------------------------------------------------------------------------- */
static
void priv_job_skipUpdate( job_t *job )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	while (job->count == job->limit)
	{
		priv_job_skip(job);
		tsk = core_tsk_wakeup(job->obj.queue, E_SUCCESS);
		if (tsk) priv_job_put(job, tsk->tmp.job.fun);
	}
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_job_take( job_t *job, fun_t **fun )
/* -------------------------------------------------------------------------- */
{
	assert(job);
	assert(job->data);
	assert(job->limit);

	if (job->count > 0)
	{
		*fun = priv_job_getUpdate(job);
		return E_SUCCESS;
	}

	return E_TIMEOUT;
}

/* -------------------------------------------------------------------------- */
unsigned job_take( job_t *job )
/* -------------------------------------------------------------------------- */
{
	fun_t  * fun;
	unsigned event;

	sys_lock();
	{
		event = priv_job_take(job, &fun);
	}
	sys_unlock();

	if (event == E_SUCCESS)
		fun();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned job_waitFor( job_t *job, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	fun_t  * fun;
	unsigned event;

	assert_tsk_context();

	sys_lock();
	{
		event = priv_job_take(job, &fun);

		if (event == E_TIMEOUT)
		{
			event = core_tsk_waitFor(&job->obj.queue, delay);
			fun = System.cur->tmp.job.fun;
		}
	}
	sys_unlock();

	if (event == E_SUCCESS)
		fun();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned job_waitUntil( job_t *job, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	fun_t  * fun;
	unsigned event;

	assert_tsk_context();

	sys_lock();
	{
		event = priv_job_take(job, &fun);

		if (event == E_TIMEOUT)
		{
			event = core_tsk_waitUntil(&job->obj.queue, time);
			fun = System.cur->tmp.job.fun;
		}
	}
	sys_unlock();

	if (event == E_SUCCESS)
		fun();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_job_give( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	assert(job);
	assert(job->data);
	assert(job->limit);
	assert(fun);

	if (job->count < job->limit)
	{
		priv_job_putUpdate(job, fun);
		return E_SUCCESS;
	}

	return E_TIMEOUT;
}

/* -------------------------------------------------------------------------- */
unsigned job_give( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_job_give(job, fun);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned job_sendFor( job_t *job, fun_t *fun, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();

	sys_lock();
	{
		event = priv_job_give(job, fun);

		if (event == E_TIMEOUT)
		{
			System.cur->tmp.job.fun = fun;
			event = core_tsk_waitFor(&job->obj.queue, delay);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned job_sendUntil( job_t *job, fun_t *fun, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert_tsk_context();

	sys_lock();
	{
		event = priv_job_give(job, fun);

		if (event == E_TIMEOUT)
		{
			System.cur->tmp.job.fun = fun;
			event = core_tsk_waitUntil(&job->obj.queue, time);
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void job_push( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	assert(job);
	assert(job->data);
	assert(job->limit);
	assert(fun);

	sys_lock();
	{
		priv_job_skipUpdate(job);
		priv_job_putUpdate(job, fun);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned job_count( job_t *job )
/* -------------------------------------------------------------------------- */
{
	unsigned count;

	assert(job);

	sys_lock();
	{
		count = job->count;
	}
	sys_unlock();

	return count;
}

/* -------------------------------------------------------------------------- */
unsigned job_space( job_t *job )
/* -------------------------------------------------------------------------- */
{
	unsigned space;

	assert(job);

	sys_lock();
	{
		space = job->limit - job->count;
	}
	sys_unlock();

	return space;
}

/* -------------------------------------------------------------------------- */
unsigned job_limit( job_t *job )
/* -------------------------------------------------------------------------- */
{
	unsigned limit;

	assert(job);

	sys_lock();
	{
		limit = job->limit;
	}
	sys_unlock();

	return limit;
}

/* -------------------------------------------------------------------------- */

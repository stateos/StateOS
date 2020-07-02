/******************************************************************************

    @file    StateOS: osjobqueue.c
    @author  Rajmund Szymanski
    @date    02.07.2020
    @brief   This file provides set of functions for StateOS.

 ******************************************************************************

   Copyright (c) 2018-2020 Rajmund Szymanski. All rights reserved.

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
static
void priv_job_init( job_t *job, fun_t **data, size_t bufsize, void *res )
/* -------------------------------------------------------------------------- */
{
	memset(job, 0, sizeof(job_t));

	core_obj_init(&job->obj, res);

	job->limit = bufsize / sizeof(fun_t *);
	job->data  = data;
}

/* -------------------------------------------------------------------------- */
void job_init( job_t *job, fun_t **data, size_t bufsize )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(job);
	assert(data);
	assert(bufsize);

	sys_lock();
	{
		priv_job_init(job, data, bufsize, NULL);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
job_t *job_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	struct job_T { job_t job; fun_t *buf[]; } *tmp;
	job_t *job = NULL;
	size_t bufsize;

	assert_tsk_context();
	assert(limit);

	sys_lock();
	{
		bufsize = limit * sizeof(fun_t *);
		tmp = malloc(sizeof(struct job_T) + bufsize);
		if (tmp)
			priv_job_init(job = &tmp->job, tmp->buf, bufsize, tmp);
	}
	sys_unlock();

	return job;
}

/* -------------------------------------------------------------------------- */
static
void priv_job_reset( job_t *job, int event )
/* -------------------------------------------------------------------------- */
{
	job->count = 0;
	job->head  = 0;
	job->tail  = 0;

	core_all_wakeup(job->obj.queue, event);
}

/* -------------------------------------------------------------------------- */
void job_reset( job_t *job )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(job);
	assert(job->obj.res!=RELEASED);

	sys_lock();
	{
		priv_job_reset(job, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void job_destroy( job_t *job )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(job);
	assert(job->obj.res!=RELEASED);

	sys_lock();
	{
		priv_job_reset(job, job->obj.res ? E_DELETED : E_STOPPED);
		core_res_free(&job->obj);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
fun_t *priv_job_get( job_t *job )
/* -------------------------------------------------------------------------- */
{
	unsigned i = job->head;

	fun_t *fun = job->data[i++];

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
	unsigned i = job->head + 1;

	job->head = (i < job->limit) ? i : 0;
	job->count--;
}

/* -------------------------------------------------------------------------- */
static
fun_t *priv_job_getUpdate( job_t *job )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	fun_t *fun = priv_job_get(job);

	tsk = core_one_wakeup(job->obj.queue, E_SUCCESS);
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

	tsk = core_one_wakeup(job->obj.queue, E_SUCCESS);
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
		tsk = core_one_wakeup(job->obj.queue, E_SUCCESS);
		if (tsk) priv_job_put(job, tsk->tmp.job.fun);
	}
}

/* -------------------------------------------------------------------------- */
static
int priv_job_take( job_t *job, fun_t **fun )
/* -------------------------------------------------------------------------- */
{
	if (job->count > 0)
	{
		*fun = priv_job_getUpdate(job);
		return E_SUCCESS;
	}

	return E_TIMEOUT;
}

/* -------------------------------------------------------------------------- */
int job_take( job_t *job )
/* -------------------------------------------------------------------------- */
{
	fun_t *fun;
	int result;

	assert(job);
	assert(job->obj.res!=RELEASED);
	assert(job->data);
	assert(job->limit);

	sys_lock();
	{
		result = priv_job_take(job, &fun);
	}
	sys_unlock();

	if (result == E_SUCCESS)
		fun();

	return result;
}

/* -------------------------------------------------------------------------- */
int job_waitFor( job_t *job, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(job);
	assert(job->obj.res!=RELEASED);
	assert(job->data);
	assert(job->limit);

	sys_lock();
	{
		result = priv_job_take(job, &System.cur->tmp.job.fun);
		if (result == E_TIMEOUT)
			result = core_tsk_waitFor(&job->obj.queue, delay);
	}
	sys_unlock();

	if (result == E_SUCCESS)
		System.cur->tmp.job.fun();

	return result;
}

/* -------------------------------------------------------------------------- */
int job_waitUntil( job_t *job, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(job);
	assert(job->obj.res!=RELEASED);
	assert(job->data);
	assert(job->limit);

	sys_lock();
	{
		result = priv_job_take(job, &System.cur->tmp.job.fun);
		if (result == E_TIMEOUT)
			result = core_tsk_waitUntil(&job->obj.queue, time);
	}
	sys_unlock();

	if (result == E_SUCCESS)
		System.cur->tmp.job.fun();

	return result;
}

/* -------------------------------------------------------------------------- */
static
int priv_job_give( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	if (job->count < job->limit)
	{
		priv_job_putUpdate(job, fun);
		return E_SUCCESS;
	}

	return E_TIMEOUT;
}

/* -------------------------------------------------------------------------- */
int job_give( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert(job);
	assert(job->obj.res!=RELEASED);
	assert(job->data);
	assert(job->limit);
	assert(fun);

	sys_lock();
	{
		result = priv_job_give(job, fun);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
int job_sendFor( job_t *job, fun_t *fun, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(job);
	assert(job->obj.res!=RELEASED);
	assert(job->data);
	assert(job->limit);
	assert(fun);

	sys_lock();
	{
		result = priv_job_give(job, fun);
		if (result == E_TIMEOUT)
		{
			System.cur->tmp.job.fun = fun;
			result = core_tsk_waitFor(&job->obj.queue, delay);
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
int job_sendUntil( job_t *job, fun_t *fun, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(job);
	assert(job->obj.res!=RELEASED);
	assert(job->data);
	assert(job->limit);
	assert(fun);

	sys_lock();
	{
		result = priv_job_give(job, fun);
		if (result == E_TIMEOUT)
		{
			System.cur->tmp.job.fun = fun;
			result = core_tsk_waitUntil(&job->obj.queue, time);
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void job_push( job_t *job, fun_t *fun )
/* -------------------------------------------------------------------------- */
{
	assert(job);
	assert(job->obj.res!=RELEASED);
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
	assert(job->obj.res!=RELEASED);

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
	assert(job->obj.res!=RELEASED);

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
	assert(job->obj.res!=RELEASED);

	sys_lock();
	{
		limit = job->limit;
	}
	sys_unlock();

	return limit;
}

/* -------------------------------------------------------------------------- */

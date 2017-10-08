/******************************************************************************

    @file    StateOS: os_job.c
    @author  Rajmund Szymanski
    @date    08.10.2017
    @brief   This file provides set of functions for StateOS.

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
	job_init(job, limit, (void *)ABOVE(job + 1));

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
unsigned priv_job_wait( job_t *job, uint32_t time, unsigned(*wait)(void*,uint32_t) )
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
		priv_job_get(job, &Current->tmp.fun);

		tsk = core_one_wakeup(job, E_SUCCESS);

		if (tsk) priv_job_put(job, tsk->tmp.fun);
	}

	if (event == E_SUCCESS)
	{
		Current->tmp.fun();
	}
	
	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned job_waitUntil( job_t *job, uint32_t time )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	return priv_job_wait(job, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned job_waitFor( job_t *job, uint32_t delay )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside() || !delay);

	return priv_job_wait(job, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_job_send( job_t *job, fun_t *fun, uint32_t time, unsigned(*wait)(void*,uint32_t) )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned event = E_SUCCESS;

	assert(job);
	assert(fun);

	port_sys_lock();

	if (job->count >= job->limit)
	{
		Current->tmp.fun = fun;

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
unsigned job_sendUntil( job_t *job, fun_t *fun, uint32_t time )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	return priv_job_send(job, fun, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned job_sendFor( job_t *job, fun_t *fun, uint32_t delay )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside() || !delay);

	return priv_job_send(job, fun, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */

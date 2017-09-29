/******************************************************************************

    @file    StateOS: os_job.c
    @author  Rajmund Szymanski
    @date    29.09.2017
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
void job_init( job_t *job, unsigned limit, fun_t **data, unsigned prio )
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
	job->prio  = prio;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
job_t *job_create( unsigned limit, unsigned prio )
/* -------------------------------------------------------------------------- */
{
	job_t *job;

	assert(!port_isr_inside());
	assert(limit);

	port_sys_lock();

	job = core_sys_alloc(ABOVE(sizeof(job_t)) + limit * sizeof(fun_t *));
	job_init(job, limit, (void *)ABOVE(job + 1), prio);

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
void priv_job_get( job_t *job, fun_t **proc )
/* -------------------------------------------------------------------------- */
{
	*proc = job->data[job->first];

	job->first = (job->first + 1) % job->limit;
	job->count--;
}

/* -------------------------------------------------------------------------- */
static
void priv_job_put( job_t *job, fun_t *proc )
/* -------------------------------------------------------------------------- */
{
	job->data[job->next] = proc;

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

	core_cur_prio(job->prio);

	if (job->count == 0)
	{
		event = wait(job, time);
	}
	else
	{
		priv_job_get(job, &Current->fun);

		tsk = core_one_wakeup(job, E_SUCCESS);

		if (tsk) priv_job_put(job, tsk->fun);
	}

	if (event == E_SUCCESS)
	{
		port_clr_lock();
		Current->fun();
	}
	
	core_cur_prio(0);

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
unsigned priv_job_send( job_t *job, fun_t *proc, uint32_t time, unsigned(*wait)(void*,uint32_t) )
/* -------------------------------------------------------------------------- */
{
	tsk_t  * tsk;
	unsigned event = E_SUCCESS;

	assert(job);
	assert(proc);

	port_sys_lock();

	if (job->count >= job->limit)
	{
		Current->fun = proc;

		event = wait(job, time);
	}
	else
	{
		priv_job_put(job, proc);

		tsk = core_one_wakeup(job, E_SUCCESS);

		if (tsk) priv_job_get(job, &tsk->fun);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned job_sendUntil( job_t *job, fun_t *proc, uint32_t time )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	return priv_job_send(job, proc, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned job_sendFor( job_t *job, fun_t *proc, uint32_t delay )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside() || !delay);

	return priv_job_send(job, proc, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */

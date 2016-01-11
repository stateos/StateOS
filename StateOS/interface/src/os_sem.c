/******************************************************************************

    @file    State Machine OS: os_sem.c
    @author  Rajmund Szymanski
    @date    11.01.2016
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
sem_id sem_create( unsigned init, unsigned limit )
/* -------------------------------------------------------------------------- */
{
	sem_id sem;

	port_sys_lock();

	sem = core_sys_alloc(sizeof(sem_t));

	if (sem)
	{
		sem->count = init;
		sem->limit = limit;
	}

	port_sys_unlock();

	return sem;
}

/* -------------------------------------------------------------------------- */
void sem_kill( sem_id sem )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	sem->count = 0;

	core_all_wakeup(sem, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static inline __attribute__((always_inline))
unsigned priv_sem_wait( sem_id sem, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	port_sys_lock();

	if (sem->count == 0)
		event = wait(sem, time);
	else
	if (core_one_wakeup(sem, E_SUCCESS) == 0)
		sem->count--;

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned sem_waitUntil( sem_id sem, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_sem_wait(sem, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned sem_waitFor( sem_id sem, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_sem_wait(sem, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
static inline __attribute__((always_inline))
unsigned priv_sem_send( sem_id sem, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	port_sys_lock();

	if (sem->count >= sem->limit)
		event = wait(sem, time);
	else
	if (core_one_wakeup(sem, E_SUCCESS) == 0)
		sem->count++;

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned sem_sendUntil( sem_id sem, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_sem_send(sem, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned sem_sendFor( sem_id sem, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_sem_send(sem, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */

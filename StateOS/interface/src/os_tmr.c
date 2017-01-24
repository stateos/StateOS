/******************************************************************************

    @file    StateOS: os_tmr.c
    @author  Rajmund Szymanski
    @date    24.01.2017
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
tmr_t *tmr_create( void )
/* -------------------------------------------------------------------------- */
{
	tmr_t *tmr;

	port_sys_lock();

	tmr = core_sys_alloc(sizeof(tmr_t));

	port_sys_unlock();

	return tmr;
}

/* -------------------------------------------------------------------------- */
void tmr_kill( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	port_sys_lock();

	if (tmr->obj.id != ID_STOPPED)
	{
		core_all_wakeup(tmr, E_STOPPED);
		core_tmr_remove(tmr);
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_tmr_start( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	if (tmr->obj.id != ID_STOPPED)
	core_tmr_remove(tmr);
	core_tmr_insert(tmr, ID_TIMER);
}

/* -------------------------------------------------------------------------- */
void tmr_startUntil( tmr_t *tmr, unsigned time, fun_t *proc )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	port_sys_lock();

	tmr->state  = proc;
	tmr->start  = Counter;
	tmr->delay  = time - tmr->start;
	tmr->period = 0;

	priv_tmr_start(tmr);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tmr_start( tmr_t *tmr, unsigned delay, unsigned period, fun_t *proc )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	port_sys_lock();

	tmr->state  = proc;
	tmr->start  = Counter;
	tmr->delay  = delay;
	tmr->period = period;

	priv_tmr_start(tmr);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_tmr_wait( tmr_t *tmr, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	assert(tmr);

	port_sys_lock();

	if (tmr->obj.id != ID_STOPPED)
	{
		event = wait(tmr, time);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned tmr_waitUntil( tmr_t *tmr, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_tmr_wait(tmr, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned tmr_waitFor( tmr_t *tmr, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_tmr_wait(tmr, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */

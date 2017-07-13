/******************************************************************************

    @file    StateOS: os_tmr.c
    @author  Rajmund Szymanski
    @date    06.07.2017
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
void tmr_init( tmr_t *tmr, fun_t *state )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(tmr);

	port_sys_lock();

	memset(tmr, 0, sizeof(tmr_t));

	tmr->state = state;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
tmr_t *tmr_create( fun_t *state )
/* -------------------------------------------------------------------------- */
{
	tmr_t *tmr;

	assert(!port_isr_inside());

	port_sys_lock();

	tmr = core_sys_alloc(sizeof(tmr_t));

	if (tmr)
		tmr_init(tmr, state);

	port_sys_unlock();

	return tmr;
}

/* -------------------------------------------------------------------------- */
void tmr_kill( tmr_t *tmr )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
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
	assert(!port_isr_inside());

	if (tmr->obj.id != ID_STOPPED)
	core_tmr_remove(tmr);
	core_tmr_insert(tmr, ID_TIMER);
}

/* -------------------------------------------------------------------------- */
void tmr_startUntil( tmr_t *tmr, uint32_t time )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	port_sys_lock();

	tmr->start  = Counter;
	tmr->delay  = time - tmr->start;
	tmr->period = 0;

	priv_tmr_start(tmr);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tmr_start( tmr_t *tmr, uint32_t delay, uint32_t period )
/* -------------------------------------------------------------------------- */
{
	assert(tmr);

	port_sys_lock();

	tmr->start  = Counter;
	tmr->delay  = delay;
	tmr->period = period;

	priv_tmr_start(tmr);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tmr_startFrom( tmr_t *tmr, uint32_t delay, uint32_t period, fun_t *proc )
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
unsigned priv_tmr_wait( tmr_t *tmr, uint32_t time, unsigned(*wait)(void*,uint32_t) )
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
unsigned tmr_waitUntil( tmr_t *tmr, uint32_t time )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	return priv_tmr_wait(tmr, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned tmr_waitFor( tmr_t *tmr, uint32_t delay )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside() || !delay);

	return priv_tmr_wait(tmr, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */

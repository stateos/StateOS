/******************************************************************************

    @file    StateOS: os_tsk.c
    @author  Rajmund Szymanski
    @date    09.04.2016
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
tsk_id tsk_create( unsigned prio, fun_id state, unsigned size )
/* -------------------------------------------------------------------------- */
{
	size = ASIZE(sizeof(tsk_t) + size);

	tsk_id tsk;

	port_sys_lock();

	tsk = core_sys_alloc(size);

	if (tsk)
	{
		tsk->state = state;
		tsk->top   = (char *)tsk + size;
		tsk->prio  = prio;
		tsk->basic = prio;

		core_tsk_insert(tsk);
	}

	port_sys_unlock();

	return tsk;
}

/* -------------------------------------------------------------------------- */
void tsk_start( tsk_id tsk )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	if (tsk->obj.id == ID_STOPPED)
	{
		tsk->sp = 0; // necessary because of the tsk_kill function

		core_tsk_insert(tsk);
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_startFrom( tsk_id tsk, fun_id state )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	if (tsk->obj.id == ID_STOPPED)
	{
		tsk->state = state;
		tsk->sp = 0; // necessary because of the tsk_kill function

		core_tsk_insert(tsk);
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_stop( void )
/* -------------------------------------------------------------------------- */
{
	port_set_lock();

//	while (Current->list) mtx_kill(Current->list);

	core_tsk_remove(Current);
	core_tsk_break();
}

/* -------------------------------------------------------------------------- */
void tsk_kill( tsk_id tsk )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

//	while (tsk->list) mtx_kill(tsk->list);

	switch (tsk->obj.id)
	{
	case ID_READY:
		if (tsk != Current) // instead use tsk_stop
		core_tsk_remove((tsk_id)tsk);
		break;
	case ID_DELAYED:
		core_tsk_unlink((tsk_id)tsk, E_STOPPED);
		core_tmr_remove((tmr_id)tsk);
		break;
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_flip( fun_id state )
/* -------------------------------------------------------------------------- */
{
	port_set_lock();

	Current->state = state;

	core_tsk_break();
}

/* -------------------------------------------------------------------------- */
void tsk_prio( unsigned prio )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	Current->basic = prio;

	core_tsk_prio(Current, prio);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static inline
unsigned priv_tsk_sleep( unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	port_sys_lock();

	event = wait(Current, time);

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_sleepUntil( unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_tsk_sleep(time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned tsk_sleepFor( unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_tsk_sleep(delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void tsk_resume( tsk_id tsk, unsigned event )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	if (tsk->obj.id == ID_DELAYED)
	core_tsk_wakeup(tsk, event);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */

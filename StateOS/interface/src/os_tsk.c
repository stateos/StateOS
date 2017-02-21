/******************************************************************************

    @file    StateOS: os_tsk.c
    @author  Rajmund Szymanski
    @date    21.02.2017
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
tsk_t *tsk_create( unsigned prio, fun_t *state, unsigned size )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	assert(state);
	assert(size);

	size = ASIZE(sizeof(tsk_t) + size);

	port_sys_lock();

	tsk = core_sys_alloc(size * sizeof(stk_t));

	if (tsk)
	{
		tsk->state = state;
		tsk->top   = (stk_t *)tsk + size;
		tsk->prio  = prio;
		tsk->basic = prio;

		core_ctx_init(tsk);
		core_tsk_insert(tsk);
	}

	port_sys_unlock();

	return tsk;
}

/* -------------------------------------------------------------------------- */
void tsk_start( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);
	assert(tsk->state);

	port_sys_lock();

	if (tsk->obj.id == ID_STOPPED)
	{
		core_ctx_init(tsk);
		core_tsk_insert(tsk);
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_startFrom( tsk_t *tsk, fun_t *state )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);
	assert(state);

	port_sys_lock();

	if (tsk->obj.id == ID_STOPPED)
	{
		tsk->state = state;

		core_ctx_init(tsk);
		core_tsk_insert(tsk);
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_stop( void )
/* -------------------------------------------------------------------------- */
{
	port_set_lock();

	while (Current->list)
		mtx_kill(Current->list);
	if (Current->join != DETACHED)
		core_all_wakeup(&Current->join, E_SUCCESS);

	core_tsk_remove(Current);

	for (;;);
}

/* -------------------------------------------------------------------------- */
void tsk_kill( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	port_sys_lock();

	while (tsk->list)
		mtx_kill(tsk->list);
	if (tsk->join != DETACHED)
		core_all_wakeup(&tsk->join, E_STOPPED);

	if (tsk->obj.id == ID_READY)
		core_tsk_remove(tsk);
	else
	if (tsk->obj.id == ID_DELAYED)
	{
		core_tsk_unlink((tsk_t *)tsk, E_STOPPED);
		core_tmr_remove((tmr_t *)tsk);
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_detach( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	port_sys_lock();

	if (tsk->join != DETACHED)
		if (tsk->obj.id != ID_STOPPED)
			core_all_wakeup(&tsk->join, E_TIMEOUT);

	tsk->join = DETACHED;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned tsk_join( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(tsk);

	port_sys_lock();

	if (tsk->join == DETACHED)
		event = E_TIMEOUT;
	else
	if (tsk->obj.id == ID_STOPPED)
		event = E_SUCCESS;
	else
		event = core_tsk_waitFor(&tsk->join, INFINITE);

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void tsk_yield( void )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	core_ctx_switch();

	port_clr_lock();
	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_flip( fun_t *state )
/* -------------------------------------------------------------------------- */
{
	assert(state);

	port_set_lock();

	Current->state = state;
	core_tsk_flip(Current->top);
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
static
unsigned priv_tsk_sleep( unsigned flags, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	port_sys_lock();

	Current->flags = flags;
	event = wait(Current, time);

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_waitUntil( unsigned flags, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_tsk_sleep(flags, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned tsk_waitFor( unsigned flags, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_tsk_sleep(flags, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void tsk_give( tsk_t *tsk, unsigned flags )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	port_sys_lock();

	if (tsk->obj.id == ID_DELAYED)
	if (tsk->guard  == tsk)
	{
		tsk->msg    =  flags;
		tsk->flags &= ~flags;
		if (tsk->flags == 0)
		core_tsk_wakeup(tsk, E_SUCCESS);
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_resume( tsk_t *tsk, unsigned event )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	port_sys_lock();

	if (tsk->obj.id == ID_DELAYED)
	core_tsk_wakeup(tsk, event);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */

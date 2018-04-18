/******************************************************************************

    @file    StateOS: os_tsk.c
    @author  Rajmund Szymanski
    @date    18.04.2018
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

#include "inc/os_tsk.h"

/* -------------------------------------------------------------------------- */
void tsk_init( tsk_t *tsk, unsigned prio, fun_t *state, void *stack, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(tsk);
	assert(state);
	assert(stack);
	assert(size);

	port_sys_lock();

	memset(tsk, 0, sizeof(tsk_t));
	
	tsk->prio  = prio;
	tsk->basic = prio;
	tsk->state = state;
	tsk->stack = stack;
	tsk->top   = (stk_t *) LIMITED((char *)stack + size, stk_t);

	core_ctx_init(tsk);
	core_tsk_insert(tsk);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
tsk_t *wrk_create( unsigned prio, fun_t *state, unsigned size )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;

	assert(!port_isr_inside());
	assert(state);
	assert(size);

	port_sys_lock();

	size = ABOVE(size);
	tsk = core_sys_alloc(ABOVE(sizeof(tsk_t)) + size);
	tsk_init(tsk, prio, state, (void *)((size_t)tsk + ABOVE(sizeof(tsk_t))), size);
	tsk->obj.res = tsk;

	port_sys_unlock();

	return tsk;
}

/* -------------------------------------------------------------------------- */
void tsk_start( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(tsk);
	assert(tsk->state);

	port_sys_lock();

	if (tsk->id == ID_STOPPED)
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
	assert(!port_isr_inside());
	assert(tsk);
	assert(state);

	port_sys_lock();

	if (tsk->id == ID_STOPPED)
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
	assert(!port_isr_inside());
	assert(!System.cur->mlist);

	port_set_lock();

	if (System.cur->join != DETACHED)
		core_tsk_wakeup(System.cur->join, E_SUCCESS);
	else
		core_sys_free(System.cur->obj.res);

	core_tsk_remove(System.cur);

	for (;;);
}

/* -------------------------------------------------------------------------- */
void tsk_kill( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(tsk);

	port_sys_lock();

	if (tsk->id != ID_STOPPED)
	{
		tsk->mtree = 0;
		while (tsk->mlist)
			mtx_kill(tsk->mlist);

		if (tsk->join != DETACHED)
			core_tsk_wakeup(tsk->join, E_STOPPED);
		else
			core_sys_free(tsk->obj.res);

		if (tsk->id == ID_READY)
			core_tsk_remove(tsk);
		else
		if (tsk->id == ID_DELAYED)
		{
			core_tsk_unlink((tsk_t *)tsk, E_STOPPED);
			core_tmr_remove((tmr_t *)tsk);
		}
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_delete( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	tsk_detach(tsk);
	tsk_kill(tsk);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned tsk_detach( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(!port_isr_inside());
	assert(tsk);

	port_sys_lock();

	if ((tsk->id      != ID_STOPPED) &&
		(tsk->join    != DETACHED) &&
		(tsk->obj.res != 0))
	{
		core_tsk_wakeup(tsk->join, E_TIMEOUT);
		tsk->join = DETACHED;
		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_join( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(!port_isr_inside());
	assert(tsk);

	port_sys_lock();

	if (tsk->join == JOINABLE)
	{
		if (tsk->id != ID_STOPPED)
			event = core_tsk_waitFor(&tsk->join, INFINITE);
		else
			event = E_SUCCESS;

		if (event != E_TIMEOUT) // !detached
			core_sys_free(tsk->obj.res);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void tsk_yield( void )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	port_sys_lock();

	core_ctx_switch();

	port_clr_lock();
	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_flip( fun_t *state )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(state);

	port_set_lock();

	System.cur->state = state;

	core_ctx_switch();
	core_tsk_flip(System.cur->top);
}

/* -------------------------------------------------------------------------- */
void tsk_prio( unsigned prio )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	port_sys_lock();

	System.cur->basic = prio;
	core_cur_prio(prio);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_tsk_wait( unsigned flags, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_inside());

	port_sys_lock();

	System.cur->evt.flags = flags;
	event = wait(System.cur, time);

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_waitUntil( unsigned flags, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_tsk_wait(flags, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned tsk_waitFor( unsigned flags, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_tsk_wait(flags, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void tsk_give( tsk_t *tsk, unsigned flags )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	port_sys_lock();

	if (tsk->guard == tsk)
	{
		tsk->evt.flags &= ~flags;
		if (tsk->evt.flags == 0)
			core_tsk_wakeup(tsk, flags);
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned tsk_suspend( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_STOPPED;
	
	assert(tsk);

	port_sys_lock();

	if (tsk->id == ID_READY)
	{
		core_tsk_suspend(tsk);
		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_resume( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_STOPPED;

	assert(tsk);

	port_sys_lock();

	if (tsk->guard == &WAIT)
	{
		core_tsk_wakeup(tsk, E_STOPPED);
		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */

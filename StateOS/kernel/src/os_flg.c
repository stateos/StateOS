/******************************************************************************

    @file    StateOS: os_flg.c
    @author  Rajmund Szymanski
    @date    16.04.2018
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

#include "inc/os_flg.h"
#include "inc/os_tsk.h"

/* -------------------------------------------------------------------------- */
void flg_init( flg_t *flg )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(flg);

	port_sys_lock();

	memset(flg, 0, sizeof(flg_t));

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
flg_t *flg_create( void )
/* -------------------------------------------------------------------------- */
{
	flg_t *flg;

	assert(!port_isr_inside());

	port_sys_lock();

	flg = core_sys_alloc(sizeof(flg_t));
	flg_init(flg);
	flg->res = flg;

	port_sys_unlock();

	return flg;
}

/* -------------------------------------------------------------------------- */
void flg_kill( flg_t *flg )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(flg);

	port_sys_lock();

	core_all_wakeup(flg, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void flg_delete( flg_t *flg )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	flg_kill(flg);
	core_sys_free(flg->res);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned flg_take( flg_t *flg, unsigned flags, unsigned mode )
/* -------------------------------------------------------------------------- */
{
	unsigned value = flags;
	unsigned event = E_SUCCESS;

	assert(flg);
	assert((mode & ~flgMASK) == 0U);

	port_sys_lock();

	if ((mode & flgIgnore)  == 0) value &= ~flg->flags;
	if ((mode & flgProtect) == 0) flg->flags &= ~flags;

	if (value && ((mode & flgAll) || (value == flags)))
	{
		event = E_TIMEOUT;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_flg_wait( flg_t *flg, unsigned flags, unsigned mode, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned value = flags;
	unsigned event = E_SUCCESS;

	assert(!port_isr_inside());
	assert(flg);
	assert((mode & ~flgMASK) == 0U);

	port_sys_lock();

	if ((mode & flgIgnore)  == 0) value &= ~flg->flags;
	if ((mode & flgProtect) == 0) flg->flags &= ~flags;

	if (value && ((mode & flgAll) || (value == flags)))
	{
		System.cur->tmp.mode  = mode;
		System.cur->evt.flags = value;
		event = wait(flg, time);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned flg_waitUntil( flg_t *flg, unsigned flags, unsigned mode, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_flg_wait(flg, flags, mode, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned flg_waitFor( flg_t *flg, unsigned flags, unsigned mode, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_flg_wait(flg, flags, mode, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned flg_give( flg_t *flg, unsigned flags )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;
	
	assert(flg);

	port_sys_lock();

	flags = flg->flags |= flags;

	for (tsk = flg->queue; tsk; tsk = tsk->obj.queue)
	{
		if (tsk->evt.flags & flags)
		{
			if ((tsk->tmp.mode & flgProtect) == 0)
				flg->flags &= ~tsk->evt.flags;
			tsk->evt.flags &= ~flags;
			if (tsk->evt.flags && (tsk->tmp.mode & flgAll))
				continue;
			core_one_wakeup(tsk = tsk->back, E_SUCCESS);
		}
	}

	flags = flg->flags;

	port_sys_unlock();

	return flags;
}

/* -------------------------------------------------------------------------- */
unsigned flg_clear( flg_t *flg, unsigned flags )
/* -------------------------------------------------------------------------- */
{
	unsigned state;

	assert(flg);

	port_sys_lock();

	state = flg->flags;
	flg->flags &= ~flags;

	port_sys_unlock();

	return state;
}

/* -------------------------------------------------------------------------- */

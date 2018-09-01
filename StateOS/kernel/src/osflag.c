/******************************************************************************

    @file    StateOS: osflag.c
    @author  Rajmund Szymanski
    @date    01.09.2018
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

#include "inc/osflag.h"
#include "inc/ostask.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void flg_init( flg_t *flg, unsigned init )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(flg);

	sys_lock();
	{
		memset(flg, 0, sizeof(flg_t));

		core_obj_init(&flg->obj);

		flg->flags = init;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
flg_t *flg_create( unsigned init )
/* -------------------------------------------------------------------------- */
{
	flg_t *flg;

	assert(!port_isr_context());

	sys_lock();
	{
		flg = core_sys_alloc(sizeof(flg_t));
		flg_init(flg, init);
		flg->obj.res = flg;
	}
	sys_unlock();

	return flg;
}

/* -------------------------------------------------------------------------- */
void flg_kill( flg_t *flg )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(flg);

	sys_lock();
	{
		core_all_wakeup(&flg->obj.queue, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void flg_delete( flg_t *flg )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		flg_kill(flg);
		core_sys_free(flg->obj.res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned flg_take( flg_t *flg, unsigned flags, char mode )
/* -------------------------------------------------------------------------- */
{
	unsigned event;
	unsigned value = flags;

	assert(flg);
	assert((mode & ~flgMASK) == 0);

	sys_lock();
	{
		if ((mode & flgIgnore)  == 0) value &= ~flg->flags;
		if ((mode & flgProtect) == 0) flg->flags &= ~flags;

		if (value == 0 || (value != flags && (mode & flgAll) == 0))
			event = E_SUCCESS;
		else
			event = E_TIMEOUT;
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_flg_wait( flg_t *flg, unsigned flags, char mode, cnt_t time, unsigned(*wait)(tsk_t**,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned value = flags;

	assert(!port_isr_context());
	assert(flg);
	assert((mode & ~flgMASK) == 0);

	if ((mode & flgIgnore)  == 0) value &= ~flg->flags;
	if ((mode & flgProtect) == 0) flg->flags &= ~flags;

	if (value == 0 || (value != flags && (mode & flgAll) == 0))
		return E_SUCCESS;
	
	System.cur->tmp.flg.mode  = mode;
	System.cur->tmp.flg.flags = value;
	return wait(&flg->obj.queue, time);
}

/* -------------------------------------------------------------------------- */
unsigned flg_waitFor( flg_t *flg, unsigned flags, char mode, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_flg_wait(flg, flags, mode, delay, core_tsk_waitFor);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned flg_waitUntil( flg_t *flg, unsigned flags, char mode, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_flg_wait(flg, flags, mode, time, core_tsk_waitUntil);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned flg_give( flg_t *flg, unsigned flags )
/* -------------------------------------------------------------------------- */
{
	unsigned state;
	obj_t  * obj;
	tsk_t  * tsk;

	assert(flg);

	sys_lock();
	{
		state = flg->flags;
		flags = flg->flags |= flags;

		for (obj = &flg->obj; obj->queue; obj = &obj->queue->hdr.obj)
		{
			tsk = obj->queue;
			if (tsk->tmp.flg.flags & flags)
			{
				if ((tsk->tmp.flg.mode & flgProtect) == 0)
					flg->flags &= ~tsk->tmp.flg.flags;
				tsk->tmp.flg.flags &= ~flags;
				if (tsk->tmp.flg.flags == 0 || (tsk->tmp.flg.mode & flgAll) == 0)
					core_tsk_wakeup(tsk, E_SUCCESS);
			}
		}

		flags = flg->flags;
	}
	sys_unlock();

	return state;
}

/* -------------------------------------------------------------------------- */
unsigned flg_clear( flg_t *flg, unsigned flags )
/* -------------------------------------------------------------------------- */
{
	unsigned state;

	assert(flg);

	sys_lock();
	{
		state = flg->flags;
		flg->flags &= ~flags;
	}
	sys_unlock();

	return state;
}

/* -------------------------------------------------------------------------- */

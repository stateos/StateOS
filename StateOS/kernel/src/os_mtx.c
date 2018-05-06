/******************************************************************************

    @file    StateOS: os_mtx.c
    @author  Rajmund Szymanski
    @date    06.05.2018
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

#include "inc/os_mtx.h"
#include "inc/os_tsk.h"

/* -------------------------------------------------------------------------- */
void mtx_init( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(mtx);

	port_sys_lock();

	memset(mtx, 0, sizeof(mtx_t));

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
mtx_t *mtx_create( void )
/* -------------------------------------------------------------------------- */
{
	mtx_t *mtx;

	assert(!port_isr_inside());

	port_sys_lock();

	mtx = core_sys_alloc(sizeof(mtx_t));
	mtx_init(mtx);
	mtx->res = mtx;

	port_sys_unlock();

	return mtx;
}

/* -------------------------------------------------------------------------- */
static
void priv_mtx_link( mtx_t *mtx, tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(mtx);

	mtx->owner = tsk;

	if (tsk)
	{
		mtx->list = tsk->mtx.list;
		tsk->mtx.list = mtx;
	}
}

/* -------------------------------------------------------------------------- */
static
void priv_mtx_unlink( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;
	mtx_t *lst;

	assert(mtx);

	if (mtx->owner)
	{
		tsk = mtx->owner;

		if (tsk->mtx.list == mtx)
			tsk->mtx.list = mtx->list;

		for (lst = tsk->mtx.list; lst; lst = lst->list)
			if (lst->list == mtx)
				lst->list = mtx->list;

		mtx->list  = 0;
		mtx->owner = 0;

		core_tsk_prio(tsk, tsk->basic);
	}
}

/* -------------------------------------------------------------------------- */
void mtx_kill( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(mtx);

	port_sys_lock();

	priv_mtx_unlink(mtx);

	mtx->count = 0;

	core_all_wakeup(mtx, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void mtx_delete( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	mtx_kill(mtx);
	core_sys_free(mtx->res);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_mtx_wait( mtx_t *mtx, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(!port_isr_inside());
	assert(mtx);

	port_sys_lock();

	if (mtx->owner == 0)
	{
		priv_mtx_link(mtx, System.cur);
		event = E_SUCCESS;
	}
	else
	if (mtx->owner == System.cur)
	{
		if (mtx->count < ~0U)
		{
			mtx->count++;
			event = E_SUCCESS;
		}
	}
	else
	{
		if (mtx->owner->prio < System.cur->prio)
			core_tsk_prio(mtx->owner, System.cur->prio);

		System.cur->mtx.tree = mtx->owner;
		event = wait(mtx, time);
		System.cur->mtx.tree = 0;
	}
	
	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned mtx_waitUntil( mtx_t *mtx, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_mtx_wait(mtx, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned mtx_waitFor( mtx_t *mtx, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_mtx_wait(mtx, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned mtx_give( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;
	
	assert(!port_isr_inside());
	assert(mtx);

	port_sys_lock();

	if (mtx->owner == System.cur)
	{
		if (mtx->count)
		{
			mtx->count--;
		}
		else
		{
			priv_mtx_unlink(mtx);
			priv_mtx_link(mtx, core_one_wakeup(mtx, E_SUCCESS));
		}

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */

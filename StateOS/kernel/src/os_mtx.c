/******************************************************************************

    @file    StateOS: os_mtx.c
    @author  Rajmund Szymanski
    @date    01.01.2018
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
		mtx->list = tsk->mlist;
		tsk->mlist = mtx;
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

		if (tsk->mlist == mtx)
			tsk->mlist = mtx->list;

		for (lst = tsk->mlist; lst; lst = lst->list)
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

		System.cur->mtree = mtx->owner;
		event = wait(mtx, time);
		System.cur->mtree = 0;
	}
	
	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned mtx_waitUntil( mtx_t *mtx, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	return priv_mtx_wait(mtx, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned mtx_waitFor( mtx_t *mtx, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

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

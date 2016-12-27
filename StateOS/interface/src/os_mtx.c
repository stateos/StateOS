/******************************************************************************

    @file    StateOS: os_mtx.c
    @author  Rajmund Szymanski
    @date    27.12.2016
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
mtx_id mtx_create( void )
/* -------------------------------------------------------------------------- */
{
	mtx_id mtx;

	port_sys_lock();

	mtx = core_sys_alloc(sizeof(mtx_t));

	port_sys_unlock();

	return mtx;
}

/* -------------------------------------------------------------------------- */
static
void priv_mtx_link( mtx_id mtx, tsk_id tsk )
/* -------------------------------------------------------------------------- */
{
	mtx->owner = tsk;

	if (tsk)
	{
		mtx->list = tsk->list;
		tsk->list = mtx;
	}
}

/* -------------------------------------------------------------------------- */
static
void priv_mtx_unlink( mtx_id mtx )
/* -------------------------------------------------------------------------- */
{
	tsk_id tsk;
	mtx_id lst;

	if (mtx->owner)
	{
		tsk = mtx->owner;

		if (tsk->list == mtx)
			tsk->list = mtx->list;

		for (lst = tsk->list; lst; lst = lst->list)
			if (lst->list == mtx)
				lst->list = mtx->list;

		mtx->list  = 0;
		mtx->owner = 0;

		core_tsk_prio(tsk, tsk->basic);
	}
}

/* -------------------------------------------------------------------------- */
void mtx_kill( mtx_id mtx )
/* -------------------------------------------------------------------------- */
{
	assert(mtx);

	port_sys_lock();

	priv_mtx_unlink(mtx);

	mtx->count = 0;

	core_all_wakeup(mtx, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_mtx_wait( mtx_id mtx, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	port_sys_lock();

	if (mtx->owner == 0)
	{
		priv_mtx_link(mtx, Current);

		event = E_SUCCESS;
	}
	else
	if (mtx->owner == Current)
	{
		if (mtx->count < ~0U)
		{
			mtx->count++;
			event = E_SUCCESS;
		}
	}
	else
	{
		if (mtx->owner->prio < Current->prio)
			core_tsk_prio(mtx->owner, Current->prio);

		event = wait(mtx, time);
	}
	
	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned mtx_waitUntil( mtx_id mtx, unsigned time )
/* -------------------------------------------------------------------------- */
{
	assert(mtx);

	return priv_mtx_wait(mtx, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned mtx_waitFor( mtx_id mtx, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	assert(mtx);

	return priv_mtx_wait(mtx, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned mtx_give( mtx_id mtx )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;
	
	assert(mtx);

	port_sys_lock();

	if (mtx->owner == Current)
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

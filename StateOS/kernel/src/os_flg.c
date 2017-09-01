/******************************************************************************

    @file    StateOS: os_flg.c
    @author  Rajmund Szymanski
    @date    01.09.2017
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
	assert(flg);

	flg_init(flg);

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
static
unsigned priv_flg_wait( flg_t *flg, unsigned flags, unsigned mode, uint32_t time, unsigned(*wait)(void*,uint32_t) )
/* -------------------------------------------------------------------------- */
{
	tsk_t *cur = Current;
	unsigned event = E_SUCCESS;

	assert(flg);
	assert((mode & ~flgMASK) == 0U);

	port_sys_lock();

	cur->mode  = mode;
	cur->flags = flags;
	if ((mode & flgIgnore)  == 0) cur->flags &= ~flg->flags;
	if ((mode & flgProtect) == 0) flg->flags &= ~flags;

	if (cur->flags && ((mode & flgAll) || (cur->flags == flags)))
		event = wait(flg, time);

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned flg_waitUntil( flg_t *flg, unsigned flags, unsigned mode, uint32_t time )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	return priv_flg_wait(flg, flags, mode, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned flg_waitFor( flg_t *flg, unsigned flags, unsigned mode, uint32_t delay )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside() || !delay);

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
		if (tsk->flags & flags)
		{
			if ((tsk->mode & flgProtect) == 0)
			flg->flags &= ~tsk->flags;
			tsk->flags &= ~flags;
			if (tsk->flags && (tsk->mode & flgAll)) continue;
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

/******************************************************************************

    @file    StateOS: os_cnd.c
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
void cnd_init( cnd_t *cnd )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(cnd);

	port_sys_lock();

	memset(cnd, 0, sizeof(cnd_t));

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
cnd_t *cnd_create( void )
/* -------------------------------------------------------------------------- */
{
	cnd_t *cnd;

	assert(!port_isr_inside());

	port_sys_lock();

	cnd = core_sys_alloc(sizeof(cnd_t));

	if (cnd)
		cnd_init(cnd);

	port_sys_unlock();

	return cnd;
}

/* -------------------------------------------------------------------------- */
void cnd_kill( cnd_t *cnd )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(cnd);

	port_sys_lock();

	core_all_wakeup(cnd, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_cnd_wait( cnd_t *cnd, mtx_t *mtx, uint32_t time, unsigned(*wait)(void*,uint32_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(cnd);
	assert(mtx);

	port_sys_lock();

	if ((event = mtx_give(mtx))   == E_SUCCESS)
	if ((event = wait(cnd, time)) == E_SUCCESS)
	     event = mtx_wait(mtx);

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned cnd_waitUntil( cnd_t *cnd, mtx_t *mtx, uint32_t time )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	return priv_cnd_wait(cnd, mtx, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned cnd_waitFor( cnd_t *cnd, mtx_t *mtx, uint32_t delay )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	return priv_cnd_wait(cnd, mtx, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void cnd_give( cnd_t *cnd, bool all )
/* -------------------------------------------------------------------------- */
{
	assert(cnd);

	port_sys_lock();

	if (all) core_all_wakeup(cnd, E_SUCCESS);
	else     core_one_wakeup(cnd, E_SUCCESS);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */

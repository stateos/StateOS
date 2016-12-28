/******************************************************************************

    @file    StateOS: os_mut.c
    @author  Rajmund Szymanski
    @date    28.12.2016
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
mut_id mut_create( void )
/* -------------------------------------------------------------------------- */
{
	mut_id mut;

	port_sys_lock();

	mut = core_sys_alloc(sizeof(mut_t));

	port_sys_unlock();

	return mut;
}

/* -------------------------------------------------------------------------- */
void mut_kill( mut_id mut )
/* -------------------------------------------------------------------------- */
{
	assert(mut);

	port_sys_lock();

	core_all_wakeup(mut, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_mut_wait( mut_id mut, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(mut);
	assert(mut->owner != Current);

	port_sys_lock();

	if (mut->owner == 0)
	{
		mut->owner = Current;
		event = E_SUCCESS;
	}
	else
	{
		event = wait(mut, time);
	}
	
	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned mut_waitUntil( mut_id mut, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_mut_wait(mut, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned mut_waitFor( mut_id mut, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_mut_wait(mut, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
unsigned mut_give( mut_id mut )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;
	
	assert(mut);

	port_sys_lock();

	if (mut->owner == Current)
	{
		mut->owner = core_one_wakeup(mut, E_SUCCESS);
		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */

/******************************************************************************

    @file    State Machine OS: os_bar.c
    @author  Rajmund Szymanski
    @date    15.12.2015
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
bar_id bar_create( unsigned limit )
/* -------------------------------------------------------------------------- */
{
	bar_id bar;

	port_sys_lock();

	bar = core_sys_alloc(sizeof(bar_t));

	if (bar)
	{
		bar->count = limit;
		bar->limit = limit;
	}

	port_sys_unlock();

	return bar;
}

/* -------------------------------------------------------------------------- */
void bar_kill( bar_id bar )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	bar->count = bar->limit;

	core_all_wakeup(bar, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
__attribute__((always_inline)) static inline
unsigned priv_bar_wait( bar_id bar, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	port_sys_lock();

	if (--bar->count > 0)
	{
		event = wait(bar, time);
	}
	else
	{
		bar->count = bar->limit;

		core_all_wakeup(bar, E_SUCCESS);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned bar_waitUntil( bar_id bar, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_bar_wait(bar, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned bar_waitFor( bar_id bar, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_bar_wait(bar, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */

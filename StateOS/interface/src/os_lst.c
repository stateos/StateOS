/******************************************************************************

    @file    StateOS: os_lst.c
    @author  Rajmund Szymanski
    @date    08.11.2016
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
lst_id lst_create( void )
/* -------------------------------------------------------------------------- */
{
	lst_id lst;

	port_sys_lock();

	lst = core_sys_alloc(sizeof(lst_t));

	port_sys_unlock();

	return lst;
}

/* -------------------------------------------------------------------------- */
void lst_kill( lst_id lst )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	core_all_wakeup(lst, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static inline
unsigned priv_lst_wait( lst_id lst, void **data, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	port_sys_lock();

	if (lst->next)
	{
		*data = lst->next + 1;
		lst->next = lst->next->next;
	}
	else
	{
		Current->data = data;
		event = wait(lst, time);
	}
	
	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned lst_waitUntil( lst_id lst, void **data, unsigned time )
/* -------------------------------------------------------------------------- */
{
	return priv_lst_wait(lst, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned lst_waitFor( lst_id lst, void **data, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	return priv_lst_wait(lst, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void lst_give( lst_id lst, void *data )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	tsk_id tsk = core_one_wakeup(lst, E_SUCCESS);

	if (tsk)
	{
		*(void**)tsk->data = data;
	}
	else
	{
		que_id ptr = (que_id)&(lst->next);
		while (ptr->next) ptr = ptr->next;
		ptr->next = (que_id)data - 1;
		ptr->next->next = 0;
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */

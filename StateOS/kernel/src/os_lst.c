/******************************************************************************

    @file    StateOS: os_lst.c
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

#include "inc/os_lst.h"
#include "inc/os_tsk.h"

/* -------------------------------------------------------------------------- */
void lst_init( lst_t *lst )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(lst);

	port_sys_lock();

	memset(lst, 0, sizeof(lst_t));

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
lst_t *lst_create( void )
/* -------------------------------------------------------------------------- */
{
	lst_t *lst;

	assert(!port_isr_inside());

	port_sys_lock();

	lst = core_sys_alloc(sizeof(lst_t));
	lst_init(lst);
	lst->res = lst;

	port_sys_unlock();

	return lst;
}

/* -------------------------------------------------------------------------- */
void lst_kill( lst_t *lst )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(lst);

	port_sys_lock();

	core_all_wakeup(lst, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void lst_delete( lst_t *lst )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	lst_kill(lst);
	core_sys_free(lst->res);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_lst_wait( lst_t *lst, void **data, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	assert(lst);
	assert(data);

	port_sys_lock();

	if (lst->next)
	{
		*data = lst->next + 1;
		lst->next = lst->next->next;
	}
	else
	{
		System.cur->tmp.data = data;
		event = wait(lst, time);
	}
	
	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned lst_waitUntil( lst_t *lst, void **data, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());

	return priv_lst_wait(lst, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned lst_waitFor( lst_t *lst, void **data, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside() || !delay);

	return priv_lst_wait(lst, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void lst_give( lst_t *lst, void *data )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;
	que_t *ptr;

	assert(lst);
	assert(data);

	port_sys_lock();

	tsk = core_one_wakeup(lst, E_SUCCESS);

	if (tsk)
	{
		*(void**)tsk->tmp.data = data;
	}
	else
	{
		ptr = (que_t *)&(lst->next);
		while (ptr->next) ptr = ptr->next;
		ptr->next = (que_t *)data - 1;
		ptr->next->next = 0;
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */

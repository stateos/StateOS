/******************************************************************************

    @file    StateOS: os_lst.c
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
unsigned lst_take( lst_t *lst, void **data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(lst);
	assert(data);

	port_sys_lock();

	if (lst->next)
	{
		*data = lst->next + 1;
		lst->next = lst->next->next;
		event = E_SUCCESS;
	}
	
	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_lst_wait( lst_t *lst, void **data, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_inside());
	assert(lst);
	assert(data);

	port_sys_lock();

	if (lst->next)
	{
		*data = lst->next + 1;
		lst->next = lst->next->next;
		event = E_SUCCESS;
	}
	else
	{
		System.cur->tmp.lst.data.in = data;
		event = wait(lst, time);
	}
	
	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned lst_waitUntil( lst_t *lst, void **data, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_lst_wait(lst, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned lst_waitFor( lst_t *lst, void **data, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_lst_wait(lst, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void lst_give( lst_t *lst, const void *data )
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
		*tsk->tmp.lst.data.out = data;
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

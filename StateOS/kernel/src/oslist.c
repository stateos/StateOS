/******************************************************************************

    @file    StateOS: oslist.c
    @author  Rajmund Szymanski
    @date    24.06.2020
    @brief   This file provides set of functions for StateOS.

 ******************************************************************************

   Copyright (c) 2018-2020 Rajmund Szymanski. All rights reserved.

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

#include "inc/oslist.h"
#include "inc/ostask.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
static
void priv_lst_init( lst_t *lst, void *res )
/* -------------------------------------------------------------------------- */
{
	memset(lst, 0, sizeof(lst_t));

	core_obj_init(&lst->obj, res);
}

/* -------------------------------------------------------------------------- */
void lst_init( lst_t *lst )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(lst);

	sys_lock();
	{
		priv_lst_init(lst, NULL);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
lst_t *lst_create( void )
/* -------------------------------------------------------------------------- */
{
	lst_t *lst;

	assert_tsk_context();

	sys_lock();
	{
		lst = malloc(sizeof(lst_t));
		if (lst)
			priv_lst_init(lst, lst);
	}
	sys_unlock();

	return lst;
}

/* -------------------------------------------------------------------------- */
static
void priv_lst_reset( lst_t *lst, int event )
/* -------------------------------------------------------------------------- */
{
	core_all_wakeup(lst->obj.queue, event);
}

/* -------------------------------------------------------------------------- */
void lst_reset( lst_t *lst )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(lst);
	assert(lst->obj.res!=RELEASED);

	sys_lock();
	{
		priv_lst_reset(lst, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void lst_destroy( lst_t *lst )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(lst);
	assert(lst->obj.res!=RELEASED);

	sys_lock();
	{
		priv_lst_reset(lst, lst->obj.res ? E_DELETED : E_STOPPED);
		core_res_free(&lst->obj);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
int priv_lst_take( lst_t *lst, void **data )
/* -------------------------------------------------------------------------- */
{
	if (lst->head.next)
	{
		*data = lst->head.next + 1;
		lst->head.next = lst->head.next->next;
		return E_SUCCESS;
	}

	return E_TIMEOUT;
}

/* -------------------------------------------------------------------------- */
int lst_take( lst_t *lst, void **data )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert(lst);
	assert(lst->obj.res!=RELEASED);
	assert(data);

	sys_lock();
	{
		result = priv_lst_take(lst, data);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
int lst_waitFor( lst_t *lst, void **data, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(lst);
	assert(lst->obj.res!=RELEASED);
	assert(data);

	sys_lock();
	{
		result = priv_lst_take(lst, data);

		if (result == E_TIMEOUT)
		{
			System.cur->tmp.lst.data.in = data;
			result = core_tsk_waitFor(&lst->obj.queue, delay);
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
int lst_waitUntil( lst_t *lst, void **data, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(lst);
	assert(lst->obj.res!=RELEASED);
	assert(data);

	sys_lock();
	{
		result = priv_lst_take(lst, data);

		if (result == E_TIMEOUT)
		{
			System.cur->tmp.lst.data.in = data;
			result = core_tsk_waitUntil(&lst->obj.queue, time);
		}
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void lst_give( lst_t *lst, const void *data )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;
	que_t *ptr;

	assert(lst);
	assert(lst->obj.res!=RELEASED);
	assert(data);

	sys_lock();
	{
		tsk = core_one_wakeup(lst->obj.queue, E_SUCCESS);

		if (tsk)
		{
			*tsk->tmp.lst.data.out = data;
		}
		else
		{
			for (ptr = &lst->head; ptr->next; ptr = ptr->next);
			ptr->next = (que_t *)data - 1;
			ptr->next->next = 0;
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */

/******************************************************************************

    @file    StateOS: osrwlock.c
    @author  Rajmund Szymanski
    @date    06.07.2020
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

#include "inc/osrwlock.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
static
void priv_rwl_init( rwl_t *rwl, void *res )
/* -------------------------------------------------------------------------- */
{
	memset(rwl, 0, sizeof(rwl_t));

	core_obj_init(&rwl->obj, res);
}

/* -------------------------------------------------------------------------- */
void rwl_init( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(rwl);

	sys_lock();
	{
		priv_rwl_init(rwl, NULL);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
rwl_t *rwl_create( void )
/* -------------------------------------------------------------------------- */
{
	rwl_t *rwl;

	assert_tsk_context();

	sys_lock();
	{
		rwl = malloc(sizeof(rwl_t));
		if (rwl)
			priv_rwl_init(rwl, rwl);
	}
	sys_unlock();

	return rwl;
}

/* -------------------------------------------------------------------------- */
static
void priv_rwl_reset( rwl_t *rwl, int event )
/* -------------------------------------------------------------------------- */
{
	core_all_wakeup(rwl->obj.queue, event);
	core_all_wakeup(rwl->queue, event);
}

/* -------------------------------------------------------------------------- */
void rwl_reset( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(rwl);
	assert(rwl->obj.res!=RELEASED);

	sys_lock();
	{
		priv_rwl_reset(rwl, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void rwl_destroy( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(rwl);
	assert(rwl->obj.res!=RELEASED);

	sys_lock();
	{
		priv_rwl_reset(rwl, rwl->obj.res ? E_DELETED : E_STOPPED);
		core_res_free(&rwl->obj);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
int priv_rwl_takeRead( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	if (rwl->owner != NULL)
		return E_TIMEOUT;

	rwl->count++;
	return E_SUCCESS;
}

/* -------------------------------------------------------------------------- */
int rwl_takeRead( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(rwl);
	assert(rwl->obj.res!=RELEASED);

	sys_lock();
	{
		result = priv_rwl_takeRead(rwl);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
int rwl_waitForRead( rwl_t *rwl, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(rwl);
	assert(rwl->obj.res!=RELEASED);

	sys_lock();
	{
		result = priv_rwl_takeRead(rwl);
		if (result == E_TIMEOUT)
			result = core_tsk_waitFor(&rwl->queue, delay);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
int rwl_waitUntilRead( rwl_t *rwl, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(rwl);
	assert(rwl->obj.res!=RELEASED);

	sys_lock();
	{
		result = priv_rwl_takeRead(rwl);
		if (result == E_TIMEOUT)
			result = core_tsk_waitUntil(&rwl->queue, time);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
static
void priv_rwl_giveRead( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	rwl->count--;
	if (rwl->count == 0)
		rwl->owner = core_one_wakeup(rwl->obj.queue, E_SUCCESS);
}

/* -------------------------------------------------------------------------- */
void rwl_giveRead( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(rwl);
	assert(rwl->obj.res!=RELEASED);
	assert(rwl->owner==NULL);
	assert(rwl->count>0);

	sys_lock();
	{
		priv_rwl_giveRead(rwl);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
int priv_rwl_takeWrite( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	if (rwl->owner == System.cur)
		return E_FAILURE;

	if (rwl->owner != NULL || rwl->count > 0)
		return E_TIMEOUT;

	rwl->owner = System.cur;
	return E_SUCCESS;
}

/* -------------------------------------------------------------------------- */
int rwl_takeWrite( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(rwl);
	assert(rwl->obj.res!=RELEASED);

	sys_lock();
	{
		result = priv_rwl_takeWrite(rwl);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
int rwl_waitForWrite( rwl_t *rwl, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(rwl);
	assert(rwl->obj.res!=RELEASED);

	sys_lock();
	{
		result = priv_rwl_takeWrite(rwl);
		if (result == E_TIMEOUT)
			result = core_tsk_waitFor(&rwl->obj.queue, delay);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
int rwl_waitUntilWrite( rwl_t *rwl, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(rwl);
	assert(rwl->obj.res!=RELEASED);

	sys_lock();
	{
		result = priv_rwl_takeWrite(rwl);
		if (result == E_TIMEOUT)
			result = core_tsk_waitUntil(&rwl->obj.queue, time);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
static
int priv_rwl_giveWrite( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	if (rwl->owner == System.cur)
	{
		rwl->owner = core_one_wakeup(rwl->obj.queue, E_SUCCESS);
		if (rwl->owner == NULL)
		{
			rwl->count = core_tsk_count(rwl->queue);
			core_all_wakeup(rwl->queue, E_SUCCESS);
		}

		return E_SUCCESS;
	}

	return E_FAILURE;
}

/* -------------------------------------------------------------------------- */
int rwl_giveWrite( rwl_t *rwl )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(rwl);
	assert(rwl->obj.res!=RELEASED);
	assert(rwl->count==0);

	sys_lock();
	{
		result = priv_rwl_giveWrite(rwl);
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */

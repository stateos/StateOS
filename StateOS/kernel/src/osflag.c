/******************************************************************************

    @file    StateOS: osflag.c
    @author  Rajmund Szymanski
    @date    29.06.2020
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

#include "inc/osflag.h"
#include "inc/ostask.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
static
void priv_flg_init( flg_t *flg, unsigned init, void *res )
/* -------------------------------------------------------------------------- */
{
	memset(flg, 0, sizeof(flg_t));

	core_obj_init(&flg->obj, res);

	flg->flags = init;
}

/* -------------------------------------------------------------------------- */
void flg_init( flg_t *flg, unsigned init )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(flg);

	sys_lock();
	{
		priv_flg_init(flg, init, NULL);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
flg_t *flg_create( unsigned init )
/* -------------------------------------------------------------------------- */
{
	flg_t *flg;

	assert_tsk_context();

	sys_lock();
	{
		flg = malloc(sizeof(flg_t));
		if (flg)
			priv_flg_init(flg, init, flg);
	}
	sys_unlock();

	return flg;
}

/* -------------------------------------------------------------------------- */
static
void priv_flg_reset( flg_t *flg, int event )
/* -------------------------------------------------------------------------- */
{
	flg->flags = 0;

	core_all_wakeup(flg->obj.queue, event);
}

/* -------------------------------------------------------------------------- */
void flg_reset( flg_t *flg )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(flg);
	assert(flg->obj.res!=RELEASED);

	sys_lock();
	{
		priv_flg_reset(flg, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void flg_destroy( flg_t *flg )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(flg);
	assert(flg->obj.res!=RELEASED);

	sys_lock();
	{
		priv_flg_reset(flg, flg->obj.res ? E_DELETED : E_STOPPED);
		core_res_free(&flg->obj);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
int priv_flg_take( flg_t *flg, unsigned flags, unsigned mode )
/* -------------------------------------------------------------------------- */
{
	unsigned value = flags;

	if ((mode & flgIgnore) == 0)
	{
		flags &= ~flg->flags;
		if ((mode & flgProtect) == 0)
			flg->flags &= ~value;
	}

	System.cur->tmp.flg.flags = flags;

	if (flags == 0 || (flags != value && (mode & flgAll) == 0))
		return E_SUCCESS;

	return E_TIMEOUT;
}

/* -------------------------------------------------------------------------- */
int flg_take( flg_t *flg, unsigned flags, unsigned mode, unsigned *remain )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert(flg);
	assert(flg->obj.res!=RELEASED);
	assert((mode & ~flgMASK) == 0);

	sys_lock();
	{
		result = priv_flg_take(flg, flags, mode);
		if (remain != NULL)
			*remain = System.cur->tmp.flg.flags;
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
int flg_waitFor( flg_t *flg, unsigned flags, unsigned mode, unsigned *remain, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(flg);
	assert(flg->obj.res!=RELEASED);
	assert((mode & ~flgMASK) == 0);

	sys_lock();
	{
		result = priv_flg_take(flg, flags, mode);

		if (result == E_TIMEOUT)
		{
			System.cur->tmp.flg.mode = mode;
			result = core_tsk_waitFor(&flg->obj.queue, delay);
		}

		if (remain != NULL)
			*remain = System.cur->tmp.flg.flags;
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
int flg_waitUntil( flg_t *flg, unsigned flags, unsigned mode, unsigned *remain, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	int result;

	assert_tsk_context();
	assert(flg);
	assert(flg->obj.res!=RELEASED);
	assert((mode & ~flgMASK) == 0);

	sys_lock();
	{
		result = priv_flg_take(flg, flags, mode);

		if (result == E_TIMEOUT)
		{
			System.cur->tmp.flg.mode = mode;
			result = core_tsk_waitUntil(&flg->obj.queue, time);
		}

		if (remain != NULL)
			*remain = System.cur->tmp.flg.flags;
	}
	sys_unlock();

	return result;
}

/* -------------------------------------------------------------------------- */
void flg_give( flg_t *flg, unsigned flags, unsigned *after )
/* -------------------------------------------------------------------------- */
{
	obj_t *obj;
	tsk_t *tsk;

	assert(flg);
	assert(flg->obj.res!=RELEASED);

	sys_lock();
	{
		flg->flags |= flags;

		obj = &flg->obj;
		while (obj->queue)
		{
			tsk = obj->queue;
			if (tsk->tmp.flg.flags & flags)
			{
				if ((tsk->tmp.flg.mode & flgProtect) == 0)
					flg->flags &= ~(tsk->tmp.flg.flags & flags);
				tsk->tmp.flg.flags &= ~flags;
				if (tsk->tmp.flg.flags == 0 || (tsk->tmp.flg.mode & flgAll) == 0)
				{
					core_tsk_wakeup(tsk, E_SUCCESS);
					continue;
				}
			}
			obj = &tsk->hdr.obj;
		}

	//	System.cur->tmp.flg.flags = flg->flags;
		if (after != NULL)
			*after = flg->flags;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void flg_clear( flg_t *flg, unsigned flags, unsigned *before )
/* -------------------------------------------------------------------------- */
{
	assert(flg);
	assert(flg->obj.res!=RELEASED);

	sys_lock();
	{
	//	System.cur->tmp.flg.flags = flg->flags;
		if (before != NULL)
			*before = flg->flags;
		flg->flags &= ~flags;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned flg_get( flg_t *flg )
/* -------------------------------------------------------------------------- */
{
	unsigned flags;

	assert(flg);
	assert(flg->obj.res!=RELEASED);

	sys_lock();
	{
		flags = flg->flags;
	}
	sys_unlock();

	return flags;
}

/* -------------------------------------------------------------------------- */

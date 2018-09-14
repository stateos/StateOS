/******************************************************************************

    @file    StateOS: osmutex.c
    @author  Rajmund Szymanski
    @date    14.09.2018
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

#include "inc/osmutex.h"
#include "inc/ostask.h"
#include "inc/oscriticalsection.h"
#include "osalloc.h"

/* -------------------------------------------------------------------------- */
void mtx_init( mtx_t *mtx, unsigned mode, unsigned prio )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(mtx);
	assert((mtx->mode & ~mtxMASK) == 0);
	assert((mtx->mode &  mtxTypeMASK) != mtxTypeMASK);
	assert((mtx->mode &  mtxPrioMASK) != mtxPrioMASK);

	sys_lock();
	{
		memset(mtx, 0, sizeof(mtx_t));

		core_obj_init(&mtx->obj);

		mtx->mode = mode;
		mtx->prio = prio;
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
mtx_t *mtx_create( unsigned mode, unsigned prio )
/* -------------------------------------------------------------------------- */
{
	mtx_t *mtx;

	assert(!port_isr_context());

	sys_lock();
	{
		mtx = sys_alloc(sizeof(mtx_t));
		mtx_init(mtx, mode, prio);
		mtx->obj.res = mtx;
	}
	sys_unlock();

	return mtx;
}

/* -------------------------------------------------------------------------- */
void mtx_setPrio( mtx_t *mtx, unsigned prio )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());

	sys_lock();
	{
		mtx->prio = prio;

		if ((mtx->mode & mtxPrioMASK) == mtxPrioProtect)
			while (mtx->obj.queue && mtx->obj.queue->prio > prio)
				core_tsk_wakeup(mtx->obj.queue, E_TIMEOUT);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned mtx_getPrio( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	unsigned prio;

	assert(!port_isr_context());

	sys_lock();
	{
		prio = mtx->prio;
	}
	sys_unlock();

	return prio;
}

/* -------------------------------------------------------------------------- */
static
void priv_mtx_link( mtx_t *mtx, tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(mtx);

	mtx->owner = tsk;

	if (tsk)
	{
		mtx->list = tsk->mtx.list;
		tsk->mtx.list = mtx;
	}
}

/* -------------------------------------------------------------------------- */
static
void priv_mtx_unlink( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;
	mtx_t *lst;

	assert(mtx);

	if (mtx->owner)
	{
		tsk = mtx->owner;

		if (tsk->mtx.list == mtx)
			tsk->mtx.list = mtx->list;

		for (lst = tsk->mtx.list; lst; lst = lst->list)
			if (lst->list == mtx)
				lst->list = mtx->list;

		mtx->list  = 0;
		mtx->owner = 0;

		core_tsk_prio(tsk, tsk->basic);
	}
}

/* -------------------------------------------------------------------------- */
void mtx_kill( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(mtx);

	sys_lock();
	{
		priv_mtx_unlink(mtx);

		mtx->count = 0;

		core_all_wakeup(&mtx->obj.queue, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void mtx_delete( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	sys_lock();
	{
		mtx_kill(mtx);
		sys_free(mtx->obj.res);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_mtx_wait( mtx_t *mtx, cnt_t time, unsigned(*wait)(tsk_t**,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_context());
	assert(mtx);
	assert((mtx->mode & ~mtxMASK) == 0);
	assert((mtx->mode &  mtxTypeMASK) != mtxTypeMASK);
	assert((mtx->mode &  mtxPrioMASK) != mtxPrioMASK);

	if (mtx->owner == 0)
	{
		assert(mtx->count == 0);

		priv_mtx_link(mtx, System.cur);
		return E_SUCCESS;
	}

	if ((mtx->mode & mtxTypeMASK) == mtxNormal || mtx->owner != System.cur)
	{
		unsigned event;

		if ((mtx->mode & mtxPrioMASK) == mtxPrioProtect && System.cur->prio > mtx->prio)
			return E_TIMEOUT;

		if ((mtx->mode & mtxPrioMASK) != mtxPrioNone && System.cur->prio > mtx->owner->prio)
			core_tsk_prio(mtx->owner, System.cur->prio);

		System.cur->mtx.tree = mtx->owner;
		event = wait(&mtx->obj.queue, time);
		System.cur->mtx.tree = 0;

		return event;
	}

	if ((mtx->mode & mtxTypeMASK) == mtxRecursive && mtx->count < ~0U)
	{
		mtx->count++;
		return E_SUCCESS;
	}

	return E_TIMEOUT;
}

/* -------------------------------------------------------------------------- */
unsigned mtx_waitFor( mtx_t *mtx, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_mtx_wait(mtx, delay, core_tsk_waitFor);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned mtx_waitUntil( mtx_t *mtx, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	sys_lock();
	{
		event = priv_mtx_wait(mtx, time, core_tsk_waitUntil);
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned mtx_give( mtx_t *mtx )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_context());
	assert(mtx);
	assert((mtx->mode & ~mtxMASK) == 0);
	assert((mtx->mode &  mtxTypeMASK) != mtxTypeMASK);
	assert((mtx->mode &  mtxPrioMASK) != mtxPrioMASK);

	sys_lock();
	{
		if (mtx->owner != System.cur)
		{
			event = E_TIMEOUT;
		}
		else
		if (mtx->count > 0)
		{
			mtx->count--;
			event = E_SUCCESS;
		}
		else
		{
			priv_mtx_unlink(mtx);
			priv_mtx_link(mtx, core_one_wakeup(&mtx->obj.queue, E_SUCCESS));
			event = E_SUCCESS;
		}
	}
	sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */

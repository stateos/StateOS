/******************************************************************************

    @file    StateOS: oskernel.c
    @author  Rajmund Szymanski
    @date    06.03.2017
    @brief   This file provides set of variables and functions for StateOS.

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

#include <stdlib.h>
#include <os.h>

/* -------------------------------------------------------------------------- */
// SYSTEM KERNEL SERVICES
/* -------------------------------------------------------------------------- */

static
void priv_tsk_idle( void )
{
#if OS_ROBIN || OS_TIMER == 0
	__WFI();
#endif
}

/* -------------------------------------------------------------------------- */

#ifndef MAIN_TOP
static  struct { stk_t STK[ASIZE(OS_STACK_SIZE)]; } MAIN_STACK;
#define MAIN_TOP &MAIN_STACK+1
#endif

static  union  { stk_t STK[ASIZE(OS_STACK_SIZE)];
        struct { char  stk[sizeof(stk_t[ASIZE(OS_STACK_SIZE)])-sizeof(ctx_t)]; ctx_t ctx; } CTX; } IDLE_STACK =
               { .CTX = { .ctx = _CTX_INIT(core_tsk_start) } };
#define IDLE_TOP &IDLE_STACK+1
#define IDLE_SP  &IDLE_STACK.CTX.ctx

tsk_t MAIN = { { .id=ID_READY, .prev=&IDLE, .next=&IDLE }, .top=MAIN_TOP, .basic=OS_MAIN_PRIO, .prio=OS_MAIN_PRIO }; // main task
tsk_t IDLE = { { .id=ID_IDLE,  .prev=&MAIN, .next=&MAIN }, .top=IDLE_TOP, .sp=IDLE_SP, .state=priv_tsk_idle }; // idle task and tasks queue
sys_t System = { .cur=&MAIN };

/* -------------------------------------------------------------------------- */

static
void priv_rdy_insert( obj_t *obj, obj_t *nxt )
{
	obj_t *prv = nxt->prev;

	obj->prev = prv;
	obj->next = nxt;
	nxt->prev = obj;
	prv->next = obj;
}

/* -------------------------------------------------------------------------- */

static
void priv_rdy_remove( obj_t *obj )
{
	obj_t *nxt = obj->next;
	obj_t *prv = obj->prev;

	nxt->prev = prv;
	prv->next = nxt;
}

/* -------------------------------------------------------------------------- */

static
void priv_tsk_insert( tsk_t *tsk )
{
	tsk_t *nxt = &IDLE;

	if    (tsk->prio)
	do     nxt = nxt->obj.next;
	while (tsk->prio <= nxt->prio);

	priv_rdy_insert(&tsk->obj, &nxt->obj);
}

/* -------------------------------------------------------------------------- */

static
void priv_tsk_remove( tsk_t *tsk )
{
	priv_rdy_remove(&tsk->obj);
}

/* -------------------------------------------------------------------------- */

void core_tsk_insert( tsk_t *tsk )
{
	tsk->obj.id = ID_READY;
	priv_tsk_insert(tsk);
#if OS_ROBIN
	if (tsk == IDLE.obj.next)
		port_ctx_switch();
#endif
}

/* -------------------------------------------------------------------------- */

void core_tsk_remove( tsk_t *tsk )
{
	tsk->obj.id = ID_STOPPED;
	priv_rdy_remove(&tsk->obj);
	if (tsk == System.cur)
		port_ctx_switchNow();
}

/* -------------------------------------------------------------------------- */

void core_ctx_init( tsk_t *tsk )
{
	ctx_t *ctx = (ctx_t *)tsk->top - 1;

	port_ctx_init(ctx, core_tsk_start);

	tsk->sp = ctx;
}

/* -------------------------------------------------------------------------- */

void core_ctx_switch( void )
{
	tsk_t *cur = IDLE.obj.next;
	tsk_t *nxt = cur->obj.next;
	if (nxt->prio == cur->prio)
		port_ctx_switch();
}

/* -------------------------------------------------------------------------- */

void core_tsk_start( void )
{
	for (;;)
	{
		port_clr_lock();
		System.cur->state();
		core_ctx_switch();
	}
}

/* -------------------------------------------------------------------------- */

void core_tsk_append( tsk_t *tsk, void *obj )
{
	tsk_t *prv;
	tsk_t *nxt = obj;
	tsk->guard = obj;

	do prv = nxt, nxt = nxt->obj.queue;
	while (nxt && tsk->prio <= nxt->prio);

	if (nxt)
	nxt->back = tsk;
	tsk->back = prv;
	tsk->obj.queue = nxt;
	prv->obj.queue = tsk;
}

/* -------------------------------------------------------------------------- */

void core_tsk_unlink( tsk_t *tsk, unsigned event )
{
	tsk_t *prv = tsk->back;
	tsk_t *nxt = tsk->obj.queue;
	tsk->event = event;

	if (nxt)
	nxt->back = prv;
	prv->obj.queue = nxt;
	tsk->obj.queue = 0; // necessary because of tsk_wait[Until|For] functions
}

/* -------------------------------------------------------------------------- */

static
unsigned priv_tsk_wait( tsk_t *cur, obj_t *obj )
{
	core_tsk_append((tsk_t *)cur, obj);
	priv_tsk_remove((tsk_t *)cur);
	core_tmr_insert((tmr_t *)cur, ID_DELAYED);

	port_ctx_switchLock();

	return cur->event;
}

/* -------------------------------------------------------------------------- */

unsigned core_tsk_waitUntil( void *obj, unsigned time )
{
	tsk_t *cur = System.cur;

	cur->start = Counter;
	cur->delay = time - cur->start;

	if (cur->delay == IMMEDIATE)
	return E_TIMEOUT;

	return priv_tsk_wait(cur, obj);
}

/* -------------------------------------------------------------------------- */

unsigned core_tsk_waitFor( void *obj, unsigned delay )
{
	tsk_t *cur = System.cur;

	cur->start = Counter;
	cur->delay = delay;

	if (cur->delay == IMMEDIATE)
	return E_TIMEOUT;

	return priv_tsk_wait(cur, obj);
}

/* -------------------------------------------------------------------------- */

tsk_t *core_tsk_wakeup( tsk_t *tsk, unsigned event )
{
	if (tsk)
	{
		core_tsk_unlink((tsk_t *)tsk, event);
		core_tmr_remove((tmr_t *)tsk);
		core_tsk_insert((tsk_t *)tsk);
	}

	return tsk;
}

/* -------------------------------------------------------------------------- */

tsk_t *core_one_wakeup( void *obj, unsigned event )
{
	obj_t *lst = obj;

	return core_tsk_wakeup(lst->queue, event);
}

/* -------------------------------------------------------------------------- */

void core_all_wakeup( void *obj, unsigned event )
{
	obj_t *lst = obj;

	while (core_tsk_wakeup(lst->queue, event));
}

/* -------------------------------------------------------------------------- */

void core_tsk_prio( tsk_t *tsk, unsigned prio )
{
	mtx_t *mtx;
	
	for (mtx = tsk->list; mtx; mtx = mtx->list)
	{
		if (mtx->queue)
		if (prio < mtx->queue->prio)
			prio = mtx->queue->prio;
	}

	if (tsk->prio != prio)
	{
		tsk->prio = prio;

		if (tsk == System.cur)
		{
			tsk = tsk->obj.next;
			if (tsk->prio > prio)
				port_ctx_switch();
		}
		else
		if (tsk->obj.id == ID_READY)
		{
			priv_tsk_remove(tsk);
			core_tsk_insert(tsk);
		}
		else
		if (tsk->obj.id == ID_DELAYED)
		{
			core_tsk_unlink(tsk, 0);
			core_tsk_append(tsk, tsk->guard);
		}
	}
}

/* -------------------------------------------------------------------------- */

void *core_tsk_handler( void *sp )
{
	tsk_t *cur, *nxt;
#if OS_ROBIN == 0
	core_tmr_handler();
#endif
	port_isr_lock();
	core_ctx_reset();

	cur = System.cur;
	cur->sp = sp;

	nxt = IDLE.obj.next;

	if (cur == nxt)
	{
		priv_tsk_remove(cur);
		priv_tsk_insert(cur);
		nxt = IDLE.obj.next;
	}

	System.cur = nxt;
	sp = nxt->sp;

	port_isr_unlock();

	return sp;
}

/* -------------------------------------------------------------------------- */
// SYSTEM TIMER SERVICES
/* -------------------------------------------------------------------------- */

tmr_t WAIT = { { .id=ID_TIMER, .prev=&WAIT, .next=&WAIT }, .delay=INFINITE }; // timers queue

/* -------------------------------------------------------------------------- */
static
void priv_tmr_insert( tmr_t *tmr, unsigned id )
{
	tmr_t *nxt = &WAIT;
	tmr->obj.id = id;

	if    (tmr->delay != INFINITE)
	do     nxt = nxt->obj.next;
	while (nxt->delay != INFINITE &&
	       nxt->delay <= tmr->start + tmr->delay - nxt->start);

	priv_rdy_insert(&tmr->obj, &nxt->obj);
}

/* -------------------------------------------------------------------------- */

void core_tmr_insert( tmr_t *tmr, unsigned id )
{
	priv_tmr_insert(tmr, id);
	port_tmr_force();
}

/* -------------------------------------------------------------------------- */

void core_tmr_remove( tmr_t *tmr )
{
	tmr->obj.id = ID_STOPPED;
	priv_rdy_remove(&tmr->obj);
}

/* -------------------------------------------------------------------------- */

#if OS_ROBIN && OS_TIMER

static
bool priv_tmr_expired( tmr_t *tmr )
{
	port_tmr_stop();

	if (tmr->delay == INFINITE)
	return false; // return if timer counting indefinitly

	if (tmr->delay <= Counter - tmr->start)
	return true;  // return if timer finished counting

	port_tmr_start(tmr->start + tmr->delay);

	if (tmr->delay >  Counter - tmr->start)
	return false; // return if timer still counts

	port_tmr_stop();

	return true;  // however timer finished counting
}

/* -------------------------------------------------------------------------- */

#else

static
bool priv_tmr_expired( tmr_t *tmr )
{
	if (tmr->delay >= Counter - tmr->start + 1)
	return false; // return if timer still counts or counting indefinitly

	return true;  // timer finished counting
}

#endif

/* -------------------------------------------------------------------------- */

static
void priv_tmr_wakeup( tmr_t *tmr, unsigned event )
{
	tmr->start += tmr->delay;
	tmr->delay  = tmr->period;

	if (tmr->state)
		tmr->state();

	core_tmr_remove(tmr);
	if (tmr->delay)
	priv_tmr_insert(tmr, ID_TIMER);

	core_all_wakeup(tmr, event);
}

/* -------------------------------------------------------------------------- */

void core_tmr_handler( void )
{
	tmr_t *tmr;

	port_isr_lock();

	while (priv_tmr_expired(tmr = WAIT.obj.next))
	{
		if (tmr->obj.id == ID_TIMER)
			priv_tmr_wakeup((tmr_t *)tmr, E_SUCCESS);

		else      /* id == ID_DELAYED */
			core_tsk_wakeup((tsk_t *)tmr, E_TIMEOUT);
	}

	port_isr_unlock();
}

/* -------------------------------------------------------------------------- */
// SYSTEM ALLOC/FREE SERVICES
/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

void *core_sys_alloc( size_t size )
{
	static  stk_t    Heap[ASIZE(OS_HEAP_SIZE)];
	#define HeapEnd (Heap+ASIZE(OS_HEAP_SIZE))

	static
	stk_t *heap = Heap;
	stk_t *base = 0;

	if (size)
	{
		size = ASIZE(size);

		if (heap + size <= HeapEnd)
		{
			base = heap;
			heap = base + size;

			stk_t *_mem = base;
			stk_t *_end = heap;
			do *_mem++ = 0; while (_mem < _end);
		}
	}

	return base;
}

/* -------------------------------------------------------------------------- */

void core_sys_free( void *ptr )
{
	(void) ptr;
}

/* -------------------------------------------------------------------------- */

#else

void *core_sys_alloc( size_t size )
{
	stk_t *base = 0;

	if (size)
	{
		size = ASIZE(size);

		base = malloc(size * sizeof(stk_t));

		if (base)
		{
			stk_t *_mem = base;
			stk_t *_end = base + size;
			do *_mem++ = 0; while (_mem < _end);
		}
	}

	return base;
}

/* -------------------------------------------------------------------------- */

void core_sys_free( void *ptr )
{
	free(ptr);
}

#endif

/* -------------------------------------------------------------------------- */

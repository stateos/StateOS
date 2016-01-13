/******************************************************************************

    @file    State Machine OS: oskernel.c
    @author  Rajmund Szymanski
    @date    13.01.2016
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

#include <oskernel.h>

/* -------------------------------------------------------------------------- */
// SYSTEM KERNEL SERVICES
/* -------------------------------------------------------------------------- */

#ifndef MAIN_SP
static  char     MAIN_STACK[ASIZE(OS_STACK_SIZE)] __osalign;
#define MAIN_SP &MAIN_STACK[ASIZE(OS_STACK_SIZE)]
#endif
static  char     IDLE_STACK[ASIZE(OS_STACK_SIZE)] __osalign;
#define IDLE_SP &IDLE_STACK[ASIZE(OS_STACK_SIZE)]

static tsk_t IDLE;
static tsk_t MAIN = { 0, ID_READY, &IDLE, &IDLE, 0,              0, 0, 0, 0, MAIN_SP }; // main task
static tsk_t IDLE = { 0, ID_IDLE,  &MAIN, &MAIN, port_idle_hook, 0, 0, 0, 0, IDLE_SP }; // idle task and tasks queue

       sys_t System = { &MAIN };

/* -------------------------------------------------------------------------- */

void core_ctx_switch( void )
{
	port_ctx_switch();
	port_sys_flash();
}

/* -------------------------------------------------------------------------- */

void core_tsk_loop( void )
{
	tsk_id cur = System.cur;

	for (;;)
	{
		port_ctx_switch();
		port_clr_lock();
		port_set_stack(cur->top);
		cur->state();
	}
}

/* -------------------------------------------------------------------------- */

__attribute__((noinline))
void core_rdy_insert( os_id obj, unsigned id, os_id nxt )
{
	obj_id Obj = obj;
	obj_id Nxt = nxt;
	obj_id Prv = Nxt->prev;

	Obj->id   = id;
	Obj->prev = Prv;
	Obj->next = Nxt;
	Nxt->prev = Obj;
	Prv->next = Obj;
}

/* -------------------------------------------------------------------------- */

__attribute__((noinline))
void core_rdy_remove( os_id obj )
{
	obj_id Obj = obj;
	obj_id Nxt = Obj->next;
	obj_id Prv = Obj->prev;

	Nxt->prev = Prv;
	Prv->next = Nxt;
	Obj->id   = ID_STOPPED;
}

/* -------------------------------------------------------------------------- */

static
void priv_tsk_insert( tsk_id tsk )
{
	tsk_id nxt = &IDLE;

	if (tsk->prio > 0) for (;;)
	{
		nxt = nxt->next;

		if (tsk->prio > nxt->prio) break;
	}

	core_rdy_insert(tsk, ID_READY, nxt);
}

/* -------------------------------------------------------------------------- */

void core_tsk_insert( tsk_id tsk )
{
	priv_tsk_insert(tsk);
#if OS_ROBIN
	if (IDLE.next->prio > System.cur->prio)
	port_ctx_switch();
#endif
}

/* -------------------------------------------------------------------------- */

void core_tsk_append( tsk_id tsk, os_id obj )
{
	tsk_id prv;
	tsk_id nxt = obj;
	tsk->guard = obj;

	for (;;)
	{
		prv = nxt;
		nxt = nxt->queue;

		if (nxt == 0)              break;
		if (tsk->prio > nxt->prio) break;
	}

	if (nxt)
	nxt->back  = tsk;
	tsk->back  = prv;
	tsk->queue = nxt;
	prv->queue = tsk;
}

/* -------------------------------------------------------------------------- */

void core_tsk_unlink( tsk_id tsk, unsigned event )
{
	tsk_id prv = tsk->back;
	tsk_id nxt = tsk->queue;
	tsk->event = event;

	if (nxt)
	nxt->back  = prv;
	prv->queue = nxt;
	tsk->queue = 0; // necessary because of the tsk_sleep[Until|For] functions
}

/* -------------------------------------------------------------------------- */

static
unsigned priv_tsk_wait( tsk_id tsk, obj_id obj )
{
	core_tsk_append((tsk_id)tsk, obj);
	core_tsk_remove((tsk_id)tsk);
	core_tmr_insert((tmr_id)tsk, ID_DELAYED);

	core_ctx_switch();

	return tsk->event;
}

/* -------------------------------------------------------------------------- */

unsigned core_tsk_waitUntil( os_id obj, unsigned time )
{
	tsk_id cur = System.cur;

	cur->start = Counter;
	cur->delay = time - cur->start;

	if (cur->delay == IMMEDIATE)
	return E_TIMEOUT;

	return priv_tsk_wait(cur, obj);
}

/* -------------------------------------------------------------------------- */

unsigned core_tsk_waitFor( os_id obj, unsigned delay )
{
	tsk_id cur = System.cur;

	cur->start = Counter;
	cur->delay = delay;

	if (cur->delay == IMMEDIATE)
	return E_TIMEOUT;

	return priv_tsk_wait(cur, obj);
}

/* -------------------------------------------------------------------------- */

tsk_id core_tsk_wakeup( tsk_id tsk, unsigned event )
{
	if (tsk)
	{
		core_tsk_unlink((tsk_id)tsk, event);
		core_tmr_remove((tmr_id)tsk);
		core_tsk_insert((tsk_id)tsk);
	}

	return tsk;
}

/* -------------------------------------------------------------------------- */

tsk_id core_one_wakeup( os_id obj, unsigned event )
{
	obj_id lst = obj;

	return core_tsk_wakeup(lst->queue, event);
}

/* -------------------------------------------------------------------------- */

void core_all_wakeup( os_id obj, unsigned event )
{
	obj_id lst = obj;

	while (core_tsk_wakeup(lst->queue, event));
}

/* -------------------------------------------------------------------------- */

void core_tsk_prio( tsk_id tsk, unsigned prio )
{
	for (mtx_id mtx = tsk->mlist; mtx; mtx = mtx->mlist)
	{
		if (mtx->queue)
		if (prio < mtx->queue->prio)
			prio = mtx->queue->prio;
	}

	if (tsk->prio != prio)
	{
		tsk->prio = prio;

		if (tsk->id == ID_READY)
		{
			core_tsk_remove(tsk);
			core_tsk_insert(tsk);
		}
		else
		if (tsk->id == ID_DELAYED)
		{
			core_tsk_unlink(tsk, 0);
			core_tsk_append(tsk, tsk->guard);
		}
	}
}

/* -------------------------------------------------------------------------- */

static
void priv_tsk_prepare( tsk_id cur )
{
	if (cur->sp == 0) // prepare task stack if necessary
	{
	    ctx_id ctx = (ctx_id)cur->top - 1;
	    sft_id sft = (sft_id)ctx - 1;

		ctx->psr = 0x01000000U;
		ctx->pc  = cur->state;
		ctx->lr  = core_tsk_loop;

		sft->exc_return = ~2U; // return from psp

		cur->sp  = ctx;
	}
}

/* -------------------------------------------------------------------------- */

tsk_id core_tsk_handler( void )
{
	tsk_id cur;
#if OS_ROBIN == 0
	core_tmr_handler();
#endif
	port_isr_lock();

	core_ctx_reset();

	cur = System.cur;
	if (cur->id == ID_READY)
	{
		core_tsk_remove(cur);
		priv_tsk_insert(cur);
	}
	cur = System.cur = IDLE.next;

	priv_tsk_prepare(cur); // prepare task stack if necessary

	port_isr_unlock();

	return cur;
}

/* -------------------------------------------------------------------------- */
// SYSTEM TIMER SERVICES
/* -------------------------------------------------------------------------- */

static tmr_t HEAD = { 0, ID_TIMER, &HEAD, &HEAD, 0, 0, INFINITE }; // timers queue

/* -------------------------------------------------------------------------- */
static
void priv_tmr_insert( tmr_id tmr, unsigned id )
{
	tmr_id nxt = &HEAD;

	if (tmr->delay != INFINITE) for (;;)
	{
		nxt = nxt->next;

		if (nxt->delay == INFINITE)                             break;
		if (nxt->delay >  tmr->start + tmr->delay - nxt->start) break;
	}

	core_rdy_insert(tmr, id, nxt);
}

/* -------------------------------------------------------------------------- */

void core_tmr_insert( tmr_id tmr, unsigned id )
{
	priv_tmr_insert(tmr, id);
	port_tmr_force();
}

/* -------------------------------------------------------------------------- */

#if OS_ROBIN && OS_TIMER

static
bool priv_tmr_counting( tmr_id tmr )
{
	port_tmr_stop();

	if (tmr->delay == INFINITE)
	return true;  // return if timer counting indefinitly

	if (tmr->delay <= Counter - tmr->start)
	return false; // return if timer finished counting

	port_tmr_start(tmr->start + tmr->delay);

	if (tmr->delay >  Counter - tmr->start)
	return true;  // return if timer still counts

	port_tmr_stop();

	return false; // however timer finished counting
}

/* -------------------------------------------------------------------------- */

#else

static
bool priv_tmr_counting( tmr_id tmr )
{
	if (tmr->delay >= Counter - tmr->start + 1)
	return true;  // return if timer still counts or counting indefinitly

	return false; // timer finished counting
}

#endif

/* -------------------------------------------------------------------------- */

static
void priv_tmr_wakeup( tmr_id tmr, unsigned event )
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
	port_isr_lock();

	for (;;)
	{
		tmr_id tmr = System.tmr = HEAD.next;

		if (priv_tmr_counting(tmr))
			break;

		if (tmr->id == ID_TIMER)
			priv_tmr_wakeup((tmr_id)tmr, E_SUCCESS);

		else  /* id == ID_DELAYED */
			core_tsk_wakeup((tsk_id)tmr, E_TIMEOUT);
	}

	System.tmr = 0;
	
	port_isr_unlock();
}

/* -------------------------------------------------------------------------- */
// SYSTEM ALLOC SERVICES
/* -------------------------------------------------------------------------- */

#if    (OS_HEAP_SIZE >  1)

static  char       Heap[ASIZE(OS_HEAP_SIZE)] __osalign;
#define HeapEnd  ( Heap + sizeof(Heap) )

#else

extern  char     __heap_base [];
extern  char     __heap_limit[];
#define Heap     __heap_base
#define HeapEnd  __heap_limit

#endif

/* -------------------------------------------------------------------------- */

#if    (OS_HEAP_SIZE == 1)

os_id core_sys_alloc( size_t size )
{
	return calloc(size, 1);
}

/* -------------------------------------------------------------------------- */

#else

os_id core_sys_alloc( size_t size )
{
	static
	char *heap = Heap;
	char *base = 0;

	size = ASIZE(size);

	if (heap + size <= HeapEnd)
	{
		base = heap;
		heap = heap + size;

		for (unsigned *mem = (unsigned *)base; mem < (unsigned *)heap; *mem++ = 0);
	}

	return base;
}

#endif

/* -------------------------------------------------------------------------- */

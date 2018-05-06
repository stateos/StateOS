/******************************************************************************

    @file    StateOS: os_mem.c
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

#include "inc/os_mem.h"
#include "inc/os_tsk.h"

/* -------------------------------------------------------------------------- */
void mem_bind( mem_t *mem )
/* -------------------------------------------------------------------------- */
{
	que_t  * ptr;
	unsigned cnt;

	assert(!port_isr_inside());
	assert(mem);
	assert(mem->limit);
	assert(mem->size);
	assert(mem->data);

	port_sys_lock();
	
	ptr = mem->data;
	cnt = mem->limit;

	mem->next = 0;
	while (cnt--) { mem_give(mem, ++ptr); ptr += mem->size; }

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void mem_init( mem_t *mem, unsigned limit, unsigned size, void *data )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(mem);
	assert(limit);
	assert(size);
	assert(data);

	port_sys_lock();

	memset(mem, 0, sizeof(mem_t));

	mem->limit = limit;
	mem->size  = size;
	mem->data  = data;

	mem_bind(mem);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
mem_t *mem_create( unsigned limit, unsigned size )
/* -------------------------------------------------------------------------- */
{
	mem_t *mem;

	assert(!port_isr_inside());
	assert(limit);
	assert(size);

	size = MSIZE(size);

	port_sys_lock();

	mem = core_sys_alloc(ABOVE(sizeof(mem_t)) + limit * (1 + size) * sizeof(que_t));
	mem_init(mem, limit, size, (void *)((size_t)mem + ABOVE(sizeof(mem_t))));
	mem->res = mem;

	port_sys_unlock();

	return mem;
}

/* -------------------------------------------------------------------------- */
void mem_kill( mem_t *mem )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(mem);

	port_sys_lock();

	core_all_wakeup(mem, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void mem_delete( mem_t *mem )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	mem_kill(mem);
	core_sys_free(mem->res);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned mem_take( mem_t *mem, void **data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_TIMEOUT;

	assert(mem);
	assert(data);

	port_sys_lock();

	if (mem->next)
	{
		*data = mem->next + 1;
		mem->next = mem->next->next;

		memset(*data, 0, mem->size * sizeof(que_t));

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_mem_wait( mem_t *mem, void **data, cnt_t time, unsigned(*wait)(void*,cnt_t) )
/* -------------------------------------------------------------------------- */
{
	unsigned event;

	assert(!port_isr_inside());
	assert(mem);
	assert(data);

	port_sys_lock();

	if (mem->next)
	{
		*data = mem->next + 1;
		mem->next = mem->next->next;
		event = E_SUCCESS;
	}
	else
	{
		System.cur->tmp.mem.data.in = data;
		event = wait(mem, time);
	}

	if (event == E_SUCCESS)
		memset(*data, 0, mem->size * sizeof(que_t));

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned mem_waitUntil( mem_t *mem, void **data, cnt_t time )
/* -------------------------------------------------------------------------- */
{
	return priv_mem_wait(mem, data, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned mem_waitFor( mem_t *mem, void **data, cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	return priv_mem_wait(mem, data, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void mem_give( mem_t *mem, const void *data )
/* -------------------------------------------------------------------------- */
{
	tsk_t *tsk;
	que_t *ptr;

	assert(mem);
	assert(data);

	port_sys_lock();

	tsk = core_one_wakeup(mem, E_SUCCESS);

	if (tsk)
	{
		*tsk->tmp.mem.data.out = data;
	}
	else
	{
		ptr = (que_t *)&(mem->next);
		while (ptr->next) ptr = ptr->next;
		ptr->next = (que_t *)data - 1;
		ptr->next->next = 0;
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */

/******************************************************************************

    @file    StateOS: osmemorypool.c
    @author  Rajmund Szymanski
    @date    11.07.2018
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

#include "inc/osmemorypool.h"
#include "inc/ostask.h"

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

	core_sys_lock();
	
	ptr = mem->data;
	cnt = mem->limit;

	mem->head.next = 0;
	while (cnt--) { mem_give(mem, ++ptr); ptr += mem->size; }

	core_sys_unlock();
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

	core_sys_lock();

	memset(mem, 0, sizeof(mem_t));

	mem->limit = limit;
	mem->size  = size;
	mem->data  = data;

	mem_bind(mem);

	core_sys_unlock();
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

	core_sys_lock();

	mem = core_sys_alloc(ABOVE(sizeof(mem_t)) + limit * (1 + size) * sizeof(que_t));
	mem_init(mem, limit, size, (void *)((size_t)mem + ABOVE(sizeof(mem_t))));
	mem->res = mem;

	core_sys_unlock();

	return mem;
}

/* -------------------------------------------------------------------------- */
void mem_kill( mem_t *mem )
/* -------------------------------------------------------------------------- */
{
	assert(!port_isr_inside());
	assert(mem);

	core_sys_lock();

	core_all_wakeup(mem, E_STOPPED);

	core_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void mem_delete( mem_t *mem )
/* -------------------------------------------------------------------------- */
{
	core_sys_lock();

	mem_kill(mem);
	core_sys_free(mem->res);

	core_sys_unlock();
}

/* -------------------------------------------------------------------------- */

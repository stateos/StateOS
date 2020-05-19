/******************************************************************************

    @file    StateOS: osmemorypool.c
    @author  Rajmund Szymanski
    @date    19.05.2020
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

#include "inc/osmemorypool.h"
#include "inc/ostask.h"
#include "inc/oscriticalsection.h"
#include "osalloc.h"

/* -------------------------------------------------------------------------- */
void mem_bind( mem_t *mem )
/* -------------------------------------------------------------------------- */
{
	que_t  * ptr;
	unsigned cnt;

	assert_tsk_context();
	assert(mem);
	assert(mem->limit);
	assert(mem->size);
	assert(mem->data);

	sys_lock();
	{
		ptr = mem->data;
		cnt = mem->limit;

		mem->lst.head.next = 0;
		while (cnt--) { mem_give(mem, ++ptr); ptr += mem->size; }
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_mem_init( mem_t *mem, unsigned size, que_t *data, unsigned bufsize, void *res )
/* -------------------------------------------------------------------------- */
{
	memset(mem, 0, sizeof(mem_t));

	core_obj_init(&mem->lst.obj, res);

	mem->limit = bufsize / (1 + MEM_SIZE(size)) / sizeof(que_t);
	mem->size  = MEM_SIZE(size);
	mem->data  = data;

	mem_bind(mem);
}

/* -------------------------------------------------------------------------- */
void mem_init( mem_t *mem, unsigned size, que_t *data, unsigned bufsize )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(mem);
	assert(size);
	assert(data);
	assert(bufsize);

	sys_lock();
	{
		priv_mem_init(mem, size, data, bufsize, NULL);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
mem_t *mem_create( unsigned limit, unsigned size )
/* -------------------------------------------------------------------------- */
{
	struct mem_T { mem_t mem; que_t buf[]; } *tmp;
	mem_t *mem = NULL;
	size_t bufsize;

	assert_tsk_context();
	assert(limit);
	assert(size);

	sys_lock();
	{
		bufsize = limit * (1 + MEM_SIZE(size)) * sizeof(que_t);
		tmp = sys_alloc(sizeof(struct mem_T) + bufsize);
		if (tmp)
			priv_mem_init(mem = &tmp->mem, size, tmp->buf, bufsize, tmp);
	}
	sys_unlock();

	return mem;
}

/* -------------------------------------------------------------------------- */
static
void priv_mem_reset( mem_t *mem, unsigned event )
/* -------------------------------------------------------------------------- */
{
	core_all_wakeup(mem->lst.obj.queue, event);
}

/* -------------------------------------------------------------------------- */
void mem_reset( mem_t *mem )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(mem);
	assert(mem->lst.obj.res!=RELEASED);

	sys_lock();
	{
		priv_mem_reset(mem, E_STOPPED);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void mem_destroy( mem_t *mem )
/* -------------------------------------------------------------------------- */
{
	assert_tsk_context();
	assert(mem);
	assert(mem->lst.obj.res!=RELEASED);

	sys_lock();
	{
		priv_mem_reset(mem, mem->lst.obj.res ? E_DELETED : E_STOPPED);
		core_res_free(&mem->lst.obj);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */

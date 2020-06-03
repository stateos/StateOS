/******************************************************************************

    @file    StateOS: osalloc.c
    @author  Rajmund Szymanski
    @date    03.06.2020
    @brief   This file provides set of variables and functions for StateOS.

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

#include "osalloc.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
// SYSTEM ALLOC/FREE SERVICES
/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

static
seg_t Heap[SEG_SIZE(OS_HEAP_SIZE)+1] =
  { { Heap+SEG_SIZE(OS_HEAP_SIZE), Heap } };

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

void *sys_alloc( size_t size )
{
	seg_t *mem;
	seg_t *nxt;

	assert_tsk_context();
	assert(size>0&&size<OS_HEAP_SIZE);

	size = SEG_SIZE(size + sizeof(seg_t));

	sys_lock();
	{
		//	call garbage collection procedure
		core_tsk_destructor();

		for (mem = Heap; mem; mem = mem->next)
		{
			if (mem->owner == NULL)
		//	memory segment has already been allocated
				continue;

			while (nxt = mem->next, nxt->owner != NULL)
		//	it is possible to merge adjacent free memory segments
				mem->next = nxt->next;

			if (mem + size > nxt)
		//	memory segment is too small
				continue;

			if (mem + size < nxt)
			{
		//	memory segment is larger than required
				nxt = mem + size;
				nxt->next  = mem->next;
				nxt->owner = nxt;
				mem->next  = nxt;
			}

		//	memory segment can be allocated
			mem->owner = NULL;
			mem = mem + 1;
			break;
		}
	}
	sys_unlock();

	assert(mem);

	return mem;
}

#else

void *sys_alloc( size_t size )
{
	void *mem;

	assert_tsk_context();
	assert(size);

	sys_lock();
	{
		//	call garbage collection procedure
		core_tsk_destructor();
	}
	sys_unlock();

	mem = malloc(size);

	assert(mem);

	return mem;
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

void sys_free( void *ptr )
{
	seg_t *mem;
	seg_t *seg = (seg_t *)ptr - 1;

	assert_tsk_context();

	sys_lock();
	{
		//	call garbage collection procedure
		core_tsk_destructor();

		for (mem = Heap; mem; mem = mem->next)
		{
			if (mem != seg)
		//	this is not the memory segment we are looking for
				continue;

		//	memory segment can be released
			mem->owner = mem;
			break;
		}
	}
	sys_unlock();
}

#else

void sys_free( void *ptr )
{
	assert_tsk_context();

	sys_lock();
	{
		//	call garbage collection procedure
		core_tsk_destructor();
	}
	sys_unlock();

	free(ptr);
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

size_t sys_heapSize( void )
{
	seg_t *mem;
	seg_t *nxt;
	size_t size = 0;

	assert_tsk_context(); 

	sys_lock();
	{
		//	call garbage collection procedure
		core_tsk_destructor();

		for (mem = Heap; mem; mem = mem->next)
		{
			if (mem->owner == NULL)
		//	memory segment has already been allocated
				continue;

			while (nxt = mem->next, nxt->owner != NULL)
		//	it is possible to merge adjacent free memory segments
				mem->next = nxt->next;

			size += nxt - mem - 1;
		}
	}
	sys_unlock();

	return size * sizeof(seg_t);
}

#else

size_t sys_heapSize( void )
{
	assert_tsk_context(); 

	sys_lock();
	{
		//	call garbage collection procedure
		core_tsk_destructor();
	}
	sys_unlock();

	return 0;
}

#endif

/* -------------------------------------------------------------------------- */

void core_res_free( obj_t *obj )
{
	if (obj->res != NULL && obj->res != RELEASED)
	{
		sys_free(obj->res);
		obj->res = RELEASED;
	}
}

/* -------------------------------------------------------------------------- */

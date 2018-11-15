/******************************************************************************

    @file    StateOS: osalloc.c
    @author  Rajmund Szymanski
    @date    15.11.2018
    @brief   This file provides set of variables and functions for StateOS.

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

#include "osalloc.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
// SYSTEM ALLOC/FREE SERVICES
/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

static
seg_t Heap[SEG_SIZE(OS_HEAP_SIZE)+1] =
  { { Heap+SEG_SIZE(OS_HEAP_SIZE), Heap } };

/* -------------------------------------------------------------------------- */

void *sys_alloc( size_t size )
{
	seg_t *mem;
	seg_t *nxt;

	assert(size);

	size = SEG_SIZE(size) + 1;

	assert(size);

	sys_lock();
	{
		for (mem = Heap; mem; mem = mem->next)
		{
			if (mem->owner != mem)
		//	memory segment has already been allocated
				continue;

			while (nxt = mem->next, nxt->owner)
		//	it is possible to merge adjacent free memory segments
				mem->next = nxt->next;

			if (nxt < mem + size)
		//	memory segment is too small
				continue;

			if (nxt > mem + size)
		//	memory segment is larger than required
			{
				nxt = mem + size;
				nxt->next  = mem->next;
				nxt->owner = nxt;
			}

			mem = memset(mem, 0, size * sizeof(seg_t));
			mem->next = nxt;
			mem = mem + 1;
		//	memory segment has been successfully allocated
			break;
		}
	}
	sys_unlock();

	assert(mem);

	return mem;
}

/* -------------------------------------------------------------------------- */

void sys_free( void *base )
{
	seg_t *mem;
	seg_t *seg = (seg_t *)base - 1;

	sys_lock();
	{
		for (mem = Heap; mem; mem = mem->next)
		{
			if (mem != seg)
		//	this is not the memory segment we are looking for
				continue;

			mem->owner = mem;
		//	memory segment has been successfully released
			break;
		}
	}
	sys_unlock();
}

#endif

/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE == 0

void *sys_alloc( size_t size )
{
	void *mem;

	assert(size);

	mem = malloc(size);

	if (mem)
		mem = memset(mem, 0, size);

	assert(mem);

	return mem;
}

/* -------------------------------------------------------------------------- */

void sys_free( void *base )
{
	free(base);
}

#endif

/* -------------------------------------------------------------------------- */

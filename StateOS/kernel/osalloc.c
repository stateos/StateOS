/******************************************************************************

    @file    StateOS: osalloc.c
    @author  Rajmund Szymanski
    @date    16.07.2018
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

#include "oskernel.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
// SYSTEM ALLOC/FREE SERVICES
/* -------------------------------------------------------------------------- */

#if OS_HEAP_SIZE

/* -------------------------------------------------------------------------- */

typedef struct __hdr hdr_t;

struct __hdr
{
	hdr_t  * next;
	size_t   size;
};

/* -------------------------------------------------------------------------- */

#define HSIZE( size ) \
 ALIGNED_SIZE( size, hdr_t )

/* -------------------------------------------------------------------------- */

static
hdr_t Heap[HSIZE(OS_HEAP_SIZE)+1] =
  { { Heap+HSIZE(OS_HEAP_SIZE), HSIZE(OS_HEAP_SIZE) } };

/* -------------------------------------------------------------------------- */

void *core_sys_alloc( size_t size )
{
	hdr_t *heap;
	hdr_t *next;

	assert(HSIZE(size));

	size = HSIZE(size) + 1;

	sys_lock();
	{
		for (heap = Heap; heap; heap = heap->next)
		{
			if (heap->size == 0)				// memory segment has already been allocated
				continue;

			while ((next = heap->next)->size)	// it is possible to merge adjacent free memory segments
			{
				heap->next = next->next;
				heap->size += next->size;
			}

			if (heap->size < size)				// memory segment is too small
				continue;

			if (heap->size > size)				// memory segment is larger than required
			{
				next = heap + size;
				next->next = heap->next;
				next->size = heap->size - size;
			}

			heap = memset(heap, 0, size * sizeof(hdr_t));
			heap->next = next;
			heap = heap + 1;
			break;								// memory segment was successfully allocated
		}
	}
	sys_unlock();

	assert(heap);

	return heap;
}

/* -------------------------------------------------------------------------- */

void core_sys_free( void *base )
{
	hdr_t *heap;

	base = (hdr_t *) base - 1;

	sys_lock();
	{
		for (heap = Heap; heap; heap = heap->next)
		{
			if (heap != base)					// this is not the memory segment we are looking for
				continue;

			heap->size = heap->next - heap;
			break;								// memory segment was successfully released
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */

#else

void *core_sys_alloc( size_t size )
{
	void *base;

	assert(size);

	base = malloc(size);

	if (base)
		base = memset(base, 0, size);

	assert(base);

	return base;
}

/* -------------------------------------------------------------------------- */

void core_sys_free( void *base )
{
	free(base);
}

#endif

/* -------------------------------------------------------------------------- */

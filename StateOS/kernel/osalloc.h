/******************************************************************************

    @file    StateOS: osalloc.h
    @author  Rajmund Szymanski
    @date    30.04.2020
    @brief   This file contains definitions for StateOS.

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

#ifndef __STATEOSALLOC_H
#define __STATEOSALLOC_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#define SEG_SIZE( size ) \
    ALIGNED_SIZE( size, seg_t )

#define SEG_OVER( size ) \
         ALIGNED( size, seg_t )

/******************************************************************************
 *
 * Name              : memory segment header
 *
 ******************************************************************************/

typedef struct __seg seg_t;

struct __seg
{
	seg_t  * next;  // next memory block
	seg_t  * owner; // owner of memory block
};

/******************************************************************************
 *
 * Name              : sys_alloc
 *
 * Description       : system malloc procedure with clearing the allocated memory
 *
 * Parameters
 *   size            : required size of the memory segment (in bytes)
 *
 * Return            : pointer to the beginning of allocated and cleared memory segment
 *   0               : memory segment not allocated (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

#if OS_HEAP_SIZE == 0
__STATIC_INLINE
void *sys_alloc( size_t size )
{
	void *mem;
	assert(size);
	mem = malloc(size);
	assert(mem);
	return mem;
}
#else
void *sys_alloc( size_t size );
#endif

/******************************************************************************
 *
 * Name              : sys_free
 *
 * Description       : system free procedure
 *
 * Parameters
 *   ptr             : pointer to a memory segment previously allocated with sys_alloc, xxx_create or xxx_new functions
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

#if OS_HEAP_SIZE == 0
__STATIC_INLINE
void sys_free( void *ptr )
{
	free(ptr);
}
#else
void sys_free( void *ptr );
#endif

/******************************************************************************
 *
 * Name              : sys_heap
 *
 * Description       : get total size of free heap memory
 *
 * Parameters
 *   ptr             : none
 *
 * Return            : size of free heap memory
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

#if OS_HEAP_SIZE == 0
__STATIC_INLINE
size_t sys_heap( void )
{
	return 0;
}
#else
size_t sys_heap( void );
#endif

/******************************************************************************
 *
 * Name              : core_res_free
 *
 * Description       : frees given resources
 *
 * Parameters
 *   res             : pointer to a pointer to resources
 *
 * Return            : true if any resources have been released, otherwise false
 *
 * Note              : for internal use
 *
 ******************************************************************************/

__STATIC_INLINE
void core_res_free( void **res )
{
	void *tmp;

	if (*res != NULL && *res != RELEASED)
	{
		tmp = *res;
		*res = RELEASED;
		sys_free(tmp);
	}
}

#ifdef __cplusplus
}
#endif

#endif//__STATEOSALLOC_H

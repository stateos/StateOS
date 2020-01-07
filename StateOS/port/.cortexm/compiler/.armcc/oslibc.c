/******************************************************************************

    @file    StateOS: oslibc.c
    @author  Rajmund Szymanski
    @date    03.01.2020
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
#include "inc/ostask.h"

/* -------------------------------------------------------------------------- */

#if !defined(__MICROLIB)

__USED
void *__user_perthread_libspace( void )
{
	return &System.cur->libspace;  /* provide separate libspace for each task */
}

/* -------------------------------------------------------------------------- */

__USED
int _mutex_initialize( lck_t *mutex )
{
	(void) mutex;

	return 1;
}

/* -------------------------------------------------------------------------- */

__USED
void _mutex_acquire( lck_t *mutex )
{
	lck_t lock = port_get_lock();
	port_set_lock();
	*mutex = lock;
}

/* -------------------------------------------------------------------------- */

__USED
void _mutex_release( lck_t *mutex )
{
	lck_t lock = *mutex;
	port_put_lock(lock);
}

/* -------------------------------------------------------------------------- */

__USED
void _mutex_free( lck_t *mutex )
{
	(void) mutex;
}

/* -------------------------------------------------------------------------- */

__WEAK
char *_sys_command_string( char *cmd, int len )
{
	(void) len;

	return cmd;
}

#endif // !__MICROLIB

/* -------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>

void __aeabi_assert(const char* expr, const char* file, int line)
{
	printf("\nassert error at %s:%d:%s\n", file, line, expr);
	abort();
}

/* -------------------------------------------------------------------------- */

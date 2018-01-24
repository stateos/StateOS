/******************************************************************************

    @file    StateOS: oslibc.c
    @author  Rajmund Szymanski
    @date    24.01.2018
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

#if  defined(__CC_ARM)
#if !defined(__MICROLIB)

#include <oskernel.h>

/* -------------------------------------------------------------------------- */

__attribute__((used))
void *__user_perthread_libspace( void )
{
	return &System.cur->libspace;  /* provide separate libspace for each task */
}

/* -------------------------------------------------------------------------- */

__attribute__((used))
int _mutex_initialize( unsigned *mutex )
{
	return (int) mutex;
}

/* -------------------------------------------------------------------------- */

__attribute__((used))
void _mutex_acquire( unsigned *mutex )
{
	unsigned lock = port_get_lock();
	port_set_lock();
	*mutex = lock;
}

/* -------------------------------------------------------------------------- */

__attribute__((used))
void _mutex_release( unsigned *mutex )
{
	port_put_lock(*mutex);
}

/* -------------------------------------------------------------------------- */

__attribute__((used))
void _mutex_free( unsigned *mutex )
{
	(void) mutex;
}

/* -------------------------------------------------------------------------- */

__attribute__((weak))
char *_sys_command_string( char *cmd, int len )
{
	(void) len;

	return cmd;
}

/* -------------------------------------------------------------------------- */

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

#endif // __CC_ARM

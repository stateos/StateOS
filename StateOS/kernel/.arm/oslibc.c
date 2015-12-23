/******************************************************************************

    @file    State Machine OS: oslibc.c
    @author  Rajmund Szymanski
    @date    23.12.2015
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

#if defined(__CC_ARM) && !defined(__MICROLIB)

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

void _mutex_free( unsigned *mutex )
{
}

/* -------------------------------------------------------------------------- */

#endif

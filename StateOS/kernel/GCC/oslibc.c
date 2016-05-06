/******************************************************************************

    @file    StateOS: oslibc.c
    @author  Rajmund Szymanski
    @date    06.05.2016
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

#ifdef __GNUC__

#include <errno.h>
#include <sys/stat.h>
#include <os.h>

/* -------------------------------------------------------------------------- */

#ifndef USE_NANO

static unsigned LCK = 0;
static unsigned CNT = 0;

/* -------------------------------------------------------------------------- */

void __malloc_lock()
{
	unsigned lock = port_get_lock();
	port_set_lock();
	if (CNT == 0U) LCK = lock;
	if (CNT < ~0U) CNT++;
}

/* -------------------------------------------------------------------------- */

void __malloc_unlock()
{
	if (CNT > 0U)
		if (--CNT == 0U)
			port_put_lock(LCK);
}

#endif // USE_NANO

/* -------------------------------------------------------------------------- */

caddr_t _sbrk_r( struct _reent *reent, size_t size )
{
	extern char __heap_start[];
	extern char __heap_end[];
	static char * heap = __heap_start;
	       char * base;
	      (void)  reent;
#ifdef USE_NANO
	port_sys_lock();
#endif
	if (heap + size <= __heap_end)
	{
		base  = heap;
		heap += size;
	}
	else
	{
		errno = ENOMEM;
		base  = (caddr_t) -1;
	}
#ifdef USE_NANO
	port_sys_unlock();
#endif
	return base;
}

/* -------------------------------------------------------------------------- */

static
int __enosys()
{
	errno = ENOSYS; // procedure not implemented

	return -1;
}

/* -------------------------------------------------------------------------- */

int   _open_r( struct _reent *reent, const char *path, int flags, int mode ) __attribute__((weak, alias("__enosys")));
int  _close_r( struct _reent *reent, int file )                              __attribute__((weak, alias("__enosys")));
int  _lseek_r( struct _reent *reent, int file, int pos, int whence )         __attribute__((weak, alias("__enosys")));
int   _read_r( struct _reent *reent, int file, char *buf, size_t size )      __attribute__((weak, alias("__enosys")));
int  _write_r( struct _reent *reent, int file, char *buf, size_t size )      __attribute__((weak, alias("__enosys")));
int _isatty_r( struct _reent *reent, int file )                              __attribute__((weak, alias("__enosys")));
int  _fstat_r( struct _reent *reent, int file, struct stat *st )             __attribute__((weak, alias("__enosys")));
int _getpid_r( struct _reent *reent )                                        __attribute__((weak, alias("__enosys")));
int   _kill_r( struct _reent *reent, int pid, int sig )                      __attribute__((weak, alias("__enosys")));

/* -------------------------------------------------------------------------- */

#endif // __GNUC__

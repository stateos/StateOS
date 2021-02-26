#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>

/* -------------------------------------------------------------------------- */

__attribute__((used, weak))
caddr_t _sbrk_r( struct _reent *reent, size_t size )
{
	extern char __heap_start[];
	extern char __heap_end[];
	static char * heap = __heap_start;
	       char * base;
	      (void)  reent;

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

	return base;
}

/* -------------------------------------------------------------------------- */

#if !defined(USE_SEMIHOST) && !defined(USE_NOHOST)

/* -------------------------------------------------------------------------- */

static
int __enosys()
{
	errno = ENOSYS; // procedure not implemented
	return -1;
}

/* -------------------------------------------------------------------------- */

int   _open_r( struct _reent *, const char *, int, int ) __attribute__((weak, alias("__enosys")));
int  _close_r( struct _reent *, int )                    __attribute__((weak, alias("__enosys")));
int  _lseek_r( struct _reent *, int, int, int )          __attribute__((weak, alias("__enosys")));
int   _read_r( struct _reent *, int, char *, size_t )    __attribute__((weak, alias("__enosys")));
int  _write_r( struct _reent *, int, char *, size_t )    __attribute__((weak, alias("__enosys")));
int _isatty_r( struct _reent *, int )                    __attribute__((weak, alias("__enosys")));
int  _fstat_r( struct _reent *, int, struct stat * )     __attribute__((weak, alias("__enosys")));
int _getpid_r( struct _reent * )                         __attribute__((weak, alias("__enosys")));
int   _kill_r( struct _reent *, int, int )               __attribute__((weak, alias("__enosys")));

/* -------------------------------------------------------------------------- */

#endif // !USE_SEMIHOST && !USE_NOHOST

/* -------------------------------------------------------------------------- */

__attribute__((weak, noreturn))
void __assert_func(const char* const file, const int line, const char* const func, const char* const expr)
{
	printf("\nassert error at %s:%d:%s:%s\n", file, line, expr, func);
	abort();
}

/* -------------------------------------------------------------------------- */

__attribute__((weak, noreturn))
void __cxa_pure_virtual()
{
	abort();
}

/* -------------------------------------------------------------------------- */

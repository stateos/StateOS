#include <stdlib.h>
#include <stdio.h>

/* -------------------------------------------------------------------------- */

#if !defined(__MICROLIB)

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

__attribute__((weak, noreturn))
void __aeabi_assert(const char* expr, const char* file, int line)
{
	printf("\nassert error at %s:%d:%s\n", file, line, expr);
	abort();
}

/* -------------------------------------------------------------------------- */

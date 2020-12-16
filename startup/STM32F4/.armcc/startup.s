;*******************************************************************************
;file     startup.s
;author   Rajmund Szymanski
;date     07.01.2020
;brief    Startup code for armcc compiler.
;*******************************************************************************

	#ifndef    main_stack_size
	#define    main_stack_size     0
	#endif
	#ifndef    proc_stack_size
	#define    proc_stack_size     0
	#endif

	#define    __main_stack_size (((main_stack_size)+7)&(~7))
	#define    __proc_stack_size (((proc_stack_size)+7)&(~7))

	#define    __RAM_end           0x20020000

	AREA       |.heap|, NOINIT, READWRITE, ALIGN=3
__heap_base
__heap_limit   EQU   __RAM_end - __proc_stack_size

	#if        __main_stack_size > 0
	AREA       |.stack|, NOINIT, READWRITE, ALIGN=3
__main_stack   SPACE __main_stack_size
__initial_msp
	#else
__initial_msp  EQU   __heap_limit
	#endif

	#if        __proc_stack_size > 0
	AREA       |.heap|, NOINIT, READWRITE, ALIGN=3
__proc_stack   SPACE __proc_stack_size
__initial_sp   EQU   __RAM_end
	#else
__initial_sp   EQU   __initial_msp
	#endif

	#if        __proc_stack_size > 0
	#ifndef    __MICROLIB
	EXPORT     __use_two_region_memory
	#endif
	#endif

	EXPORT     __heap_base
	EXPORT     __heap_limit
	EXPORT     __initial_msp
	EXPORT     __initial_sp

	END

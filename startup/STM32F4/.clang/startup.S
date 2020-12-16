/*******************************************************************************
@file     startup.S
@author   Rajmund Szymanski
@date     07.01.2020
@brief    Startup code for armclang compiler.
*******************************************************************************/

	#ifndef     main_stack_size
	#define     main_stack_size    0
	#endif
	#ifndef     proc_stack_size
	#define     proc_stack_size    0
	#endif

	#define   __main_stack_size (((main_stack_size)+7)&(~7))
	#define   __proc_stack_size (((proc_stack_size)+7)&(~7))

	#define   __RAM_end            0x20020000

	.syntax     unified
	.arch       armv7e-m

	.section    .heap, "w","nobits"
	.align 3
__heap_base   = .
__heap_limit  = __RAM_end - __proc_stack_size

	#if         __main_stack_size > 0
	.section    .stack, "w","nobits"
	.align 3
	.space      __main_stack_size
__initial_msp = .
	#else
__initial_msp = __heap_limit
	#endif

	#if         __proc_stack_size > 0
	.section    .heap, "w","nobits"
	.align 3
	.space      __proc_stack_size
__initial_sp  = __RAM_end
	#else
__initial_sp  = __initial_msp
	#endif

	#if         __proc_stack_size > 0
	#ifndef     __MICROLIB
	.global     __use_two_region_memory
	#endif
	#endif

	.global     __heap_base
	.global     __heap_limit
	.global     __initial_msp
	.global     __initial_sp

	.end

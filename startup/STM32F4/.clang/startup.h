/*******************************************************************************
@file     startup.h
@author   Rajmund Szymanski
@date     19.07.2018
@brief    Startup file header for armclang compiler.
*******************************************************************************/

#pragma once

#define NUM(n) #n"\n"
#define STR(n) NUM(n)

/*******************************************************************************
 Configuration of stacks and heap
*******************************************************************************/

#define __main_stack_size (((main_stack_size)+7)&(~7))
#define __proc_stack_size (((proc_stack_size)+7)&(~7))

__attribute__ ((naked))
void __user_stacks_and_heap_config( void )
{
	__asm volatile
	(
"               .pushsection .heap, \"w\",\"nobits\"\n"
"__heap_base    =       .               \n"
"__heap_limit   =      "STR(RAM_end - __proc_stack_size)
"               .popsection             \n"
                #if     __main_stack_size > 0
"               .pushsection .stack,\"w\",\"nobits\"\n"
"               .align  3               \n"
"               .space "STR(__main_stack_size)
"__initial_msp  =       .               \n"
"               .popsection             \n"
                #else
"__initial_msp  =       __heap_limit    \n"
                #endif
                #if     __proc_stack_size > 0
"               .pushsection .heap, \"w\",\"nobits\"\n"
"               .align  3               \n"
"               .space "STR(__proc_stack_size)
"               .popsection             \n"
"__initial_sp   =      "STR(RAM_end)
                #else
"__initial_sp   =       __initial_msp   \n"
                #endif

                #if     __proc_stack_size > 0
                #ifndef __MICROLIB
"               .global __use_two_region_memory\n"
                #endif
                #endif

"               .global __heap_base     \n"
"               .global __heap_limit    \n"
"               .global __initial_msp   \n"
"               .global __initial_sp    \n"
	);
}

/*******************************************************************************
 Additional definitions
*******************************************************************************/

#define __ALIAS(function) __attribute__ ((weak, alias(#function)))
#define __VECTORS         __attribute__ ((used, section(".vectors")))

/*******************************************************************************
 Prototypes of external functions
*******************************************************************************/

__NO_RETURN __ALIAS(Fault_Handler) void _microlib_exit( void );
__NO_RETURN __ALIAS(Fault_Handler) void      _sys_exit( void );
__NO_RETURN                        void         __main( void );

/******************************************************************************/

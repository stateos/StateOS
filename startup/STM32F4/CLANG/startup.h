/*******************************************************************************
@file     startup.h
@author   Rajmund Szymanski
@date     02.01.2017
@brief    Startup file header for armclang compiler.
*******************************************************************************/

#pragma once

/*******************************************************************************
 Configuration of stacks and heap
*******************************************************************************/

#define NUM(n) #n
#define STR(n) NUM(n)

__attribute__ ((naked))
void __user_stacks_and_heap_config( void )
{
	__asm volatile
	(
"               .pushsection .heap, \"w\",\"nobits\"\n"
"__heap_base    =       .               \n"
"__heap_limit   =      "STR(__ram_end - proc_stack)"\n"
"               .popsection             \n"
                #if     main_stack_size > 0
"               .pushsection .stack,\"w\",\"nobits\"\n"
"               .space "STR(main_stack)"\n"
"               .align  3               \n"
"__initial_msp  =       .               \n"
"               .popsection             \n"
                #else
"__initial_msp  =       __heap_limit    \n"
                #endif
                #if     proc_stack_size > 0
"               .pushsection .heap, \"w\",\"nobits\"\n"
"               .space "STR(proc_stack)"\n"
"               .align  3               \n"
"               .popsection             \n"
"__initial_sp   =      "STR(__ram_end) "\n"
                #else
"__initial_sp   =       __initial_msp   \n"
                #endif

				#if     proc_stack_size > 0
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
 Prototypes of external functions
*******************************************************************************/

void __main( void ) __attribute__ ((noreturn));

/******************************************************************************/

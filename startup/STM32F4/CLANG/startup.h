/*******************************************************************************
@file     startup.h
@author   Rajmund Szymanski
@date     21.05.2016
@brief    Startup file header for armclang compiler.
*******************************************************************************/

#pragma once

/*******************************************************************************
 Configuration of stacks and heap
*******************************************************************************/

#define NUM(n) #n
#define STR(n) NUM(n) "\n"

__attribute__ ((naked))
void __user_stack_config( void )
{
	__asm volatile
	(
"               .pushsection .stack, \"aw\", %nobits \n"

                #if     main_stack_size > 0
"               .space "STR(main_stack_size)
"               .align  3               \n"
"__initial_msp  =       .               \n"
                #else
"__initial_msp  =      "STR(__ram_end)
                #endif

                #if     proc_stack_size > 0
                #ifndef __MICROLIB
"               .global __use_two_region_memory \n"
                #endif
"               .space "STR(proc_stack_size)
"               .align  3               \n"
"__initial_psp  =       .               \n"
"__initial_sp   =       __initial_psp   \n"
                #else
"__initial_psp  =       .               \n"
"__initial_sp   =       __initial_msp   \n"
                #endif

"               .global __initial_msp   \n"
"               .global __initial_psp   \n"
"               .global __initial_sp    \n"

"               .popsection             \n"
	);
}

__attribute__ ((naked))
void __user_heap_config( void )
{
	__asm volatile
	(
"               .pushsection .heap, \"aw\", %nobits \n"

"__heap_base    =       .               \n"
"__heap_limit   =      "STR(__ram_end)

"               .global __heap_base     \n"
"               .global __heap_limit    \n"

"               .popsection             \n"
	);
}

/*******************************************************************************
 Prototypes of external functions
*******************************************************************************/

void __main( void ) __attribute__ ((noreturn));

/******************************************************************************/

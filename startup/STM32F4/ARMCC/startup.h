/*******************************************************************************
@file     startup.h
@author   Rajmund Szymanski
@date     02.01.2017
@brief    Startup file header for armcc compiler.
*******************************************************************************/

#pragma once

/*******************************************************************************
 Configuration of stacks and heap
*******************************************************************************/

#if     main_stack_size > 0
char  __main_stack[main_stack] __attribute__ ((used, section(".stack"), zero_init));
#endif

#if     proc_stack_size > 0
char  __proc_stack[proc_stack] __attribute__ ((used, section(".heap"),  zero_init));
#endif

__attribute__ ((section(".heap")))
__asm void __user_stacks_and_heap_config( void )
{
__heap_base     EQU     .
__heap_limit    EQU     __ram_end - proc_stack
                #if     main_stack_size > 0
__initial_msp   EQU     __ram_start + main_stack
                #else
__initial_msp   EQU     __heap_limit
                #endif
                #if     proc_stack_size > 0
__initial_sp    EQU     __ram_end
                #else
__initial_sp    EQU     __initial_msp
                #endif

				#if     proc_stack_size > 0
                #ifndef __MICROLIB
                IMPORT  __use_two_region_memory
                #endif
                #endif

                EXPORT  __heap_base
                EXPORT  __heap_limit
                EXPORT  __initial_msp
                EXPORT  __initial_sp
}

/*******************************************************************************
 Prototypes of external functions
*******************************************************************************/

void __main( void ) __attribute__ ((noreturn));

/******************************************************************************/

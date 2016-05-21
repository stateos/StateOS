/*******************************************************************************
@file     startup.h
@author   Rajmund Szymanski
@date     21.05.2016
@brief    Startup file header for armcc compiler.
*******************************************************************************/

#pragma once

/*******************************************************************************
 Configuration of stacks
*******************************************************************************/

#if     main_stack_size > 0
char  __main_stack[main_stack] __attribute__ ((used, section(".stack"), zero_init));
#endif

#if     proc_stack_size > 0
char  __proc_stack[proc_stack] __attribute__ ((used, section(".stack"), zero_init));
#endif

/*******************************************************************************
 Configuration of stacks and heap
*******************************************************************************/

__attribute__ ((section(".stack")))
__asm void __user_stack_config( void )
{
                #if     main_stack_size > 0
__initial_msp   EQU     __ram_start + main_stack
                #else
__initial_msp   EQU     __ram_end
                #endif

__initial_psp   EQU     __ram_start + main_stack + proc_stack
                #if     proc_stack_size > 0
                #ifndef __MICROLIB
                IMPORT  __use_two_region_memory
                #endif
__initial_sp    EQU     __initial_psp
                #else
__initial_sp    EQU     __initial_msp
                #endif

                EXPORT  __initial_msp
                EXPORT  __initial_psp
                EXPORT  __initial_sp
}

__attribute__ ((section(".heap")))
__asm void __user_heap_config( void )
{
__heap_base     EQU     .
__heap_limit    EQU     __ram_end

                EXPORT  __heap_base
                EXPORT  __heap_limit
}

/*******************************************************************************
 Prototypes of external functions
*******************************************************************************/

void __main( void ) __attribute__ ((noreturn));

/******************************************************************************/

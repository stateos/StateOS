/*******************************************************************************
@file     startup.h
@author   Rajmund Szymanski
@date     21.11.2017
@brief    Startup file header for armcc compiler.
*******************************************************************************/

#pragma once

/*******************************************************************************
 Configuration of stacks and heap
*******************************************************************************/

#define __main_stack_size (((main_stack_size)+7)&(~7))
#define __proc_stack_size (((proc_stack_size)+7)&(~7))

#if       main_stack_size > 0
char    __main_stack[__main_stack_size] __ALIGNED(8) __attribute__ ((used, section(".stack"), zero_init));
#endif

#if       proc_stack_size > 0
char    __proc_stack[__proc_stack_size] __ALIGNED(8) __attribute__ ((used, section(".heap"),  zero_init));
#endif

__attribute__ ((section(".heap")))
__asm void __user_stacks_and_heap_config( void )
{
__heap_base     EQU     .
__heap_limit    EQU     __ram_end - __proc_stack_size
                #if     main_stack_size > 0
__initial_msp   EQU     __ram_start + __main_stack_size
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

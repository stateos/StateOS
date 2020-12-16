/*******************************************************************************
@file     startup.h
@author   Rajmund Szymanski
@date     13.12.2020
@brief    Startup file header for armclang compiler.
*******************************************************************************/

#pragma once

/*******************************************************************************
 Additional definitions
*******************************************************************************/

#define __ALIAS(function) __attribute__ ((weak, alias(#function)))
#define __VECTORS         __attribute__ ((used, section("RESET")))
#define __CAST(sp)        (void(*)(void))(intptr_t)(sp)

/*******************************************************************************
 Prototypes of external functions
*******************************************************************************/

__NO_RETURN __ALIAS(Fault_Handler) void _microlib_exit( void );
__NO_RETURN __ALIAS(Fault_Handler) void      _sys_exit( void );
__NO_RETURN                        void         __main( void );
__NO_RETURN __ALIAS(_exit)         void           exit( int  );

/******************************************************************************/

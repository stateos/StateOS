/*******************************************************************************
@file     startup.h
@author   Rajmund Szymanski
@date     10.12.2019
@brief    Startup file header for cosmic c compiler.
*******************************************************************************/

#pragma once

/*******************************************************************************
 Additional definitions
*******************************************************************************/

#define __ALIAS(function) __WEAK
#define __VECTORS         \#pragma section const { vectors }
#define __CAST(sp)        (void(*)(void))(intptr_t)(sp)

/*******************************************************************************
 Prototypes of external functions
*******************************************************************************/

__WEAK      void  _init( void );
__NO_RETURN void _stext( void );

/*******************************************************************************
 Default reset procedures
*******************************************************************************/

__STATIC_INLINE
void __main( void )
{
	/* Call the os initialization procedure */
	_init();
	/* Call the application's entry point */
	_stext();
}

/******************************************************************************/

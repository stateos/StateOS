/*******************************************************************************
@file     startup.h
@author   Rajmund Szymanski
@date     14.11.2017
@brief    Startup file header for cosmic c compiler.
*******************************************************************************/

#pragma once

/*******************************************************************************
 Additional definitions
*******************************************************************************/

#define __ALIAS(function)  __WEAK
#define __VECTORS \#pragma section const { vectors }

/*******************************************************************************
 Prototypes of external functions
*******************************************************************************/

__WEAK      void port_sys_init( void );
__NO_RETURN void        _stext( void );

/*******************************************************************************
 Default reset procedures
*******************************************************************************/

__STATIC_INLINE
void __main( void )
{
	/* Call the os initialization procedure */
	port_sys_init();
	/* Call the application's entry point */
	_stext();
}

/******************************************************************************/

/*******************************************************************************
@file     startup.h
@author   Rajmund Szymanski
@date     04.11.2017
@brief    Startup file header for cosmic c compiler.
*******************************************************************************/

#pragma once

/*******************************************************************************
 Prototypes of external functions
*******************************************************************************/

void port_sys_init( void );
void        _stext( void );

/*******************************************************************************
 Default reset procedures
*******************************************************************************/

static inline
void __main( void )
{
	/* Call the os initialization procedure */
	port_sys_init();
	/* Call the application's entry point */
	_stext();
}

/******************************************************************************/

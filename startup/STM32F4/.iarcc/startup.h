/*******************************************************************************
@file     startup.h
@author   Rajmund Szymanski
@date     11.12.2019
@brief    Startup file header for iar c compiler.
*******************************************************************************/

#pragma once

/*******************************************************************************
 Additional definitions
*******************************************************************************/

#define __ALIAS(function) __attribute__ ((weak, alias(#function)))
#define __VECTORS         __attribute__ ((used, section(".intvec")))
#define __CAST(sp)        (void(*)(void))(intptr_t)(sp)

/*******************************************************************************
 Prototypes of external functions
*******************************************************************************/

__NO_RETURN void __iar_program_start( void );

/*******************************************************************************
 Default reset procedures
*******************************************************************************/

__STATIC_INLINE __NO_RETURN
void __main( void )
{
	/* Call the application's entry point */
	__iar_program_start();
}

/******************************************************************************/

/*******************************************************************************
@file     startup.h
@author   Rajmund Szymanski
@date     11.12.2019
@brief    Startup file header for cosmic c compiler.
*******************************************************************************/

#pragma once

/*******************************************************************************
 Additional definitions
*******************************************************************************/

#define __ALIAS(function) __WEAK
#define __CAST(sp)        (void(*)(void))(sp)

#ifndef __VECTOR_TABLE
#define __VECTOR_TABLE            _Vectors
#endif

#ifndef __VECTOR_TABLE_ATTRIBUTE
#define __VECTOR_TABLE_ATTRIBUTE  _Pragma("section const { vectors }")
#endif

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
	/* Call the low level initialization procedure */
	_init();
	/* Call the application's entry point */
	_stext();
}

/******************************************************************************/

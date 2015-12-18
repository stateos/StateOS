/******************************************************************************

    @file    State Machine OS: os_mtx.h
    @author  Rajmund Szymanski
    @date    26.11.2015
    @brief   This file contains definitions for StateOS.

 ******************************************************************************

    StateOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of StateOS distribution.

    StateOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    StateOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#pragma once

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#define mtxNormal                       ( 0U << 0 )
#define mtxRecursive                    ( 1U << 0 )
#define mtxPriorityProtect              ( 0U << 1 )
#define mtxPriorityInheritance          ( 1U << 1 )

#define mtxNormalPriorityProtect        ( mtxNormal    | mtxPriorityProtect     )
#define mtxRecursivePriorityProtect     ( mtxRecursive | mtxPriorityProtect     )
#define mtxNormalPriorityInheritance    ( mtxNormal    | mtxPriorityInheritance )
#define mtxRecursivePriorityInheritance ( mtxRecursive | mtxPriorityInheritance )

/* -------------------------------------------------------------------------- */

// deklaracja mechanizmu wzajemnego wykluczenia 'mtx'
// mutex

#define OS_MTX( mtx, type )                       \
               mtx_t mtx##__mtx = _MTX_INIT(type); \
               mtx_id mtx = & mtx##__mtx

#define static_MTX( mtx, type )                   \
        static mtx_t mtx##__mtx = _MTX_INIT(type); \
        static mtx_id mtx = & mtx##__mtx

/* -------------------------------------------------------------------------- */

// utworzenie obiektu typu mutex rodzaju 'type'
// type: mtxNormal, mtxRecursive, mtxPriorityProtect, mtxPriorityInheritance 
// zwraca adres utworzonego obiektu, lub 0
              mtx_id   mtx_create( unsigned type );

// reset obiektu 'mtx'
// wszystkie procesy oczekuj¹ce zostaj¹ wybudzone
// zostaje do nich wys³any komunikat E_STOPPED
              void     mtx_kill( mtx_id mtx );

// zawieszenie wykonywania aktualnego procesu do czasu 'time'
// lub do wybudzenia przez obiekt 'mtx'
// jeœli obiekt 'mtx' jest wolny, to nastêpuje jego zajêcie
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned mtx_waitUntil( mtx_id mtx, unsigned time );

// zawieszenie wykonywania aktualnego procesu na czas 'delay'
// lub do wybudzenia przez obiekt 'mtx'
// jeœli obiekt 'mtx' jest wolny, to nastêpuje jego zajêcie
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned mtx_waitFor( mtx_id mtx, unsigned delay );

// zawieszenie wykonywania aktualnego procesu
// do czasu wybudzenia przez obiekt 'mtx'
// jeœli obiekt 'mtx' jest wolny, to nastêpuje jego zajêcie
// zwraca E_SUCCESS lub E_STOPPED
static inline unsigned mtx_wait( mtx_id mtx ) { return mtx_waitFor(mtx, INFINITE); }

// nie zawiesza wykonywania aktualnego procesu
// jeœli obiekt 'mtx' jest wolny, to nastêpuje jego zajêcie
// zwraca E_SUCCESS lub E_TIMEOUT
static inline unsigned mtx_take( mtx_id mtx ) { return mtx_waitFor(mtx, IMMEDIATE); }

// zwolnienie obiektu 'mtx'
// obiekt 'mtx' mo¿e zwolniæ tylko proces, który go zaj¹³
// zwraca E_SUCCESS lub E_TIMEOUT
              unsigned mtx_give( mtx_id mtx );

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

#include <string.h>

// definicja klasy mechanizmu wzajemnego wykluczenia

class Mutex : public mtx_t
{
public:

	 Mutex( unsigned _type = mtxRecursive|mtxPriorityInheritance ): mtx_t(_MTX_INIT(_type)) {}

	~Mutex( void ) { mtx_kill(this); }

	unsigned waitUntil( unsigned _time  ) { return mtx_waitUntil(this, _time);  }
	unsigned waitFor  ( unsigned _delay ) { return mtx_waitFor  (this, _delay); }
	unsigned wait     ( void )            { return mtx_wait     (this);         }
	unsigned take     ( void )            { return mtx_take     (this);         }
	unsigned give     ( void )            { return mtx_give     (this);         }
};

#endif

/* -------------------------------------------------------------------------- */

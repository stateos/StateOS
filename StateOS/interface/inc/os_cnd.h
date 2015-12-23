/******************************************************************************

    @file    State Machine OS: os_cnd.h
    @author  Rajmund Szymanski
    @date    23.12.2015
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
#include <os_mtx.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

#define cndOne ( false )
#define cndAll ( true  )

/* -------------------------------------------------------------------------- */

// deklaracja 'cnd'

#define OS_CND( cnd )                         \
               cnd_t cnd##__cnd = _CND_INIT(); \
               cnd_id cnd = & cnd##__cnd

#define static_CND( cnd )                     \
        static cnd_t cnd##__cnd = _CND_INIT(); \
        static cnd_id cnd = & cnd##__cnd

/* -------------------------------------------------------------------------- */

// utworzenie obiektu typu zmienna warunkowa
// zwraca adres utworzonego obiektu, lub 0
              cnd_id   cnd_create( void );

// reset obiektu 'cnd'
// wszystkie procesy oczekuj¹ce zostaj¹ wybudzone
// zostaje do nich wys³any komunikat E_STOPPED
              void     cnd_kill( cnd_id cnd );

// zawieszenie wykonywania aktualnego procesu do czasu 'time'
// lub do wybudzenia przez obiekt 'cnd' i zajêcia obiektu 'mtx'
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned cnd_waitUntil( cnd_id cnd, mtx_id mtx, unsigned time );

// zawieszenie wykonywania aktualnego procesu na czas 'delay'
// lub do wybudzenia przez obiekt 'cnd' i zajêcia obiektu 'mtx'
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned cnd_waitFor( cnd_id cnd, mtx_id mtx, unsigned delay );

// zawieszenie wykonywania aktualnego procesu
// do czasu przez obiekt 'cnd' i zajêcia obiektu 'mtx'
// zwraca E_SUCCESS lub E_STOPPED
static inline unsigned cnd_wait( cnd_id cnd, mtx_id mtx ) { return cnd_waitFor(cnd, mtx, INFINITE); }

// uwolnienie jednego lub wszytkich ('all') obiektów oczekuj¹cych
              void     cnd_give   ( cnd_id cnd, bool all );
static inline void     cnd_giveISR( cnd_id cnd, bool all ) { cnd_give(cnd, all); }

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

#include <string.h>

// definicja klasy zdarzenia

class ConditionVariable : public cnd_t
{
public:

	 ConditionVariable( void ): cnd_t(_CND_INIT()) {}

	~ConditionVariable( void ) { cnd_kill(this); }

	unsigned waitUntil( mtx_id _mtx, unsigned _time  ) { return cnd_waitUntil(this, _mtx, _time);  }
	unsigned waitFor  ( mtx_id _mtx, unsigned _delay ) { return cnd_waitFor  (this, _mtx, _delay); }
	unsigned wait     ( mtx_id _mtx )                  { return cnd_wait     (this, _mtx);         }
	void     give     ( bool   _all = cndAll )         {        cnd_give     (this, _all);         }
	void     giveISR  ( bool   _all = cndAll )         {        cnd_giveISR  (this, _all);         }
};

#endif

/* -------------------------------------------------------------------------- */

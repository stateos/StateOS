/******************************************************************************

    @file    State Machine OS: os_bar.h
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

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

// deklaracja bariery 'bar'

#define OS_BAR( bar, limit )                       \
               bar_t bar##__bar = _BAR_INIT(limit); \
               bar_id bar = & bar##__bar

#define static_BAR( bar, limit )                   \
        static bar_t bar##__bar = _BAR_INIT(limit); \
        static bar_id bar = & bar##__bar

/* -------------------------------------------------------------------------- */

// utworzenie obiektu typu bariera
// zwraca adres utworzonego obiektu, lub 0
              bar_id   bar_create( unsigned limit );

// reset obiektu 'bar'
// wszystkie procesy oczekujace zostaja wybudzone
// zostaje do nich wyslany komunikat E_STOPPED
              void     bar_kill( bar_id bar );

// zawieszenie wykonywania aktualnego procesu do czasu 'time'
// lub do wybudzenia przez obiekt 'bar'
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned bar_waitUntil( bar_id bar, unsigned time );

// zawieszenie wykonywania aktualnego procesu na czas 'delay'
// lub do wybudzenia przez obiekt 'bar'
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned bar_waitFor( bar_id bar, unsigned delay );

// zawieszenie wykonywania aktualnego procesu
// do czasu wybudzenia przez obiekt 'bar'
// zwraca E_SUCCESS lub E_STOPPED
static inline unsigned bar_wait( bar_id bar ) { return bar_waitFor(bar, INFINITE); }

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

#include <string.h>

// definicja klasy bariery

class Barrier : public bar_t
{
public:

	 Barrier( unsigned _limit ): bar_t(_BAR_INIT(_limit)) {}

	~Barrier( void ) { bar_kill(this); }

	unsigned waitUntil( unsigned _time  ) { return bar_waitUntil(this, _time);  }
	unsigned waitFor  ( unsigned _delay ) { return bar_waitFor  (this, _delay); }
	unsigned wait     ( void )            { return bar_wait     (this);         }
};

#endif

/* -------------------------------------------------------------------------- */

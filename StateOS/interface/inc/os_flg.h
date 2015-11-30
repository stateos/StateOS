/******************************************************************************

    @file    State Machine OS: os_flg.h
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

#define flgOne       ( 0U << 0 )
#define flgAll       ( 1U << 0 )
#define flgClear     ( 0U << 1 )
#define flgAccept    ( 1U << 1 )

#define flgOneClear  ( flgOne | flgClear  )
#define flgAllClear  ( flgAll | flgClear  )
#define flgOneAccept ( flgOne | flgAccept )
#define flgAllAccept ( flgAll | flgAccept )

/* -------------------------------------------------------------------------- */

// deklaracja flagi 'flg'

#define OS_FLG( flg, mask )                       \
               flg_t flg##__flg = _FLG_INIT(mask); \
               flg_id flg = & flg##__flg

#define static_FLG( flg, mask )                   \
        static flg_t flg##__flg = _FLG_INIT(mask); \
        static flg_id flg = & flg##__flg

/* -------------------------------------------------------------------------- */

// utworzenie obiektu typu flaga (flg)
// 'mask' zabezpiecza flagi, które nie mog¹ byæ kasowane (mog¹ byæ tylko ustawiane)
// zwraca adres utworzonego obiektu, lub 0
flg_id   flg_create( unsigned mask );

// reset obiektu 'flg'
// wszystkie procesy oczekuj¹ce zostaj¹ wybudzone
// zostaje do nich wys³any komunikat E_STOPPED
void     flg_kill( flg_id flg );

// zawieszenie wykonywania aktualnego procesu do czasu 'time'
// lub do wybudzenia przez oczekiwan¹ flagê lub wszystkie flagi (all)
// all: flgOne, flgAll
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
unsigned flg_waitUntil( flg_id flg, unsigned flags, unsigned mode, unsigned time );

// zawieszenie wykonywania aktualnego procesu na czas 'delay'
// lub do wybudzenia przez oczekiwan¹ flagê lub wszystkie flagi (all)
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
unsigned flg_waitFor( flg_id flg, unsigned flags, unsigned mode, unsigned delay );

// zawieszenie wykonywania aktualnego procesu
// do czasu wybudzenia przez oczekiwan¹ flagê lub wszystkie flagi (all)
// zwraca E_SUCCESS lub E_STOPPED
static inline
unsigned flg_wait( flg_id flg, unsigned flags, unsigned mode ) { return flg_waitFor(flg, flags, mode, INFINITE); }

// nie zawieszenie wykonywania aktualnego procesu
// sprawdza oczekiwan¹ flagê lub wszystkie flagi (all)
// zwraca E_SUCCESS lub E_TIMEOUT
static inline
unsigned flg_take( flg_id flg, unsigned flags, unsigned mode ) { return flg_waitFor(flg, flags, mode, IMMEDIATE); }

static inline
unsigned flg_takeISR( flg_id flg, unsigned flags, unsigned mode ) { return flg_waitFor(flg, flags, mode, IMMEDIATE); }

// wysy³a flagê lub flagi do obiektu 'flg'
void     flg_give( flg_id flg, unsigned flags );

static inline
void     flg_giveISR( flg_id flg, unsigned flags ) { flg_give(flg, flags); }

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

#include <string.h>

// definicja klasy flagi

class Flag : public flg_t
{
public:

	 Flag( unsigned _mask = 0 ): flg_t(_FLG_INIT(_mask)) {}

	~Flag( void ) { flg_kill(this); }

	unsigned waitUntil( unsigned _flags, bool _mode, unsigned _time )    { return flg_waitUntil(this, _flags, _mode, _time);  }
	unsigned waitFor  ( unsigned _flags, bool _mode, unsigned _delay )   { return flg_waitFor  (this, _flags, _mode, _delay); }
	unsigned wait     ( unsigned _flags, bool _mode = flgAll+flgAccept ) { return flg_wait     (this, _flags, _mode);         }
	void     give     ( unsigned _flags )                                {        flg_give     (this, _flags);                }
	void     giveISR  ( unsigned _flags )                                {        flg_giveISR  (this, _flags);                }
};

#endif

/* -------------------------------------------------------------------------- */

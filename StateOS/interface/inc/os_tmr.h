/******************************************************************************

    @file    State Machine OS: os_tmr.h
    @author  Rajmund Szymanski
    @date    08.01.2016
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

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : timer                                                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_TMR                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a timer object                                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : name of a pointer to timer object                                                              *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define     OS_TMR( tmr )                     \
               tmr_t tmr##__tmr = _TMR_INIT(); \
               tmr_id tmr = & tmr##__tmr

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_TMR                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static timer object                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : name of a pointer to timer object                                                              *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define static_TMR( tmr )                     \
        static tmr_t tmr##__tmr = _TMR_INIT(); \
        static tmr_id tmr = & tmr##__tmr

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_create                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a new timer object                                                        *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to timer object (timer successfully created)                                           *
 *   0               : timer not created (not enough free memory)                                                     *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              tmr_id   tmr_create( void );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_kill                                                                                       *
 *                                                                                                                    *
 * Description       : reset the timer object and wake up all waiting tasks with 'E_STOPPED' event value              *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     tmr_kill( tmr_id tmr );

// uruchomienie zegara 'tmr'
// o czasie 'time' wszystkie procesy oczekuj¹ce zostaj¹ wybudzone
// i zostanie wykonana procedura 'proc' (jeœli 'proc' != 0)
              void     tmr_startUntil( tmr_id tmr, unsigned time, fun_id proc );

// uruchomienie zegara 'tmr'
// po czasie 'delay' wszystkie procesy oczekuj¹ce zostaj¹ wybudzone
// i zostanie wykonana procedura 'proc' (jeœli 'proc' != 0)
              void     tmr_startFor( tmr_id tmr, unsigned delay, fun_id proc );

// uruchomienie zegara 'tmr' z okresem 'period'
// po czasie 'period' wszystkie procesy oczekuj¹ce zostaj¹ wybudzone
// i zostanie wykonana procedura 'proc' (jeœli 'proc' != 0)
              void     tmr_startPeriodic( tmr_id tmr, unsigned period, fun_id proc );

// zawieszenie wykonywania aktualnego procesu do czas 'time'
// lub do wybudzenia przez obiekt 'tmr'
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned tmr_waitUntil( tmr_id tmr, unsigned time );

// zawieszenie wykonywania aktualnego procesu na czas 'delay'
// lub do wybudzenia przez obiekt 'tmr'
// zwraca E_SUCCESS, E_STOPPED lub E_TIMEOUT
              unsigned tmr_waitFor( tmr_id tmr, unsigned delay );

// zawieszenie wykonywania aktualnego procesu
// do czasu wybudzenia przez obiekt 'tmr'
// zwraca E_SUCCESS lub E_STOPPED
static inline unsigned tmr_wait( tmr_id tmr ) { return tmr_waitFor(tmr, INFINITE); }

// czy zegar zakoñczy³ odliczanie?
// zwraca E_SUCCESS, E_TIMEOUT
static inline unsigned tmr_take   ( tmr_id tmr ) { return tmr_waitFor(tmr, IMMEDIATE); }
static inline unsigned tmr_takeISR( tmr_id tmr ) { return tmr_waitFor(tmr, IMMEDIATE); }

// zmiana procedury aktualnie uruchomionego zegara
// funkcja mo¿e byc wywo³ana tylko i wy³¹cznie z wnêtrza procedury zegara
static inline void     tmr_flipISR( fun_id proc ) { System.tmr->state = proc; }

// zmiana parametrów czasowych aktualnie uruchomionego zegara
// funkcja mo¿e byc wywo³ana tylko i wy³¹cznie z wnêtrza procedury zegara
// (delay == 0) => zegar zostanie zatrzymany
static inline void     tmr_delayISR( unsigned delay ) { System.tmr->delay = delay; }

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

#include <string.h>

class Timer : public tmr_t
{
public:

	 Timer( fun_id _state = 0 ): tmr_t(_TMR_INIT()) { state = _state; }

	~Timer( void ) { tmr_kill(this); }

	void startUntil   ( unsigned _time   )                {        tmr_startUntil   (this, _time,   this->state); }
	void startUntil   ( unsigned _time,   fun_id _state ) {        tmr_startUntil   (this, _time,        _state); }
	void startFor     ( unsigned _delay  )                {        tmr_startFor     (this, _delay,  this->state); }
	void startFor     ( unsigned _delay,  fun_id _state ) {        tmr_startFor     (this, _delay,       _state); }
	void startPeriodic( unsigned _period )                {        tmr_startPeriodic(this, _period, this->state); }
	void startPeriodic( unsigned _period, fun_id _state ) {        tmr_startPeriodic(this, _period,      _state); }

	unsigned waitUntil( unsigned _time  )                 { return tmr_waitUntil    (this, _time);                }
	unsigned waitFor  ( unsigned _delay )                 { return tmr_waitFor      (this, _delay);               }
	unsigned wait     ( void )                            { return tmr_wait         (this);                       }
	unsigned take     ( void )                            { return tmr_take         (this);                       }
	unsigned takeISR  ( void )                            { return tmr_takeISR      (this);                       }
};

#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

namespace ThisTimer
{
	void flipISR      ( fun_id   _state )                 {        tmr_flipISR            (_state);               }
	void delayISR     ( unsigned _delay )                 {        tmr_delayISR           (_delay);               }
};

#endif

/* -------------------------------------------------------------------------- */

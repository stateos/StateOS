/******************************************************************************

    @file    State Machine OS: os_tmr.h
    @author  Rajmund Szymanski
    @date    11.01.2016
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

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_startUntil                                                                                 *
 *                                                                                                                    *
 * Description       : start/restart one-shot timer until given timepoint and then launch the callback procedure      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *   time            : timepoint value                                                                                *
 *   proc            : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     tmr_startUntil( tmr_id tmr, unsigned time, fun_id proc );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_startFor                                                                                   *
 *                                                                                                                    *
 * Description       : start/restart one-shot timer for given duration of time and then launch the callback procedure *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *   delay           : duration of time (maximum number of ticks to countdownd)                                       *
 *                     IMMEDIATE: don't countdown                                                                     *
 *                     INFINITE:  countdown indefinitly                                                               *
 *   proc            : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     tmr_startFor( tmr_id tmr, unsigned delay, fun_id proc );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_startPeriodic                                                                              *
 *                                                                                                                    *
 * Description       : start/restart periodic timer for given duration of time and then launch the callback procedure *
 *                     do this periodically                                                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *   period          : duration of time (maximum number of ticks to countdownd)                                       *
 *                     IMMEDIATE: don't countdown                                                                     *
 *                     INFINITE:  countdown indefinitly                                                               *
 *   proc            : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     tmr_startPeriodic( tmr_id tmr, unsigned period, fun_id proc );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_waitUntil                                                                                  *
 *                                                                                                                    *
 * Description       : wait until given timepoint until the timer finishes countdown                                  *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : timer object successfully finished countdown                                                   *
 *   E_STOPPED       : timer object was killed before the specified timeout expired                                   *
 *   E_TIMEOUT       : timer object has not finished countdown before the specified timeout expired                   *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned tmr_waitUntil( tmr_id tmr, unsigned time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_waitFor                                                                                    *
 *                                                                                                                    *
 * Description       : wait for given duration of time until the timer finishes countdown                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *   delay           : duration of time (maximum number of ticks to wait for the timer finishes countdown)            *
 *                     IMMEDIATE: don't wait for the timer finishes countdown                                         *
 *                     INFINITE:  wait indefinitly until the timer finishes countdown                                 *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : timer object successfully finished countdown                                                   *
 *   E_STOPPED       : timer object was killed before the specified timeout expired                                   *
 *   E_TIMEOUT       : timer object has not finished countdown before the specified timeout expired                   *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned tmr_waitFor( tmr_id tmr, unsigned delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_wait                                                                                       *
 *                                                                                                                    *
 * Description       : wait indefinitly until the timer finishes countdown                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : timer object successfully finished countdown                                                   *
 *   E_STOPPED       : timer object was killed before the specified timeout expired                                   *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned tmr_wait( tmr_id tmr ) { return tmr_waitFor(tmr, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_take                                                                                       *
 *                                                                                                                    *
 * Description       : check if the timer finishes countdown                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : timer object successfully finished countdown                                                   *
 *   E_TIMEOUT       : timer object has not yet completed counting                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned tmr_take   ( tmr_id tmr ) { return tmr_waitFor(tmr, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_takeISR                                                                                    *
 *                                                                                                                    *
 * Description       : check if the timer finishes countdown                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : timer object successfully finished countdown                                                   *
 *   E_TIMEOUT       : timer object has not yet completed counting                                                    *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned tmr_takeISR( tmr_id tmr ) { return tmr_waitFor(tmr, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_flipISR                                                                                    *
 *                                                                                                                    *
 * Description       : change callback procedure for current periodic timer (available in next period)                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   proc            : new callback procedure                                                                         *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in timer callback procedure                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline void     tmr_flipISR( fun_id proc ) { System.tmr->state = proc; }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_delayISR                                                                                   *
 *                                                                                                                    *
 * Description       : change delay time for current timer                                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   delay           : duration of time (maximum number of ticks to countdownd)                                       *
 *                     0:         current timer is stopped even if it was periodic                                    *
 *                     otherwise: current timer is restarted even if it was one-shot                                  *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in timer callback procedure                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline void     tmr_delayISR( unsigned delay ) { System.tmr->delay = delay; }

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

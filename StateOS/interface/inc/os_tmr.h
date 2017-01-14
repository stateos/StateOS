/******************************************************************************

    @file    StateOS: os_tmr.h
    @author  Rajmund Szymanski
    @date    13.01.2017
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

#ifndef __STATEOS_TMR_H
#define __STATEOS_TMR_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : timer                                                                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct __tmr
{
	obj_t    obj;   // object header

	fun_t  * state; // callback procedure
	unsigned start;
	unsigned delay;
	unsigned period;
};

typedef struct __tmr tmr_id[1];

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _TMR_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a timer object                                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   state           : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 * Return            : timer object                                                                                   *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _TMR_INIT( _state ) { { 0, 0, 0, 0 }, _state, 0, 0, 0 }

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

#define             OS_TMR( tmr ) \
                       tmr_id tmr = { _TMR_INIT( 0 ) }

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

#define         static_TMR( tmr ) \
                static tmr_id tmr = { _TMR_INIT( 0 ) }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : TMR_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a timer object                                                            *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : timer object                                                                                   *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                TMR_INIT() \
                      _TMR_INIT( 0 )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : TMR_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a timer object                                                            *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to timer object                                                                        *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                TMR_CREATE() \
                     { TMR_INIT( 0 ) }
#endif

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

              tmr_t  * tmr_create( void );

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

              void     tmr_kill( tmr_t *tmr );

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

              void     tmr_startUntil( tmr_t *tmr, unsigned time, fun_t *proc );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_start                                                                                      *
 *                                                                                                                    *
 * Description       : start/restart periodic timer for given duration of time and then launch the callback procedure *
 *                     do this periodically if period > 0                                                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration                   *
 *                     IMMEDIATE: don't countdown                                                                     *
 *                     INFINITE:  countdown indefinitly                                                               *
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations               *
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

              void     tmr_start( tmr_t *tmr, unsigned delay, unsigned period, fun_t *proc );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_startFor                                                                                   *
 *                                                                                                                    *
 * Description       : start/restart one-shot timer for given duration of time and then launch the callback procedure *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *   delay           : duration of time (maximum number of ticks to countdown)                                        *
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

static inline void     tmr_startFor( tmr_t *tmr, unsigned delay, fun_t *proc ) { tmr_start(tmr, delay, 0, proc); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_startPeriodic                                                                              *
 *                                                                                                                    *
 * Description       : start/restart periodic timer for given duration of time and then launch the callback procedure *
 *                     do this periodically                                                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *   period          : duration of time (maximum number of ticks to countdown)                                        *
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

static inline void     tmr_startPeriodic( tmr_t *tmr, unsigned period, fun_t *proc ) { tmr_start(tmr, period, period, proc); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_stop                                                                                       *
 *                                                                                                                    *
 * Description       : stop timer                                                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline void     tmr_stop( tmr_t *tmr ) { tmr_start(tmr, 0, 0, 0); }

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

              unsigned tmr_waitUntil( tmr_t *tmr, unsigned time );

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

              unsigned tmr_waitFor( tmr_t *tmr, unsigned delay );

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

static inline unsigned tmr_wait( tmr_t *tmr ) { return tmr_waitFor(tmr, INFINITE); }

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

static inline unsigned tmr_take( tmr_t *tmr ) { return tmr_waitFor(tmr, IMMEDIATE); }

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

static inline unsigned tmr_takeISR( tmr_t *tmr ) { return tmr_waitFor(tmr, IMMEDIATE); }

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

              void     tmr_flipISR( fun_t *proc );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_delayISR                                                                                   *
 *                                                                                                                    *
 * Description       : change delay time for current timer                                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   delay           : duration of time (maximum number of ticks to countdown)                                        *
 *                     0:         current timer is stopped even if it was periodic                                    *
 *                     otherwise: current timer is restarted even if it was one-shot                                  *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in timer callback procedure                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     tmr_delayISR( unsigned delay );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Namespace         : ThisTimer                                                                                      *
 *                                                                                                                    *
 * Description       : provide set of functions for Current Timer                                                     *
 *                                                                                                                    *
 **********************************************************************************************************************/

namespace ThisTimer
{
	void flipISR ( fun_t  * _state ) { tmr_flipISR (_state); }
	void delayISR( unsigned _delay ) { tmr_delayISR(_delay); }
}

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : Timer                                                                                          *
 *                                                                                                                    *
 * Description       : create and initilize a timer object                                                            *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *                   : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct Timer : public __tmr
{
	explicit
	 Timer( fun_t *_state = nullptr ): __tmr _TMR_INIT(0) { state = _state; }
	~Timer( void ) { assert(obj.id == ID_STOPPED); }

	void kill         ( void )                                             {        tmr_kill         (this);                               }
	void startUntil   ( unsigned _time )                                   {        tmr_startUntil   (this, _time,           this->state); }
	void startUntil   ( unsigned _time,                    fun_t *_state ) {        tmr_startUntil   (this, _time,                _state); }
	void start        ( unsigned _delay, unsigned _period )                {        tmr_start        (this, _delay, _period, this->state); }
	void start        ( unsigned _delay, unsigned _period, fun_t *_state ) {        tmr_start        (this, _delay, _period,      _state); }
	void startFor     ( unsigned _delay )                                  {        tmr_startFor     (this, _delay,          this->state); }
	void startFor     ( unsigned _delay,                   fun_t *_state ) {        tmr_startFor     (this, _delay,               _state); }
	void startPeriodic( unsigned _period )                                 {        tmr_startPeriodic(this,         _period, this->state); }
	void startPeriodic( unsigned _period,                  fun_t *_state ) {        tmr_startPeriodic(this,         _period,      _state); }
	void stop         ( void )                                             {        tmr_stop         (this);                               }

	unsigned waitUntil( unsigned _time )                                   { return tmr_waitUntil    (this, _time);                        }
	unsigned waitFor  ( unsigned _delay )                                  { return tmr_waitFor      (this, _delay);                       }
	unsigned wait     ( void )                                             { return tmr_wait         (this);                               }
	unsigned take     ( void )                                             { return tmr_take         (this);                               }
	unsigned takeISR  ( void )                                             { return tmr_takeISR      (this);                               }

	bool     operator!( void )                                             { return __tmr::obj.id == ID_STOPPED;                           }
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : startTimerUntil                                                                                *
 *                                                                                                                    *
 * Description       : create and initilize a timer object                                                            *
 *                     and start one-shot timer until given timepoint and then launch the callback procedure          *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   time            : timepoint value                                                                                *
 *   state           : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct startTimerUntil : public Timer
{
	explicit
	startTimerUntil( const unsigned _time, fun_t *_state ): Timer() { tmr_startUntil(this, _time, _state); }
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : startTimer                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a timer object                                                            *
 *                     and start periodic timer for given duration of time and then launch the callback procedure     *
 *                     do this periodically                                                                           *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration                   *
 *                     IMMEDIATE: don't countdown                                                                     *
 *                     INFINITE:  countdown indefinitly                                                               *
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations               *
 *                     IMMEDIATE: don't countdown                                                                     *
 *                     INFINITE:  countdown indefinitly                                                               *
 *   state           : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct startTimer : public Timer
{
	explicit
	startTimer( const unsigned _delay, const unsigned _period, fun_t *_state ): Timer() { tmr_start(this, _delay, _period, _state); }
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : startTimerFor                                                                                  *
 *                                                                                                                    *
 * Description       : create and initilize a timer object                                                            *
 *                     and start one-shot timer for given duration of time and then launch the callback procedure     *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   delay           : duration of time (maximum number of ticks to countdown)                                        *
 *                     IMMEDIATE: don't countdown                                                                     *
 *                     INFINITE:  countdown indefinitly                                                               *
 *   state           : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct startTimerFor : public Timer
{
	explicit
	startTimerFor( const unsigned _delay, fun_t *_state ): Timer() { tmr_startFor(this, _delay, _state); }
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : startTimerPeriodic                                                                             *
 *                                                                                                                    *
 * Description       : create and initilize a timer object                                                            *
 *                     and start periodic timer for given duration of time and then launch the callback procedure     *
 *                     do this periodically                                                                           *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   period          : duration of time (maximum number of ticks to countdown)                                        *
 *                     IMMEDIATE: don't countdown                                                                     *
 *                     INFINITE:  countdown indefinitly                                                               *
 *   state           : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct startTimerPeriodic : public Timer
{
	explicit
	startTimerPeriodic( const unsigned _period, fun_t *_state ): Timer() { tmr_startPeriodic(this, _period, _state); }
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_TMR_H

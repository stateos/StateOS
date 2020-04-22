/******************************************************************************

    @file    StateOS: ostimer.h
    @author  Rajmund Szymanski
    @date    22.04.2020
    @brief   This file contains definitions for StateOS.

 ******************************************************************************

   Copyright (c) 2018-2020 Rajmund Szymanski. All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

 ******************************************************************************/

#ifndef __STATEOS_TMR_H
#define __STATEOS_TMR_H

#include "oskernel.h"

/******************************************************************************
 *
 * Name              : timer
 *
 ******************************************************************************/

struct __tmr
{
	hdr_t    hdr;   // timer / task header

	fun_t  * state; // callback procedure
#if OS_FUNCTIONAL
	FUN_t    fun;   // function<void(void)> for internal use in c++ functions
#ifdef     __cplusplus
#define     _FUN_INIT(_state) (fun_t*)_state, { FUN_t() }
#else
#define     _FUN_INIT(_state) (fun_t*)_state, { NULL }
#endif
#else
#define     _FUN_INIT(_state) (fun_t*)_state
#endif
	cnt_t    start;
	cnt_t    delay;
	cnt_t    period;
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _TMR_INIT
 *
 * Description       : create and initialize a timer object
 *
 * Parameters
 *   state           : callback procedure
 *                     0: no callback
 *
 * Return            : timer object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _TMR_INIT( _state ) { _HDR_INIT(), _FUN_INIT(_state), 0, 0, 0 }

/******************************************************************************
 *
 * Name              : OS_TMR
 *
 * Description       : define and initialize a timer object
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   state           : callback procedure
 *                     0: no callback
 *
 ******************************************************************************/

#define             OS_TMR( tmr, state )                     \
                       tmr_t tmr##__tmr = _TMR_INIT( state ); \
                       tmr_id tmr = & tmr##__tmr

/******************************************************************************
 *
 * Name              : OS_TMR_DEF
 *
 * Description       : define and initialize complete timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *
 ******************************************************************************/

#define             OS_TMR_DEF( tmr )                            \
                       void tmr##__fun( void );                   \
                       tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun ); \
                       tmr_id tmr = & tmr##__tmr;                   \
                       void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : OS_TMR_START
 *
 * Description       : define, initialize and start complete timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define             OS_TMR_START( tmr, delay, period )                                           \
                       void tmr##__fun( void );                                                   \
                       tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );                                 \
                       tmr_id tmr = & tmr##__tmr;                                                   \
         __CONSTRUCTOR void tmr##__start( void ) { port_sys_init(); tmr_start(tmr, delay, period); } \
                       void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : OS_TMR_UNTIL
 *
 * Description       : define, initialize and start complete timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   time            : timepoint value
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define             OS_TMR_UNTIL( tmr, time )                                                \
                       void tmr##__fun( void );                                               \
                       tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );                             \
                       tmr_id tmr = & tmr##__tmr;                                               \
         __CONSTRUCTOR void tmr##__start( void ) { port_sys_init(); tmr_startUntil(tmr, time); } \
                       void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : static_TMR
 *
 * Description       : define and initialize static timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   state           : callback procedure
 *                     0: no callback
 *
 ******************************************************************************/

#define         static_TMR( tmr, state )                     \
                static tmr_t tmr##__tmr = _TMR_INIT( state ); \
                static tmr_id tmr = & tmr##__tmr

/******************************************************************************
 *
 * Name              : static_TMR_DEF
 *
 * Description       : define and initialize static timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *
 ******************************************************************************/

#define         static_TMR_DEF( tmr )                            \
                static void tmr##__fun( void );                   \
                static tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun ); \
                static tmr_id tmr = & tmr##__tmr;                   \
                static void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : static_TMR_START
 *
 * Description       : define, initialize and start static timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define         static_TMR_START( tmr, delay, period )                                           \
                static void tmr##__fun( void );                                                   \
                static tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );                                 \
                static tmr_id tmr = & tmr##__tmr;                                                   \
  __CONSTRUCTOR static void tmr##__start( void ) { port_sys_init(); tmr_start(tmr, delay, period); } \
                static void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : static_TMR_UNTIL
 *
 * Description       : define, initialize and start static timer object
 *                     timer callback procedure (function body) must be defined immediately below
 *
 * Parameters
 *   tmr             : name of a pointer to timer object
 *   time            : timepoint value
 *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent
 *
 ******************************************************************************/

#define         static_TMR_UNTIL( tmr, time )                                                \
                static void tmr##__fun( void );                                               \
                static tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );                             \
                static tmr_id tmr = & tmr##__tmr;                                               \
  __CONSTRUCTOR static void tmr##__start( void ) { port_sys_init(); tmr_startUntil(tmr, time); } \
                static  void tmr##__fun( void )

/******************************************************************************
 *
 * Name              : TMR_INIT
 *
 * Description       : create and initialize a timer object
 *
 * Parameters
 *   state           : callback procedure
 *                     0: no callback
 *
 * Return            : timer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                TMR_INIT( state ) \
                      _TMR_INIT( state )
#endif

/******************************************************************************
 *
 * Name              : TMR_CREATE
 * Alias             : TMR_NEW
 *
 * Description       : create and initialize a timer object
 *
 * Parameters
 *   state           : callback procedure
 *                     0: no callback
 *
 * Return            : pointer to timer object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                TMR_CREATE( state ) \
           (tmr_t[]) { TMR_INIT  ( state ) }
#define                TMR_NEW \
                       TMR_CREATE
#endif

/******************************************************************************
 *
 * Name              : tmr_thisISR
 *
 * Description       : return current timer object
 *
 * Parameters        : none
 *
 * Return            : current timer object
 *
 * Note              : use only in timer callback procedure
 *
 ******************************************************************************/

__STATIC_INLINE
tmr_t *tmr_thisISR( void ) { return (tmr_t *) WAIT.hdr.next; }

/******************************************************************************
 *
 * Name              : tmr_init
 *
 * Description       : initialize a timer object
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   state           : callback procedure
 *                     0: no callback
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tmr_init( tmr_t *tmr, fun_t *state );

/******************************************************************************
 *
 * Name              : tmr_create
 * Alias             : tmr_new
 *
 * Description       : create and initialize a new timer object
 *
 * Parameters
 *   state           : callback procedure
 *                     0: no callback
 *
 * Return            : pointer to timer object (timer successfully created)
 *   0               : timer not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

tmr_t *tmr_create( fun_t *state );

__STATIC_INLINE
tmr_t *tmr_new( fun_t *state ) { return tmr_create(state); }

/******************************************************************************
 *
 * Name              : tmr_reset
 * Alias             : tmr_kill
 *
 * Description       : reset the timer object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   tmr             : pointer to timer object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tmr_reset( tmr_t *tmr );

__STATIC_INLINE
void tmr_kill( tmr_t *tmr ) { tmr_reset(tmr); }

/******************************************************************************
 *
 * Name              : tmr_destroy
 * Alias             : tmr_delete
 *
 * Description       : reset the timer object, wake up all waiting tasks with 'E_DELETED' event value and free allocated resource
 *
 * Parameters
 *   tmr             : pointer to timer object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tmr_destroy( tmr_t *tmr );

__STATIC_INLINE
void tmr_delete( tmr_t *tmr ) { tmr_destroy(tmr); }

/******************************************************************************
 *
 * Name              : tmr_start
 *
 * Description       : start/restart periodic timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *                     do this periodically if period > 0
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tmr_start( tmr_t *tmr, cnt_t delay, cnt_t period );

/******************************************************************************
 *
 * Name              : tmr_startFor
 *
 * Description       : start/restart one-shot timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown)
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
void tmr_startFor( tmr_t *tmr, cnt_t delay ) { tmr_start(tmr, delay, 0); }

/******************************************************************************
 *
 * Name              : tmr_startPeriodic
 *
 * Description       : start/restart periodic timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *                     do this periodically
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   period          : duration of time (maximum number of ticks to countdown)
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
void tmr_startPeriodic( tmr_t *tmr, cnt_t period ) { tmr_start(tmr, period, period); }

/******************************************************************************
 *
 * Name              : tmr_startFrom
 *
 * Description       : start/restart periodic timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *                     do this periodically if period > 0
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   proc            : callback procedure
 *                     0: no callback
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tmr_startFrom( tmr_t *tmr, cnt_t delay, cnt_t period, fun_t *proc );

/******************************************************************************
 *
 * Name              : tmr_startNext
 *
 * Description       : restart the timer for given duration of time from the end of the previous countdown
 *                     when the timer has finished the countdown, the callback procedure is launched
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   delay           : duration of time (maximum number of ticks to countdown)
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tmr_startNext( tmr_t *tmr, cnt_t delay );

/******************************************************************************
 *
 * Name              : tmr_startUntil
 *
 * Description       : start/restart one-shot timer until given timepoint
 *                     when the timer has finished the countdown, the callback procedure is launched
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   time            : timepoint value
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void tmr_startUntil( tmr_t *tmr, cnt_t time );

/******************************************************************************
 *
 * Name              : tmr_stop
 *
 * Description       : stop timer
 *
 * Parameters
 *   tmr             : pointer to timer object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
void tmr_stop( tmr_t *tmr ) { tmr_start(tmr, 0, 0); }

/******************************************************************************
 *
 * Name              : tmr_take
 * Alias             : tmr_tryWait
 * ISR alias         : tmr_takeISR
 *
 * Description       : check if the timer finishes countdown
 *
 * Parameters
 *   tmr             : pointer to timer object
 *
 * Return
 *   E_SUCCESS       : timer object successfully finished countdown
 *   E_FAILURE       : timer has not yet been started
 *   E_TIMEOUT       : timer object has not yet completed counting, try again
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned tmr_take( tmr_t *tmr );

__STATIC_INLINE
unsigned tmr_tryWait( tmr_t *tmr ) { return tmr_take(tmr); }

__STATIC_INLINE
unsigned tmr_takeISR( tmr_t *tmr ) { return tmr_take(tmr); }

/******************************************************************************
 *
 * Name              : tmr_waitFor
 *
 * Description       : wait for given duration of time until the timer finishes countdown
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   delay           : duration of time (maximum number of ticks to wait for the timer finishes countdown)
 *                     IMMEDIATE: don't wait for the timer finishes countdown
 *                     INFINITE:  wait indefinitely until the timer finishes countdown
 *
 * Return
 *   E_SUCCESS       : timer object successfully finished countdown
 *   E_FAILURE       : timer has not yet been started
 *   E_STOPPED       : timer object was reseted before the specified timeout expired
 *   E_DELETED       : timer object was deleted before the specified timeout expired
 *   E_TIMEOUT       : timer object has not finished countdown before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned tmr_waitFor( tmr_t *tmr, cnt_t delay );

/******************************************************************************
 *
 * Name              : tmr_waitNext
 *
 * Description       : wait for given duration of time from the end of the previous countdown
 *                     until the timer finishes countdown
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   delay           : duration of time (maximum number of ticks to wait for the timer finishes countdown)
 *                     IMMEDIATE: don't wait for the timer finishes countdown
 *                     INFINITE:  wait indefinitely until the timer finishes countdown
 *
 * Return
 *   E_SUCCESS       : timer object successfully finished countdown
 *   E_FAILURE       : timer has not yet been started
 *   E_STOPPED       : timer object was reseted before the specified timeout expired
 *   E_DELETED       : timer object was deleted before the specified timeout expired
 *   E_TIMEOUT       : timer object has not finished countdown before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned tmr_waitNext( tmr_t *tmr, cnt_t delay );

/******************************************************************************
 *
 * Name              : tmr_waitUntil
 *
 * Description       : wait until given timepoint until the timer finishes countdown
 *
 * Parameters
 *   tmr             : pointer to timer object
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : timer object successfully finished countdown
 *   E_FAILURE       : timer has not yet been started
 *   E_STOPPED       : timer object was reseted before the specified timeout expired
 *   E_DELETED       : timer object was deleted before the specified timeout expired
 *   E_TIMEOUT       : timer object has not finished countdown before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned tmr_waitUntil( tmr_t *tmr, cnt_t time );

/******************************************************************************
 *
 * Name              : tmr_wait
 *
 * Description       : wait indefinitely until the timer finishes countdown
 *
 * Parameters
 *   tmr             : pointer to timer object
 *
 * Return
 *   E_SUCCESS       : timer object successfully finished countdown
 *   E_FAILURE       : timer has not yet been started
 *   E_STOPPED       : timer object was reseted before the countdown ended
 *   E_DELETED       : timer object was deleted before the countdown ended
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned tmr_wait( tmr_t *tmr ) { return tmr_waitFor(tmr, INFINITE); }

/******************************************************************************
 *
 * Name              : tmr_flipISR
 *
 * Description       : change callback procedure for current periodic timer (available in next period)
 *
 * Parameters
 *   proc            : new callback procedure
 *                     0: no callback
 *
 * Return            : none
 *
 * Note              : use only in timer callback procedure
 *
 ******************************************************************************/

__STATIC_INLINE
void tmr_flipISR( fun_t *proc ) { tmr_thisISR()->state = proc; }

/******************************************************************************
 *
 * Name              : tmr_delayISR
 *
 * Description       : change delay time for current timer
 *
 * Parameters
 *   delay           : duration of time (maximum number of ticks to countdown)
 *                     0:         current timer is stopped even if it was periodic
 *                     otherwise: current timer is restarted even if it was one-shot
 *
 * Return            : none
 *
 * Note              : use only in timer callback procedure
 *
 ******************************************************************************/

__STATIC_INLINE
void tmr_delayISR( cnt_t delay ) { tmr_thisISR()->delay = delay; }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : Timer
 *
 * Description       : create and initialize a timer object
 *
 * Constructor parameters
 *   state           : callback procedure
 *
 ******************************************************************************/

struct Timer : public __tmr
{
	Timer( void ): __tmr _TMR_INIT(NULL) {}
#if OS_FUNCTIONAL
	Timer( FUN_t _state ): __tmr _TMR_INIT(fun_) { __tmr::fun = _state; }
#else
	Timer( FUN_t _state ): __tmr _TMR_INIT(_state) {}
#endif
	Timer( Timer&& ) = default;
	Timer( const Timer& ) = delete;
	Timer& operator=( Timer&& ) = delete;
	Timer& operator=( const Timer& ) = delete;

	~Timer( void ) { assert(__tmr::hdr.id == ID_STOPPED); }

	static
	Timer *create( FUN_t _state )
	{
		Timer *tmr;
		static_assert(sizeof(__tmr) == sizeof(Timer), "unexpected error!");
#if OS_FUNCTIONAL
		tmr = reinterpret_cast<Timer *>(tmr_create(fun_));
		tmr->__tmr::fun = _state;
#else
		tmr = reinterpret_cast<Timer *>(tmr_create(_state));
#endif
		return tmr;
	}

	void reset        ( void )                                      {        tmr_reset        (this);                          }
	void kill         ( void )                                      {        tmr_kill         (this);                          }
	void destroy      ( void )                                      {        tmr_destroy      (this);                          }
	void start        ( cnt_t _delay, cnt_t _period )               {        tmr_start        (this, _delay, _period);         }
	void startFor     ( cnt_t _delay )                              {        tmr_startFor     (this, _delay);                  }
	void startPeriodic( cnt_t _period )                             {        tmr_startPeriodic(this,         _period);         }
#if OS_FUNCTIONAL
	void startFrom    ( cnt_t _delay, cnt_t _period, FUN_t _state ) {        __tmr::fun = _state;
	                                                                         tmr_startFrom    (this, _delay, _period, fun_);   }
#else
	void startFrom    ( cnt_t _delay, cnt_t _period, FUN_t _state ) {        tmr_startFrom    (this, _delay, _period, _state); }
#endif
	void startNext    ( cnt_t _delay )                              {        tmr_startNext    (this, _delay);                  }
	void startUntil   ( cnt_t _time )                               {        tmr_startUntil   (this, _time);                   }
	void stop         ( void )                                      {        tmr_stop         (this);                          }

	unsigned take     ( void )                                      { return tmr_take         (this);                          }
	unsigned tryWait  ( void )                                      { return tmr_tryWait      (this);                          }
	unsigned takeISR  ( void )                                      { return tmr_takeISR      (this);                          }
	unsigned waitFor  ( cnt_t _delay )                              { return tmr_waitFor      (this, _delay);                  }
	unsigned waitNext ( cnt_t _delay )                              { return tmr_waitNext     (this, _delay);                  }
	unsigned waitUntil( cnt_t _time )                               { return tmr_waitUntil    (this, _time);                   }
	unsigned wait     ( void )                                      { return tmr_wait         (this);                          }

	bool     operator!( void )                                      { return __tmr::hdr.id == ID_STOPPED;                      }
#if OS_FUNCTIONAL
	static
	void     fun_     ( void )                                      {        tmr_thisISR()->fun();                             }
#endif
};

/******************************************************************************
 *
 * Class             : startTimer
 *
 * Description       : create and initialize a timer object
 *                     and start periodic timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *                     do this periodically
 *
 * Constructor parameters
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   state           : callback procedure
 *
 ******************************************************************************/

struct startTimer : public Timer
{
	startTimer( const cnt_t _delay, const cnt_t _period ):               Timer()       { port_sys_init(); tmr_start(this, _delay, _period); }
	startTimer( const cnt_t _delay, const cnt_t _period, FUN_t _state ): Timer(_state) { port_sys_init(); tmr_start(this, _delay, _period); }
};

/******************************************************************************
 *
 * Class             : startTimerFor
 *
 * Description       : create and initialize a timer object
 *                     and start one-shot timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *
 * Constructor parameters
 *   delay           : duration of time (maximum number of ticks to countdown)
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   state           : callback procedure
 *
 ******************************************************************************/

struct startTimerFor : public startTimer
{
	startTimerFor( const cnt_t _delay ):               startTimer(_delay, 0)         {}
	startTimerFor( const cnt_t _delay, FUN_t _state ): startTimer(_delay, 0, _state) {}
};

/******************************************************************************
 *
 * Class             : startTimerPeriodic
 *
 * Description       : create and initialize a timer object
 *                     and start periodic timer for given duration of time
 *                     when the timer has finished the countdown, the callback procedure is launched
 *                     do this periodically
 *
 * Constructor parameters
 *   period          : duration of time (maximum number of ticks to countdown)
 *                     IMMEDIATE: don't countdown
 *                     INFINITE:  countdown indefinitely
 *   state           : callback procedure
 *
 ******************************************************************************/

struct startTimerPeriodic : public startTimer
{
	startTimerPeriodic( const cnt_t _period ):               startTimer(_period, _period)         {}
	startTimerPeriodic( const cnt_t _period, FUN_t _state ): startTimer(_period, _period, _state) {}
};

/******************************************************************************
 *
 * Class             : startTimerUntil
 *
 * Description       : create and initialize a timer object
 *                     and start one-shot timer until given timepoint
 *                     when the timer has finished the countdown, the callback procedure is launched
 *
 * Constructor parameters
 *   time            : timepoint value
 *   state           : callback procedure
 *
 ******************************************************************************/

struct startTimerUntil : public Timer
{
	startTimerUntil( const cnt_t _time ):               Timer()       { port_sys_init(); tmr_startUntil(this, _time); }
	startTimerUntil( const cnt_t _time, FUN_t _state ): Timer(_state) { port_sys_init(); tmr_startUntil(this, _time); }
};

/******************************************************************************
 *
 * Namespace         : ThisTimer
 *
 * Description       : provide set of functions for current timer
 *
 ******************************************************************************/

namespace ThisTimer
{
#if OS_FUNCTIONAL
	static inline void flipISR ( FUN_t _state ) { tmr_thisISR()->fun = _state;
	                                              tmr_flipISR (Timer::fun_); }
#else
	static inline void flipISR ( FUN_t _state ) { tmr_flipISR (_state); }
#endif
	static inline void delayISR( cnt_t _delay ) { tmr_delayISR(_delay); }
}

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_TMR_H

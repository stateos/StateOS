/******************************************************************************

    @file    StateOS: os_tmr.h
    @author  Rajmund Szymanski
    @date    14.11.2017
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

#include "oskernel.h"

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
	uint32_t start;
	uint32_t delay;
	uint32_t period;
};

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

#define               _TMR_INIT( _state ) { { 0, 0, 0, 0, 0 }, _state, 0, 0, 0 }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_TMR                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a timer object                                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : name of a pointer to timer object                                                              *
 *   state           : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_TMR( tmr, state )                     \
                       tmr_t tmr##__tmr = _TMR_INIT( state ); \
                       tmr_id tmr = & tmr##__tmr

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_TMR_DEF                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize complete timer object                                                     *
 *                     timer callback procedure (function body) must be defined immediately below                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : name of a pointer to timer object                                                              *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_TMR_DEF( tmr )                            \
                       void tmr##__fun( void );                   \
                       tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun ); \
                       tmr_id tmr = & tmr##__tmr;                   \
                       void tmr##__fun( void )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_TMR_UNTIL                                                                                   *
 *                                                                                                                    *
 * Description       : define, initilize and start complete timer object                                              *
 *                     timer callback procedure (function body) must be defined immediately below                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : name of a pointer to timer object                                                              *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_TMR_UNTIL( tmr, time )                                                \
                       void tmr##__fun( void );                                               \
                       tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );                             \
                       tmr_id tmr = & tmr##__tmr;                                               \
         __CONSTRUCTOR void tmr##__start( void ) { port_sys_init(); tmr_startUntil(tmr, time); } \
                       void tmr##__fun( void )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_TMR_START                                                                                   *
 *                                                                                                                    *
 * Description       : define, initilize and start complete timer object                                              *
 *                     timer callback procedure (function body) must be defined immediately below                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : name of a pointer to timer object                                                              *
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration                   *
 *                     IMMEDIATE: don't countdown                                                                     *
 *                     INFINITE:  countdown indefinitly                                                               *
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations               *
 *                     IMMEDIATE: don't countdown                                                                     *
 *                     INFINITE:  countdown indefinitly                                                               *
 *                                                                                                                    *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_TMR_START( tmr, delay, period )                                           \
                       void tmr##__fun( void );                                                   \
                       tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );                                 \
                       tmr_id tmr = & tmr##__tmr;                                                   \
         __CONSTRUCTOR void tmr##__start( void ) { port_sys_init(); tmr_start(tmr, delay, period); } \
                       void tmr##__fun( void )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_TMR                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize static timer object                                                       *
 *                     timer callback procedure (function body) must be defined immediately below                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : name of a pointer to timer object                                                              *
 *   state           : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_TMR( tmr, state )                     \
                static tmr_t tmr##__tmr = _TMR_INIT( state ); \
                static tmr_id tmr = & tmr##__tmr

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_TMR_DEF                                                                                 *
 *                                                                                                                    *
 * Description       : define and initilize static timer object                                                       *
 *                     timer callback procedure (function body) must be defined immediately below                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : name of a pointer to timer object                                                              *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_TMR_DEF( tmr )                            \
                static void tmr##__fun( void );                   \
                static tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun ); \
                static tmr_id tmr = & tmr##__tmr;                   \
                static void tmr##__fun( void )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_TMR_UNTIL                                                                               *
 *                                                                                                                    *
 * Description       : define, initilize and start static timer object                                                *
 *                     timer callback procedure (function body) must be defined immediately below                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : name of a pointer to timer object                                                              *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_TMR_UNTIL( tmr, time )                                                \
                static void tmr##__fun( void );                                               \
                static tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );                             \
                static tmr_id tmr = & tmr##__tmr;                                               \
  __CONSTRUCTOR static void tmr##__start( void ) { port_sys_init(); tmr_startUntil(tmr, time); } \
                static  void tmr##__fun( void )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_TMR_START                                                                               *
 *                                                                                                                    *
 * Description       : define, initilize and start static timer object                                                *
 *                     timer callback procedure (function body) must be defined immediately below                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : name of a pointer to timer object                                                              *
 *   delay           : duration of time (maximum number of ticks to countdown) for first expiration                   *
 *                     IMMEDIATE: don't countdown                                                                     *
 *                     INFINITE:  countdown indefinitly                                                               *
 *   period          : duration of time (maximum number of ticks to countdown) for all next expirations               *
 *                     IMMEDIATE: don't countdown                                                                     *
 *                     INFINITE:  countdown indefinitly                                                               *
 *                                                                                                                    *
 * Note              : only available for compilers supporting the "constructor" function attribute or its equivalent *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_TMR_START( tmr, delay, period )                                           \
                static void tmr##__fun( void );                                                   \
                static tmr_t tmr##__tmr = _TMR_INIT( tmr##__fun );                                 \
                static tmr_id tmr = & tmr##__tmr;                                                   \
  __CONSTRUCTOR static void tmr##__start( void ) { port_sys_init(); tmr_start(tmr, delay, period); } \
                static void tmr##__fun( void )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : TMR_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a timer object                                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   state           : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 * Return            : timer object                                                                                   *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                TMR_INIT( state ) \
                      _TMR_INIT( state )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : TMR_CREATE                                                                                     *
 * Alias             : TMR_NEW                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize a timer object                                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   state           : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 * Return            : pointer to timer object                                                                        *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                TMR_CREATE( state ) \
             & (tmr_t) TMR_INIT  ( state )
#define                TMR_NEW \
                       TMR_CREATE
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_init                                                                                       *
 *                                                                                                                    *
 * Description       : initilize a timer object                                                                       *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *   state           : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tmr_init( tmr_t *tmr, fun_t *state );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_create                                                                                     *
 * Alias             : tmr_new                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize a new timer object                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   state           : callback procedure                                                                             *
 *                     0: no callback                                                                                 *
 *                                                                                                                    *
 * Return            : pointer to timer object (timer successfully created)                                           *
 *   0               : timer not created (not enough free memory)                                                     *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

tmr_t *tmr_create( fun_t *state );
__STATIC_INLINE
tmr_t *tmr_new   ( fun_t *state ) { return tmr_create(state); }

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

void tmr_kill( tmr_t *tmr );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_delete                                                                                     *
 *                                                                                                                    *
 * Description       : reset the timer object and free allocated resource                                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tmr_delete( tmr_t *tmr );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_startUntil                                                                                 *
 *                                                                                                                    *
 * Description       : start/restart one-shot timer until given timepoint and then launch the callback procedure      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   tmr             : pointer to timer object                                                                        *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tmr_startUntil( tmr_t *tmr, uint32_t time );

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
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void tmr_start( tmr_t *tmr, uint32_t delay, uint32_t period );

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
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
void tmr_startFor( tmr_t *tmr, uint32_t delay ) { tmr_start(tmr, delay, 0); }

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
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
void tmr_startPeriodic( tmr_t *tmr, uint32_t period ) { tmr_start(tmr, period, period); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : tmr_startFrom                                                                                  *
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

void tmr_startFrom( tmr_t *tmr, uint32_t delay, uint32_t period, fun_t *proc );

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

__STATIC_INLINE
void tmr_stop( tmr_t *tmr ) { tmr_start(tmr, 0, 0); }

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
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned tmr_waitUntil( tmr_t *tmr, uint32_t time );

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
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned tmr_waitFor( tmr_t *tmr, uint32_t delay );

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
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned tmr_wait( tmr_t *tmr ) { return tmr_waitFor(tmr, INFINITE); }

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

__STATIC_INLINE
unsigned tmr_take( tmr_t *tmr ) { return tmr_waitFor(tmr, IMMEDIATE); }

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

__STATIC_INLINE
unsigned tmr_takeISR( tmr_t *tmr ) { return tmr_waitFor(tmr, IMMEDIATE); }

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

__STATIC_INLINE
void tmr_flipISR( fun_t *proc ) { ((tmr_t *)WAIT.obj.next)->state = proc; }

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

__STATIC_INLINE
void tmr_delayISR( uint32_t delay ) { ((tmr_t *)WAIT.obj.next)->delay = delay; }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : Timer                                                                                          *
 *                                                                                                                    *
 * Description       : create and initilize a timer object                                                            *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   state           : callback procedure                                                                             *
 *                                                                                                                    *
 **********************************************************************************************************************/

struct Timer : public __tmr
{
	 explicit
	 Timer( void ):         __tmr _TMR_INIT(0) {}
#if OS_FUNCTIONAL
	 explicit
	 Timer( FUN_t _state ): __tmr _TMR_INIT(run_), fun_(_state) {}
	~Timer( void ) { assert(__tmr::obj.id == ID_STOPPED); }
#else
	 explicit
	 Timer( FUN_t _state ): __tmr _TMR_INIT(_state) {}
	~Timer( void ) { assert(__tmr::obj.id == ID_STOPPED); }
#endif

	void kill         ( void )                                            {        tmr_kill         (this);                          }
	void startUntil   ( uint32_t _time )                                  {        tmr_startUntil   (this, _time);                   }
	void start        ( uint32_t _delay, uint32_t _period )               {        tmr_start        (this, _delay, _period);         }
	void startFor     ( uint32_t _delay )                                 {        tmr_startFor     (this, _delay);                  }
	void startPeriodic( uint32_t _period )                                {        tmr_startPeriodic(this,         _period);         }
#if OS_FUNCTIONAL
	void startFrom    ( uint32_t _delay, uint32_t _period, FUN_t _state ) {        fun_ = _state;
	                                                                               tmr_startFrom    (this, _delay, _period, run_);   }
#else
	void startFrom    ( uint32_t _delay, uint32_t _period, FUN_t _state ) {        tmr_startFrom    (this, _delay, _period, _state); }
#endif
	void stop         ( void )                                            {        tmr_stop         (this);                          }

	unsigned waitUntil( uint32_t _time )                                  { return tmr_waitUntil    (this, _time);                   }
	unsigned waitFor  ( uint32_t _delay )                                 { return tmr_waitFor      (this, _delay);                  }
	unsigned wait     ( void )                                            { return tmr_wait         (this);                          }
	unsigned take     ( void )                                            { return tmr_take         (this);                          }
	unsigned takeISR  ( void )                                            { return tmr_takeISR      (this);                          }

	bool     operator!( void )                                            { return __tmr::obj.id == ID_STOPPED;                      }
#if OS_FUNCTIONAL
	static
	void     run_( void ) { ((Timer *) WAIT.obj.next)->fun_(); }
	FUN_t    fun_;
#endif
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
 *                                                                                                                    *
 **********************************************************************************************************************/

struct startTimerUntil : public Timer
{
	explicit
	startTimerUntil( const uint32_t _time ):               Timer()       { port_sys_init(); tmr_startUntil(this, _time); }
	explicit
	startTimerUntil( const uint32_t _time, FUN_t _state ): Timer(_state) { port_sys_init(); tmr_startUntil(this, _time); }
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
 *                                                                                                                    *
 **********************************************************************************************************************/

struct startTimer : public Timer
{
	explicit
	startTimer( const uint32_t _delay, const uint32_t _period ):               Timer()       { port_sys_init(); tmr_start(this, _delay, _period); }
	explicit
	startTimer( const uint32_t _delay, const uint32_t _period, FUN_t _state ): Timer(_state) { port_sys_init(); tmr_start(this, _delay, _period); }
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
 *                                                                                                                    *
 **********************************************************************************************************************/

struct startTimerFor : public startTimer
{
	explicit
	startTimerFor( const uint32_t _delay ):               startTimer(_delay, 0)         {}
	explicit
	startTimerFor( const uint32_t _delay, FUN_t _state ): startTimer(_delay, 0, _state) {}
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
 *                                                                                                                    *
 **********************************************************************************************************************/

struct startTimerPeriodic : public startTimer
{
	explicit
	startTimerPeriodic( const uint32_t _period ):               startTimer(_period, _period)         {}
	explicit
	startTimerPeriodic( const uint32_t _period, FUN_t _state ): startTimer(_period, _period, _state) {}
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Namespace         : ThisTimer                                                                                      *
 *                                                                                                                    *
 * Description       : provide set of functions for Current Timer                                                     *
 *                                                                                                                    *
 **********************************************************************************************************************/

namespace ThisTimer
{
#if OS_FUNCTIONAL
	static inline void flipISR ( FUN_t    _state ) { ((Timer *) WAIT.obj.next)->fun_ = _state;
	                                                 tmr_flipISR (Timer::run_);                }
#else
	static inline void flipISR ( FUN_t    _state ) { tmr_flipISR (_state);                     }
#endif
	static inline void delayISR( uint32_t _delay ) { tmr_delayISR(_delay);                     }
}

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_TMR_H

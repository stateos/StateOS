/******************************************************************************

    @file    StateOS: os_sig.h
    @author  Rajmund Szymanski
    @date    17.03.2016
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
 * Name              : signal                                                                                         *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define sigClear     ( 0U << 0 ) // auto clearing signal
#define sigProtect   ( 1U << 0 ) // protected signal
#define sigMASK      ( 1U )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_SIG                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a signal object                                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : name of a pointer to signal object                                                             *
 *   type            : signal type                                                                                    *
 *                     sigClear:   auto clearing signal                                                               *
 *                     sigProtect: protected signal                                                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_SIG( sig, type )                               \
                       sig_t sig##__sig = _SIG_INIT( (type)&sigMASK ); \
                       sig_id sig = & sig##__sig

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_SIG                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static signal object                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : name of a pointer to signal object                                                             *
 *   type            : signal type                                                                                    *
 *                     sigClear:   auto clearing signal                                                               *
 *                     sigProtect: protected signal                                                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_SIG( sig, type )                               \
                static sig_t sig##__sig = _SIG_INIT( (type)&sigMASK ); \
                static sig_id sig = & sig##__sig

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : SIG_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize an signal object                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   type            : signal type                                                                                    *
 *                     sigClear:   auto clearing signal                                                               *
 *                     sigProtect: protected signal                                                                   *
 *                                                                                                                    *
 * Return            : signal object                                                                                  *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                SIG_INIT( type ) \
                      _SIG_INIT( (type)&sigMASK )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : SIG_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a signal object                                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   type            : signal type                                                                                    *
 *                     sigClear:   auto clearing signal                                                               *
 *                     sigProtect: protected signal                                                                   *
 *                                                                                                                    *
 * Return            : pointer to signal object                                                                       *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                SIG_CREATE( type ) \
               &(sig_t)SIG_INIT( type )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_create                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a new signal object                                                       *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   type            : signal type                                                                                    *
 *                     sigClear:   auto clearing signal                                                               *
 *                     sigProtect: protected signal                                                                   *
 *                                                                                                                    *
 * Return            : pointer to signal object (signal successfully created)                                         *
 *   0               : signal not created (not enough free memory)                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              sig_id   sig_create( unsigned type );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_kill                                                                                       *
 *                                                                                                                    *
 * Description       : reset the signal object and wake up all waiting tasks with 'E_STOPPED' event value             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : pointer to signal object                                                                       *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     sig_kill( sig_id sig );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_waitUntil                                                                                  *
 *                                                                                                                    *
 * Description       : wait for release the signal object until given timepoint                                       *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : pointer to signal object                                                                       *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_STOPPED       : signal object was killed before the specified timeout expired                                  *
 *   E_TIMEOUT       : signal object was not released before the specified timeout expired                            *
 *   'another'       : signal object was successfully released or task was resumed with 'another' event value         *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned sig_waitUntil( sig_id sig, unsigned time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_waitFor                                                                                    *
 *                                                                                                                    *
 * Description       : wait for release the signal object for given duration of time                                  *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : pointer to signal object                                                                       *
 *   delay           : duration of time (maximum number of ticks to wait for release the signal object)               *
 *                     IMMEDIATE: don't wait until the signal object has been released                                *
 *                     INFINITE:  wait indefinitly until the signal object has been released                          *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_STOPPED       : signal object was killed before the specified timeout expired                                  *
 *   E_TIMEOUT       : signal object was not released before the specified timeout expired                            *
 *   'another'       : signal object was successfully released or task was resumed with 'another' event value         *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned sig_waitFor( sig_id sig, unsigned delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_wait                                                                                       *
 *                                                                                                                    *
 * Description       : wait indefinitly until the signal object has been released                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : pointer to signal object                                                                       *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_STOPPED       : signal object was killed                                                                       *
 *   'another'       : signal object was successfully released or task was resumed with 'another' event value         *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned sig_wait( sig_id sig ) { return sig_waitFor(sig, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_take                                                                                       *
 *                                                                                                                    *
 * Description       : don't wait until the signal object has been released                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : pointer to signal object                                                                       *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : signal object was successfully released                                                        *
 *   E_TIMEOUT       : signal object was not released before the specified timeout expired                            *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned sig_take( sig_id sig ) { return sig_waitFor(sig, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_takeISR                                                                                    *
 *                                                                                                                    *
 * Description       : don't wait until the signal object has been released                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : pointer to signal object                                                                       *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : signal object was successfully released                                                        *
 *   E_TIMEOUT       : signal object was not released before the specified timeout expired                            *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned sig_takeISR( sig_id sig ) { return sig_waitFor(sig, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_give                                                                                       *
 *                                                                                                                    *
 * Description       : resume one (sigClear) or all (sigProtect) tasks that are waiting on the signal object          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : pointer to signal object                                                                       *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     sig_give( sig_id sig );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_giveISR                                                                                    *
 *                                                                                                                    *
 * Description       : resume one (for auto clearing signals) or all (for normal signals) tasks                       *
 *                     that are waiting on the signal object                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : pointer to signal object                                                                       *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline void     sig_giveISR( sig_id sig ) { sig_give(sig); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_clear                                                                                      *
 *                                                                                                                    *
 * Description       : reset the signal object                                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : pointer to signal object                                                                       *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     sig_clear( sig_id sig );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_clearISR                                                                                   *
 *                                                                                                                    *
 * Description       : reset the signal object                                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : pointer to signal object                                                                       *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline void     sig_clearISR( sig_id sig ) { sig_clear(sig); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : Signal                                                                                         *
 *                                                                                                                    *
 * Description       : create and initilize a signal object                                                           *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   type            : signal type                                                                                    *
 *                     sigClear:   auto clearing signal (default)                                                     *
 *                     sigProtect: protected signal                                                                   *
 *                                                                                                                    *
 **********************************************************************************************************************/

class Signal : public __sig, private EventGuard<__sig>
{
public:

	constexpr explicit
	Signal( const unsigned _type = sigClear ): __sig(_SIG_INIT(_type & sigMASK)) {}

	void     kill     ( void )            {        sig_kill     (this);         }
	unsigned waitUntil( unsigned _time  ) { return sig_waitUntil(this, _time);  }
	unsigned waitFor  ( unsigned _delay ) { return sig_waitFor  (this, _delay); }
	unsigned wait     ( void )            { return sig_wait     (this);         }
	unsigned take     ( void )            { return sig_take     (this);         }
	unsigned takeISR  ( void )            { return sig_takeISR  (this);         }
	void     give     ( void )            {        sig_give     (this);         }
	void     giveISR  ( void )            {        sig_giveISR  (this);         }
	void     clear    ( void )            {        sig_clear    (this);         }
	void     clearISR ( void )            {        sig_clearISR (this);         }
};

#endif

/* -------------------------------------------------------------------------- */

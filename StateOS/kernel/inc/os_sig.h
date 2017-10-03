/******************************************************************************

    @file    StateOS: os_sig.h
    @author  Rajmund Szymanski
    @date    03.10.2017
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

#ifndef __STATEOS_SIG_H
#define __STATEOS_SIG_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : signal                                                                                         *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __sig sig_t, * const sig_id;

struct __sig
{
	tsk_t  * queue; // next process in the DELAYED queue
	unsigned flag;  // signal's current value
	unsigned type;  // signal type: sigClear, sigProtect
};

/* -------------------------------------------------------------------------- */

#define sigClear     ( 0U << 0 ) // auto clearing signal
#define sigProtect   ( 1U << 0 ) // protected signal
#define sigMASK      ( 1U )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _SIG_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a signal object                                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   type            : signal type                                                                                    *
 *                     sigClear:   auto clearing signal                                                               *
 *                     sigProtect: protected signal                                                                   *
 *                                                                                                                    *
 * Return            : signal object                                                                                  *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _SIG_INIT( _type ) { 0, 0, (_type)&sigMASK }

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

#define             OS_SIG( sig, type )                     \
                       sig_t sig##__sig = _SIG_INIT( type ); \
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

#define         static_SIG( sig, type )                     \
                static sig_t sig##__sig = _SIG_INIT( type ); \
                static sig_id sig = & sig##__sig

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : SIG_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a signal object                                                           *
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

#ifndef __cplusplus
#define                SIG_INIT( type ) \
                      _SIG_INIT( type )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : SIG_CREATE                                                                                     *
 * Alias             : SIG_NEW                                                                                        *
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
             & (sig_t) SIG_INIT  ( type )
#define                SIG_NEW \
                       SIG_CREATE
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_init                                                                                       *
 *                                                                                                                    *
 * Description       : initilize a signal object                                                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   sig             : pointer to signal object                                                                       *
 *   type            : signal type                                                                                    *
 *                     sigClear:   auto clearing signal                                                               *
 *                     sigProtect: protected signal                                                                   *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void sig_init( sig_t *sig, unsigned type );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : sig_create                                                                                     *
 * Alias             : sig_new                                                                                        *
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

sig_t *sig_create( unsigned type );
__STATIC_INLINE
sig_t *sig_new   ( unsigned type ) { return sig_create(type); }

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

void sig_kill( sig_t *sig );

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
 *   E_SUCCESS       : signal object was successfully released                                                        *
 *   E_STOPPED       : signal object was killed before the specified timeout expired                                  *
 *   E_TIMEOUT       : signal object was not released before the specified timeout expired                            *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned sig_waitUntil( sig_t *sig, uint32_t time );

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
 *   E_SUCCESS       : signal object was successfully released                                                        *
 *   E_STOPPED       : signal object was killed before the specified timeout expired                                  *
 *   E_TIMEOUT       : signal object was not released before the specified timeout expired                            *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned sig_waitFor( sig_t *sig, uint32_t delay );

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
 *   E_SUCCESS       : signal object was successfully released                                                        *
 *   E_STOPPED       : signal object was killed before the specified timeout expired                                  *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned sig_wait( sig_t *sig ) { return sig_waitFor(sig, INFINITE); }

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
 *   E_TIMEOUT       : signal object is not set                                                                       *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned sig_take( sig_t *sig ) { return sig_waitFor(sig, IMMEDIATE); }

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
 *   E_TIMEOUT       : signal object is not set                                                                       *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned sig_takeISR( sig_t *sig ) { return sig_waitFor(sig, IMMEDIATE); }

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

void sig_give( sig_t *sig );

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

__STATIC_INLINE
void sig_giveISR( sig_t *sig ) { sig_give(sig); }

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

void sig_clear( sig_t *sig );

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

__STATIC_INLINE
void sig_clearISR( sig_t *sig ) { sig_clear(sig); }

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

struct Signal : public __sig
{
	 explicit
	 Signal( const unsigned _type = sigClear ): __sig _SIG_INIT(_type) {}
	~Signal( void ) { assert(queue == nullptr); }

	void     kill     ( void )            {        sig_kill     (this);         }
	unsigned waitUntil( uint32_t _time  ) { return sig_waitUntil(this, _time);  }
	unsigned waitFor  ( uint32_t _delay ) { return sig_waitFor  (this, _delay); }
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

#endif//__STATEOS_SIG_H

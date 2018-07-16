/******************************************************************************

    @file    StateOS: ossignal.h
    @author  Rajmund Szymanski
    @date    15.07.2018
    @brief   This file contains definitions for StateOS.

 ******************************************************************************

   Copyright (c) 2018 Rajmund Szymanski. All rights reserved.

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

#ifndef __STATEOS_SIG_H
#define __STATEOS_SIG_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : signal
 *
 ******************************************************************************/

typedef struct __sig sig_t, * const sig_id;

struct __sig
{
	tsk_t  * queue; // next process in the DELAYED queue
	void   * res;   // allocated signal object's resource
	unsigned flag;  // signal's current value
	unsigned type;  // signal type: sigClear, sigProtect
};

/* -------------------------------------------------------------------------- */

#define sigClear     ( 0U << 0 ) // auto clearing signal
#define sigProtect   ( 1U << 0 ) // protected signal
#define sigMASK      ( 1U )

/******************************************************************************
 *
 * Name              : _SIG_INIT
 *
 * Description       : create and initialize a signal object
 *
 * Parameters
 *   type            : signal type
 *                     sigClear:   auto clearing signal
 *                     sigProtect: protected signal
 *
 * Return            : signal object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _SIG_INIT( _type ) { 0, 0, 0, (_type)&sigMASK }

/******************************************************************************
 *
 * Name              : OS_SIG
 *
 * Description       : define and initialize a signal object
 *
 * Parameters
 *   sig             : name of a pointer to signal object
 *   type            : signal type
 *                     sigClear:   auto clearing signal
 *                     sigProtect: protected signal
 *
 ******************************************************************************/

#define             OS_SIG( sig, type )                     \
                       sig_t sig##__sig = _SIG_INIT( type ); \
                       sig_id sig = & sig##__sig

/******************************************************************************
 *
 * Name              : static_SIG
 *
 * Description       : define and initialize a static signal object
 *
 * Parameters
 *   sig             : name of a pointer to signal object
 *   type            : signal type
 *                     sigClear:   auto clearing signal
 *                     sigProtect: protected signal
 *
 ******************************************************************************/

#define         static_SIG( sig, type )                     \
                static sig_t sig##__sig = _SIG_INIT( type ); \
                static sig_id sig = & sig##__sig

/******************************************************************************
 *
 * Name              : SIG_INIT
 *
 * Description       : create and initialize a signal object
 *
 * Parameters
 *   type            : signal type
 *                     sigClear:   auto clearing signal
 *                     sigProtect: protected signal
 *
 * Return            : signal object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                SIG_INIT( type ) \
                      _SIG_INIT( type )
#endif

/******************************************************************************
 *
 * Name              : SIG_CREATE
 * Alias             : SIG_NEW
 *
 * Description       : create and initialize a signal object
 *
 * Parameters
 *   type            : signal type
 *                     sigClear:   auto clearing signal
 *                     sigProtect: protected signal
 *
 * Return            : pointer to signal object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                SIG_CREATE( type ) \
           (sig_t[]) { SIG_INIT  ( type ) }
#define                SIG_NEW \
                       SIG_CREATE
#endif

/******************************************************************************
 *
 * Name              : sig_init
 *
 * Description       : initialize a signal object
 *
 * Parameters
 *   sig             : pointer to signal object
 *   type            : signal type
 *                     sigClear:   auto clearing signal
 *                     sigProtect: protected signal
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void sig_init( sig_t *sig, unsigned type );

/******************************************************************************
 *
 * Name              : sig_create
 * Alias             : sig_new
 *
 * Description       : create and initialize a new signal object
 *
 * Parameters
 *   type            : signal type
 *                     sigClear:   auto clearing signal
 *                     sigProtect: protected signal
 *
 * Return            : pointer to signal object (signal successfully created)
 *   0               : signal not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

sig_t *sig_create( unsigned type );

__STATIC_INLINE
sig_t *sig_new( unsigned type ) { return sig_create(type); }

/******************************************************************************
 *
 * Name              : sig_kill
 *
 * Description       : reset the signal object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   sig             : pointer to signal object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void sig_kill( sig_t *sig );

/******************************************************************************
 *
 * Name              : sig_delete
 *
 * Description       : reset the signal object and free allocated resource
 *
 * Parameters
 *   sig             : pointer to signal object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void sig_delete( sig_t *sig );

/******************************************************************************
 *
 * Name              : sig_waitUntil
 *
 * Description       : wait for release the signal object until given timepoint
 *
 * Parameters
 *   sig             : pointer to signal object
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : signal object was successfully released
 *   E_STOPPED       : signal object was killed before the specified timeout expired
 *   E_TIMEOUT       : signal object was not released before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned sig_waitUntil( sig_t *sig, cnt_t time );

/******************************************************************************
 *
 * Name              : sig_waitFor
 *
 * Description       : wait for release the signal object for given duration of time
 *
 * Parameters
 *   sig             : pointer to signal object
 *   delay           : duration of time (maximum number of ticks to wait for release the signal object)
 *                     IMMEDIATE: don't wait until the signal object has been released
 *                     INFINITE:  wait indefinitely until the signal object has been released
 *
 * Return
 *   E_SUCCESS       : signal object was successfully released
 *   E_STOPPED       : signal object was killed before the specified timeout expired
 *   E_TIMEOUT       : signal object was not released before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned sig_waitFor( sig_t *sig, cnt_t delay );

/******************************************************************************
 *
 * Name              : sig_wait
 *
 * Description       : wait indefinitely until the signal object has been released
 *
 * Parameters
 *   sig             : pointer to signal object
 *
 * Return
 *   E_SUCCESS       : signal object was successfully released
 *   E_STOPPED       : signal object was killed before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned sig_wait( sig_t *sig ) { return sig_waitFor(sig, INFINITE); }

/******************************************************************************
 *
 * Name              : sig_take
 * ISR alias         : sig_takeISR
 *
 * Description       : check if the signal object has been released
 *
 * Parameters
 *   sig             : pointer to signal object
 *
 * Return
 *   E_SUCCESS       : signal object was successfully released
 *   E_TIMEOUT       : signal object was not released
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned sig_take( sig_t *sig );

__STATIC_INLINE
unsigned sig_takeISR( sig_t *sig ) { return sig_take(sig); }

/******************************************************************************
 *
 * Name              : sig_give
 * ISR alias         : sig_giveISR
 *
 * Description       : release the signal object
 *                     resume one (sigClear) or all (sigProtect) tasks that are waiting on the signal object
 *
 * Parameters
 *   sig             : pointer to signal object
 *
 * Return            : none
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

void sig_give( sig_t *sig );

__STATIC_INLINE
void sig_giveISR( sig_t *sig ) { sig_give(sig); }

/******************************************************************************
 *
 * Name              : sig_clear
 * ISR alias         : sig_clearISR
 *
 * Description       : reset the signal object
 *
 * Parameters
 *   sig             : pointer to signal object
 *
 * Return            : none
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

void sig_clear( sig_t *sig );

__STATIC_INLINE
void sig_clearISR( sig_t *sig ) { sig_clear(sig); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : Signal
 *
 * Description       : create and initialize a signal object
 *
 * Constructor parameters
 *   type            : signal type
 *                     sigClear:   auto clearing signal (default)
 *                     sigProtect: protected signal
 *
 ******************************************************************************/

struct Signal : public __sig
{
	 explicit
	 Signal( const unsigned _type = sigClear ): __sig _SIG_INIT(_type) {}
	~Signal( void ) { assert(queue == nullptr); }

	void     kill     ( void )         {        sig_kill     (this);         }
	unsigned waitUntil( cnt_t _time  ) { return sig_waitUntil(this, _time);  }
	unsigned waitFor  ( cnt_t _delay ) { return sig_waitFor  (this, _delay); }
	unsigned wait     ( void )         { return sig_wait     (this);         }
	unsigned take     ( void )         { return sig_take     (this);         }
	unsigned takeISR  ( void )         { return sig_takeISR  (this);         }
	void     give     ( void )         {        sig_give     (this);         }
	void     giveISR  ( void )         {        sig_giveISR  (this);         }
	void     clear    ( void )         {        sig_clear    (this);         }
	void     clearISR ( void )         {        sig_clearISR (this);         }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_SIG_H

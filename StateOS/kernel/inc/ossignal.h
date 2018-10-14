/******************************************************************************

    @file    StateOS: ossignal.h
    @author  Rajmund Szymanski
    @date    14.10.2018
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

/* -------------------------------------------------------------------------- */

#define UINT_BIT      (sizeof(unsigned) * CHAR_BIT)

#define SIGSET(signo) (((signo) < UINT_BIT) ? 1U << (signo) : 0U)   // signal mask from the signal number
#define sigAll        (0U-1)                                        // signal mask for all signals
#define sigAny        (0U)

/******************************************************************************
 *
 * Name              : signal
 *
 ******************************************************************************/

typedef struct __sig sig_t, * const sig_id;

struct __sig
{
	obj_t    obj;   // object header

	unsigned flags; // pending signals
	unsigned mask;  // protection mask
};

/******************************************************************************
 *
 * Name              : _SIG_INIT
 *
 * Description       : create and initialize a signal object
 *
 * Parameters
 *   mask            : protection mask of signal object
 *
 * Return            : signal object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _SIG_INIT( _mask ) { _OBJ_INIT(), 0, _mask }

/******************************************************************************
 *
 * Name              : _VA_SIG
 *
 * Description       : calculate protection mask from optional parameter
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _VA_SIG( _mask ) ( _mask + 0 )

/******************************************************************************
 *
 * Name              : OS_SIG
 *
 * Description       : define and initialize a signal object
 *
 * Parameters
 *   sig             : name of a pointer to signal object
 *   mask            : (optional) protection mask; default: 0
 *
 ******************************************************************************/

#define             OS_SIG( sig, ... )                                      \
                       sig_t sig##__sig = _SIG_INIT( _VA_SIG(__VA_ARGS__) ); \
                       sig_id sig = & sig##__sig

/******************************************************************************
 *
 * Name              : static_SIG
 *
 * Description       : define and initialize a static signal object
 *
 * Parameters
 *   sig             : name of a pointer to signal object
 *   mask            : (optional) protection mask; default: 0
 *
 ******************************************************************************/

#define         static_SIG( sig, ... )                                      \
                static sig_t sig##__sig = _SIG_INIT( _VA_SIG(__VA_ARGS__) ); \
                static sig_id sig = & sig##__sig

/******************************************************************************
 *
 * Name              : SIG_INIT
 *
 * Description       : create and initialize a signal object
 *
 * Parameters
 *   mask            : (optional) protection mask; default: 0
 *
 * Return            : signal object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                SIG_INIT( ... ) \
                      _SIG_INIT( _VA_SIG(__VA_ARGS__) )
#endif

/******************************************************************************
 *
 * Name              : SIG_CREATE
 * Alias             : SIG_NEW
 *
 * Description       : create and initialize a signal object
 *
 * Parameters
 *   mask            : (optional) protection mask; default: 0
 *
 * Return            : pointer to signal object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                SIG_CREATE( ... ) \
           (sig_t[]) { SIG_INIT  ( _VA_SIG(__VA_ARGS__) ) }
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
 *   mask            : protection mask of signal object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void sig_init( sig_t *sig, unsigned mask );

/******************************************************************************
 *
 * Name              : sig_create
 * Alias             : sig_new
 *
 * Description       : create and initialize a new signal object
 *
 * Parameters
 *   mask            : protection mask of signal object
 *
 * Return            : pointer to signal object (signal successfully created)
 *   0               : signal not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

sig_t *sig_create( unsigned mask );

__STATIC_INLINE
sig_t *sig_new( unsigned mask ) { return sig_create(mask); }

/******************************************************************************
 *
 * Name              : sig_kill
 * Alias             : sig_reset
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

__STATIC_INLINE
void sig_reset( sig_t *sig ) { sig_kill(sig); }

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
 * Name              : sig_take
 * Alias             : sig_tryWait
 * ISR alias         : sig_takeISR
 *
 * Description       : check signal object for a given set of signals
 *
 * Parameters
 *   sig             : pointer to signal object
 *   sigset          : set of expected signals
 *
 * Return            : the lowest number of expected signal from the set of all pending signals or
 *   E_TIMEOUT       : no expected signal has been set, try again
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned sig_take( sig_t *sig, unsigned sigset );

__STATIC_INLINE
unsigned sig_tryWait( sig_t *sig, unsigned sigset ) { return sig_take(sig, sigset); }

__STATIC_INLINE
unsigned sig_takeISR( sig_t *sig, unsigned sigset ) { return sig_take(sig, sigset); }

/******************************************************************************
 *
 * Name              : sig_waitFor
 *
 * Description       : wait for a signal from the given set of signals for given duration of time
 *
 * Parameters
 *   sig             : pointer to signal object
 *   sigset          : set of expected signals
 *   delay           : duration of time (maximum number of ticks to wait for release the signal object)
 *                     IMMEDIATE: don't wait until the signal object has been released
 *                     INFINITE:  wait indefinitely until the signal object has been released
 *
 * Return            : the lowest number of expected signal from the set of all pending signals or
 *   E_STOPPED       : signal object was killed before the specified timeout expired
 *   E_TIMEOUT       : no expected signal has been set before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned sig_waitFor( sig_t *sig, unsigned sigset, cnt_t delay );

/******************************************************************************
 *
 * Name              : sig_waitUntil
 *
 * Description       : wait for a signal from the given set of signals until given timepoint
 *
 * Parameters
 *   sig             : pointer to signal object
 *   sigset          : set of expected signals
 *   time            : timepoint value
 *
 * Return            : the lowest number of expected signal from the set of all pending signals or
 *   E_STOPPED       : signal object was killed before the specified timeout expired
 *   E_TIMEOUT       : no expected signal has been set before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned sig_waitUntil( sig_t *sig, unsigned sigset, cnt_t time );

/******************************************************************************
 *
 * Name              : sig_wait
 *
 * Description       : wait indefinitely for a signal from the given set of signals
 *
 * Parameters
 *   sig             : pointer to signal object
 *   sigset          : set of expected signals
 *
 * Return            : the lowest number of expected signal from the set of all pending signals or
 *   E_STOPPED       : signal object was killed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned sig_wait( sig_t *sig, unsigned sigset ) { return sig_waitFor(sig, sigset, INFINITE); }

/******************************************************************************
 *
 * Name              : sig_give
 * Alias             : sig_set
 * ISR alias         : sig_giveISR
 *
 * Description       : set given signal in the signal object
 *
 * Parameters
 *   sig             : pointer to signal object
 *   signo           : signal number
 *
 * Return            : none
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

void sig_give( sig_t *sig, unsigned signo );

__STATIC_INLINE
void sig_set( sig_t *sig, unsigned signo ) { sig_give(sig, signo); }

__STATIC_INLINE
void sig_giveISR( sig_t *sig, unsigned signo ) { sig_give(sig, signo); }

/******************************************************************************
 *
 * Name              : sig_clear
 * ISR alias         : sig_clearISR
 *
 * Description       : reset given signal in the signal object
 *
 * Parameters
 *   sig             : pointer to signal object
 *   signo           : signal number
 *
 * Return            : none
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

void sig_clear( sig_t *sig, unsigned signo );

__STATIC_INLINE
void sig_clearISR( sig_t *sig, unsigned signo ) { sig_clear(sig, signo); }

/******************************************************************************
 *
 * Name              : sig_get
 * ISR alias         : sig_getISR
 *
 * Description       : get state of the given signal in the signal object
 *
 * Parameters
 *   sig             : pointer to signal object
 *   signo           : signal number
 *
 * Return            : signal state in signal object
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

bool sig_get( sig_t *sig, unsigned signo );

__STATIC_INLINE
bool sig_getISR( sig_t *sig, unsigned signo ) { return sig_get(sig, signo); }

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
 *   mask            : protection mask of signal object
 *
 ******************************************************************************/

struct Signal : public __sig
{
	 Signal( const unsigned _mask = 0 ): __sig _SIG_INIT(_mask) {}
	~Signal( void ) { assert(__sig::obj.queue == nullptr); }

	void     kill     ( void )                           {        sig_kill     (this);                  }
	void     reset    ( void )                           {        sig_reset    (this);                  }
	unsigned take     ( unsigned _sigset )               { return sig_take     (this, _sigset);         }
	unsigned tryWait  ( unsigned _sigset )               { return sig_tryWait  (this, _sigset);         }
	unsigned takeISR  ( unsigned _sigset )               { return sig_takeISR  (this, _sigset);         }
	unsigned waitFor  ( unsigned _sigset, cnt_t _delay ) { return sig_waitFor  (this, _sigset, _delay); }
	unsigned waitUntil( unsigned _sigset, cnt_t _time )  { return sig_waitUntil(this, _sigset, _time);  }
	unsigned wait     ( unsigned _sigset )               { return sig_wait     (this, _sigset);         }
	void     give     ( unsigned _signo )                {        sig_give     (this, _signo);          }
	void     set      ( unsigned _signo )                {        sig_set      (this, _signo);          }
	void     giveISR  ( unsigned _signo )                {        sig_giveISR  (this, _signo);          }
	void     clear    ( unsigned _signo )                {        sig_clear    (this, _signo);          }
	void     clearISR ( unsigned _signo )                {        sig_clearISR (this, _signo);          }
	bool     get      ( unsigned _signo )                { return sig_get      (this, _signo);          }
	bool     getISR   ( unsigned _signo )                { return sig_getISR   (this, _signo);          }
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_SIG_H

/******************************************************************************

    @file    StateOS: ossemaphore.h
    @author  Rajmund Szymanski
    @date    28.04.2020
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

#ifndef __STATEOS_SEM_H
#define __STATEOS_SEM_H

#include "oskernel.h"

/* -------------------------------------------------------------------------- */

#define semDirect    ( 0U )     // direct semaphore
#define semBinary    ( 1U )     // binary semaphore
#define semCounting  ( 0U-1 )   // counting semaphore
#define semDefault     semCounting

/******************************************************************************
 *
 * Name              : semaphore
 *                     like a POSIX semaphore
 *
 ******************************************************************************/

typedef struct __sem sem_t, * const sem_id;

struct __sem
{
	obj_t    obj;   // object header

	unsigned count; // current value of the semaphore counter
	unsigned limit; // limit value of the semaphore counter
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _SEM_INIT
 *
 * Description       : create and initialize a semaphore object
 *
 * Parameters
 *   init            : initial value of semaphore counter
 *   limit           : maximum value of semaphore counter
 *                     semBinary: binary semaphore
 *                     semCounting: counting semaphore
 *                     otherwise: limited semaphore
 *
 * Return            : semaphore object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _SEM_INIT( _init, _limit ) { _OBJ_INIT(), _init, _limit }

/******************************************************************************
 *
 * Name              : _VA_SEM
 *
 * Description       : calculate maximum value of semaphore counter from optional parameter
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _VA_SEM( _limit ) ( (_limit + 0) ? (_limit + 0) : (semDefault) )

/******************************************************************************
 *
 * Name              : OS_SEM
 *
 * Description       : define and initialize a semaphore object
 *
 * Parameters
 *   sem             : name of a pointer to semaphore object
 *   init            : initial value of semaphore counter
 *   limit           : (optional) maximum value of semaphore counter
 *                     semBinary: binary semaphore
 *                     semCounting: counting semaphore (default)
 *                     otherwise: limited semaphore
 *
 ******************************************************************************/

#define             OS_SEM( sem, init, ... )                                      \
                       sem_t sem##__sem = _SEM_INIT( init, _VA_SEM(__VA_ARGS__) ); \
                       sem_id sem = & sem##__sem

/******************************************************************************
 *
 * Name              : static_SEM
 *
 * Description       : define and initialize a static semaphore object
 *
 * Parameters
 *   sem             : name of a pointer to semaphore object
 *   init            : initial value of semaphore counter
 *   limit           : (optional) maximum value of semaphore counter
 *                     semBinary: binary semaphore
 *                     semCounting: counting semaphore (default)
 *                     otherwise: limited semaphore
 *
 ******************************************************************************/

#define         static_SEM( sem, init, ... )                                      \
                static sem_t sem##__sem = _SEM_INIT( init, _VA_SEM(__VA_ARGS__) ); \
                static sem_id sem = & sem##__sem

/******************************************************************************
 *
 * Name              : SEM_INIT
 *
 * Description       : create and initialize a semaphore object
 *
 * Parameters
 *   init            : initial value of semaphore counter
 *   limit           : (optional) maximum value of semaphore counter
 *                     semBinary: binary semaphore
 *                     semCounting: counting semaphore (default)
 *                     otherwise: limited semaphore
 *
 * Return            : semaphore object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                SEM_INIT( init, ... ) \
                      _SEM_INIT( init, _VA_SEM(__VA_ARGS__) )
#endif

/******************************************************************************
 *
 * Name              : SEM_CREATE
 * Alias             : SEM_NEW
 *
 * Description       : create and initialize a semaphore object
 *
 * Parameters
 *   init            : initial value of semaphore counter
 *   limit           : (optional) maximum value of semaphore counter
 *                     semBinary: binary semaphore
 *                     semCounting: counting semaphore (default)
 *                     otherwise: limited semaphore
 *
 * Return            : pointer to semaphore object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                SEM_CREATE( init, ... ) \
           (sem_t[]) { SEM_INIT  ( init, _VA_SEM(__VA_ARGS__) ) }
#define                SEM_NEW \
                       SEM_CREATE
#endif

/******************************************************************************
 *
 * Name              : sem_init
 *
 * Description       : initialize a semaphore object
 *
 * Parameters
 *   sem             : pointer to semaphore object
 *   init            : initial value of semaphore counter
 *   limit           : maximum value of semaphore counter
 *                     semBinary: binary semaphore
 *                     semCounting: counting semaphore
 *                     otherwise: limited semaphore
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void sem_init( sem_t *sem, unsigned init, unsigned limit );

/******************************************************************************
 *
 * Name              : sem_create
 * Alias             : sem_new
 *
 * Description       : create and initialize a new semaphore object
 *
 * Parameters
 *   init            : initial value of semaphore counter
 *   limit           : maximum value of semaphore counter
 *                     semBinary: binary semaphore
 *                     semCounting: counting semaphore
 *                     otherwise: limited semaphore
 *
 * Return            : pointer to semaphore object (semaphore successfully created)
 *   0               : semaphore not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

sem_t *sem_create( unsigned init, unsigned limit );

__STATIC_INLINE
sem_t *sem_new( unsigned init, unsigned limit ) { return sem_create(init, limit); }

/******************************************************************************
 *
 * Name              : sem_reset
 * Alias             : sem_kill
 *
 * Description       : reset the semaphore object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   sem             : pointer to semaphore object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void sem_reset( sem_t *sem );

__STATIC_INLINE
void sem_kill( sem_t *sem ) { sem_reset(sem); }

/******************************************************************************
 *
 * Name              : sem_destroy
 * Alias             : dem_delete
 *
 * Description       : reset the semaphore object, wake up all waiting tasks with 'E_DELETED' event value and free allocated resource
 *
 * Parameters
 *   sem             : pointer to semaphore object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void sem_destroy( sem_t *sem );

__STATIC_INLINE
void sem_delete( sem_t *sem ) { sem_destroy(sem); }

/******************************************************************************
 *
 * Name              : sem_take
 * Alias             : sem_tryWait
 * ISR alias         : sem_takeISR
 *
 * Description       : try to lock the semaphore object,
 *                     don't wait if the semaphore object can't be locked immediately
 *
 * Parameters
 *   sem             : pointer to semaphore object
 *
 * Return
 *   E_SUCCESS       : semaphore object was successfully locked
 *   E_TIMEOUT       : semaphore object can't be locked immediately, try again
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned sem_take( sem_t *sem );

__STATIC_INLINE
unsigned sem_tryWait( sem_t *sem ) { return sem_take(sem); }

__STATIC_INLINE
unsigned sem_takeISR( sem_t *sem ) { return sem_take(sem); }

/******************************************************************************
 *
 * Name              : sem_waitFor
 *
 * Description       : try to lock the semaphore object,
 *                     wait for given duration of time if the semaphore object can't be locked immediately
 *
 * Parameters
 *   sem             : pointer to semaphore object
 *   delay           : duration of time (maximum number of ticks to wait for lock the semaphore object)
 *                     IMMEDIATE: don't wait if the semaphore object can't be locked immediately
 *                     INFINITE:  wait indefinitely until the semaphore object has been locked
 *
 * Return
 *   E_SUCCESS       : semaphore object was successfully locked
 *   E_STOPPED       : semaphore object was reseted before the specified timeout expired
 *   E_DELETED       : semaphore object was deleted before the specified timeout expired
 *   E_TIMEOUT       : semaphore object was not locked before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned sem_waitFor( sem_t *sem, cnt_t delay );

/******************************************************************************
 *
 * Name              : sem_waitUntil
 *
 * Description       : try to lock the semaphore object,
 *                     wait until given timepoint if the semaphore object can't be locked immediately
 *
 * Parameters
 *   sem             : pointer to semaphore object
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : semaphore object was successfully locked
 *   E_STOPPED       : semaphore object was reseted before the specified timeout expired
 *   E_DELETED       : semaphore object was deleted before the specified timeout expired
 *   E_TIMEOUT       : semaphore object was not locked before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned sem_waitUntil( sem_t *sem, cnt_t time );

/******************************************************************************
 *
 * Name              : sem_wait
 *
 * Description       : try to lock the semaphore object,
 *                     wait indefinitely if the semaphore object can't be locked immediately
 *
 * Parameters
 *   sem             : pointer to semaphore object
 *
 * Return
 *   E_SUCCESS       : semaphore object was successfully locked
 *   E_STOPPED       : semaphore object was reseted
 *   E_DELETED       : semaphore object was deleted
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned sem_wait( sem_t *sem ) { return sem_waitFor(sem, INFINITE); }

/******************************************************************************
 *
 * Name              : sem_give
 * Alias             : sem_post
 * ISR alias         : sem_giveISR
 *
 * Description       : try to unlock the semaphore object,
 *                     don't wait if the semaphore object can't be unlocked immediately
 *
 * Parameters
 *   sem             : pointer to semaphore object
 *
 * Return
 *   E_SUCCESS       : semaphore object was successfully unlocked
 *   E_FAILURE       : semaphore object can't be unlocked immediately and waiting is not possible
 *   E_TIMEOUT       : semaphore object can't be unlocked immediately, try again
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned sem_give( sem_t *sem );

__STATIC_INLINE
unsigned sem_post( sem_t *sem ) { return sem_give(sem); }

__STATIC_INLINE
unsigned sem_giveISR( sem_t *sem ) { return sem_give(sem); }

/******************************************************************************
 *
 * Name              : sem_sendFor
 *
 * Description       : try to unlock the semaphore object,
 *                     wait for given duration of time if the semaphore object can't be unlocked immediately
 *
 * Parameters
 *   sem             : pointer to semaphore object
 *   delay           : duration of time (maximum number of ticks to wait for unlock the semaphore object)
 *                     IMMEDIATE: don't wait if the semaphore object can't be unlocked immediately
 *                     INFINITE:  wait indefinitely until the semaphore object has been unlocked
 *
 * Return
 *   E_SUCCESS       : semaphore object was successfully unlocked
 *   E_FAILURE       : semaphore object can't be unlocked immediately and waiting is not possible
 *   E_STOPPED       : semaphore object was reseted before the specified timeout expired
 *   E_DELETED       : semaphore object was deleted before the specified timeout expired
 *   E_TIMEOUT       : semaphore object was not unlocked before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned sem_sendFor( sem_t *sem, cnt_t delay );

/******************************************************************************
 *
 * Name              : sem_sendUntil
 *
 * Description       : try to unlock the semaphore object,
 *                     wait until given timepoint if the semaphore object can't be unlocked immediately
 *
 * Parameters
 *   sem             : pointer to semaphore object
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : semaphore object was successfully unlocked
 *   E_FAILURE       : semaphore object can't be unlocked immediately and waiting is not possible
 *   E_STOPPED       : semaphore object was reseted before the specified timeout expired
 *   E_DELETED       : semaphore object was deleted before the specified timeout expired
 *   E_TIMEOUT       : semaphore object was not unlocked before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned sem_sendUntil( sem_t *sem, cnt_t time );

/******************************************************************************
 *
 * Name              : sem_send
 *
 * Description       : try to unlock the semaphore object,
 *                     wait indefinitely if the semaphore object can't be unlocked immediately
 *
 * Parameters
 *   sem             : pointer to semaphore object
 *
 * Return
 *   E_SUCCESS       : semaphore object was successfully unlocked
 *   E_FAILURE       : semaphore object can't be unlocked immediately and waiting is not possible
 *   E_STOPPED       : semaphore object was reseted
 *   E_DELETED       : semaphore object was deleted
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned sem_send( sem_t *sem ) { return sem_sendFor(sem, INFINITE); }

/******************************************************************************
 *
 * Name              : sem_getValue
 *
 * Description       : return current value of semaphore
 *
 * Parameters
 *   sem             : pointer to semaphore object
 *
 * Return            : current value of semaphore
 *
 ******************************************************************************/

unsigned sem_getValue( sem_t *sem );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : Semaphore
 *
 * Description       : create and initialize a semaphore object
 *
 * Constructor parameters
 *   init            : initial value of semaphore counter
 *   limit           : maximum value of semaphore counter
 *                     semBinary: binary semaphore
 *                     semCounting: counting semaphore (default)
 *                     otherwise: limited semaphore
 *
 ******************************************************************************/

struct Semaphore : public __sem
{
	Semaphore( const unsigned _init, const unsigned _limit = semCounting ): __sem _SEM_INIT(_init, _limit) {}

	Semaphore( Semaphore&& ) = default;
	Semaphore( const Semaphore& ) = delete;
	Semaphore& operator=( Semaphore&& ) = delete;
	Semaphore& operator=( const Semaphore& ) = delete;

	~Semaphore( void ) { assert(__sem::obj.queue == nullptr); }

	static // create dynamic object with manageable resources
	Semaphore *create( const unsigned _init, const unsigned _limit = semCounting )
	{
		static_assert(sizeof(__sem) == sizeof(Semaphore), "unexpected error!");
		return reinterpret_cast<Semaphore *>(sem_create(_init, _limit));
	}

	void     reset    ( void )         {        sem_reset    (this);         }
	void     kill     ( void )         {        sem_kill     (this);         }
	void     destroy  ( void )         {        sem_destroy  (this);         }
	unsigned take     ( void )         { return sem_take     (this);         }
	unsigned tryWait  ( void )         { return sem_tryWait  (this);         }
	unsigned takeISR  ( void )         { return sem_takeISR  (this);         }
	unsigned waitFor  ( cnt_t _delay ) { return sem_waitFor  (this, _delay); }
	unsigned waitUntil( cnt_t _time )  { return sem_waitUntil(this, _time);  }
	unsigned wait     ( void )         { return sem_wait     (this);         }
	unsigned give     ( void )         { return sem_give     (this);         }
	unsigned post     ( void )         { return sem_post     (this);         }
	unsigned giveISR  ( void )         { return sem_giveISR  (this);         }
	unsigned sendFor  ( cnt_t _delay ) { return sem_sendFor  (this, _delay); }
	unsigned sendUntil( cnt_t _time )  { return sem_sendUntil(this, _time);  }
	unsigned send     ( void )         { return sem_send     (this);         }
	unsigned getValue ( void )         { return sem_getValue (this);         }
};

/******************************************************************************
 *
 * Class             : BinarySemaphore
 *
 * Description       : create and initialize a binary semaphore object
 *
 * Constructor parameters
 *   init            : initial value of semaphore counter
 *
 ******************************************************************************/

struct BinarySemaphore : public Semaphore
{
	BinarySemaphore( const unsigned _init = 0 ): Semaphore(_init, semBinary) {}

	static // create dynamic object with manageable resources
	BinarySemaphore *create( const unsigned _init = 0 )
	{
		static_assert(sizeof(__sem) == sizeof(BinarySemaphore), "unexpected error!");
		return reinterpret_cast<BinarySemaphore *>(sem_create(_init, semBinary));
	}
};

/******************************************************************************
 *
 * Class             : CountingSemaphore
 *
 * Description       : create and initialize a counting semaphore object
 *
 * Constructor parameters
 *   init            : initial value of semaphore counter
 *
 ******************************************************************************/

struct CountingSemaphore : public Semaphore
{
	CountingSemaphore( const unsigned _init = 0 ): Semaphore(_init, semCounting) {}

	static // create dynamic object with manageable resources
	CountingSemaphore *create( const unsigned _init = 0 )
	{
		static_assert(sizeof(__sem) == sizeof(CountingSemaphore), "unexpected error!");
		return reinterpret_cast<CountingSemaphore *>(sem_create(_init, semCounting));
	}
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_SEM_H

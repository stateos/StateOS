/******************************************************************************

    @file    StateOS: osrwlock.h
    @author  Rajmund Szymanski
    @date    08.07.2020
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

#ifndef __STATEOS_RWL_H
#define __STATEOS_RWL_H

#include "oskernel.h"
#include "osclock.h"

/* -------------------------------------------------------------------------- */

#define RDR_LIMIT    ( 0U-1 )

/******************************************************************************
 *
 * Name              : read/write lock (one write, many read)
 *
 ******************************************************************************/

typedef struct __rwl rwl_t, * const rwl_id;

struct __rwl
{
	obj_t    obj;   // object header

	tsk_t  * queue; // readers queue
	bool     write; // writer is active
	unsigned count; // number of active readers
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _RWL_INIT
 *
 * Description       : create and initialize a read/write lock object
 *
 * Parameters        : none
 *
 * Return            : read/write lock object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _RWL_INIT() { _OBJ_INIT(), NULL, false, 0 }

/******************************************************************************
 *
 * Name              : OS_RWL
 *
 * Description       : define and initialize a read/write lock object
 *
 * Parameters
 *   rwl             : name of a pointer to read/write lock object
 *
 ******************************************************************************/

#define             OS_RWL( rwl )                     \
                       rwl_t rwl##__rwl = _RWL_INIT(); \
                       rwl_id rwl = & rwl##__rwl

/******************************************************************************
 *
 * Name              : static_RWL
 *
 * Description       : define and initialize a static read/write lock object
 *
 * Parameters
 *   rwl             : name of a pointer to read/write lock object
 *
 ******************************************************************************/

#define         static_RWL( rwl )                     \
                static rwl_t rwl##__rwl = _RWL_INIT(); \
                static rwl_id rwl = & rwl##__rwl

/******************************************************************************
 *
 * Name              : RWL_INIT
 *
 * Description       : create and initialize a read/write lock object
 *
 * Parameters        : none
 *
 * Return            : read/write lock object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                RWL_INIT() \
                      _RWL_INIT()
#endif

/******************************************************************************
 *
 * Name              : RWL_CREATE
 * Alias             : RWL_NEW
 *
 * Description       : create and initialize a read/write lock object
 *
 * Parameters        : none
 *
 * Return            : pointer to read/write lock object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                RWL_CREATE() \
           (rwl_t[]) { RWL_INIT  () }
#define                RWL_NEW \
                       RWL_CREATE
#endif

/******************************************************************************
 *
 * Name              : rwl_init
 *
 * Description       : initialize a read/write lock object
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void rwl_init( rwl_t *rwl );

/******************************************************************************
 *
 * Name              : rwl_create
 * Alias             : rwl_new
 *
 * Description       : create and initialize a new read/write lock object
 *
 * Parameters        : none
 *
 * Return            : pointer to read/write lock object
 *   NULL            : object not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

rwl_t *rwl_create( void );

__STATIC_INLINE
rwl_t *rwl_new( void ) { return rwl_create(); }

/******************************************************************************
 *
 * Name              : rwl_reset
 * Alias             : rwl_kill
 *
 * Description       : reset the read/write lock object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void rwl_reset( rwl_t *rwl );

__STATIC_INLINE
void rwl_kill( rwl_t *rwl ) { rwl_reset(rwl); }

/******************************************************************************
 *
 * Name              : rwl_destroy
 * Alias             : rwl_delete
 *
 * Description       : reset the read/write lock object, wake up all waiting tasks with 'E_DELETED' event value and free allocated resource
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void rwl_destroy( rwl_t *rwl );

__STATIC_INLINE
void rwl_delete( rwl_t *rwl ) { rwl_destroy(rwl); }

/******************************************************************************
 *
 * Name              : rwl_takeRead
 * Alias             : rwl_tryLockRead
 *
 * Description       : try to lock the reader,
 *                     don't wait if the reader can't be locked immediately
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *
 * Return
 *   E_SUCCESS       : reader was successfully locked
 *   E_TIMEOUT       : reader can't be locked immediately, try again
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int rwl_takeRead( rwl_t *rwl );

__STATIC_INLINE
int rwl_tryLockRead( rwl_t *rwl ) { return rwl_takeRead(rwl); }

/******************************************************************************
 *
 * Name              : rwl_waitReadFor
 *
 * Description       : try to lock the reader,
 *                     wait for given duration of time if the reader can't be locked immediately
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *   delay           : duration of time (maximum number of ticks to wait for lock the reader)
 *                     IMMEDIATE: don't wait if the reader can't be locked immediately
 *                     INFINITE:  wait indefinitely until the reader has been locked
 *
 * Return
 *   E_SUCCESS       : reader was successfully locked
 *   E_STOPPED       : reader was reseted before the specified timeout expired
 *   E_DELETED       : reader was deleted before the specified timeout expired
 *   E_TIMEOUT       : reader was not locked before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int rwl_waitReadFor( rwl_t *rwl, cnt_t delay );

/******************************************************************************
 *
 * Name              : rwl_waitReadUntil
 *
 * Description       : try to lock the reader,
 *                     wait until given timepoint if the reader can't be locked immediately
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : reader was successfully locked
 *   E_STOPPED       : reader was reseted before the specified timeout expired
 *   E_DELETED       : reader was deleted before the specified timeout expired
 *   E_TIMEOUT       : reader was not locked before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int rwl_waitReadUntil( rwl_t *rwl, cnt_t time );

/******************************************************************************
 *
 * Name              : rwl_waitRead
 * Alias             : rwl_lockRead
 *
 * Description       : try to lock the reader,
 *                     wait indefinitely if the reader can't be locked immediately
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *
 * Return
 *   E_SUCCESS       : reader was successfully locked
 *   E_STOPPED       : reader was reseted
 *   E_DELETED       : reader was deleted
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
int rwl_waitRead( rwl_t *rwl ) { return rwl_waitReadFor(rwl, INFINITE); }

__STATIC_INLINE
int rwl_lockRead( rwl_t *rwl ) { return rwl_waitRead(rwl); }

/******************************************************************************
 *
 * Name              : rwl_giveRead
 * Alias             : rwl_unlockRead
 *
 * Description       : unlock the reader
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void rwl_giveRead( rwl_t *rwl );

__STATIC_INLINE
void rwl_unlockRead( rwl_t *rwl ) { rwl_giveRead(rwl); }

/******************************************************************************
 *
 * Name              : rwl_takeWrite
 * Alias             : rwl_tryLockWrite
 *
 * Description       : try to lock the writer,
 *                     don't wait if the writer can't be locked immediately
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *
 * Return
 *   E_SUCCESS       : writer was successfully locked
 *   E_TIMEOUT       : writer can't be locked immediately, try again
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int rwl_takeWrite( rwl_t *rwl );

__STATIC_INLINE
int rwl_tryLockWrite( rwl_t *rwl ) { return rwl_takeWrite(rwl); }

/******************************************************************************
 *
 * Name              : rwl_waitWriteFor
 *
 * Description       : try to lock the writer,
 *                     wait for given duration of time if the writer can't be locked immediately
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *   delay           : duration of time (maximum number of ticks to wait for lock the writer)
 *                     IMMEDIATE: don't wait if the writer can't be locked immediately
 *                     INFINITE:  wait indefinitely until the writer has been locked
 *
 * Return
 *   E_SUCCESS       : writer was successfully locked
 *   E_STOPPED       : writer was reseted before the specified timeout expired
 *   E_DELETED       : writer was deleted before the specified timeout expired
 *   E_TIMEOUT       : writer was not locked before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int rwl_waitWriteFor( rwl_t *rwl, cnt_t delay );

/******************************************************************************
 *
 * Name              : rwl_waitWriteUntil
 *
 * Description       : try to lock the writer,
 *                     wait until given timepoint if the writer can't be locked immediately
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : writer was successfully locked
 *   E_STOPPED       : writer was reseted before the specified timeout expired
 *   E_DELETED       : writer was deleted before the specified timeout expired
 *   E_TIMEOUT       : writer was not locked before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int rwl_waitWriteUntil( rwl_t *rwl, cnt_t time );

/******************************************************************************
 *
 * Name              : rwl_waitWrite
 * Alias             : rwl_lockWrite
 *
 * Description       : try to lock the writer,
 *                     wait indefinitely if the writer can't be locked immediately
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *
 * Return
 *   E_SUCCESS       : writer was successfully locked
 *   E_STOPPED       : writer was reseted
 *   E_DELETED       : writer was deleted
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
int rwl_waitWrite( rwl_t *rwl ) { return rwl_waitWriteFor(rwl, INFINITE); }

__STATIC_INLINE
int rwl_lockWrite( rwl_t *rwl ) { return rwl_waitWrite(rwl); }

/******************************************************************************
 *
 * Name              : rwl_giveWrite
 * Alias             : rwl_unlockWrite
 *
 * Description       : unlock the writer
 *
 * Parameters
 *   rwl             : pointer to read/write lock object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void rwl_giveWrite( rwl_t *rwl );

__STATIC_INLINE
void rwl_unlockWrite( rwl_t *rwl ) { rwl_giveWrite(rwl); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : RWLock
 *
 * Description       : create and initialize a read/write lock object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct RWLock : public __rwl
{
	constexpr
	RWLock( void ): __rwl _RWL_INIT() {}

	RWLock( RWLock&& ) = default;
	RWLock( const RWLock& ) = delete;
	RWLock& operator=( RWLock&& ) = delete;
	RWLock& operator=( const RWLock& ) = delete;

	~RWLock( void ) { assert(__rwl::write == false && __rwl::count == 0); }

#if __cplusplus >= 201402
	using Ptr = std::unique_ptr<RWLock>;
#else
	using Ptr = RWLock *;
#endif

/******************************************************************************
 *
 * Name              : RWLock::Create
 *
 * Description       : create dynamic object with manageable resources
 *
 * Parameters        : none
 *
 * Return            : std::unique_pointer / pointer to RWLock object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

	static
	Ptr Create( void )
	{
		auto rwl = new RWLock();
		if (rwl != nullptr)
			rwl->__rwl::obj.res = rwl;
		return Ptr(rwl);
	}

	void reset         ( void )           {        rwl_reset         (this); }
	void kill          ( void )           {        rwl_kill          (this); }
	void destroy       ( void )           {        rwl_destroy       (this); }
	int  takeRead      ( void )           { return rwl_takeRead      (this); }
	int  tryLockRead   ( void )           { return rwl_tryLockRead   (this); }
	template<typename T>
	int  waitReadFor   ( const T _delay ) { return rwl_waitReadFor   (this, Clock::count(_delay)); }
	template<typename T>
	int  waitReadUntil ( const T _time )  { return rwl_waitReadUntil (this, Clock::until(_time)); }
	int  waitRead      ( void )           { return rwl_waitRead      (this); }
	int  lockRead      ( void )           { return rwl_lockRead      (this); }
	void giveRead      ( void )           {        rwl_giveRead      (this); }
	void unlockRead    ( void )           {        rwl_unlockRead    (this); }
	int  takeWrite     ( void )           { return rwl_takeWrite     (this); }
	int  tryLockWrite  ( void )           { return rwl_tryLockWrite  (this); }
	template<typename T>
	int  waitWriteFor  ( const T _delay ) { return rwl_waitWriteFor  (this, Clock::count(_delay)); }
	template<typename T>
	int  waitWriteUntil( const T _time )  { return rwl_waitWriteUntil(this, Clock::until(_time)); }
	int  waitWrite     ( void )           { return rwl_waitWrite     (this); }
	int  lockWrite     ( void )           { return rwl_lockWrite     (this); }
	void giveWrite     ( void )           {        rwl_giveWrite     (this); }
	void unlockWrite   ( void )           {        rwl_unlockWrite   (this); }
};

/******************************************************************************
 *
 * Class             : ReadLock
 *
 * Description       : create and initialize a read guard object
 *
 * Constructor parameters
 *   rwl             : RWLock object
 *
 ******************************************************************************/

struct ReadLock
{
	 ReadLock( RWLock &_rwl ): lck_(_rwl), result_(lck_.lockRead()) { assert(result_==E_SUCCESS); }
	~ReadLock( void ) { if (result_ == E_SUCCESS) lck_.unlockRead(); }

	ReadLock( ReadLock&& ) = default;
	ReadLock( const ReadLock& ) = delete;
	ReadLock& operator=( ReadLock&& ) = delete;
	ReadLock& operator=( const ReadLock& ) = delete;

	private:
	RWLock &lck_;
	const int result_;
};

/******************************************************************************
 *
 * Class             : WriteLock
 *
 * Description       : create and initialize a write guard object
 *
 * Constructor parameters
 *   rwl             : RWLock object
 *
 ******************************************************************************/

struct WriteLock
{
	 WriteLock( RWLock &_rwl ): lck_(_rwl), result_(lck_.lockWrite()) { assert(result_==E_SUCCESS); }
	~WriteLock( void ) { if (result_ == E_SUCCESS) lck_.unlockWrite(); }

	WriteLock( WriteLock&& ) = default;
	WriteLock( const WriteLock& ) = delete;
	WriteLock& operator=( WriteLock&& ) = delete;
	WriteLock& operator=( const WriteLock& ) = delete;

	private:
	RWLock &lck_;
	const int result_;
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_RWL_H

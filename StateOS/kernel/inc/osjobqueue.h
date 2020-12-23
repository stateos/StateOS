/******************************************************************************

    @file    StateOS: osjobqueue.h
    @author  Rajmund Szymanski
    @date    23.12.2020
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

#ifndef __STATEOS_JOB_H
#define __STATEOS_JOB_H

#include "oskernel.h"
#include "osclock.h"

/******************************************************************************
 *
 * Name              : job queue
 *
 ******************************************************************************/

typedef struct __job job_t, * const job_id;

struct __job
{
	obj_t    obj;   // object header

	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned head;  // first element to read from data buffer
	unsigned tail;  // first element to write into data buffer
	fun_t ** data;  // data buffer
};

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : _JOB_INIT
 *
 * Description       : create and initialize a job queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored job procedures)
 *   data            : job queue data buffer
 *
 * Return            : job queue object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _JOB_INIT( _limit, _data ) { _OBJ_INIT(), 0, _limit, 0, 0, _data }

/******************************************************************************
 *
 * Name              : _JOB_DATA
 *
 * Description       : create a job queue data buffer
 *
 * Parameters
 *   limit           : size of a queue (max number of stored job procedures)
 *
 * Return            : job queue data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#ifndef __cplusplus
#define               _JOB_DATA( _limit ) (fun_t *[_limit]){ NULL }
#endif

/******************************************************************************
 *
 * Name              : OS_JOB
 *
 * Description       : define and initialize a job queue object
 *
 * Parameters
 *   job             : name of a pointer to job queue object
 *   limit           : size of a queue (max number of stored job procedures)
 *
 ******************************************************************************/

#define             OS_JOB( job, limit )                                \
                       fun_t *job##__buf[limit];                         \
                       job_t job##__job = _JOB_INIT( limit, job##__buf ); \
                       job_id job = & job##__job

/******************************************************************************
 *
 * Name              : static_JOB
 *
 * Description       : define and initialize a static job queue object
 *
 * Parameters
 *   job             : name of a pointer to job queue object
 *   limit           : size of a queue (max number of stored job procedures)
 *
 ******************************************************************************/

#define         static_JOB( job, limit )                                \
                static fun_t *job##__buf[limit];                         \
                static job_t job##__job = _JOB_INIT( limit, job##__buf ); \
                static job_id job = & job##__job

/******************************************************************************
 *
 * Name              : JOB_INIT
 *
 * Description       : create and initialize a job queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored job procedures)
 *
 * Return            : job queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                JOB_INIT( limit ) \
                      _JOB_INIT( limit, _JOB_DATA( limit ) )
#endif

/******************************************************************************
 *
 * Name              : JOB_CREATE
 * Alias             : JOB_NEW
 *
 * Description       : create and initialize a job queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored job procedures)
 *
 * Return            : pointer to job queue object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                JOB_CREATE( limit ) \
           (job_t[]) { JOB_INIT  ( limit ) }
#define                JOB_NEW \
                       JOB_CREATE
#endif

/******************************************************************************
 *
 * Name              : job_init
 *
 * Description       : initialize a job queue object
 *
 * Parameters
 *   job             : pointer to job queue object
 *   data            : job queue data buffer
 *   bufsize         : size of the data buffer (in bytes)
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void job_init( job_t *job, fun_t **data, size_t bufsize );

/******************************************************************************
 *
 * Name              : job_create
 * Alias             : job_new
 *
 * Description       : create and initialize a new job queue object
 *
 * Parameters
 *   limit           : size of a queue (max number of stored job procedures)
 *
 * Return            : pointer to job queue object
 *   NULL            : object not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

job_t *job_create( unsigned limit );

__STATIC_INLINE
job_t *job_new( unsigned limit ) { return job_create(limit); }

/******************************************************************************
 *
 * Name              : job_reset
 * Alias             : job_kill
 *
 * Description       : reset the job queue object and wake up all waiting tasks with 'E_STOPPED' event
 *
 * Parameters
 *   job             : pointer to job queue object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void job_reset( job_t *job );

__STATIC_INLINE
void job_kill( job_t *job ) { job_reset(job); }

/******************************************************************************
 *
 * Name              : job_destroy
 * Alias             : job_delete
 *
 * Description       : reset the job queue object, wake up all waiting tasks with 'E_DELETED' event value and free allocated resource
 *
 * Parameters
 *   job             : pointer to job queue object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void job_destroy( job_t *job );

__STATIC_INLINE
void job_delete( job_t *job ) { job_destroy(job); }

/******************************************************************************
 *
 * Name              : job_take
 * Alias             : job_tryWait
 * ISR alias         : job_takeISR
 * Async alias       : job_takeAsync
 *
 * Description       : try to transfer job data from the job queue object and execute the job procedure,
 *                     don't wait if the job queue object is empty
 *
 * Parameters
 *   job             : pointer to job queue object
 *
 * Return
 *   E_SUCCESS       : job data was successfully transferred from the job queue object
 *   E_TIMEOUT       : job queue object is empty, try again
 *
 * Note              : can be used in both thread and handler mode
 *                     use ISR alias in blockable interrupt handlers
 *                     use Async alias for communication with unblockable interrupt handlers
 *
 ******************************************************************************/

int job_take( job_t *job );

__STATIC_INLINE
int job_tryWait( job_t *job ) { return job_take(job); }

__STATIC_INLINE
int job_takeISR( job_t *job ) { return job_take(job); }

#if OS_ATOMICS
int job_takeAsync( job_t *job );
#endif

/******************************************************************************
 *
 * Name              : job_waitFor
 *
 * Description       : try to transfer job data from the job queue object and execute the job procedure,
 *                     wait for given duration of time while the job queue object is empty
 *
 * Parameters
 *   job             : pointer to job queue object
 *   delay           : duration of time (maximum number of ticks to wait while the job queue object is empty)
 *                     IMMEDIATE: don't wait if the job queue object is empty
 *                     INFINITE:  wait indefinitely while the job queue object is empty
 *
 * Return
 *   E_SUCCESS       : job data was successfully transferred from the job queue object
 *   E_STOPPED       : job queue object was reseted before the specified timeout expired
 *   E_DELETED       : job queue object was deleted before the specified timeout expired
 *   E_TIMEOUT       : job queue object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int job_waitFor( job_t *job, cnt_t delay );

/******************************************************************************
 *
 * Name              : job_waitUntil
 *
 * Description       : try to transfer job data from the job queue object and execute the job procedure,
 *                     wait until given timepoint while the job queue object is empty
 *
 * Parameters
 *   job             : pointer to job queue object
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : job data was successfully transferred from the job queue object
 *   E_STOPPED       : job queue object was reseted before the specified timeout expired
 *   E_DELETED       : job queue object was deleted before the specified timeout expired
 *   E_TIMEOUT       : job queue object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int job_waitUntil( job_t *job, cnt_t time );

/******************************************************************************
 *
 * Name              : job_wait
 * Async alias       : job_waitAsync
 *
 * Description       : try to transfer job data from the job queue object and execute the job procedure,
 *                     wait indefinitely while the job queue object is empty
 *
 * Parameters
 *   job             : pointer to job queue object
 *
 * Return
 *   E_SUCCESS       : job data was successfully transferred from the job queue object
 *   E_STOPPED       : job queue object was reseted (unavailable for async version)
 *   E_DELETED       : job queue object was deleted (unavailable for async version)
 *
 * Note              : use only in thread mode
 *                     use Async alias for communication with unblockable interrupt handlers
 *
 ******************************************************************************/

__STATIC_INLINE
int job_wait( job_t *job ) { return job_waitFor(job, INFINITE); }

#if OS_ATOMICS
int job_waitAsync( job_t *job );
#endif

/******************************************************************************
 *
 * Name              : job_give
 * ISR alias         : job_giveISR
 * Async alias       : job_giveAsync
 *
 * Description       : try to transfer job data to the job queue object,
 *                     don't wait if the job queue object is full
 *
 * Parameters
 *   job             : pointer to job queue object
 *   fun             : pointer to job procedure
 *
 * Return
 *   E_SUCCESS       : job data was successfully transferred to the job queue object
 *   E_TIMEOUT       : job queue object is full, try again
 *
 * Note              : can be used in both thread and handler mode
 *                     use ISR alias in blockable interrupt handlers
 *                     use Async alias for communication with unblockable interrupt handlers
 *
 ******************************************************************************/

int job_give( job_t *job, fun_t *fun );

__STATIC_INLINE
int job_giveISR( job_t *job, fun_t *fun ) { return job_give(job, fun); }

#if OS_ATOMICS
int job_giveAsync( job_t *job, fun_t *fun );
#endif

/******************************************************************************
 *
 * Name              : job_sendFor
 *
 * Description       : try to transfer job data to the job queue object,
 *                     wait for given duration of time while the job queue object is full
 *
 * Parameters
 *   job             : pointer to job queue object
 *   fun             : pointer to job procedure
 *   delay           : duration of time (maximum number of ticks to wait while the job queue object is full)
 *                     IMMEDIATE: don't wait if the job queue object is full
 *                     INFINITE:  wait indefinitely while the job queue object is full
 *
 * Return
 *   E_SUCCESS       : job data was successfully transferred to the job queue object
 *   E_STOPPED       : job queue object was reseted before the specified timeout expired
 *   E_DELETED       : job queue object was deleted before the specified timeout expired
 *   E_TIMEOUT       : job queue object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int job_sendFor( job_t *job, fun_t *fun, cnt_t delay );

/******************************************************************************
 *
 * Name              : job_sendUntil
 *
 * Description       : try to transfer job data to the job queue object,
 *                     wait until given timepoint while the job queue object is full
 *
 * Parameters
 *   job             : pointer to job queue object
 *   fun             : pointer to job procedure
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : job data was successfully transferred to the job queue object
 *   E_STOPPED       : job queue object was reseted before the specified timeout expired
 *   E_DELETED       : job queue object was deleted before the specified timeout expired
 *   E_TIMEOUT       : job queue object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

int job_sendUntil( job_t *job, fun_t *fun, cnt_t time );

/******************************************************************************
 *
 * Name              : job_send
 * Async alias       : job_sendAsync
 *
 * Description       : try to transfer job data to the job queue object,
 *                     wait indefinitely while the job queue object is full
 *
 * Parameters
 *   job             : pointer to job queue object
 *   fun             : pointer to job procedure
 *
 * Return
 *   E_SUCCESS       : job data was successfully transferred to the job queue object
 *   E_STOPPED       : job queue object was reseted (unavailable for async version)
 *   E_DELETED       : job queue object was deleted (unavailable for async version)
 *
 * Note              : use only in thread mode
 *                     use Async alias for communication with unblockable interrupt handlers
 *
 ******************************************************************************/

__STATIC_INLINE
int job_send( job_t *job, fun_t *fun ) { return job_sendFor(job, fun, INFINITE); }

#if OS_ATOMICS
int job_sendAsync( job_t *job, fun_t *fun );
#endif

/******************************************************************************
 *
 * Name              : job_push
 * ISR alias         : job_pushISR
 *
 * Description       : try to transfer job data to the job queue object,
 *                     remove the oldest job data if the job queue object is full
 *
 * Parameters
 *   job             : pointer to job queue object
 *   fun             : pointer to job procedure
 *
 * Return            : none
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

void job_push( job_t *job, fun_t *fun );

__STATIC_INLINE
void job_pushISR( job_t *job, fun_t *fun ) { job_push(job, fun); }

/******************************************************************************
 *
 * Name              : job_count
 * ISR alias         : job_countISR
 *
 * Description       : return the amount of data contained in the job queue
 *
 * Parameters
 *   job             : pointer to job queue object
 *
 * Return            : amount of data contained in the job queue
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

unsigned job_count( job_t *job );

__STATIC_INLINE
unsigned job_countISR( job_t *job ) { return job_count(job); }

/******************************************************************************
 *
 * Name              : job_space
 * ISR alias         : job_spaceISR
 *
 * Description       : return the amount of free space in the job queue
 *
 * Parameters
 *   job             : pointer to job queue object
 *
 * Return            : amount of free space in the job queue
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

unsigned job_space( job_t *job );

__STATIC_INLINE
unsigned job_spaceISR( job_t *job ) { return job_space(job); }

/******************************************************************************
 *
 * Name              : job_limit
 * ISR alias         : job_limitISR
 *
 * Description       : return the size of the job queue
 *
 * Parameters
 *   job             : pointer to job queue object
 *
 * Return            : size of the job queue
 *
 * Note              : can be used in both thread and handler mode (for blockable interrupts)
 *                     use ISR alias in blockable interrupt handlers
 *
 ******************************************************************************/

unsigned job_limit( job_t *job );

__STATIC_INLINE
unsigned job_limitISR( job_t *job ) { return job_limit(job); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : JobQueueT<>
 *
 * Description       : create and initialize a job queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored job procedures)
 *
 ******************************************************************************/

template<unsigned limit_>
struct JobQueueT : public __job
{
	constexpr
	JobQueueT( void ): __job _JOB_INIT(limit_, data_) {}

	JobQueueT( JobQueueT&& ) = default;
	JobQueueT( const JobQueueT& ) = delete;
	JobQueueT& operator=( JobQueueT&& ) = delete;
	JobQueueT& operator=( const JobQueueT& ) = delete;

	~JobQueueT( void ) { assert(__job::obj.queue == nullptr); }

#if __cplusplus >= 201402
	using Ptr = std::unique_ptr<JobQueueT<limit_>>;
#else
	using Ptr = JobQueueT<limit_> *;
#endif

/******************************************************************************
 *
 * Name              : JobQueueT<>::Create
 *
 * Description       : create dynamic object with manageable resources
 *
 * Parameters
 *   limit           : size of a queue (max number of stored job procedures)
 *
 * Return            : std::unique_pointer / pointer to JobQueueT<> object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

	static
	Ptr Create( void )
	{
		auto job = new JobQueueT<limit_>();
		if (job != nullptr)
			job->__job::obj.res = job;
		return Ptr(job);
	}

	void     reset    ( void )                        {        job_reset    (this); }
	void     kill     ( void )                        {        job_kill     (this); }
	void     destroy  ( void )                        {        job_destroy  (this); }
	int      take     ( void )                        { return job_take     (this); }
	int      tryWait  ( void )                        { return job_tryWait  (this); }
	int      takeISR  ( void )                        { return job_takeISR  (this); }
	template<typename T>
	int      waitFor  ( const T _delay )              { return job_waitFor  (this, Clock::count(_delay)); }
	template<typename T>
	int      waitUntil( const T _time )               { return job_waitUntil(this, Clock::until(_time)); }
	int      wait     ( void )                        { return job_wait     (this); }
	int      give     ( fun_t *_fun )                 { return job_give     (this, _fun); }
	int      giveISR  ( fun_t *_fun )                 { return job_giveISR  (this, _fun); }
	template<typename T>
	int      sendFor  ( fun_t *_fun, const T _delay ) { return job_sendFor  (this, _fun, Clock::count(_delay)); }
	template<typename T>
	int      sendUntil( fun_t *_fun, const T _time )  { return job_sendUntil(this, _fun, Clock::until(_time)); }
	int      send     ( fun_t *_fun )                 { return job_send     (this, _fun); }
	void     push     ( fun_t *_fun )                 {        job_push     (this, _fun); }
	void     pushISR  ( fun_t *_fun )                 {        job_pushISR  (this, _fun); }
	unsigned count    ( void )                        { return job_count    (this); }
	unsigned countISR ( void )                        { return job_countISR (this); }
	unsigned space    ( void )                        { return job_space    (this); }
	unsigned spaceISR ( void )                        { return job_spaceISR (this); }
	unsigned limit    ( void )                        { return job_limit    (this); }
	unsigned limitISR ( void )                        { return job_limitISR (this); }
#if OS_ATOMICS
	int      takeAsync( void )                        { return job_takeAsync(this); }
	int      waitAsync( void )                        { return job_waitAsync(this); }
	int      giveAsync( fun_t *_fun )                 { return job_giveAsync(this, _fun); }
	int      sendAsync( fun_t *_fun )                 { return job_sendAsync(this, _fun); }
#endif

	private:
	fun_t *data_[limit_];
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_JOB_H

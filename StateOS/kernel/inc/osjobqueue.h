/******************************************************************************

    @file    StateOS: osjobqueue.h
    @author  Rajmund Szymanski
    @date    18.05.2020
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

#define             OS_JOB( job, limit )                                    \
                       struct { job_t job; fun_t *buf[limit]; } job##__wrk = \
                       { _JOB_INIT( limit, job##__wrk.buf ), { NULL } };      \
                       job_id job = & job##__wrk.job

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

#define         static_JOB( job, limit )                                    \
                static struct { job_t job; fun_t *buf[limit]; } job##__wrk = \
                       { _JOB_INIT( limit, job##__wrk.buf ), { NULL } };      \
                static job_id job = & job##__wrk.job

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

void job_init( job_t *job, fun_t **data, unsigned bufsize );

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
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned job_take( job_t *job );

__STATIC_INLINE
unsigned job_tryWait( job_t *job ) { return job_take(job); }

__STATIC_INLINE
unsigned job_takeISR( job_t *job ) { return job_take(job); }

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

unsigned job_waitFor( job_t *job, cnt_t delay );

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

unsigned job_waitUntil( job_t *job, cnt_t time );

/******************************************************************************
 *
 * Name              : job_wait
 *
 * Description       : try to transfer job data from the job queue object and execute the job procedure,
 *                     wait indefinitely while the job queue object is empty
 *
 * Parameters
 *   job             : pointer to job queue object
 *
 * Return
 *   E_SUCCESS       : job data was successfully transferred from the job queue object
 *   E_STOPPED       : job queue object was reseted
 *   E_DELETED       : job queue object was deleted
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned job_wait( job_t *job ) { return job_waitFor(job, INFINITE); }

/******************************************************************************
 *
 * Name              : job_give
 * ISR alias         : job_giveISR
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
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned job_give( job_t *job, fun_t *fun );

__STATIC_INLINE
unsigned job_giveISR( job_t *job, fun_t *fun ) { return job_give(job, fun); }

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

unsigned job_sendFor( job_t *job, fun_t *fun, cnt_t delay );

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

unsigned job_sendUntil( job_t *job, fun_t *fun, cnt_t time );

/******************************************************************************
 *
 * Name              : job_send
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
 *   E_STOPPED       : job queue object was reseted
 *   E_DELETED       : job queue object was deleted
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned job_send( job_t *job, fun_t *fun ) { return job_sendFor(job, fun, INFINITE); }

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
 * Note              : may be used both in thread and handler mode
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
 * Note              : may be used both in thread and handler mode
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
 * Note              : may be used both in thread and handler mode
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
 * Note              : may be used both in thread and handler mode
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
	JobQueueT( void ): __job _JOB_INIT(limit_, data_) {}

	JobQueueT( JobQueueT&& ) = default;
	JobQueueT( const JobQueueT& ) = delete;
	JobQueueT& operator=( JobQueueT&& ) = delete;
	JobQueueT& operator=( const JobQueueT& ) = delete;

	~JobQueueT( void ) { assert(__job::obj.queue == nullptr); }

/******************************************************************************
 *
 * Name              : JobQueueT<>::Create
 *
 * Description       : create dynamic object with manageable resources
 *
 * Parameters
 *   limit           : size of a queue (max number of stored job procedures)
 *
 * Return            : pointer to JobQueueT<> object
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

	static
	JobQueueT<limit_> *Create( void )
	{
#if __cplusplus >= 201402
		auto job = reinterpret_cast<JobQueueT<limit_> *>(sys_alloc(sizeof(JobQueueT<limit_>)));
		new (job) JobQueueT<limit_>();
		job->__job::obj.res = job;
		return job;
#else
		return reinterpret_cast<JobQueueT<limit_> *>(job_create(limit_));
#endif
	}

	void reset    ( void )                        {        job_reset    (this); }
	void kill     ( void )                        {        job_kill     (this); }
	void destroy  ( void )                        {        job_destroy  (this); }
	template<typename T>
	uint waitFor  ( const T _delay )              { return job_waitFor  (this, Clock::count(_delay)); }
	template<typename T>
	uint waitUntil( const T _time )               { return job_waitUntil(this, Clock::until(_time)); }
	uint wait     ( void )                        { return job_wait     (this); }
	uint take     ( void )                        { return job_take     (this); }
	uint tryWait  ( void )                        { return job_tryWait  (this); }
	uint takeISR  ( void )                        { return job_takeISR  (this); }
	template<typename T>
	uint sendFor  ( fun_t *_fun, const T _delay ) { return job_sendFor  (this, _fun, Clock::count(_delay)); }
	template<typename T>
	uint sendUntil( fun_t *_fun, const T _time )  { return job_sendUntil(this, _fun, Clock::until(_time)); }
	uint send     ( fun_t *_fun )                 { return job_send     (this, _fun); }
	uint give     ( fun_t *_fun )                 { return job_give     (this, _fun); }
	uint giveISR  ( fun_t *_fun )                 { return job_giveISR  (this, _fun); }
	void push     ( fun_t *_fun )                 {        job_push     (this, _fun); }
	void pushISR  ( fun_t *_fun )                 {        job_pushISR  (this, _fun); }
	uint count    ( void )                        { return job_count    (this); }
	uint countISR ( void )                        { return job_countISR (this); }
	uint space    ( void )                        { return job_space    (this); }
	uint spaceISR ( void )                        { return job_spaceISR (this); }
	uint limit    ( void )                        { return job_limit    (this); }
	uint limitISR ( void )                        { return job_limitISR (this); }

	private:
	fun_t *data_[limit_];
};

#endif//__cplusplus

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_JOB_H

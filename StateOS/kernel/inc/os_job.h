/******************************************************************************

    @file    StateOS: os_job.h
    @author  Rajmund Szymanski
    @date    12.04.2018
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

#ifndef __STATEOS_JOB_H
#define __STATEOS_JOB_H

#include "oskernel.h"
#include "os_box.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : job queue
 *
 ******************************************************************************/

typedef struct __job job_t, * const job_id;

struct __job
{
	tsk_t  * queue; // inherited from semaphore
	void   * res;   // allocated job queue object's resource
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned first; // first element to read from queue
	unsigned next;  // next element to write into queue
	fun_t ** data;  // job queue data
};

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

#define               _JOB_INIT( _limit, _data ) { 0, 0, 0, _limit, 0, 0, _data }

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
#define               _JOB_DATA( _limit ) (fun_t *[_limit]){ 0 }
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

#define             OS_JOB( job, limit )                                 \
                       fun_t *job##__buf[limit];                          \
                       job_t  job##__job = _JOB_INIT( limit, job##__buf ); \
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

#define         static_JOB( job, limit )                                 \
                static fun_t *job##__buf[limit];                          \
                static job_t  job##__job = _JOB_INIT( limit, job##__buf ); \
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
             & (job_t) JOB_INIT  ( limit )
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
 *   limit           : size of a queue (max number of stored job procedures)
 *   data            : job queue data buffer
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void job_init( job_t *job, unsigned limit, fun_t **data );

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
 * Return            : pointer to job queue object (job queue successfully created)
 *   0               : job queue not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

job_t *job_create( unsigned limit );

__STATIC_INLINE
job_t *job_new( unsigned limit ) { return job_create(limit); }

/******************************************************************************
 *
 * Name              : job_kill
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

void job_kill( job_t *job );

/******************************************************************************
 *
 * Name              : job_delete
 *
 * Description       : reset the job queue object and free allocated resource
 *
 * Parameters
 *   job             : pointer to job queue object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void job_delete( job_t *job );

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
 *   E_SUCCESS       : job data was successfully transfered from the job queue object
 *   E_STOPPED       : job queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : job queue object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned job_waitUntil( job_t *job, cnt_t time );

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
 *   E_SUCCESS       : job data was successfully transfered from the job queue object
 *   E_STOPPED       : job queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : job queue object is empty and was not received data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned job_waitFor( job_t *job, cnt_t delay );

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
 *   E_SUCCESS       : job data was successfully transfered from the job queue object
 *   E_STOPPED       : job queue object was killed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned job_wait( job_t *job ) { return job_waitFor(job, INFINITE); }

/******************************************************************************
 *
 * Name              : job_take
 * ISR alias         : job_takeISR
 *
 * Description       : try to transfer job data from the job queue object and execute the job procedure,
 *                     don't wait if the job queue object is empty
 *
 * Parameters
 *   job             : pointer to job queue object
 *
 * Return
 *   E_SUCCESS       : job data was successfully transfered from the job queue object
 *   E_TIMEOUT       : job queue object is empty
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned job_take( job_t *job );

__STATIC_INLINE
unsigned job_takeISR( job_t *job ) { return job_take(job); }

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
 *   E_SUCCESS       : job data was successfully transfered to the job queue object
 *   E_STOPPED       : job queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : job queue object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned job_sendUntil( job_t *job, fun_t *fun, cnt_t time );

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
 *   E_SUCCESS       : job data was successfully transfered to the job queue object
 *   E_STOPPED       : job queue object was killed before the specified timeout expired
 *   E_TIMEOUT       : job queue object is full and was not issued data before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned job_sendFor( job_t *job, fun_t *fun, cnt_t delay );

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
 *   E_SUCCESS       : job data was successfully transfered to the job queue object
 *   E_STOPPED       : job queue object was killed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned job_send( job_t *job, fun_t *fun ) { return job_sendFor(job, fun, INFINITE); }

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
 *   E_SUCCESS       : job data was successfully transfered to the job queue object
 *   E_TIMEOUT       : job queue object is full
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned job_give( job_t *job, fun_t *fun );

__STATIC_INLINE
unsigned job_giveISR( job_t *job, fun_t *fun ) { return job_give(job, fun); }

/******************************************************************************
 *
 * Name              : job_push
 * ISR alias         : job_pushISR
 *
 * Description       : transfer job data to the job queue object,
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

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : baseJobQueue
 *
 * Description       : create and initialize a job queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored job procedures)
 *   data            : job queue data buffer
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#if OS_FUNCTIONAL

struct baseJobQueue : public __box
{
	 explicit
	 baseJobQueue( const unsigned _limit, FUN_t * const _data ): __box _BOX_INIT( _limit, sizeof(FUN_t), reinterpret_cast<char *>(_data) ) {}
	~baseJobQueue( void ) { assert(queue == nullptr); }

	void     kill     ( void )                     {                              box_kill     (this);                                                              }
	unsigned waitUntil( cnt_t _time )              { FUN_t _fun; unsigned event = box_waitUntil(this, &_fun, _time);  if (event == E_SUCCESS) _fun(); return event; }
	unsigned waitFor  ( cnt_t _delay )             { FUN_t _fun; unsigned event = box_waitFor  (this, &_fun, _delay); if (event == E_SUCCESS) _fun(); return event; }
	unsigned wait     ( void )                     { FUN_t _fun; unsigned event = box_wait     (this, &_fun);         if (event == E_SUCCESS) _fun(); return event; }
	unsigned take     ( void )                     { FUN_t _fun; unsigned event = box_take     (this, &_fun);         if (event == E_SUCCESS) _fun(); return event; }
	unsigned sendUntil( FUN_t _fun, cnt_t _time )  {             unsigned event = box_sendUntil(this, &_fun, _time);                                  return event; }
	unsigned sendFor  ( FUN_t _fun, cnt_t _delay ) {             unsigned event = box_sendFor  (this, &_fun, _delay);                                 return event; }
	unsigned send     ( FUN_t _fun )               {             unsigned event = box_send     (this, &_fun);                                         return event; }
	unsigned give     ( FUN_t _fun )               {             unsigned event = box_give     (this, &_fun);                                         return event; }
	unsigned giveISR  ( FUN_t _fun )               {             unsigned event = box_giveISR  (this, &_fun);                                         return event; }
	void     push     ( FUN_t _fun )               {                              box_push     (this, &_fun);                                                       }
	void     pushISR  ( FUN_t _fun )               {                              box_pushISR  (this, &_fun);                                                       }
};

#else

struct baseJobQueue : public __job
{
	 explicit
	 baseJobQueue( const unsigned _limit, FUN_t * const _data ): __job _JOB_INIT( _limit, _data ) {}
	~baseJobQueue( void ) { assert(queue == nullptr); }

	void     kill     ( void )                     {        job_kill     (this);               }
	unsigned waitUntil( cnt_t _time )              { return job_waitUntil(this, _time);        }
	unsigned waitFor  ( cnt_t _delay )             { return job_waitFor  (this, _delay);       }
	unsigned wait     ( void )                     { return job_wait     (this);               }
	unsigned take     ( void )                     { return job_take     (this);               }
	unsigned sendUntil( FUN_t _fun, cnt_t _time )  { return job_sendUntil(this, _fun, _time);  }
	unsigned sendFor  ( FUN_t _fun, cnt_t _delay ) { return job_sendFor  (this, _fun, _delay); }
	unsigned send     ( FUN_t _fun )               { return job_send     (this, _fun);         }
	unsigned give     ( FUN_t _fun )               { return job_give     (this, _fun);         }
	unsigned giveISR  ( FUN_t _fun )               { return job_giveISR  (this, _fun);         }
	void     push     ( FUN_t _fun )               {        job_push     (this, _fun);         }
	void     pushISR  ( FUN_t _fun )               {        job_pushISR  (this, _fun);         }
};

#endif

/******************************************************************************
 *
 * Class             : JobQueue
 *
 * Description       : create and initialize a job queue object
 *
 * Constructor parameters
 *   limit           : size of a queue (max number of stored job procedures)
 *
 ******************************************************************************/

template<unsigned _limit>
struct JobQueueT : public baseJobQueue
{
	explicit
	JobQueueT( void ): baseJobQueue(_limit, data_) {}

	private:
	FUN_t data_[_limit];
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_JOB_H

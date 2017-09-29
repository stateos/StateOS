/******************************************************************************

    @file    StateOS: os_job.h
    @author  Rajmund Szymanski
    @date    29.09.2017
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

#ifndef __STATEOS_JOB_H
#define __STATEOS_JOB_H

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job queue                                                                                      *
 *                                                                                                                    *
 **********************************************************************************************************************/

typedef struct __job job_t, * const job_id;

struct __job
{
	tsk_t  * queue; // inherited from semaphore
	unsigned count; // inherited from semaphore
	unsigned limit; // inherited from semaphore

	unsigned first; // first element to read from queue
	unsigned next;  // next element to write into queue
	fun_t ** data;  // job queue data
	unsigned prio;  // job queue priority
};

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _JOB_INIT                                                                                      *
 *                                                                                                                    *
 * Description       : create and initilize a job queue object                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *   data            : job queue data buffer                                                                          *
 *   prio            : job queue priority                                                                             *
 *                                                                                                                    *
 * Return            : job queue object                                                                               *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define               _JOB_INIT( _limit, _data, _prio ) { 0, 0, _limit, 0, 0, _data, _prio }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : _JOB_DATA                                                                                      *
 *                                                                                                                    *
 * Description       : create a job queue data buffer                                                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *                                                                                                                    *
 * Return            : job queue data buffer                                                                          *
 *                                                                                                                    *
 * Note              : for internal use                                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define               _JOB_DATA( _limit ) (fun_t *[_limit]){ 0 }
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_JOB                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a job queue object                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : name of a pointer to job queue object                                                          *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *   prio            : job queue priority                                                                             *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_JOB( job, limit, prio )                                \
                       fun_t*job##__buf[limit];                                \
                       job_t job##__job = _JOB_INIT( limit, job##__buf, prio ); \
                       job_id job = & job##__job

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_JOB                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static job queue object                                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : name of a pointer to job queue object                                                          *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *   prio            : job queue priority                                                                             *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_JOB( job, limit, prio )                                \
                static fun_t*job##__buf[limit];                                \
                static job_t job##__job = _JOB_INIT( limit, job##__buf, prio ); \
                static job_id job = & job##__job

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : JOB_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a job queue object                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *   prio            : job queue priority                                                                             *
 *                                                                                                                    *
 * Return            : job queue object                                                                               *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                JOB_INIT( limit, prio ) \
                      _JOB_INIT( limit, _JOB_DATA( limit ), prio )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : JOB_CREATE                                                                                     *
 * Alias             : JOB_NEW                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize a job queue object                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *   prio            : job queue priority                                                                             *
 *                                                                                                                    *
 * Return            : pointer to job queue object                                                                    *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                JOB_CREATE( limit, prio ) \
             & (job_t) JOB_INIT  ( limit, prio )
#define                JOB_NEW \
                       JOB_CREATE
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_init                                                                                       *
 *                                                                                                                    *
 * Description       : initilize a job queue object                                                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : pointer to job queue object                                                                    *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *   data            : job queue data buffer                                                                          *
 *   prio            : job queue priority                                                                             *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void job_init( job_t *job, unsigned limit, fun_t **data, unsigned prio );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_create                                                                                     *
 * Alias             : job_new                                                                                        *
 *                                                                                                                    *
 * Description       : create and initilize a new job queue object                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *   prio            : job queue priority                                                                             *
 *                                                                                                                    *
 * Return            : pointer to job queue object (job queue successfully created)                                   *
 *   0               : job queue not created (not enough free memory)                                                 *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

job_t *job_create( unsigned limit, unsigned prio );
__STATIC_INLINE
job_t *job_new   ( unsigned limit, unsigned prio ) { return job_create(limit, prio); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_kill                                                                                       *
 *                                                                                                                    *
 * Description       : reset the job queue object and wake up all waiting tasks with 'E_STOPPED' event                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : pointer to job queue object                                                                    *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

void job_kill( job_t *job );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_waitUntil                                                                                  *
 *                                                                                                                    *
 * Description       : try to transfer job data from the job queue object and execute the job procedure,              *
 *                     wait until given timepoint while the job queue object is empty                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : pointer to job queue object                                                                    *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : job data was successfully transfered from the job queue object                                 *
 *   E_STOPPED       : job queue object was killed before the specified timeout expired                               *
 *   E_TIMEOUT       : job queue object is empty and was not received data before the specified timeout expired       *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned job_waitUntil( job_t *job, uint32_t time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_waitFor                                                                                    *
 *                                                                                                                    *
 * Description       : try to transfer job data from the job queue object and execute the job procedure,              *
 *                     wait for given duration of time while the job queue object is empty                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : pointer to job queue object                                                                    *
 *   delay           : duration of time (maximum number of ticks to wait while the job queue object is empty)         *
 *                     IMMEDIATE: don't wait if the job queue object is empty                                         *
 *                     INFINITE:  wait indefinitly while the job queue object is empty                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : job data was successfully transfered from the job queue object                                 *
 *   E_STOPPED       : job queue object was killed before the specified timeout expired                               *
 *   E_TIMEOUT       : job queue object is empty and was not received data before the specified timeout expired       *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned job_waitFor( job_t *job, uint32_t delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_wait                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer job data from the job queue object and execute the job procedure,              *
 *                     wait indefinitly while the job queue object is empty                                           *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : pointer to job queue object                                                                    *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : job data was successfully transfered from the job queue object                                 *
 *   E_STOPPED       : job queue object was killed                                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned job_wait( job_t *job ) { return job_waitFor(job, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_take                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer job data from the job queue object and execute the job procedure,              *
 *                     don't wait if the job queue object is empty                                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : pointer to job queue object                                                                    *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : job data was successfully transfered from the job queue object                                 *
 *   E_TIMEOUT       : job queue object is empty                                                                      *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned job_take( job_t *job ) { return job_waitFor(job, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_sendUntil                                                                                  *
 *                                                                                                                    *
 * Description       : try to transfer job data to the job queue object,                                              *
 *                     wait until given timepoint while the job queue object is full                                  *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : pointer to job queue object                                                                    *
 *   proc            : pointer to job procedure                                                                       *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : job data was successfully transfered to the job queue object                                   *
 *   E_STOPPED       : job queue object was killed before the specified timeout expired                               *
 *   E_TIMEOUT       : job queue object is full and was not issued data before the specified timeout expired          *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned job_sendUntil( job_t *job, fun_t *proc, uint32_t time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_sendFor                                                                                    *
 *                                                                                                                    *
 * Description       : try to transfer job data to the job queue object,                                              *
 *                     wait for given duration of time while the job queue object is full                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : pointer to job queue object                                                                    *
 *   proc            : pointer to job procedure                                                                       *
 *   delay           : duration of time (maximum number of ticks to wait while the job queue object is full)          *
 *                     IMMEDIATE: don't wait if the job queue object is full                                          *
 *                     INFINITE:  wait indefinitly while the job queue object is full                                 *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : job data was successfully transfered to the job queue object                                   *
 *   E_STOPPED       : job queue object was killed before the specified timeout expired                               *
 *   E_TIMEOUT       : job queue object is full and was not issued data before the specified timeout expired          *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

unsigned job_sendFor( job_t *job, fun_t *proc, uint32_t delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_send                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer job data to the job queue object,                                              *
 *                     wait indefinitly while the job queue object is full                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : pointer to job queue object                                                                    *
 *   proc            : pointer to job procedure                                                                       *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : job data was successfully transfered to the job queue object                                   *
 *   E_STOPPED       : job queue object was killed                                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned job_send( job_t *job, fun_t *proc ) { return job_sendFor(job, proc, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_give                                                                                       *
 *                                                                                                                    *
 * Description       : try to transfer job data to the job queue object,                                              *
 *                     don't wait if the job queue object is full                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : pointer to job queue object                                                                    *
 *   proc            : pointer to job procedure                                                                       *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : job data was successfully transfered to the job queue object                                   *
 *   E_TIMEOUT       : job queue object is full                                                                       *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned job_give( job_t *job, fun_t *proc ) { return job_sendFor(job, proc, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : job_giveISR                                                                                    *
 *                                                                                                                    *
 * Description       : try to transfer job data to the job queue object,                                              *
 *                     don't wait if the job queue object is full                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   job             : pointer to job queue object                                                                    *
 *   proc            : pointer to job procedure                                                                       *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : job data was successfully transfered to the job queue object                                   *
 *   E_TIMEOUT       : job queue object is full                                                                       *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

__STATIC_INLINE
unsigned job_giveISR( job_t *job, fun_t *proc ) { return job_sendFor(job, proc, IMMEDIATE); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/**********************************************************************************************************************
 *                                                                                                                    *
 * Class             : JobQueue                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a job queue object                                                        *
 *                                                                                                                    *
 * Constructor parameters                                                                                             *
 *   limit           : size of a queue (max number of stored job procedures)                                          *
 *                                                                                                                    *
 **********************************************************************************************************************/

template<unsigned _limit>
struct JobQueueT : public __job
{
	 explicit
	 JobQueueT( unsigned _prio = 0 ): __job _JOB_INIT(_limit, _data, _prio) {}
	~JobQueueT( void ) { assert(queue == nullptr); }

	void     kill     ( void )                          {        job_kill     (this);                }
	unsigned waitUntil( uint32_t _time  )               { return job_waitUntil(this, _time);         }
	unsigned waitFor  ( uint32_t _delay )               { return job_waitFor  (this, _delay);        }
	unsigned wait     ( void )                          { return job_wait     (this);                }
	unsigned take     ( void )                          { return job_take     (this);                }
	unsigned sendUntil( fun_t *_proc, uint32_t _time  ) { return job_sendUntil(this, _proc, _time);  }
	unsigned sendFor  ( fun_t *_proc, uint32_t _delay ) { return job_sendFor  (this, _proc, _delay); }
	unsigned send     ( fun_t *_proc )                  { return job_send     (this, _proc);         }
	unsigned give     ( fun_t *_proc )                  { return job_give     (this, _proc);         }
	unsigned giveISR  ( fun_t *_proc )                  { return job_giveISR  (this, _proc);         }

	private:
	fun_t *_data[_limit];
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_JOB_H

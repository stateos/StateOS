/******************************************************************************

    @file    StateOS: osflag.h
    @author  Rajmund Szymanski
    @date    31.07.2018
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

#ifndef __STATEOS_FLG_H
#define __STATEOS_FLG_H

#include "oskernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *
 * Name              : flag
 *
 ******************************************************************************/

typedef struct __flg flg_t, * const flg_id;

struct __flg
{
	tsk_t  * queue; // next process in the DELAYED queue
	void   * res;   // allocated flag object's resource
	unsigned flags; // flag's current value
};

/* -------------------------------------------------------------------------- */

#define flgAny        ( 0U << 0 )
#define flgAll        ( 1U << 0 )
#define flgProtect    ( 1U << 1 )
#define flgIgnore     ( 1U << 2 )
#define flgMASK       ( 7U )

#define flgAnyProtect ( flgAny | flgProtect )
#define flgAllProtect ( flgAll | flgProtect )
#define flgAnyIgnore  ( flgAny | flgIgnore  )
#define flgAllIgnore  ( flgAll | flgIgnore  )

/******************************************************************************
 *
 * Name              : _FLG_INIT
 *
 * Description       : create and initialize a flag object
 *
 * Parameters        : none
 *
 * Return            : flag object
 *
 * Note              : for internal use
 *
 ******************************************************************************/

#define               _FLG_INIT() { 0, 0, 0 }

/******************************************************************************
 *
 * Name              : OS_FLG
 *
 * Description       : define and initialize a flag object
 *
 * Parameters
 *   flg             : name of a pointer to flag object
 *
 ******************************************************************************/

#define             OS_FLG( flg )                     \
                       flg_t flg##__flg = _FLG_INIT(); \
                       flg_id flg = & flg##__flg

/******************************************************************************
 *
 * Name              : static_FLG
 *
 * Description       : define and initialize a static flag object
 *
 * Parameters
 *   flg             : name of a pointer to flag object
 *
 ******************************************************************************/

#define         static_FLG( flg )                     \
                static flg_t flg##__flg = _FLG_INIT(); \
                static flg_id flg = & flg##__flg

/******************************************************************************
 *
 * Name              : FLG_INIT
 *
 * Description       : create and initialize a flag object
 *
 * Parameters        : none
 *
 * Return            : flag object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                FLG_INIT() \
                      _FLG_INIT()
#endif

/******************************************************************************
 *
 * Name              : FLG_CREATE
 * Alias             : FLG_NEW
 *
 * Description       : create and initialize a flag object
 *
 * Parameters        : none
 *
 * Return            : pointer to flag object
 *
 * Note              : use only in 'C' code
 *
 ******************************************************************************/

#ifndef __cplusplus
#define                FLG_CREATE() \
           (flg_t[]) { FLG_INIT  () }
#define                FLG_NEW \
                       FLG_CREATE
#endif

/******************************************************************************
 *
 * Name              : flg_init
 *
 * Description       : initialize a flag object
 *
 * Parameters
 *   flg             : pointer to flag object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void flg_init( flg_t *flg );

/******************************************************************************
 *
 * Name              : flg_create
 * Alias             : flg_new
 *
 * Description       : create and initialize a new flag object
 *
 * Parameters        : none
 *
 * Return            : pointer to flag object (flag successfully created)
 *   0               : flag not created (not enough free memory)
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

flg_t *flg_create( void );

__STATIC_INLINE
flg_t *flg_new( void ) { return flg_create(); }

/******************************************************************************
 *
 * Name              : flg_kill
 *
 * Description       : reset the flag object and wake up all waiting tasks with 'E_STOPPED' event value
 *
 * Parameters
 *   flg             : pointer to flag object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void flg_kill( flg_t *flg );

/******************************************************************************
 *
 * Name              : flg_delete
 *
 * Description       : reset the flag object and free allocated resource
 *
 * Parameters
 *   flg             : pointer to flag object
 *
 * Return            : none
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

void flg_delete( flg_t *flg );

/******************************************************************************
 *
 * Name              : flg_waitFor
 *
 * Description       : wait on flag object for given flags for given duration of time
 *
 * Parameters
 *   flg             : pointer to flag object
 *   flags           : all flags to wait
 *   mode            : waiting mode
 *                     flgAny:     wait for any flags to be set
 *                     flgAll:     wait for all flags to be set
 *                     flgProtect: don't clear flags in flag object
 *                     flgIgnore:  ignore flags in flag object that have been set and not accepted before
 *                     ( either flgAny or flgAll can be OR'ed with flgProtect or flgIgnore )
 *   delay           : duration of time (maximum number of ticks to wait on flag object for given flags)
 *                     IMMEDIATE: don't wait until requested flags have been set
 *                     INFINITE:  wait indefinitely until requested flags have been set
 *
 * Return
 *   E_SUCCESS       : requested flags have been set before the specified timeout expired
 *   E_STOPPED       : flag object was killed before the specified timeout expired
 *   E_TIMEOUT       : requested flags have not been set before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned flg_waitFor( flg_t *flg, unsigned flags, unsigned mode, cnt_t delay );

/******************************************************************************
 *
 * Name              : flg_waitUntil
 *
 * Description       : wait on flag object for given flags until given timepoint
 *
 * Parameters
 *   flg             : pointer to flag object
 *   flags           : all flags to wait
 *   mode            : waiting mode
 *                     flgAny:     wait for any flags to be set
 *                     flgAll:     wait for all flags to be set
 *                     flgProtect: don't clear flags in flag object
 *                     flgIgnore:  ignore flags in flag object that have been set and not accepted before
 *                     ( either flgAny or flgAll can be OR'ed with flgProtect or flgIgnore )
 *   time            : timepoint value
 *
 * Return
 *   E_SUCCESS       : requested flags have been set before the specified timeout expired
 *   E_STOPPED       : flag object was killed before the specified timeout expired
 *   E_TIMEOUT       : requested flags have not been set before the specified timeout expired
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

unsigned flg_waitUntil( flg_t *flg, unsigned flags, unsigned mode, cnt_t time );

/******************************************************************************
 *
 * Name              : flg_wait
 *
 * Description       : wait indefinitely on flag object until requested flags have been set
 *
 * Parameters
 *   flg             : pointer to flag object
 *   flags           : all flags to wait
 *   mode            : waiting mode
 *                     flgAny:     wait for any flags to be set
 *                     flgAll:     wait for all flags to be set
 *                     flgProtect: don't clear flags in flag object
 *                     flgIgnore:  ignore flags in flag object that have been set and not accepted before
 *                     ( either flgAny or flgAll can be OR'ed with flgProtect or flgIgnore )
 *
 * Return
 *   E_SUCCESS       : requested flags have been set
 *   E_STOPPED       : flag object was killed
 *
 * Note              : use only in thread mode
 *
 ******************************************************************************/

__STATIC_INLINE
unsigned flg_wait( flg_t *flg, unsigned flags, unsigned mode ) { return flg_waitFor(flg, flags, mode, INFINITE); }

/******************************************************************************
 *
 * Name              : flg_take
 * ISR alias         : flg_takeISR
 *
 * Description       : don't wait on flag object until requested flags have been set
 *
 * Parameters
 *   flg             : pointer to flag object
 *   flags           : all flags to wait
 *   mode            : waiting mode
 *                     flgAny:     wait for any flags to be set
 *                     flgAll:     wait for all flags to be set
 *                     flgProtect: don't clear flags in flag object
 *                     flgIgnore:  ignore flags in flag object that have been set and not accepted before
 *                     ( either flgAny or flgAll can be OR'ed with flgProtect or flgIgnore )
 *
 * Return
 *   E_SUCCESS       : requested flags have been set
 *   E_TIMEOUT       : requested flags have not been set
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned flg_take( flg_t *flg, unsigned flags, unsigned mode );

__STATIC_INLINE
unsigned flg_takeISR( flg_t *flg, unsigned flags, unsigned mode ) { return flg_take(flg, flags, mode); }

/******************************************************************************
 *
 * Name              : flg_give
 * ISR alias         : flg_giveISR
 *
 * Description       : set given flags in flag object
 *
 * Parameters
 *   flg             : pointer to flag object
 *   flags           : all flags to set
 *
 * Return            : flags in flag object after setting
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned flg_give( flg_t *flg, unsigned flags );

__STATIC_INLINE
unsigned flg_giveISR( flg_t *flg, unsigned flags ) { return flg_give(flg, flags); }

/******************************************************************************
 *
 * Name              : flg_clear
 * ISR alias         : flg_clearISR
 *
 * Description       : clear given flags in flag object
 *
 * Parameters
 *   flg             : pointer to flag object
 *   flags           : all flags to clear
 *
 * Return            : flags in flag object before clearing
 *
 * Note              : may be used both in thread and handler mode
 *
 ******************************************************************************/

unsigned flg_clear( flg_t *flg, unsigned flags );

__STATIC_INLINE
unsigned flg_clearISR( flg_t *flg, unsigned flags ) { return flg_clear(flg, flags); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

/******************************************************************************
 *
 * Class             : Flag
 *
 * Description       : create and initialize a flag object
 *
 * Constructor parameters
 *                   : none
 *
 ******************************************************************************/

struct Flag : public __flg
{
	 explicit
	 Flag( void ): __flg _FLG_INIT() {}
	~Flag( void ) { assert(queue == nullptr); }

	void     kill     ( void )                                          {        flg_kill     (this);                        }
	unsigned waitFor  ( unsigned _flags, unsigned _mode, cnt_t _delay ) { return flg_waitFor  (this, _flags, _mode, _delay); }
	unsigned waitUntil( unsigned _flags, unsigned _mode, cnt_t _time )  { return flg_waitUntil(this, _flags, _mode, _time);  }
	unsigned wait     ( unsigned _flags, unsigned _mode = flgAll )      { return flg_wait     (this, _flags, _mode);         }
	unsigned take     ( unsigned _flags, unsigned _mode = flgAll )      { return flg_take     (this, _flags, _mode);         }
	unsigned takeISR  ( unsigned _flags, unsigned _mode = flgAll )      { return flg_takeISR  (this, _flags, _mode);         }
	unsigned give     ( unsigned _flags )                               { return flg_give     (this, _flags);                }
	unsigned giveISR  ( unsigned _flags )                               { return flg_giveISR  (this, _flags);                }
	unsigned clear    ( unsigned _flags )                               { return flg_clear    (this, _flags);                }
	unsigned clearISR ( unsigned _flags )                               { return flg_clearISR (this, _flags);                }
};

#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOS_FLG_H

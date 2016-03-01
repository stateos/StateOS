/******************************************************************************

    @file    StateOS: os_flg.h
    @author  Rajmund Szymanski
    @date    01.03.2016
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

#pragma once

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : flag                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define flgOne       ( 0U << 0 )
#define flgAll       ( 1U << 0 )
#define flgAccept    ( 0U << 1 )
#define flgIgnore    ( 1U << 1 )
#define flgMASK      ( 3U )

#define flgOneAccept ( flgOne | flgAccept )
#define flgAllAccept ( flgAll | flgAccept )
#define flgOneIgnore ( flgOne | flgIgnore )
#define flgAllIgnore ( flgAll | flgIgnore )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_FLG                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a flag object                                                             *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   flg             : name of a pointer to flag object                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_FLG( flg )                     \
                       flg_t flg##__flg = _FLG_INIT(); \
                       flg_id flg = & flg##__flg

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_FLG                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static flag object                                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   flg             : name of a pointer to flag object                                                               *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_FLG( flg )                     \
                static flg_t flg##__flg = _FLG_INIT(); \
                static flg_id flg = & flg##__flg

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : FLG_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : create and initilize a flag object                                                             *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : flag object                                                                                    *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                FLG_INIT() \
                      _FLG_INIT()

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : FLG_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a flag object                                                             *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to flag object                                                                         *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                FLG_CREATE() \
               &(flg_t)FLG_INIT()
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : flg_create                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a new flag object                                                         *
 *                                                                                                                    *
 * Parameters        : none                                                                                           *
 *                                                                                                                    *
 * Return            : pointer to flag object (flag successfully created)                                             *
 *   0               : flag not created (not enough free memory)                                                      *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              flg_id   flg_create( void );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : flg_kill                                                                                       *
 *                                                                                                                    *
 * Description       : reset the flag object and wake up all waiting tasks with 'E_STOPPED' event value               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   flg             : pointer to flag object                                                                         *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     flg_kill( flg_id flg );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : flg_waitUntil                                                                                  *
 *                                                                                                                    *
 * Description       : wait on flag object for given flags until given timepoint                                      *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   flg             : pointer to flag object                                                                         *
 *   flags           : all flags to wait                                                                              *
 *   mode            : waiting mode                                                                                   *
 *                     flgOne:    wait for any flags to be set                                                        *
 *                     flgAll:    wait for all flags to be set                                                        *
 *                     flgAccept: accept flags that have been set and not accepted before                             *
 *                     flgIgnore: ignore flags that have been set and not accepted before                             *
 *                     ( either flgOne or flgAll can be OR'ed with flgAccept or flgIgnore )                           *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : requested flags have been set before the specified timeout expired                             *
 *   E_STOPPED       : flag object was killed before the specified timeout expired                                    *
 *   E_TIMEOUT       : requested flags have not been set before the specified timeout expired                         *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned flg_waitUntil( flg_id flg, unsigned flags, unsigned mode, unsigned time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : flg_waitFor                                                                                    *
 *                                                                                                                    *
 * Description       : wait on flag object for given flags for given duration of time                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   flg             : pointer to flag object                                                                         *
 *   flags           : all flags to wait                                                                              *
 *   mode            : waiting mode                                                                                   *
 *                     flgOne:    wait for any flags to be set                                                        *
 *                     flgAll:    wait for all flags to be set                                                        *
 *                     flgAccept: accept flags that have been set and not accepted before                             *
 *                     flgIgnore: ignore flags that have been set and not accepted before                             *
 *                     ( either flgOne or flgAll can be OR'ed with flgAccept or flgIgnore )                           *
 *   delay           : duration of time (maximum number of ticks to wait on flag object for given flags)              *
 *                     IMMEDIATE: don't wait until requested flags have been set                                      *
 *                     INFINITE:  wait indefinitly until requested flags have been set                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : requested flags have been set before the specified timeout expired                             *
 *   E_STOPPED       : flag object was killed before the specified timeout expired                                    *
 *   E_TIMEOUT       : requested flags have not been set before the specified timeout expired                         *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              unsigned flg_waitFor( flg_id flg, unsigned flags, unsigned mode, unsigned delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : flg_wait                                                                                       *
 *                                                                                                                    *
 * Description       : wait indefinitly on flag object until requested flags have been set                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   flg             : pointer to flag object                                                                         *
 *   flags           : all flags to wait                                                                              *
 *   mode            : waiting mode                                                                                   *
 *                     flgOne:    wait for any flags to be set                                                        *
 *                     flgAll:    wait for all flags to be set                                                        *
 *                     flgAccept: accept flags that have been set and not accepted before                             *
 *                     flgIgnore: ignore flags that have been set and not accepted before                             *
 *                     ( either flgOne or flgAll can be OR'ed with flgAccept or flgIgnore )                           *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : requested flags have been set                                                                  *
 *   E_STOPPED       : flag object was killed                                                                         *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned flg_wait( flg_id flg, unsigned flags, unsigned mode ) { return flg_waitFor(flg, flags, mode, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : flg_take                                                                                       *
 *                                                                                                                    *
 * Description       : don't wait on flag object until requested flags have been set                                  *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   flg             : pointer to flag object                                                                         *
 *   flags           : all flags to wait                                                                              *
 *   mode            : waiting mode                                                                                   *
 *                     flgOne:    wait for any flags to be set                                                        *
 *                     flgAll:    wait for all flags to be set                                                        *
 *                     flgAccept: accept flags that have been set and not accepted before                             *
 *                     flgIgnore: ignore flags that have been set and not accepted before                             *
 *                     ( either flgOne or flgAll can be OR'ed with flgAccept or flgIgnore )                           *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : requested flags have been set                                                                  *
 *   E_TIMEOUT       : requested flags have not been set                                                              *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned flg_take( flg_id flg, unsigned flags, unsigned mode ) { return flg_waitFor(flg, flags, mode, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : flg_takeISR                                                                                    *
 *                                                                                                                    *
 * Description       : don't wait on flag object until requested flags have been set                                  *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   flg             : pointer to flag object                                                                         *
 *   flags           : all flags to wait                                                                              *
 *   mode            : waiting mode                                                                                   *
 *                     flgOne:    wait for any flags to be set                                                        *
 *                     flgAll:    wait for all flags to be set                                                        *
 *                     flgAccept: accept flags that have been set and not accepted before                             *
 *                     flgIgnore: ignore flags that have been set and not accepted before                             *
 *                     ( either flgOne or flgAll can be OR'ed with flgAccept or flgIgnore )                           *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : requested flags have been set                                                                  *
 *   E_TIMEOUT       : requested flags have not been set                                                              *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline unsigned flg_takeISR( flg_id flg, unsigned flags, unsigned mode ) { return flg_waitFor(flg, flags, mode, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : flg_give                                                                                       *
 *                                                                                                                    *
 * Description       : set given flags in flag object                                                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   flg             : pointer to flag object                                                                         *
 *   flags           : all flags to set                                                                               *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     flg_give( flg_id flg, unsigned flags );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : flg_giveISR                                                                                    *
 *                                                                                                                    *
 * Description       : set given flags in flag object                                                                 *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   flg             : pointer to flag object                                                                         *
 *   flags           : all flags to set                                                                               *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Note              : use only in handler mode                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

static inline void     flg_giveISR( flg_id flg, unsigned flags ) { flg_give(flg, flags); }

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

class Flag : public flg_t
{
public:

	 Flag( void ): flg_t(_FLG_INIT()) {}

	~Flag( void ) { flg_kill(this); }

	unsigned waitUntil( unsigned _flags, unsigned _mode, unsigned _time )  { return flg_waitUntil(this, _flags, _mode, _time);  }
	unsigned waitFor  ( unsigned _flags, unsigned _mode, unsigned _delay ) { return flg_waitFor  (this, _flags, _mode, _delay); }
	unsigned wait     ( unsigned _flags, unsigned _mode = flgAllAccept )   { return flg_wait     (this, _flags, _mode);         }
	unsigned take     ( unsigned _flags, unsigned _mode = flgAllAccept )   { return flg_take     (this, _flags, _mode);         }
	unsigned takeISR  ( unsigned _flags, unsigned _mode = flgAllAccept )   { return flg_takeISR  (this, _flags, _mode);         }
	void     give     ( unsigned _flags )                                  {        flg_give     (this, _flags);                }
	void     giveISR  ( unsigned _flags )                                  {        flg_giveISR  (this, _flags);                }
};

#endif

/* -------------------------------------------------------------------------- */

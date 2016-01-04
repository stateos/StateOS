/******************************************************************************

    @file    State Machine OS: os_mtx.h
    @author  Rajmund Szymanski
    @date    04.01.2016
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

#define mtxNormal                       ( 0U << 0 ) // normal mutex
#define mtxRecursive                    ( 1U << 0 ) // recursive mutex
#define mtxPriorityProtect              ( 0U << 1 ) // priority protect mutex
#define mtxPriorityInheritance          ( 1U << 1 ) // priority inheritance mutex

#define mtxNormalPriorityProtect        ( mtxNormal    | mtxPriorityProtect     )
#define mtxRecursivePriorityProtect     ( mtxRecursive | mtxPriorityProtect     )
#define mtxNormalPriorityInheritance    ( mtxNormal    | mtxPriorityInheritance )
#define mtxRecursivePriorityInheritance ( mtxRecursive | mtxPriorityInheritance )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_MTX                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a mutex object                                                            *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : name of a pointer to mutex object                                                              *
 *   type            : mutex type                                                                                     *
 *                     mtxNormal:                       normal    priority protect     mutex                          *
 *                     mtxRecursive:                    recursive priority protect     mutex                          *
 *                     mtxPriorityProtect:              normal    priority protect     mutex                          *
 *                     mtxPriorityInheritance:          normal    priority inheritance mutex                          *
 *                     mtxNormalPriorityProtect:        normal    priority protect     mutex                          *
 *                     mtxRecursivePriorityProtect:     recursive priority protect     mutex                          *
 *                     mtxNormalPriorityInheritance:    normal    priority inheritance mutex                          *
 *                     mtxRecursivePriorityInheritance: recursive priority inheritance mutex                          *
 *                                                                                                                    *
 **********************************************************************************************************************/
#define     OS_MTX( mtx, type )                   \
               mtx_t mtx##__mtx = _MTX_INIT(type); \
               mtx_id mtx = & mtx##__mtx

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_MTX                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static mutex object                                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : name of a pointer to mutex object                                                              *
 *   type            : mutex type                                                                                     *
 *                     mtxNormal:                       normal    priority protect     mutex                          *
 *                     mtxRecursive:                    recursive priority protect     mutex                          *
 *                     mtxPriorityProtect:              normal    priority protect     mutex                          *
 *                     mtxPriorityInheritance:          normal    priority inheritance mutex                          *
 *                     mtxNormalPriorityProtect:        normal    priority protect     mutex                          *
 *                     mtxRecursivePriorityProtect:     recursive priority protect     mutex                          *
 *                     mtxNormalPriorityInheritance:    normal    priority inheritance mutex                          *
 *                     mtxRecursivePriorityInheritance: recursive priority inheritance mutex                          *
 *                                                                                                                    *
 **********************************************************************************************************************/
#define static_MTX( mtx, type )                   \
        static mtx_t mtx##__mtx = _MTX_INIT(type); \
        static mtx_id mtx = & mtx##__mtx

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_create                                                                                     *
 *                                                                                                                    *
 * Description       : create and initilize a new mutex object                                                        *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   type            : mutex type                                                                                     *
 *                     mtxNormal:                       normal    priority protect     mutex                          *
 *                     mtxRecursive:                    recursive priority protect     mutex                          *
 *                     mtxPriorityProtect:              normal    priority protect     mutex                          *
 *                     mtxPriorityInheritance:          normal    priority inheritance mutex                          *
 *                     mtxNormalPriorityProtect:        normal    priority protect     mutex                          *
 *                     mtxRecursivePriorityProtect:     recursive priority protect     mutex                          *
 *                     mtxNormalPriorityInheritance:    normal    priority inheritance mutex                          *
 *                     mtxRecursivePriorityInheritance: recursive priority inheritance mutex                          *
 *                                                                                                                    *
 * Return            : pointer to mutex object (mutex successfully created)                                           *
 *   0               : mutex not created (not enough free memory)                                                     *
 *                                                                                                                    *
 * Notes             : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/
              mtx_id   mtx_create( unsigned type );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_kill                                                                                       *
 *                                                                                                                    *
 * Description       : reset the mutex object and wake up all waiting tasks with 'E_STOPPED' event value              *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : pointer to mutex object                                                                        *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 * Notes             : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/
              void     mtx_kill( mtx_id mtx );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_waitUntil                                                                                  *
 *                                                                                                                    *
 * Description       : try lock the mutex object,                                                                     *
 *                     wait until given timepoint if the mutex object can't be locked immediately                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : pointer to mutex object                                                                        *
 *   time            : timepoint value                                                                                *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mutex object was successfully locked                                                           *
 *   E_STOPPED       : mutex object was killed before the specified timeout expired                                   *
 *   E_TIMEOUT       : mutex object was not locked before the specified timeout expired                               *
 *   'another'       : task was resumed with 'another' event                                                          *
 *                                                                                                                    *
 * Notes             : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/
              unsigned mtx_waitUntil( mtx_id mtx, unsigned time );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_waitFor                                                                                    *
 *                                                                                                                    *
 * Description       : try lock the mutex object,                                                                     *
 *                     wait for given duration of time if the mutex object can't be locked immediately                *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : pointer to mutex object                                                                        *
 *   delay           : duration of time (maximum number of ticks to wait for lock the mutex object)                   *
 *                     IMMEDIATE: don't wait if the mutex object can't be locked immediately                          *
 *                     INFINITE:  wait indefinitly until the mutex object have been locked                            *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mutex object was successfully locked                                                           *
 *   E_STOPPED       : mutex object was killed before the specified timeout expired                                   *
 *   E_TIMEOUT       : mutex object was not locked before the specified timeout expired                               *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Notes             : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/
              unsigned mtx_waitFor( mtx_id mtx, unsigned delay );

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_wait                                                                                       *
 *                                                                                                                    *
 * Description       : try lock the mutex object,                                                                     *
 *                     wait indefinitly if the mutex object can't be locked immediately                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : pointer to mutex object                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mutex object was successfully locked                                                           *
 *   E_STOPPED       : mutex object was killed                                                                        *
 *   'another'       : task was resumed with 'another' event value                                                    *
 *                                                                                                                    *
 * Notes             : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/
static inline unsigned mtx_wait( mtx_id mtx ) { return mtx_waitFor(mtx, INFINITE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_take                                                                                       *
 *                                                                                                                    *
 * Description       : try lock the mutex object,                                                                     *
 *                     don't wait if the mutex object can't be locked immediately                                     *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : pointer to mutex object                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mutex object was successfully locked                                                           *
 *   E_TIMEOUT       : mutex object can't be locked immediately                                                       *
 *                                                                                                                    *
 * Notes             : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/
static inline unsigned mtx_take( mtx_id mtx ) { return mtx_waitFor(mtx, IMMEDIATE); }

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : mtx_give                                                                                       *
 *                                                                                                                    *
 * Description       : try unlock the mutex object (only owner task can unlock mutex object),                         *
 *                     don't wait if the mutex object can't be unlocked                                               *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   mtx             : pointer to mutex object                                                                        *
 *                                                                                                                    *
 * Return                                                                                                             *
 *   E_SUCCESS       : mutex object was successfully unlocked                                                         *
 *   E_TIMEOUT       : mutex object can't be unlocked                                                                 *
 *                                                                                                                    *
 * Notes             : use only in thread mode                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/
              unsigned mtx_give( mtx_id mtx );

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus

#include <string.h>

class Mutex : public mtx_t
{
public:

	 Mutex( unsigned _type = mtxRecursive|mtxPriorityInheritance ): mtx_t(_MTX_INIT(_type)) {}

	~Mutex( void ) { mtx_kill(this); }

	unsigned waitUntil( unsigned _time  ) { return mtx_waitUntil(this, _time);  }
	unsigned waitFor  ( unsigned _delay ) { return mtx_waitFor  (this, _delay); }
	unsigned wait     ( void )            { return mtx_wait     (this);         }
	unsigned take     ( void )            { return mtx_take     (this);         }
	unsigned give     ( void )            { return mtx_give     (this);         }
};

#endif

/* -------------------------------------------------------------------------- */

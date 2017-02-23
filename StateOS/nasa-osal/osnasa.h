/******************************************************************************

    @file    StateOS: osnasa.h
    @author  Rajmund Szymanski
    @date    23.02.2017
    @brief   NASA OSAPI implementation for StateOS.

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

#ifndef __STATEOSNASA_H
#define __STATEOSNASA_H

#include <os.h>
#include <osapi.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/
/* queues */

typedef struct
{
	box_t  box;
	char   name [OS_MAX_API_NAME];
	uint32 creator;
	uint32 used;
}	OS_queue_record_t;

/*---------------------------------------------------------------------------*/
/* binary semaphores */

typedef struct
{
	sem_t  sem;
	char   name [OS_MAX_API_NAME];
	uint32 creator;
	uint32 used;
}	OS_bin_sem_record_t;

/*---------------------------------------------------------------------------*/
/* counting semaphores */

typedef struct
{
	sem_t  sem;
	char   name [OS_MAX_API_NAME];
	uint32 creator;
	uint32 used;
}	OS_count_sem_record_t;

/*---------------------------------------------------------------------------*/
/* mutexes */

typedef struct
{
	mtx_t  mtx;
	char   name [OS_MAX_API_NAME];
	uint32 creator;
	uint32 used;
}	OS_mut_sem_record_t;

/*---------------------------------------------------------------------------*/
/* tasks */

typedef struct
{
	tsk_t  tsk;
	char   name [OS_MAX_API_NAME];
	uint32 creator;
	uint32 used;
	void (*handler)(void);
	void  *stack;
	uint32 size;
	void (*delete_handler)(void);
}	OS_task_record_t;

/*---------------------------------------------------------------------------*/
/* timers */

typedef struct
{
	tmr_t  tmr;
	char   name [OS_MAX_API_NAME];
	uint32 creator;
	uint32 used;
	void (*handler)(uint32);
}	OS_timer_record_t;

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------- */

#endif//__STATEOSNASA_H

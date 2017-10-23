/******************************************************************************

    @file    StateOS: os.c
    @author  Rajmund Szymanski
    @date    23.10.2017
    @brief   This file provides set of functions for StateOS.

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

#include "oskernel.h"

/* -------------------------------------------------------------------------- */
uint32_t sys_time( void )
/* -------------------------------------------------------------------------- */
{
	uint32_t cnt;

	port_cnt_lock();

	cnt = Counter;

	port_cnt_unlock();

	return cnt;
}

/* -------------------------------------------------------------------------- */

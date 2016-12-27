/******************************************************************************

    @file    StateOS: os_sig.c
    @author  Rajmund Szymanski
    @date    27.12.2016
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

#include <os.h>

/* -------------------------------------------------------------------------- */
sig_id sig_create( unsigned type )
/* -------------------------------------------------------------------------- */
{
	sig_id sig;

	port_sys_lock();

	sig = core_sys_alloc(sizeof(sig_t));

	if (sig)
	{
		sig->type = type & sigMASK;
	}

	port_sys_unlock();

	return sig;
}

/* -------------------------------------------------------------------------- */
void sig_kill( sig_id sig )
/* -------------------------------------------------------------------------- */
{
	assert(sig);

	port_sys_lock();

	sig->flag = 0;
	
	core_all_wakeup(sig, E_STOPPED);

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_sig_wait( sig_id sig, unsigned time, unsigned(*wait)() )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_SUCCESS;

	port_sys_lock();

	if (sig->flag)
	{
		sig->flag = sig->type;
	}
	else
	{
		event = wait(sig, time);
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned sig_waitUntil( sig_id sig, unsigned time )
/* -------------------------------------------------------------------------- */
{
	assert(sig);

	return priv_sig_wait(sig, time, core_tsk_waitUntil);
}

/* -------------------------------------------------------------------------- */
unsigned sig_waitFor( sig_id sig, unsigned delay )
/* -------------------------------------------------------------------------- */
{
	assert(sig);

	return priv_sig_wait(sig, delay, core_tsk_waitFor);
}

/* -------------------------------------------------------------------------- */
void sig_give( sig_id sig )
/* -------------------------------------------------------------------------- */
{
	assert(sig);

	port_sys_lock();

	sig->flag = 1;

	if (sig->type == sigClear)
	{
		if (core_one_wakeup(sig, E_SUCCESS))
		sig->flag = 0;
	}
	else
	{
		core_all_wakeup(sig, E_SUCCESS);
	}

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
void sig_clear( sig_id sig )
/* -------------------------------------------------------------------------- */
{
	assert(sig);

	sig->flag = 0;
}

/* -------------------------------------------------------------------------- */

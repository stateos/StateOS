/******************************************************************************

    @file    State Machine OS: oskernel.h
    @author  Rajmund Szymanski
    @date    22.12.2015
    @brief   This file defines set of kernel functions for StateOS.

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

#include <osbase.h>
#include <bitband.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

extern sys_t System; // system data

/* -------------------------------------------------------------------------- */

// initiating and running the system timer
// the port_sys_init procedure is normally called as a constructor
void port_sys_init( void );

// procedure performed in the 'idle' task; waiting for an interrupt
void port_idle_hook( void );

/* -------------------------------------------------------------------------- */

static inline
void core_ctx_reset( void )
{
#if OS_ROBIN && OS_TIMER == 0
	System.dly = 0;
#endif
	port_ctx_reset();
}

// save status of current process and switch system to the next
void core_ctx_switch( void );

// force context switch
void port_ctx_switch( void );

/* -------------------------------------------------------------------------- */

// abort and reset current process and switch system to the next
void core_tsk_loop( void ) __noreturn;

/* -------------------------------------------------------------------------- */

// force timer interrupt
void port_tmr_force( void );

// clear time breakpoint
void port_tmr_stop( void );

// set time breakpoint
void port_tmr_start( unsigned timeout );

/* -------------------------------------------------------------------------- */

// procedura alokacji pamięci na stercie
os_id core_sys_alloc( size_t size );

/* -------------------------------------------------------------------------- */

// wstawienie obiektu 'obj' do kolejki procesów / zegarów przed obiektem 'nxt'
// i ustawienie id obiektu 'obj'
void core_rdy_insert( os_id obj, unsigned id, os_id nxt );

// usunięcie obiektu 'obj' z kolejki procesów / zegarów
// i ustawienie id obiektu 'obj' na ID_STOPPED
void core_rdy_remove( os_id obj );

/* -------------------------------------------------------------------------- */

// dodanie zegara 'tmr' do listy zadań
// ewentualne uruchomienie odliczania
void core_tmr_insert( tmr_id tmr, unsigned id );

// usuniecie zegara 'tmr' z listy zadań
static inline
void core_tmr_remove( tmr_id tmr ) { core_rdy_remove(tmr); }

// obsługa przerwania zegarowego
void core_tmr_handler( void );

/* -------------------------------------------------------------------------- */

// dodanie procesu 'tsk' do listy zadań i ustawienie id = ID_READY procesu 'tsk'
// ewentualna zmiana kontekstu (jeśli kernel pracuje w trybie wywłaszczania)
void     core_tsk_insert( tsk_id tsk );

// usunięcie procesu 'tsk' z listy zadań
static inline
void     core_tsk_remove( tsk_id tsk ) { core_rdy_remove(tsk); }

// dodanie procesu 'tsk' do kolejki obiektu nadzorującego 'obj'
void     core_tsk_append( tsk_id tsk, os_id obj );

// usunięcie procesu 'tsk' z kolejki obiektu nadzorującego
// oraz listy aktywnych zegarów
// do procesu zostaje przesyłany komunikat o zdarzeniu 'event'
void     core_tsk_unlink( tsk_id tsk, unsigned event );

// zawieszenie wykonywania procesu 'tsk' do czasu 'time'
// i dołączenie do kolejki obiektu nadzorującego 'obj'
// zwraca wartość komunikatu o zdarzeniu 'event'
unsigned core_tsk_waitUntil( os_id obj, unsigned time );

// zawieszenie wykonywania procesu 'tsk' na czas 'delay'
// i dołączenie do kolejki obiektu nadzorującego 'obj'
// zwraca wartość komunikatu o zdarzeniu 'event'
unsigned core_tsk_waitFor( os_id obj, unsigned delay );

// uwolnienie procesu 'tsk' z kolejki obiektu nadzorującego
// do procesu zostaje przesyłany komunikat o zdarzeniu 'event'
// zwraca wskaźnik do uwolnionego procesu lub 0
tsk_id   core_tsk_wakeup( tsk_id tsk, unsigned event );

// uwolnienie pierwszego procesu oczekującego z kolejki obiektu 'obj'
// do procesu zostaje przesyłany komunikat o zdarzeniu 'event'
// zwraca wskaźnik do uwolnionego procesu lub 0
tsk_id   core_one_wakeup( os_id obj, unsigned event );

// uwolnienie wszystkich procesów oczekujących z kolejki obiektu 'obj'
// do procesów zostaje przesyłany komunikat o zdarzeniu 'event'
void     core_all_wakeup( os_id obj, unsigned event );

// ustawienie priorytetu procesu 'tsk' z ewentualnym przekazaniem
// sterowania do innego procesu (o wyÂższym priorytecie)
void     core_tsk_prio( tsk_id tsk, unsigned prio );

// wybranie następnego procesu do uruchomienia z listy zadań
// zwraca adres TCB wybranego procesu
tsk_id   core_tsk_handler( void );

/* -------------------------------------------------------------------------- */

// procedure inside ISR?
static inline
unsigned port_isr_inside( void )
{
	return __get_IPSR();
}

/* -------------------------------------------------------------------------- */

// reset task stack pointer
static inline
void port_set_stack( void *top )
{
	__set_PSP((unsigned)top);
}

/* -------------------------------------------------------------------------- */

#if OS_LOCK_LEVEL && (__CORTEX_M >= 3)

static inline unsigned port_get_lock( void )           { return __get_BASEPRI();                                      }
static inline void     port_put_lock( unsigned state ) {        __set_BASEPRI(state);                                 }
static inline void     port_set_lock( void )           {        __set_BASEPRI((OS_LOCK_LEVEL)<<(8-__NVIC_PRIO_BITS)); }
static inline void     port_clr_lock( void )           {        __set_BASEPRI(0);                                     }

#else

static inline unsigned port_get_lock( void )           { return __get_PRIMASK();      }
static inline void     port_put_lock( unsigned state ) {        __set_PRIMASK(state); }
static inline void     port_set_lock( void )           {        __disable_irq();      }
static inline void     port_clr_lock( void )           {         __enable_irq();      }

#endif

#define port_sys_lock()                             do { unsigned __LOCK = port_get_lock(); port_set_lock()
#define port_sys_unlock()                                port_put_lock(__LOCK); } while(0)

#define port_sys_enable()                           do { unsigned __LOCK = port_get_lock(); port_clr_lock()
#define port_sys_disable()                               port_put_lock(__LOCK); } while(0)

#define port_isr_lock()                             do { port_set_lock()
#define port_isr_unlock()                                port_clr_lock(); } while(0)

#define port_isr_enable()                           do { port_clr_lock()
#define port_isr_disable()                               port_set_lock(); } while(0)

/* -------------------------------------------------------------------------- */

static inline
void port_sys_flash( void )
{
	port_sys_enable();
	port_sys_disable();
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

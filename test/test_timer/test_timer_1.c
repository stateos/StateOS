#include "test.h"

static_TMR_DEF(tmr3) {}

static int counter;

static void proc()
{
	        sys_lock();
	        {
		        counter++;
	        }
	        sys_unlock();
}

static void test()
{
	unsigned event;

	        counter = 0;
	        tmr_startFrom(tmr3, 3, 0, proc);
	        tmr_startFrom(tmr2, 2, 0, proc);
	        tmr_startFrom(tmr1, 1, 0, proc);
	        tmr_startFrom(&tmr0, 0, 0, proc);
	event = tmr_wait(&tmr0);                     assert_success(event);
	event = tmr_wait(tmr1);                      assert_success(event);
	event = tmr_wait(tmr2);                      assert_success(event);
	event = tmr_wait(tmr3);                      assert_success(event);
	                                             assert(counter == 4);
}

void test_timer_1()
{
	TEST_Notify();
	TEST_Call();
}

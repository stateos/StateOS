#include "test.h"

static_TMR_DEF(tmr3) {}

static_TMR_START(tmr4, 4, 0) {}
static_TMR_START(tmr5, 5, 0) {}
static_TMR_START(tmr6, 6, 0) {}
static_TMR_START(tmr7, 7, 0) {}

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
	event = tmr_wait(tmr4);                      assert_success(event);
	event = tmr_wait(tmr5);                      assert_success(event);
	event = tmr_wait(tmr6);                      assert_success(event);
	event = tmr_wait(tmr7);                      assert_success(event);
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

extern "C"
void test_timer_2()
{
	TEST_Notify();
	TEST_Call();
}

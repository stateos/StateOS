#include "test.h"

static int counter = 0;

static void proc()
{
	        sys_lock();
	        {
		        counter++;
	        }
	        sys_unlock();
}

static void proc0()
{
	        one_call(&one0, proc);               assert(counter == 1);
	        tsk_stop();
}

static void test()
{
	unsigned event;
	                                             assert_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);
	event = tsk_join(&tsk0);                     assert_success(event);
}

void test_once_flag_0()
{
	int i;
	TEST_Notify();
	for (i = 0; i < PASS; i++)
		test();
}

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

static void proc1()
{
	        one_call(one1, proc);                assert(counter == 1);
	        tsk_stop();
}

static void test()
{
	unsigned event;
	                                             assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);
	event = tsk_join(tsk1);                      assert_success(event);
}

void test_once_flag_1()
{
	int i;
	TEST_Notify();
	for (i = 0; i < PASS; i++)
		test();
}

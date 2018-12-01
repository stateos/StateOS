#include "test.h"

static_ONE(one3);

static int counter = 0;

static void proc()
{
	        sys_lock();
	        {
		        counter++;
	        }
	        sys_unlock();
}

static void proc3()
{
	        one_call(one3, proc);                assert(counter == 1);
	        tsk_stop();
}

static void test()
{
	unsigned event;
	                                             assert_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);
	event = tsk_join(tsk3);                      assert_success(event);
}

void test_once_flag_3()
{
	int i;
	TEST_Notify();
	for (i = 0; i < PASS; i++)
		test();
}

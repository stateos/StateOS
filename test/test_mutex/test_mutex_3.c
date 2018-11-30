#include "test.h"

static_MTX(mtx3, mtxPrioNone);

static void proc2()
{
	unsigned event;

	event = mtx_wait(mtx3);                      assert_success(event);
	event = mtx_give(mtx3);                      assert_success(event);
	                                             assert(!"test program cannot be caught here");
}

static void proc1()
{
	unsigned event;
	                                             assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);
	event = tsk_kill(tsk2);                      assert_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;

	event = mtx_wait(mtx3);                      assert_success(event);
	                                             assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);
	event = mtx_give(mtx3);                      assert_success(event);
	event = tsk_join(tsk1);                      assert_success(event);
}

void test_mutex_3()
{
	int i;
	TEST_Notify();
	for (i = 0; i < PASS; i++)
		test();
}

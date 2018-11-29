#include "test.h"

static void proc2()
{
	unsigned event;

	event = mtx_wait(mtx1);                      assert_success(event);
	event = tsk_kill(tsk1);                      assert_success(event);
	event = mtx_give(mtx1);                      assert_success(event);
	        tsk_stop();
}

static void proc1()
{
	                                             assert_stopped(tsk2);
	        tsk_startFrom(tsk2, proc2);
	                                             assert(!"test program cannot be caught here");
}

static void test()
{
	unsigned event;

	event = mtx_wait(mtx1);                      assert_success(event);
	                                             assert_stopped(tsk1);
	        tsk_startFrom(tsk1, proc1);
	event = mtx_give(mtx1);                      assert_success(event);
	event = tsk_join(tsk2);                      assert_success(event);
}

void test_mutex_1()
{
	int i;
	TEST_Notify();
	mtx_init(mtx1, mtxPrioInherit, 0);
	for (i = 0; i < PASS; i++)
		test();
}

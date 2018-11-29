#include "test.h"

static void proc1()
{
	unsigned event;

	event = mtx_wait(mtx1);                      assert_success(event);
	        cnd_give(cnd1, cndOne);
	event = mtx_give(mtx1);                      assert_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned event;
	                                             assert_stopped(tsk1);
	        tsk_startFrom(tsk1, proc1);
	event = mtx_wait(mtx1);                      assert_success(event);
	event = cnd_wait(cnd1, mtx1);                assert_success(event);
	        cnd_give(cnd1, cndOne);
	event = mtx_give(mtx1);                      assert_success(event);
	event = tsk_join(tsk1);                      assert_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;

	event = mtx_wait(mtx1);                      assert_success(event);
	                                             assert_stopped(tsk2);
	        tsk_startFrom(tsk2, proc2);
	event = cnd_wait(cnd1, mtx1);                assert_success(event);
	event = mtx_give(mtx1);                      assert_success(event);
	event = tsk_join(tsk2);                      assert_success(event);
}

void test_condition_variable_1()
{
	int i;
	TEST_Notify();
	mtx_init(mtx1, mtxDefault, 0);
	for (i = 0; i < PASS; i++)
		test();
}

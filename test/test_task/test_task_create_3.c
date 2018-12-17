#include "test.h"

static void proc()
{
	        tsk_stop();
}

static void test()
{
	unsigned event;

	tsk_t *tsk6 = tsk_create(6, proc);
	tsk_t *tsk7 = wrk_create(7, proc, 256);
	tsk_t *tsk8 = wrk_create(8, proc, 512);
	tsk_t *tsk9 = tsk_create(9, proc);

	        tsk_start(tsk6);
	event = tsk_join(tsk6);                      assert_success(event);
	                                             assert(tsk6->hdr.obj.res == RELEASED);
	        tsk_start(tsk7);
	event = tsk_join(tsk7);                      assert_success(event);
	                                             assert(tsk7->hdr.obj.res == RELEASED);
	        tsk_start(tsk8);
	event = tsk_join(tsk8);                      assert_success(event);
	                                             assert(tsk8->hdr.obj.res == RELEASED);
	        tsk_start(tsk9);
	event = tsk_join(tsk9);                      assert_success(event);
	                                             assert(tsk9->hdr.obj.res == RELEASED);
}

void test_task_create_3()
{
	TEST_Notify();
	TEST_Call();
}

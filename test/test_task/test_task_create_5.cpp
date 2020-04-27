#include "test.h"

static void proc()
{
	        tsk_stop();
}

static void test()
{
	tsk_t *tsk6 = wrk_detached(6, proc, 256);    ASSERT(tsk6->hdr.obj.res == tsk6);
	tsk_t *tsk7 = tsk_detached(7, proc);         ASSERT(tsk7->hdr.obj.res == tsk7);
	tsk_t *tsk8 = wrk_detached(8, proc, 512);    ASSERT(tsk8->hdr.obj.res == tsk8);
	tsk_t *tsk9 = tsk_detached(9, proc);         ASSERT(tsk9->hdr.obj.res == tsk9);
	idle_tsk_destructor();
	                                             ASSERT(tsk6->hdr.obj.res == RELEASED);
	                                             ASSERT(tsk7->hdr.obj.res == RELEASED);
	                                             ASSERT(tsk8->hdr.obj.res == RELEASED);
	                                             ASSERT(tsk9->hdr.obj.res == RELEASED);
}

extern "C"
void test_task_create_5()
{
	TEST_Notify();
	TEST_Call();
}

#include "test.h"

static void proc5()
{
	unsigned event;

	event = mtx_wait(mtx2);                      ASSERT_failure(event);
	        tsk_stop();
}

static void proc4()
{
	unsigned event;
	                                             ASSERT_dead(tsk5);
	        tsk_startFrom(tsk5, proc5);
	event = tsk_join(tsk5);                      ASSERT_success(event);
	event = mtx_wait(mtx2);                      ASSERT_success(event);
	event = tsk_kill(tsk3);                      ASSERT_success(event);
	event = mtx_give(mtx2);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc3()
{
	                                             ASSERT_dead(tsk4);
	        tsk_startFrom(tsk4, proc4);          ASSERT(!"test program cannot be caught here");
}

static void proc2()
{
	unsigned event;

	event = mtx_wait(mtx2);                      ASSERT_success(event);
	event = mtx_wait(mtx1);                      ASSERT_success(event);
	event = tsk_kill(tsk1);                      ASSERT_success(event);
	event = mtx_give(mtx1);                      ASSERT_success(event);
	event = mtx_give(mtx2);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc1()
{
	                                             ASSERT_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          ASSERT(!"test program cannot be caught here");
}

static void test()
{
	unsigned event;

	event = mtx_wait(mtx1);                      ASSERT_success(event);
	                                             ASSERT_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);
	                                             ASSERT_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);
	event = mtx_give(mtx1);                      ASSERT_success(event);
	event = tsk_join(tsk3);                      ASSERT_success(event);
	event = tsk_join(tsk1);                      ASSERT_success(event);
}

void test_mutex_3()
{
	TEST_Notify();
	mtx_init(mtx1, mtxPrioProtect, 4);
	mtx_init(mtx2, mtxPrioProtect, 4);
	TEST_Call();
}

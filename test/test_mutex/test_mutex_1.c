#include "test.h"

static_MTX(mtx3, mtxRobust);

static void proc5()
{
	unsigned event;

	event = mtx_wait(mtx3);                      ASSERT_owndead(event);
	event = mtx_give(mtx3);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc4()
{
	unsigned event;

	event = mtx_wait(mtx3);                      ASSERT_success(event);
	                                             ASSERT_dead(tsk5);
	        tsk_startFrom(tsk5, proc5);          ASSERT_ready(tsk5);
	event = mtx_wait(mtx1);                      ASSERT(!"test program cannot be caught here");
}

static void proc3()
{
	unsigned event;

	event = mtx_take(mtx2);                      ASSERT_timeout(event);
	event = mtx_wait(mtx3);                      ASSERT_success(event);
	event = mtx_take(mtx3);                      ASSERT_timeout(event);
	                                             ASSERT_dead(tsk4);
	        tsk_startFrom(tsk4, proc4);          ASSERT_ready(tsk4);
	event = mtx_give(mtx3);                      ASSERT_success(event);
	event = mtx_give(mtx3);                      ASSERT_failure(event);
	event = tsk_kill(tsk4);                      ASSERT_success(event);
	event = tsk_join(tsk5);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned event;

	event = mtx_take(mtx1);                      ASSERT_timeout(event);
	event = mtx_wait(mtx2);                      ASSERT_success(event);
	event = mtx_take(mtx2);                      ASSERT_success(event);
	                                             ASSERT_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          ASSERT_dead(tsk3);
	event = tsk_join(tsk3);                      ASSERT_success(event);
	event = mtx_give(mtx2);                      ASSERT_success(event);
	event = mtx_give(mtx2);                      ASSERT_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned event;

	event = mtx_take(&mtx0);                     ASSERT_timeout(event);
	event = mtx_wait(mtx1);                      ASSERT_success(event);
	event = mtx_take(mtx1);                      ASSERT_failure(event);
	                                             ASSERT_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          ASSERT_dead(tsk2);
	event = tsk_join(tsk2);                      ASSERT_success(event);
	event = mtx_give(mtx1);                      ASSERT_success(event);
	event = mtx_give(mtx1);                      ASSERT_failure(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned event;

	event = mtx_wait(&mtx0);                     ASSERT_success(event);
	event = mtx_take(&mtx0);                     ASSERT_timeout(event);
	                                             ASSERT_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          ASSERT_dead(tsk1);
	event = tsk_join(tsk1);                      ASSERT_success(event);
	event = mtx_give(&mtx0);                     ASSERT_success(event);
	event = mtx_give(&mtx0);                     ASSERT_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;
	                                             ASSERT_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);
	event = tsk_join(&tsk0);                     ASSERT_success(event);
}

void test_mutex_1()
{
	TEST_Notify();
	mtx_init(&mtx0, mtxNormal, 0);
	mtx_init(mtx1, mtxErrorCheck, 0);
	mtx_init(mtx2, mtxRecursive, 0);
	TEST_Call();
}

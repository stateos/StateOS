#include "test.h"

static_MTX(mtx3, mtxRobust);

static void proc5()
{
	unsigned event;

	event = mtx_wait(mtx3);                      assert_owndead(event);
	event = mtx_give(mtx3);                      assert_success(event);
	        tsk_stop();
}

static void proc4()
{
	unsigned event;

	event = mtx_wait(mtx3);                      assert_success(event);
	                                             assert_dead(tsk5);
	        tsk_startFrom(tsk5, proc5);          assert_ready(tsk5);
	event = mtx_wait(mtx1);                      assert(!"test program cannot be caught here");
}

static void proc3()
{
	unsigned event;

	event = mtx_take(mtx2);                      assert_timeout(event);
	event = mtx_wait(mtx3);                      assert_success(event);
	event = mtx_take(mtx3);                      assert_timeout(event);
	                                             assert_dead(tsk4);
	        tsk_startFrom(tsk4, proc4);          assert_ready(tsk4);
	event = mtx_give(mtx3);                      assert_success(event);
	event = mtx_give(mtx3);                      assert_failure(event);
	event = tsk_kill(tsk4);                      assert_success(event);
	event = tsk_join(tsk5);                      assert_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned event;

	event = mtx_take(mtx1);                      assert_timeout(event);
	event = mtx_wait(mtx2);                      assert_success(event);
	event = mtx_take(mtx2);                      assert_success(event);
	                                             assert_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          assert_dead(tsk3);
	event = tsk_join(tsk3);                      assert_success(event);
	event = mtx_give(mtx2);                      assert_success(event);
	event = mtx_give(mtx2);                      assert_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned event;

	event = mtx_take(&mtx0);                     assert_timeout(event);
	event = mtx_wait(mtx1);                      assert_success(event);
	event = mtx_take(mtx1);                      assert_failure(event);
	                                             assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          assert_dead(tsk2);
	event = tsk_join(tsk2);                      assert_success(event);
	event = mtx_give(mtx1);                      assert_success(event);
	event = mtx_give(mtx1);                      assert_failure(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned event;

	event = mtx_wait(&mtx0);                     assert_success(event);
	event = mtx_take(&mtx0);                     assert_timeout(event);
	                                             assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          assert_dead(tsk1);
	event = tsk_join(tsk1);                      assert_success(event);
	event = mtx_give(&mtx0);                     assert_success(event);
	event = mtx_give(&mtx0);                     assert_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;
	                                             assert_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);
	event = tsk_join(&tsk0);                     assert_success(event);
}

void test_mutex_1()
{
	TEST_Notify();
	mtx_init(&mtx0, mtxNormal, 0);
	mtx_init(mtx1, mtxErrorCheck, 0);
	mtx_init(mtx2, mtxRecursive, 0);
	TEST_Call();
}

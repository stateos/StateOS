#include "test.h"

static_MTX(mtx3, mtxPrioProtect, 3);
static_CND(cnd3);

static void proc3()
{
	unsigned event;

	event = mtx_wait(mtx3);                      assert_success(event);
	event = cnd_wait(cnd3, mtx3);                assert_success(event);
	event = mtx_wait(mtx2);                      assert_success(event);
	        cnd_give(cnd2, cndOne);
	event = mtx_give(mtx2);                      assert_success(event);
	event = mtx_give(mtx3);                      assert_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned event;

	event = mtx_wait(mtx2);                      assert_success(event);
	                                             assert_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          assert_ready(tsk3);
	event = cnd_wait(cnd2, mtx2);                assert_success(event);
	event = mtx_wait(mtx3);                      assert_success(event);
	        cnd_give(cnd3, cndOne);
	event = mtx_give(mtx3);                      assert_success(event);
	event = cnd_wait(cnd2, mtx2);                assert_success(event);
	event = mtx_wait(mtx1);                      assert_success(event);
	        cnd_give(cnd1, cndOne);
	event = mtx_give(mtx1);                      assert_success(event);
	event = mtx_give(mtx2);                      assert_success(event);
	event = tsk_join(tsk3);                      assert_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned event;

	event = mtx_wait(mtx1);                      assert_success(event);
	                                             assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          assert_ready(tsk2);
	event = cnd_wait(cnd1, mtx1);                assert_success(event);
	event = mtx_wait(mtx2);                      assert_success(event);
	        cnd_give(cnd2, cndOne);
	event = mtx_give(mtx2);                      assert_success(event);
	event = cnd_wait(cnd1, mtx1);                assert_success(event);
	event = mtx_wait(&mtx0);                     assert_success(event);
	        cnd_give(&cnd0, cndOne);
	event = mtx_give(&mtx0);                     assert_success(event);
	event = mtx_give(mtx1);                      assert_success(event);
	event = tsk_join(tsk2);                      assert_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned event;

	event = mtx_wait(&mtx0);                     assert_success(event);
	                                             assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          assert_ready(tsk1);
	event = cnd_wait(&cnd0, &mtx0);              assert_success(event);
	event = mtx_wait(mtx1);                      assert_success(event);
	        cnd_give(cnd1, cndOne);
	event = mtx_give(mtx1);                      assert_success(event);
	event = cnd_wait(&cnd0, &mtx0);              assert_success(event);
	event = mtx_give(&mtx0);                     assert_success(event);
	event = tsk_join(tsk1);                      assert_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;
	                                             assert_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);         assert_ready(&tsk0);
	        tsk_yield();
	        tsk_yield();
	event = mtx_wait(&mtx0);                     assert_success(event);
	        cnd_give(&cnd0, cndOne);
	event = mtx_give(&mtx0);                     assert_success(event);
	event = tsk_join(&tsk0);                     assert_success(event);
}

void test_condition_variable_1()
{
	TEST_Notify();
	mtx_init(&mtx0, mtxDefault, 0);
	mtx_init(mtx1, mtxErrorCheck, 0);
	mtx_init(mtx2, mtxPrioInherit, 0);
	TEST_Call();
}

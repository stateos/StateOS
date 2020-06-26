#include "test.h"

static_MTX(mtx3, mtxPrioProtect, 3);
static_CND(cnd3);

static void proc3()
{
	int result;

	result = mtx_wait(mtx3);                      ASSERT_success(result);
	result = cnd_wait(cnd3, mtx3);                ASSERT_success(result);
	result = mtx_wait(mtx2);                      ASSERT_success(result);
	         cnd_give(cnd2, cndOne);
	result = mtx_give(mtx2);                      ASSERT_success(result);
	result = mtx_give(mtx3);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc2()
{
	int result;

	result = mtx_wait(mtx2);                      ASSERT_success(result);
	                                              ASSERT_dead(tsk3);
	         tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
	result = cnd_wait(cnd2, mtx2);                ASSERT_success(result);
	result = mtx_wait(mtx3);                      ASSERT_success(result);
	         cnd_give(cnd3, cndOne);
	result = mtx_give(mtx3);                      ASSERT_success(result);
	result = cnd_wait(cnd2, mtx2);                ASSERT_success(result);
	result = mtx_wait(mtx1);                      ASSERT_success(result);
	         cnd_give(cnd1, cndOne);
	result = mtx_give(mtx1);                      ASSERT_success(result);
	result = mtx_give(mtx2);                      ASSERT_success(result);
	result = tsk_join(tsk3);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc1()
{
	int result;

	result = mtx_wait(mtx1);                      ASSERT_success(result);
	                                              ASSERT_dead(tsk2);
	         tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
	result = cnd_wait(cnd1, mtx1);                ASSERT_success(result);
	result = mtx_wait(mtx2);                      ASSERT_success(result);
	         cnd_give(cnd2, cndOne);
	result = mtx_give(mtx2);                      ASSERT_success(result);
	result = cnd_wait(cnd1, mtx1);                ASSERT_success(result);
	result = mtx_wait(&mtx0);                     ASSERT_success(result);
	         cnd_give(&cnd0, cndOne);
	result = mtx_give(&mtx0);                     ASSERT_success(result);
	result = mtx_give(mtx1);                      ASSERT_success(result);
	result = tsk_join(tsk2);                      ASSERT_success(result);
	         tsk_stop();
}

static void proc0()
{
	int result;

	result = mtx_wait(&mtx0);                     ASSERT_success(result);
	                                              ASSERT_dead(tsk1);
	         tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
	result = cnd_wait(&cnd0, &mtx0);              ASSERT_success(result);
	result = mtx_wait(mtx1);                      ASSERT_success(result);
	         cnd_give(cnd1, cndOne);
	result = mtx_give(mtx1);                      ASSERT_success(result);
	result = cnd_wait(&cnd0, &mtx0);              ASSERT_success(result);
	result = mtx_give(&mtx0);                     ASSERT_success(result);
	result = tsk_join(tsk1);                      ASSERT_success(result);
	         tsk_stop();
}

static void test()
{
	int result;
	                                              ASSERT_dead(&tsk0);
	         tsk_startFrom(&tsk0, proc0);         ASSERT_ready(&tsk0);
	         tsk_yield();
	         tsk_yield();
	result = mtx_wait(&mtx0);                     ASSERT_success(result);
	         cnd_give(&cnd0, cndOne);
	result = mtx_give(&mtx0);                     ASSERT_success(result);
	result = tsk_join(&tsk0);                     ASSERT_success(result);
}

extern "C"
void test_condition_variable_2()
{
	TEST_Notify();
	mtx_init(&mtx0, mtxDefault, 0);
	mtx_init(mtx1, mtxErrorCheck, 0);
	mtx_init(mtx2, mtxPrioInherit, 0);
	TEST_Call();
}

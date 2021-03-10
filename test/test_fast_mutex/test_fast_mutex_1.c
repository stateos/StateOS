#include "test.h"

static_MUT(mut3);

static void proc3()
{
	int result;

	result = mut_take(mut2);                      ASSERT_timeout(result);
	result = mut_wait(mut3);                      ASSERT_success(result);
	result = mut_take(mut3);                      ASSERT_failure(result);
	result = mut_give(mut3);                      ASSERT_success(result);
	result = mut_give(mut3);                      ASSERT_failure(result);
	         tsk_stop();
}

static void proc2()
{
	int result;

	result = mut_take(mut1);                      ASSERT_timeout(result);
	result = mut_wait(mut2);                      ASSERT_success(result);
	result = mut_take(mut2);                      ASSERT_failure(result);
	                                              ASSERT_dead(tsk3);
	         tsk_startFrom(tsk3, proc3);          ASSERT_dead(tsk3);
	result = tsk_join(tsk3);                      ASSERT_success(result);
	result = mut_give(mut2);                      ASSERT_success(result);
	result = mut_give(mut2);                      ASSERT_failure(result);
	         tsk_stop();
}

static void proc1()
{
	int result;

	result = mut_take(&mut0);                     ASSERT_timeout(result);
	result = mut_wait(mut1);                      ASSERT_success(result);
	result = mut_take(mut1);                      ASSERT_failure(result);
	                                              ASSERT_dead(tsk2);
	         tsk_startFrom(tsk2, proc2);          ASSERT_dead(tsk2);
	result = tsk_join(tsk2);                      ASSERT_success(result);
	result = mut_give(mut1);                      ASSERT_success(result);
	result = mut_give(mut1);                      ASSERT_failure(result);
	         tsk_stop();
}

static void proc0()
{
	int result;

	result = mut_wait(&mut0);                     ASSERT_success(result);
	result = mut_take(&mut0);                     ASSERT_failure(result);
	                                              ASSERT_dead(tsk1);
	         tsk_startFrom(tsk1, proc1);          ASSERT_dead(tsk1);
	result = tsk_join(tsk1);                      ASSERT_success(result);
	result = mut_give(&mut0);                     ASSERT_success(result);
	result = mut_give(&mut0);                     ASSERT_failure(result);
	         tsk_stop();
}

static void test()
{
	int result;
	                                              ASSERT_dead(&tsk0);
	         tsk_startFrom(&tsk0, proc0);
	result = tsk_join(&tsk0);                     ASSERT_success(result);
}

void test_fast_mutex_1()
{
	TEST_Notify();
	TEST_Call();
}

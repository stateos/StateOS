#include "test.h"

static_MUT(mut3);

static void proc3()
{
	unsigned event;

	event = mut_take(mut2);                      assert_timeout(event);
	event = mut_wait(mut3);                      assert_success(event);
	event = mut_take(mut3);                      assert_failure(event);
	event = mut_give(mut3);                      assert_success(event);
	event = mut_give(mut3);                      assert_failure(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned event;

	event = mut_take(mut1);                      assert_timeout(event);
	event = mut_wait(mut2);                      assert_success(event);
	event = mut_take(mut2);                      assert_failure(event);
	                                             assert_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          assert_dead(tsk3);
	event = tsk_join(tsk3);                      assert_success(event);
	event = mut_give(mut2);                      assert_success(event);
	event = mut_give(mut2);                      assert_failure(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned event;

	event = mut_take(&mut0);                     assert_timeout(event);
	event = mut_wait(mut1);                      assert_success(event);
	event = mut_take(mut1);                      assert_failure(event);
	                                             assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          assert_dead(tsk2);
	event = tsk_join(tsk2);                      assert_success(event);
	event = mut_give(mut1);                      assert_success(event);
	event = mut_give(mut1);                      assert_failure(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned event;

	event = mut_wait(&mut0);                     assert_success(event);
	event = mut_take(&mut0);                     assert_failure(event);
	                                             assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          assert_dead(tsk1);
	event = tsk_join(tsk1);                      assert_success(event);
	event = mut_give(&mut0);                     assert_success(event);
	event = mut_give(&mut0);                     assert_failure(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;
	                                             assert_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);
	event = tsk_join(&tsk0);                     assert_success(event);
}

void test_fast_mutex_1()
{
	TEST_Notify();
	TEST_Call();
}

#include "test.h"

static_BAR(bar3, 5);

static void proc3()
{
	unsigned event;

	event = bar_wait(bar3);                      assert_success(event);
	        tsk_stop();
}

static void proc2()
{
	unsigned event;
		                                         assert_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          assert_ready(tsk3);
	event = bar_wait(bar2);                      assert_success(event);
	event = bar_wait(bar3);                      assert_success(event);
	event = tsk_join(tsk3);                      assert_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned event;
		                                         assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          assert_ready(tsk2);
	event = bar_wait(bar1);                      assert_success(event);
	event = bar_wait(bar2);                      assert_success(event);
	event = bar_wait(bar3);                      assert_success(event);
	event = tsk_join(tsk2);                      assert_success(event);
	        tsk_stop();
}

static void proc0()
{
	unsigned event;
		                                         assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          assert_ready(tsk1);
	event = bar_wait(&bar0);                     assert_success(event);
	event = bar_wait(bar1);                      assert_success(event);
	event = bar_wait(bar2);                      assert_success(event);
	event = bar_wait(bar3);                      assert_success(event);
	event = tsk_join(tsk1);                      assert_success(event);
	        tsk_stop();
}

static void test()
{
	unsigned event;
		                                         assert_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);         assert_ready(&tsk0);
	event = bar_wait(&bar0);                     assert_success(event);
	event = bar_wait(bar1);                      assert_success(event);
	event = bar_wait(bar2);                      assert_success(event);
	event = bar_wait(bar3);                      assert_success(event);
	event = tsk_join(&tsk0);                     assert_success(event);
}

extern "C"
void test_barrier_2()
{
	TEST_Notify();
	bar_init(&bar0, 2);
	bar_init(bar1, 3);
	bar_init(bar2, 4);
	TEST_Call();
}

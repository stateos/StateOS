#include "test.h"

static_BAR(bar3, 4);

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
	        tsk_startFrom(tsk3, proc3);
	event = bar_wait(bar2);                      assert_success(event);
	event = bar_wait(bar3);                      assert_success(event);
	event = tsk_join(tsk3);                      assert_success(event);
	        tsk_stop();
}

static void proc1()
{
	unsigned event;
		                                         assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);
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
	        tsk_startFrom(tsk1, proc1);
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
	        tsk_startFrom(&tsk0, proc0);
	event = tsk_join(&tsk0);                     assert_success(event);
}

extern "C"
void test_barrier_2()
{
	TEST_Notify();
	bar_init(&bar0, 1);
	bar_init(bar1, 2);
	bar_init(bar2, 3);
	TEST_Call();
}

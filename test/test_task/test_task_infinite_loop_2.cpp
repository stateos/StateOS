#include "test.h"

static void proc3()
{
	        for (;;);
}

static void proc2()
{
	unsigned event;

	        tsk_prio(3);                         assert_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          assert_ready(tsk3);
	        tsk_yield();
	event = tsk_kill(tsk3);                      assert_success(event);
	        tsk_prio(2);
	        for (;;);
}

static void proc1()
{
	unsigned event;

	        tsk_prio(2);                         assert_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          assert_ready(tsk2);
	        tsk_yield();
	event = tsk_kill(tsk2);                      assert_success(event);
	        tsk_prio(1);
	        for (;;);
}

static void proc0()
{
	unsigned event;

	        tsk_prio(1);                         assert_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          assert_ready(tsk1);
	        tsk_yield();
	event = tsk_kill(tsk1);                      assert_success(event);
	        tsk_prio(0);
	        for (;;);
}

static void test()
{
	unsigned event;
	                                             assert_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);         assert_ready(&tsk0);
	        tsk_yield();
	event = tsk_kill(&tsk0);                     assert_success(event);
}

extern "C"
void test_task_infinite_loop_2()
{
	TEST_Notify();
	TEST_Call();
}

#include "test.h"

static void proc3()
{
	        for (;;);
}

static void proc2()
{
	unsigned event;

	        tsk_prio(3);                         ASSERT_dead(tsk3);
	        tsk_startFrom(tsk3, proc3);          ASSERT_ready(tsk3);
	        tsk_yield();
	event = tsk_kill(tsk3);                      ASSERT_success(event);
	        tsk_prio(2);
	        for (;;);
}

static void proc1()
{
	unsigned event;

	        tsk_prio(2);                         ASSERT_dead(tsk2);
	        tsk_startFrom(tsk2, proc2);          ASSERT_ready(tsk2);
	        tsk_yield();
	event = tsk_kill(tsk2);                      ASSERT_success(event);
	        tsk_prio(1);
	        for (;;);
}

static void proc0()
{
	unsigned event;

	        tsk_prio(1);                         ASSERT_dead(tsk1);
	        tsk_startFrom(tsk1, proc1);          ASSERT_ready(tsk1);
	        tsk_yield();
	event = tsk_kill(tsk1);                      ASSERT_success(event);
	        tsk_prio(0);
	        for (;;);
}

static void test()
{
	unsigned event;
	                                             ASSERT_dead(&tsk0);
	        tsk_startFrom(&tsk0, proc0);         ASSERT_ready(&tsk0);
	        tsk_yield();
	event = tsk_kill(&tsk0);                     ASSERT_success(event);
}

void test_task_infinite_loop_1()
{
	TEST_Notify();
	TEST_Call();
}

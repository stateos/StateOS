#include "test.h"

static void proc3()
{
	        for (;;);
}

static void proc2()
{
	unsigned event;

	        ThisTask::setPrio(3);                ASSERT(!Tsk3);
	        Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	        ThisTask::yield();
	event = Tsk3.kill();                         ASSERT_success(event);
	        ThisTask::setPrio(2);
	        for (;;);
}

static void proc1()
{
	unsigned event;

	        ThisTask::setPrio(2);                ASSERT(!Tsk2);
	        Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	        ThisTask::yield();
	event = Tsk2.kill();                         ASSERT_success(event);
	        ThisTask::setPrio(1);
	        for (;;);
}

static void proc0()
{
	unsigned event;

	        ThisTask::setPrio(1);                ASSERT(!Tsk1);
	        Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	        ThisTask::yield();
	event = Tsk1.kill();                         ASSERT_success(event);
	        ThisTask::setPrio(0);
	        for (;;);
}

static void test()
{
	unsigned event;
	                                             ASSERT(!Tsk0);
	        Tsk0.startFrom(proc0);               ASSERT(!!Tsk0);
	        ThisTask::yield();
	event = Tsk0.kill();                         ASSERT_success(event);
}

extern "C"
void test_task_infinite_loop_3()
{
	TEST_Notify();
	TEST_Call();
}

#include "test.h"

static void proc3()
{
	        for (;;);
}

static void proc2()
{
	unsigned event;

	        ThisTask::setPrio(3);                assert(!Tsk3);
	        Tsk3.startFrom(proc3);               assert(!!Tsk3);
	        ThisTask::yield();
	event = Tsk3.kill();                         assert_success(event);
	        ThisTask::setPrio(2);
	        for (;;);
}

static void proc1()
{
	unsigned event;

	        ThisTask::setPrio(2);                assert(!Tsk2);
	        Tsk2.startFrom(proc2);               assert(!!Tsk2);
	        ThisTask::yield();
	event = Tsk2.kill();                         assert_success(event);
	        ThisTask::setPrio(1);
	        for (;;);
}

static void proc0()
{
	unsigned event;

	        ThisTask::setPrio(1);                assert(!Tsk1);
	        Tsk1.startFrom(proc1);               assert(!!Tsk1);
	        ThisTask::yield();
	event = Tsk1.kill();                         assert_success(event);
	        ThisTask::setPrio(0);
	        for (;;);
}

static void test()
{
	unsigned event;
	                                             assert(!Tsk0);
	        Tsk0.startFrom(proc0);               assert(!!Tsk0);
	        ThisTask::yield();
	event = Tsk0.kill();                         assert_success(event);
}

extern "C"
void test_task_infinite_loop_3()
{
	TEST_Notify();
	TEST_Call();
}

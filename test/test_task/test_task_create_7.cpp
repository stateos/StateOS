#include "test.h"

static void proc()
{
	        ThisTask::stop();
}

static void test()
{
	unsigned event;

	auto Tsk6 = startTaskT<512>::detached(6, proc);  ASSERT(Tsk6 && Tsk6->__tsk::hdr.obj.res == Tsk6);
	event = Tsk6->join();                            ASSERT_failure(event);

	auto Tsk7 = startTask::create(7, proc);          ASSERT(Tsk7 && Tsk7->__tsk::hdr.obj.res == Tsk7);
	event = Tsk7->join();                            ASSERT_success(event);

	auto Tsk8 = startTaskT<512>::create(8, proc);    ASSERT(Tsk8 && Tsk8->__tsk::hdr.obj.res == Tsk8);
	event = Tsk8->join();                            ASSERT_success(event);

	auto Tsk9 = startTask::detached(9, proc);        ASSERT(Tsk9 && Tsk9->__tsk::hdr.obj.res == Tsk9);
	event = Tsk9->join();                            ASSERT_failure(event);
}

extern "C"
void test_task_create_7()
{
	TEST_Notify();
	TEST_Call();
}

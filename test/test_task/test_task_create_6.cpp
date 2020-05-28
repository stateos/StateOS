#include "test.h"

static void proc()
{
	        ThisTask::stop();
}

static void test()
{
	unsigned event;

	auto Tsk6 = TaskT<512>::Detached(6, proc);  ASSERT(Tsk6);
	event = Tsk6->join();                       ASSERT_failure(event);

	auto Tsk7 = Task::Create(7, proc);          ASSERT(Tsk7);
	event = Tsk7->join();                       ASSERT_success(event);

	auto Tsk8 = TaskT<512>::Create(8, proc);    ASSERT(Tsk8);
	event = Tsk8->join();                       ASSERT_success(event);

	auto Tsk9 = Task::Detached(9, proc);        ASSERT(Tsk9);
	event = Tsk9->join();                       ASSERT_failure(event);
}

extern "C"
void test_task_create_6()
{
	TEST_Notify();
	TEST_Call();
}

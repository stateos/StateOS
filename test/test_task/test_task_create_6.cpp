#include "test.h"

using namespace stateos;

static void proc()
{
	         ThisTask::stop();
}

static void test()
{
	int result;

	auto Tsk6 = TaskT<512>::Detached(6, proc);   ASSERT(Tsk6);
	result = Tsk6->join();                       ASSERT_failure(result);

	auto Tsk7 = Task::Create(7, proc);           ASSERT(Tsk7);
	result = Tsk7->join();                       ASSERT_success(result);

	auto Tsk8 = TaskT<512>::Create(8, proc);     ASSERT(Tsk8);
	result = Tsk8->join();                       ASSERT_success(result);

	auto Tsk9 = Task::Detached(9, proc);         ASSERT(Tsk9);
	result = Tsk9->join();                       ASSERT_failure(result);
}

extern "C"
void test_task_create_6()
{
	TEST_Notify();
	TEST_Call();
}

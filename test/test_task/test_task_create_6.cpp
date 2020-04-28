#include "test.h"

static void proc()
{
	        ThisTask::stop();
}

static void test()
{
	unsigned event;

	auto Tsk6 = TaskT<256>::create(6, proc);
	auto Tsk7 = Task::create(7, proc);
	auto Tsk8 = TaskT<512>::create(8, proc);
	auto Tsk9 = Task::create(9, proc);

	        Tsk6->start();
	event = Tsk6->join();                        ASSERT_success(event);
	                                             ASSERT(Tsk6->__tsk::hdr.obj.res == RELEASED);
	        Tsk7->start();
	event = Tsk7->join();                        ASSERT_success(event);
	                                             ASSERT(Tsk7->__tsk::hdr.obj.res == RELEASED);
	        Tsk8->start();
	event = Tsk8->join();                        ASSERT_success(event);
	                                             ASSERT(Tsk8->__tsk::hdr.obj.res == RELEASED);
	        Tsk9->start();
	event = Tsk9->join();                        ASSERT_success(event);
	                                             ASSERT(Tsk9->__tsk::hdr.obj.res == RELEASED);
}

extern "C"
void test_task_create_6()
{
	TEST_Notify();
	TEST_Call();
}

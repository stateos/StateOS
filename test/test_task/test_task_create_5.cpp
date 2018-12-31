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

	                                             ASSERT(!*Tsk6);
	        Tsk6->start();                       ASSERT(!*Tsk6);
	event = Tsk6->join();                        ASSERT_success(event);
	                                             ASSERT(Tsk6->__tsk::hdr.obj.res == RELEASED);
	                                             ASSERT(!*Tsk7);
	        Tsk7->start();                       ASSERT(!*Tsk7);
	event = Tsk7->join();                        ASSERT_success(event);
	                                             ASSERT(Tsk7->__tsk::hdr.obj.res == RELEASED);
	                                             ASSERT(!*Tsk8);
	        Tsk8->start();                       ASSERT(!*Tsk8);
	event = Tsk8->join();                        ASSERT_success(event);
	                                             ASSERT(Tsk8->__tsk::hdr.obj.res == RELEASED);
	                                             ASSERT(!*Tsk9);
	        Tsk9->start();                       ASSERT(!*Tsk9);
	event = Tsk9->join();                        ASSERT_success(event);
	                                             ASSERT(Tsk9->__tsk::hdr.obj.res == RELEASED);
}

extern "C"
void test_task_create_5()
{
	TEST_Notify();
	TEST_Call();
}

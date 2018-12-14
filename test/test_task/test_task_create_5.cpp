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

	                                             assert(!*Tsk6);
	        Tsk6->start();                       assert(!*Tsk6);
	event = Tsk6->join();                        assert_success(event);
	                                             assert(Tsk6->__tsk::hdr.obj.res == RELEASED);
	                                             assert(!*Tsk7);
	        Tsk7->start();                       assert(!*Tsk7);
	event = Tsk7->join();                        assert_success(event);
	                                             assert(Tsk7->__tsk::hdr.obj.res == RELEASED);
	                                             assert(!*Tsk8);
	        Tsk8->start();                       assert(!*Tsk8);
	event = Tsk8->join();                        assert_success(event);
	                                             assert(Tsk8->__tsk::hdr.obj.res == RELEASED);
	                                             assert(!*Tsk9);
	        Tsk9->start();                       assert(!*Tsk9);
	event = Tsk9->join();                        assert_success(event);
	                                             assert(Tsk9->__tsk::hdr.obj.res == RELEASED);
}

extern "C"
void test_task_create_5()
{
	TEST_Notify();
	TEST_Call();
}

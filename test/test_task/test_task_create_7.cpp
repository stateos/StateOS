#include "test.h"

static void proc()
{
	        ThisTask::stop();
}

static void test()
{
	unsigned event;

	auto Tsk6 = startTaskT<512>::detached(6, proc);  ASSERT(Tsk6 && Tsk6->__tsk::hdr.obj.res == Tsk6);
	auto Tsk7 = startTask::create(7, proc);          ASSERT(Tsk7 && Tsk7->__tsk::hdr.obj.res == Tsk7);
	auto Tsk8 = startTaskT<512>::create(8, proc);    ASSERT(Tsk8 && Tsk8->__tsk::hdr.obj.res == Tsk8);
	auto Tsk9 = startTask::detached(9, proc);        ASSERT(Tsk9 && Tsk9->__tsk::hdr.obj.res == Tsk9);

	event = Tsk7->join();                        ASSERT_success(event);
	event = Tsk8->join();                        ASSERT_success(event);

	                                             ASSERT(Tsk6->__tsk::hdr.obj.res == RELEASED);
	                                             ASSERT(Tsk7->__tsk::hdr.obj.res == RELEASED);
	                                             ASSERT(Tsk8->__tsk::hdr.obj.res == RELEASED);
	                                             ASSERT(Tsk9->__tsk::hdr.obj.res != RELEASED);
	        sys_clean();
}

extern "C"
void test_task_create_7()
{
	TEST_Notify();
	TEST_Call();
}

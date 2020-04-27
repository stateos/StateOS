#include "test.h"

static void proc()
{
	        ThisTask::stop();
}

static void test()
{
	auto Tsk6 = TaskT<256>::detached(6, proc);   ASSERT(Tsk6->__tsk::hdr.obj.res == Tsk6);
	auto Tsk7 = Task::detached(7, proc);         ASSERT(Tsk7->__tsk::hdr.obj.res == Tsk7);
	auto Tsk8 = TaskT<512>::detached(8, proc);   ASSERT(Tsk8->__tsk::hdr.obj.res == Tsk8);
	auto Tsk9 = Task::detached(9, proc);         ASSERT(Tsk9->__tsk::hdr.obj.res == Tsk9);
	idle_tsk_destructor();
	                                             ASSERT(Tsk6->__tsk::hdr.obj.res == RELEASED);
	                                             ASSERT(Tsk7->__tsk::hdr.obj.res == RELEASED);
	                                             ASSERT(Tsk8->__tsk::hdr.obj.res == RELEASED);
	                                             ASSERT(Tsk9->__tsk::hdr.obj.res == RELEASED);
}

extern "C"
void test_task_create_7()
{
	TEST_Notify();
	TEST_Call();
}

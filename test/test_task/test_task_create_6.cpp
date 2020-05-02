#include "test.h"

static void proc()
{
	        ThisTask::stop();
}

static void test()
{
	unsigned event;

	auto Tsk6 = TaskT<512>::create(6, proc);     ASSERT(Tsk6);
	auto Tsk7 = Task::detached(7, proc);         ASSERT(Tsk7);
	auto Tsk8 = TaskT<512>::detached(8, proc);   ASSERT(Tsk8);
	auto Tsk9 = Task::create(9, proc);           ASSERT(Tsk9);

	        Tsk6->start();                       ASSERT_dead(Tsk6);
	event = Tsk6->join();                        ASSERT_success(event);
	                                             ASSERT(Tsk6->__tsk::hdr.obj.res == RELEASED);
	        Tsk7->start();                       ASSERT_ready(Tsk7);
	                                             ASSERT(Tsk7->__tsk::hdr.obj.res != RELEASED);

	        Tsk8->start();                       ASSERT_ready(Tsk8);
	                                             ASSERT(Tsk8->__tsk::hdr.obj.res != RELEASED);

	        Tsk9->start();                       ASSERT_dead(Tsk9);
	event = Tsk9->join();                        ASSERT_success(event);
	                                             ASSERT(Tsk9->__tsk::hdr.obj.res == RELEASED);
	        sys_clean();
}

extern "C"
void test_task_create_6()
{
	TEST_Notify();
	TEST_Call();
}

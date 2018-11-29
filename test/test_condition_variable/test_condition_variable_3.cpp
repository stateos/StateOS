#include "test.h"

static auto Mtx1 = Mutex(mtxDefault);
static auto Cnd1 = ConditionVariable();

static void proc1()
{
	unsigned event;

	event = Mtx1.wait();                         assert_success(event);
	        Cnd1.give(cndOne);
	event = Mtx1.give();                         assert_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned event;
	                                             assert(!Tsk1);
	        Tsk1.startFrom(proc1);
	event = Mtx1.wait();                         assert_success(event);
	event = Cnd1.wait(Mtx1);                     assert_success(event);
	        Cnd1.give(cndOne);
	event = Mtx1.give();                         assert_success(event);
	event = Tsk1.join();                         assert_success(event);
	        ThisTask::stop();
}

static void test()
{
	unsigned event;

	event = Mtx1.wait();                         assert_success(event);
	                                             assert(!Tsk2);
	        Tsk2.startFrom(proc2);
	event = Cnd1.wait(Mtx1);                     assert_success(event);
	event = Mtx1.give();                         assert_success(event);
	event = Tsk2.join();                         assert_success(event);
}

extern "C"
void test_condition_variable_3()
{
	int i;
	TEST_Notify();
	for (i = 0; i < PASS; i++)
		test();
}

#include "test.h"

auto Sem0 = Semaphore(0);
auto Sem1 = Semaphore(0);
auto Sem2 = Semaphore(0);
auto Sem3 = Semaphore(0);

static void proc3()
{
	unsigned event;

 	event = Sem3.wait();                         ASSERT_success(event);
 	event = Sem2.give();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned event;
		                                         ASSERT(!Tsk3);
	        Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
 	event = Sem2.wait();                         ASSERT_success(event);
 	event = Sem3.give();                         ASSERT_success(event);
 	event = Sem2.wait();                         ASSERT_success(event);
 	event = Sem1.give();                         ASSERT_success(event);
	event = Tsk3.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned event;
		                                         ASSERT(!Tsk2);
	        Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
 	event = Sem1.wait();                         ASSERT_success(event);
 	event = Sem2.give();                         ASSERT_success(event);
 	event = Sem1.wait();                         ASSERT_success(event);
 	event = Sem0.give();                         ASSERT_success(event);
	event = Tsk2.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned event;
		                                         ASSERT(!Tsk1);
	        Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
 	event = Sem0.wait();                         ASSERT_success(event);
 	event = Sem1.give();                         ASSERT_success(event);
 	event = Sem0.wait();                         ASSERT_success(event);
	event = Tsk1.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void test()
{
	unsigned event;
		                                         ASSERT(!Tsk0);
	        Tsk0.startFrom(proc0);               ASSERT(!!Tsk0);
	        ThisTask::yield();
	        ThisTask::yield();
 	event = Sem0.give();                         ASSERT_success(event);
	event = Tsk0.join();                         ASSERT_success(event);
}

extern "C"
void test_semaphore_3()
{
	TEST_Notify();
	TEST_Call();
}

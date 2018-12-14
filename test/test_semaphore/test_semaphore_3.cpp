#include "test.h"

auto Sem0 = Semaphore(0);
auto Sem1 = Semaphore(0);
auto Sem2 = Semaphore(0);
auto Sem3 = Semaphore(0);

static void proc3()
{
	unsigned event;

 	event = Sem3.wait();                         assert_success(event);
 	event = Sem2.give();                         assert_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned event;
		                                         assert(!Tsk3);
	        Tsk3.startFrom(proc3);               assert(!!Tsk3);
 	event = Sem2.wait();                         assert_success(event);
 	event = Sem3.give();                         assert_success(event);
 	event = Sem2.wait();                         assert_success(event);
 	event = Sem1.give();                         assert_success(event);
	event = Tsk3.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned event;
		                                         assert(!Tsk2);
	        Tsk2.startFrom(proc2);               assert(!!Tsk2);
 	event = Sem1.wait();                         assert_success(event);
 	event = Sem2.give();                         assert_success(event);
 	event = Sem1.wait();                         assert_success(event);
 	event = Sem0.give();                         assert_success(event);
	event = Tsk2.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned event;
		                                         assert(!Tsk1);
	        Tsk1.startFrom(proc1);               assert(!!Tsk1);
 	event = Sem0.wait();                         assert_success(event);
 	event = Sem1.give();                         assert_success(event);
 	event = Sem0.wait();                         assert_success(event);
	event = Tsk1.join();                         assert_success(event);
	        ThisTask::stop();
}

static void test()
{
	unsigned event;
		                                         assert(!Tsk0);
	        Tsk0.startFrom(proc0);               assert(!!Tsk0);
	        ThisTask::yield();
	        ThisTask::yield();
 	event = Sem0.give();                         assert_success(event);
	event = Tsk0.join();                         assert_success(event);
}

extern "C"
void test_semaphore_3()
{
	TEST_Notify();
	TEST_Call();
}

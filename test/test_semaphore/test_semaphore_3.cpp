#include "test.h"

using namespace stateos;

auto Sem0 = Semaphore(0);
auto Sem1 = Semaphore(0);
auto Sem2 = Semaphore(0);
auto Sem3 = Semaphore(0);

static void proc3()
{
	int result;

	result = Sem3.wait();                         ASSERT_success(result);
	result = Sem2.give();                         ASSERT_success(result);
	         this_task::stop();
}

static void proc2()
{
	int result;
	                                              ASSERT(!Tsk3);
	         Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	result = Sem2.wait();                         ASSERT_success(result);
	result = Sem3.give();                         ASSERT_success(result);
	result = Sem2.wait();                         ASSERT_success(result);
	result = Sem1.give();                         ASSERT_success(result);
	result = Tsk3.join();                         ASSERT_success(result);
	         this_task::stop();
}

static void proc1()
{
	int result;
	                                              ASSERT(!Tsk2);
	         Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	result = Sem1.wait();                         ASSERT_success(result);
	result = Sem2.give();                         ASSERT_success(result);
	result = Sem1.wait();                         ASSERT_success(result);
	result = Sem0.give();                         ASSERT_success(result);
	result = Tsk2.join();                         ASSERT_success(result);
	         this_task::stop();
}

static void proc0()
{
	int result;
	                                              ASSERT(!Tsk1);
	         Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	result = Sem0.wait();                         ASSERT_success(result);
	result = Sem1.give();                         ASSERT_success(result);
	result = Sem0.wait();                         ASSERT_success(result);
	result = Tsk1.join();                         ASSERT_success(result);
	         this_task::stop();
}

static void test()
{
	int result;
	                                              ASSERT(!Tsk0);
	         Tsk0.startFrom(proc0);               ASSERT(!!Tsk0);
	         this_task::yield();
	         this_task::yield();
	result = Sem0.give();                         ASSERT_success(result);
	result = Tsk0.join();                         ASSERT_success(result);
}

extern "C"
void test_semaphore_3()
{
	TEST_Notify();
	TEST_Call();
}

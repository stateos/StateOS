#include "test.h"

using namespace stateos;

static auto Mut0 = FastMutex();
static auto Mut1 = FastMutex();
static auto Mut2 = FastMutex();
static auto Mut3 = FastMutex();

static void proc3()
{
	int result;

	result = Mut2.take();                         ASSERT_timeout(result);
	result = Mut3.wait();                         ASSERT_success(result);
	result = Mut3.take();                         ASSERT_failure(result);
	result = Mut3.give();                         ASSERT_success(result);
	result = Mut3.give();                         ASSERT_failure(result);
	         ThisTask::stop();
}

static void proc2()
{
	int result;

	result = Mut1.take();                         ASSERT_timeout(result);
	result = Mut2.wait();                         ASSERT_success(result);
	result = Mut2.take();                         ASSERT_failure(result);
	                                              ASSERT(!Tsk3);
	         Tsk3.startFrom(proc3);               ASSERT(!Tsk3);
	result = Tsk3.join();                         ASSERT_success(result);
	result = Mut2.give();                         ASSERT_success(result);
	result = Mut2.give();                         ASSERT_failure(result);
	         ThisTask::stop();
}

static void proc1()
{
	int result;

	result = Mut0.take();                         ASSERT_timeout(result);
	result = Mut1.wait();                         ASSERT_success(result);
	result = Mut1.take();                         ASSERT_failure(result);
	                                              ASSERT(!Tsk2);
	         Tsk2.startFrom(proc2);               ASSERT(!Tsk2);
	result = Tsk2.join();                         ASSERT_success(result);
	result = Mut1.give();                         ASSERT_success(result);
	result = Mut1.give();                         ASSERT_failure(result);
	         ThisTask::stop();
}

static void proc0()
{
	int result;

	result = Mut0.wait();                         ASSERT_success(result);
	result = Mut0.take();                         ASSERT_failure(result);
	                                              ASSERT(!Tsk1);
	         Tsk1.startFrom(proc1);               ASSERT(!Tsk1);
	result = Tsk1.join();                         ASSERT_success(result);
	result = Mut0.give();                         ASSERT_success(result);
	result = Mut0.give();                         ASSERT_failure(result);
	         ThisTask::stop();
}

static void test()
{
	int result;
	                                              ASSERT(!Tsk0);
	         Tsk0.startFrom(proc0);
	result = Tsk0.join();                         ASSERT_success(result);
}

extern "C"
void test_fast_mutex_3()
{
	TEST_Notify();
	TEST_Call();
}

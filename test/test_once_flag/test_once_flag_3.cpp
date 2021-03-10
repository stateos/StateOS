#include "test.h"

using namespace stateos;

auto One0 = OnceFlag();
auto One1 = OnceFlag();
auto One2 = OnceFlag();
auto One3 = OnceFlag();

static int counter = 0;

static void proc()
{
	CriticalSection cs;
	counter++;
}

static void proc3()
{
	         One3.call(proc);
	         ThisTask::stop();
}

static void proc2()
{
	int result;
	                                              ASSERT(!Tsk3);
	         Tsk3.startFrom(proc3);               ASSERT(!Tsk3);
	         One2.call(proc);
	result = Tsk3.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void proc1()
{
	int result;
	                                              ASSERT(!Tsk2);
	         Tsk2.startFrom(proc2);               ASSERT(!Tsk2);
	         One1.call(proc);
	result = Tsk2.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void proc0()
{
	int result;
	                                              ASSERT(!Tsk1);
	         Tsk1.startFrom(proc1);               ASSERT(!Tsk1);
	         One0.call(proc);                     ASSERT(counter == 1 || counter == 4);
	result = Tsk1.join();                         ASSERT_success(result);
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
void test_once_flag_3()
{
	TEST_Notify();
	TEST_Call();
}

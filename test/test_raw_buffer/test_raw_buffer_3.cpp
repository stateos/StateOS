#include "test.h"

#define SIZE sizeof(unsigned)

using namespace stateos;

static auto Raw0 = RawBufferTT<1, unsigned>();
static auto Raw1 = RawBufferTT<1, unsigned>();
static auto Raw2 = RawBufferTT<1, unsigned>();
static auto Raw3 = RawBufferTT<1, unsigned>();

static unsigned sent;

static void proc3()
{
	int result;
	unsigned value;

	result = Raw3.wait(&value);                   ASSERT_success(result);
	                                              ASSERT(value == sent);
	result = Raw2.give(&value);                   ASSERT_success(result);
	         thisTask::stop();
}

static void proc2()
{
	int result;
	unsigned value;
	                                              ASSERT(!Tsk3);
	         Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	result = Raw2.wait(&value);                   ASSERT_success(result);
	                                              ASSERT(value == sent);
	result = Raw3.give(&value);                   ASSERT_success(result);
	result = Raw2.wait(&value);                   ASSERT_success(result);
	                                              ASSERT(value == sent);
	result = Raw1.give(&value);                   ASSERT_success(result);
	result = Tsk3.join();                         ASSERT_success(result);
	         thisTask::stop();
}

static void proc1()
{
	int result;
	unsigned value;
	                                              ASSERT(!Tsk2);
	         Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	result = Raw1.wait(&value);                   ASSERT_success(result);
	                                              ASSERT(value == sent);
	result = Raw2.give(&value);                   ASSERT_success(result);
	result = Raw1.wait(&value);                   ASSERT_success(result);
	                                              ASSERT(value == sent);
	result = Raw0.give(&value);                   ASSERT_success(result);
	result = Tsk2.join();                         ASSERT_success(result);
	         thisTask::stop();
}

static void proc0()
{
	int result;
	unsigned value;
	                                              ASSERT(!Tsk1);
	         Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	result = Raw0.wait(&value);                   ASSERT_success(result);
	                                              ASSERT(value == sent);
	result = Raw1.give(&value);                   ASSERT_success(result);
	result = Raw0.wait(&value);                   ASSERT_success(result);
	                                              ASSERT(value == sent);
	result = Tsk1.join();                         ASSERT_success(result);
	         thisTask::stop();
}

static void test()
{
	int result;
	                                              ASSERT(!Tsk0);
	         Tsk0.startFrom(proc0);               ASSERT(!!Tsk0);
	         thisTask::yield();
	         thisTask::yield();
	         sent = (unsigned)rand();
	result = Raw0.give(&sent);                    ASSERT_success(result);
	result = Tsk0.join();                         ASSERT_success(result);
}

extern "C"
void test_raw_buffer_3()
{
	TEST_Notify();
	TEST_Call();
}

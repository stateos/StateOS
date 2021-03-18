#include "test.h"

#define SIZE sizeof(unsigned)

using namespace stateos;

static auto Stm0 = StreamBufferTT<1, unsigned>();
static auto Stm1 = StreamBufferTT<1, unsigned>();
static auto Stm2 = StreamBufferTT<1, unsigned>();
static auto Stm3 = StreamBufferTT<1, unsigned>();

static unsigned sent;

static void proc3()
{
	int result;
	unsigned value;

	result = Stm3.wait(&value);                   ASSERT_success(result);
	                                              ASSERT(value == sent);
	result = Stm2.give(&value);                   ASSERT_success(result);
	         this_task::stop();
}

static void proc2()
{
	int result;
	unsigned value;
	                                              ASSERT(!Tsk3);
	         Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	result = Stm2.wait(&value);                   ASSERT_success(result);
	                                              ASSERT(value == sent);
	result = Stm3.give(&value);                   ASSERT_success(result);
	result = Stm2.wait(&value);                   ASSERT_success(result);
	                                              ASSERT(value == sent);
	result = Stm1.give(&value);                   ASSERT_success(result);
	result = Tsk3.join();                         ASSERT_success(result);
	         this_task::stop();
}

static void proc1()
{
	int result;
	unsigned value;
	                                              ASSERT(!Tsk2);
	         Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	result = Stm1.wait(&value);                   ASSERT_success(result);
	                                              ASSERT(value == sent);
	result = Stm2.give(&value);                   ASSERT_success(result);
	result = Stm1.wait(&value);                   ASSERT_success(result);
	                                              ASSERT(value == sent);
	result = Stm0.give(&value);                   ASSERT_success(result);
	result = Tsk2.join();                         ASSERT_success(result);
	         this_task::stop();
}

static void proc0()
{
	int result;
	unsigned value;
	                                              ASSERT(!Tsk1);
	         Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	result = Stm0.wait(&value);                   ASSERT_success(result);
	                                              ASSERT(value == sent);
	result = Stm1.give(&value);                   ASSERT_success(result);
	result = Stm0.wait(&value);                   ASSERT_success(result);
	                                              ASSERT(value == sent);
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
	         sent = (unsigned)rand();
	result = Stm0.give(&sent);                    ASSERT_success(result);
	result = Tsk0.join();                         ASSERT_success(result);
}

extern "C"
void test_stream_buffer_3()
{
	TEST_Notify();
	TEST_Call();
}

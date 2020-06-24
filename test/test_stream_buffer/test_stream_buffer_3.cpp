#include "test.h"

#define SIZE sizeof(unsigned)

static auto Stm0 = StreamBufferTT<1, unsigned>();
static auto Stm1 = StreamBufferTT<1, unsigned>();
static auto Stm2 = StreamBufferTT<1, unsigned>();
static auto Stm3 = StreamBufferTT<1, unsigned>();

static unsigned sent;

static void proc3()
{
	unsigned event;
	unsigned value;

 	event = Stm3.wait(&value);                   ASSERT_success(event);
	                                             ASSERT(value == sent);
	event = Stm2.give(&value);                   ASSERT_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned event;
	unsigned value;
		                                         ASSERT(!Tsk3);
	        Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
 	event = Stm2.wait(&value);                   ASSERT_success(event);
	                                             ASSERT(value == sent);
	event = Stm3.give(&value);                   ASSERT_success(event);
 	event = Stm2.wait(&value);                   ASSERT_success(event);
	                                             ASSERT(value == sent);
	event = Stm1.give(&value);                   ASSERT_success(event);
	event = Tsk3.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned event;
	unsigned value;
		                                         ASSERT(!Tsk2);
	        Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
 	event = Stm1.wait(&value);                   ASSERT_success(event);
 	                                             ASSERT(value == sent);
	event = Stm2.give(&value);                   ASSERT_success(event);
 	event = Stm1.wait(&value);                   ASSERT_success(event);
	                                             ASSERT(value == sent);
	event = Stm0.give(&value);                   ASSERT_success(event);
	event = Tsk2.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned event;
	unsigned value;
		                                         ASSERT(!Tsk1);
	        Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
 	event = Stm0.wait(&value);                   ASSERT_success(event);
 	                                             ASSERT(value == sent);
	event = Stm1.give(&value);                   ASSERT_success(event);
 	event = Stm0.wait(&value);                   ASSERT_success(event);
	                                             ASSERT(value == sent);
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
	        sent = rand();
	event = Stm0.give(&sent);                    ASSERT_success(event);
	event = Tsk0.join();                         ASSERT_success(event);
}

extern "C"
void test_stream_buffer_3()
{
	TEST_Notify();
	TEST_Call();
}

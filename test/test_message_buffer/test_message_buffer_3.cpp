#include "test.h"

#define SIZE sizeof(unsigned)

static auto Msg0 = MessageBufferTT<1, unsigned>();
static auto Msg1 = MessageBufferTT<1, unsigned>();
static auto Msg2 = MessageBufferTT<1, unsigned>();
static auto Msg3 = MessageBufferTT<1, unsigned>();

static unsigned sent;

static void proc3()
{
	unsigned event;
	unsigned value;

	event = Msg3.wait(&value);                   ASSERT_success(event);
	                                             ASSERT(value == sent);
	event = Msg2.give(&value);                   ASSERT_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned event;
	unsigned value;
		                                         ASSERT(!Tsk3);
	        Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
 	event = Msg2.wait(&value);                   ASSERT_success(event);
 	                                             ASSERT(value == sent);
	event = Msg3.give(&value);                   ASSERT_success(event);
 	event = Msg2.wait(&value);                   ASSERT_success(event);
	                                             ASSERT(value == sent);
	event = Msg1.give(&value);                   ASSERT_success(event);
	event = Tsk3.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned event;
	unsigned value;
		                                         ASSERT(!Tsk2);
	        Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
 	event = Msg1.wait(&value);                   ASSERT_success(event);
 	                                             ASSERT(value == sent);
	event = Msg2.give(&value);                   ASSERT_success(event);
 	event = Msg1.wait(&value);                   ASSERT_success(event);
	                                             ASSERT(value == sent);
	event = Msg0.give(&value);                   ASSERT_success(event);
	event = Tsk2.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned event;
	unsigned value;
		                                         ASSERT(!Tsk1);
	        Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
 	event = Msg0.wait(&value);                   ASSERT_success(event);
 	                                             ASSERT(value == sent);
	event = Msg1.give(&value);                   ASSERT_success(event);
 	event = Msg0.wait(&value);                   ASSERT_success(event);
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
	event = Msg0.give(&sent);                    ASSERT_success(event);
	event = Tsk0.join();                         ASSERT_success(event);
}

extern "C"
void test_message_buffer_3()
{
	TEST_Notify();
	TEST_Call();
}

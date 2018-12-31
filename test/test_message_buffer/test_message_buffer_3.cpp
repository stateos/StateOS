#include "test.h"

#define SIZE sizeof(unsigned)

static auto Msg0 = MessageBufferTT<1, unsigned>();
static auto Msg1 = MessageBufferTT<1, unsigned>();
static auto Msg2 = MessageBufferTT<1, unsigned>();
static auto Msg3 = MessageBufferTT<1, unsigned>();

static unsigned sent;

static void proc3()
{
	unsigned bytes;
	unsigned event;

 	bytes = Msg3.wait(&event);                   ASSERT(bytes == SIZE);
	                                             ASSERT(event == sent);
	event = Msg2.give(&event);                   ASSERT_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned bytes;
	unsigned event;
		                                         ASSERT(!Tsk3);
	        Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
 	bytes = Msg2.wait(&event);                   ASSERT(bytes == SIZE);
 	                                             ASSERT(event == sent);
	event = Msg3.give(&event);                   ASSERT_success(event);
 	bytes = Msg2.wait(&event);                   ASSERT(bytes == SIZE);
	                                             ASSERT(event == sent);
	event = Msg1.give(&event);                   ASSERT_success(event);
	event = Tsk3.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned bytes;
	unsigned event;
		                                         ASSERT(!Tsk2);
	        Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
 	bytes = Msg1.wait(&event);                   ASSERT(bytes == SIZE);
 	                                             ASSERT(event == sent);
	event = Msg2.give(&event);                   ASSERT_success(event);
 	bytes = Msg1.wait(&event);                   ASSERT(bytes == SIZE);
	                                             ASSERT(event == sent);
	event = Msg0.give(&event);                   ASSERT_success(event);
	event = Tsk2.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned bytes;
	unsigned event;
		                                         ASSERT(!Tsk1);
	        Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
 	bytes = Msg0.wait(&event);                   ASSERT(bytes == SIZE);
 	                                             ASSERT(event == sent);
	event = Msg1.give(&event);                   ASSERT_success(event);
 	bytes = Msg0.wait(&event);                   ASSERT(bytes == SIZE);
	                                             ASSERT(event == sent);
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

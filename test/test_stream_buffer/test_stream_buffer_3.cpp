#include "test.h"

#define SIZE sizeof(unsigned)

static auto Stm0 = StreamBufferTT<1, unsigned>();
static auto Stm1 = StreamBufferTT<1, unsigned>();
static auto Stm2 = StreamBufferTT<1, unsigned>();
static auto Stm3 = StreamBufferTT<1, unsigned>();

static unsigned sent;

static void proc3()
{
	unsigned bytes;
	unsigned event;

 	bytes = Stm3.wait(&event);                   assert(bytes == SIZE);
	                                             assert(event == sent);
	event = Stm2.give(&event);                   assert_success(event);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned bytes;
	unsigned event;
		                                         assert(!Tsk3);
	        Tsk3.startFrom(proc3);               assert(!!Tsk3);
 	bytes = Stm2.wait(&event);                   assert(bytes == SIZE);
 	                                             assert(event == sent);
	event = Stm3.give(&event);                   assert_success(event);
 	bytes = Stm2.wait(&event);                   assert(bytes == SIZE);
	                                             assert(event == sent);
	event = Stm1.give(&event);                   assert_success(event);
	event = Tsk3.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned bytes;
	unsigned event;
		                                         assert(!Tsk2);
	        Tsk2.startFrom(proc2);               assert(!!Tsk2);
 	bytes = Stm1.wait(&event);                   assert(bytes == SIZE);
 	                                             assert(event == sent);
	event = Stm2.give(&event);                   assert_success(event);
 	bytes = Stm1.wait(&event);                   assert(bytes == SIZE);
	                                             assert(event == sent);
	event = Stm0.give(&event);                   assert_success(event);
	event = Tsk2.join();                         assert_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned bytes;
	unsigned event;
		                                         assert(!Tsk1);
	        Tsk1.startFrom(proc1);               assert(!!Tsk1);
 	bytes = Stm0.wait(&event);                   assert(bytes == SIZE);
 	                                             assert(event == sent);
	event = Stm1.give(&event);                   assert_success(event);
 	bytes = Stm0.wait(&event);                   assert(bytes == SIZE);
	                                             assert(event == sent);
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
	        sent = rand();
	event = Stm0.give(&sent);                    assert_success(event);
	event = Tsk0.join();                         assert_success(event);
}

extern "C"
void test_stream_buffer_3()
{
	TEST_Notify();
	TEST_Call();
}

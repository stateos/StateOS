#include "test.h"

auto Lst0 = ListTT<unsigned>();
auto Lst1 = ListTT<unsigned>();
auto Lst2 = ListTT<unsigned>();
auto Lst3 = ListTT<unsigned>();
auto Mem0 = MemoryPoolTT<1, unsigned>();
auto Mem1 = MemoryPoolTT<1, unsigned>();
auto Mem2 = MemoryPoolTT<1, unsigned>();
auto Mem3 = MemoryPoolTT<1, unsigned>();

static unsigned sent;

static void proc3()
{
	unsigned *p;
	unsigned received;
	int result;

	result = Lst3.wait(&p);                       ASSERT_success(result);
	         received = *p;                       ASSERT(sent == received);
             Mem3.give(p);
	result = Mem2.wait(&p);                       ASSERT_success(result);
	         *p = received;
	         Lst2.give(p);
	         ThisTask::stop();
}

static void proc2()
{
	unsigned *p;
	unsigned received;
	int result;
	                                              ASSERT(!Tsk3);
	         Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	result = Lst2.wait(&p);                       ASSERT_success(result);
	         received = *p;                       ASSERT(sent == received);
             Mem2.give(p);
	result = Mem3.wait(&p);                       ASSERT_success(result);
	         *p = received;
	         Lst3.give(p);
	result = Lst2.wait(&p);                       ASSERT_success(result);
	         received = *p;                       ASSERT(sent == received);
             Mem2.give(p);
	result = Mem1.wait(&p);                       ASSERT_success(result);
	         *p = received;
	         Lst1.give(p);
	result = Tsk3.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void proc1()
{
	unsigned *p;
	unsigned received;
	int result;
	                                              ASSERT(!Tsk2);
	         Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	result = Lst1.wait(&p);                       ASSERT_success(result);
	         received = *p;                       ASSERT(sent == received);
             Mem1.give(p);
	result = Mem2.wait(&p);                       ASSERT_success(result);
	         *p = received;
	         Lst2.give(p);
	result = Lst1.wait(&p);                       ASSERT_success(result);
	         received = *p;                       ASSERT(sent == received);
             Mem1.give(p);
	result = Mem0.wait(&p);                       ASSERT_success(result);
	         *p = received;
	         Lst0.give(p);
	result = Tsk2.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void proc0()
{
	unsigned *p;
	unsigned received;
	int result;
	                                              ASSERT(!Tsk1);
	         Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	result = Lst0.wait(&p);                       ASSERT_success(result);
	         received = *p;                       ASSERT(sent == received);
             Mem0.give(p);
	result = Mem1.wait(&p);                       ASSERT_success(result);
	         *p = received;
	         Lst1.give(p);
	result = Lst0.wait(&p);                       ASSERT_success(result);
	         received = *p;                       ASSERT(sent == received);
             Mem0.give(p);
	result = Tsk1.join();                         ASSERT_success(result);
	         ThisTask::stop();
}

static void test()
{
	unsigned *p;
	int result;
	                                              ASSERT(!Tsk0);
	         Tsk0.startFrom(proc0);               ASSERT(!!Tsk0);
	         tsk_yield();
	         tsk_yield();
	result = Mem0.wait(&p);                       ASSERT_success(result);
	         *p = sent = (unsigned)rand();
	         Lst0.give(p);
	result = Tsk0.join();                         ASSERT_success(result);
}

extern "C"
void test_memory_pool_3()
{
	TEST_Notify();
	TEST_Call();
}

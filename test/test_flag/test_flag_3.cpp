#include "test.h"

#define FLAG0 14U
#define FLAG1 13U
#define FLAG2 11U
#define FLAG3  7U

using namespace stateos;

static auto Flg0 = Flag();
static auto Flg1 = Flag();
static auto Flg2 = Flag();
static auto Flg3 = Flag(FLAG3);

static void give(Flag &Flg, unsigned bitfield, unsigned check)
{
	unsigned flag;
	unsigned flags;

	while (bitfield)
	{
		flag  = bitfield & -bitfield;
		bitfield -= flag;
		flags = Flg.give(flag);                   ASSERT(flags == check);
	}
}

static void proc3()
{
	unsigned flags;
	int result;

	result = Flg3.wait(FLAG3, flgAllNew);
	                                              ASSERT_success(result);
	         flags = Flg2.give(FLAG2);            ASSERT(flags == FLAG2);
	         thisTask::stop();
}

static void proc2()
{
	unsigned flags;
	int result;
	                                              ASSERT(!Tsk3);
	         Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	result = Flg2.wait(FLAG2, flgAll);            ASSERT_success(result);
	         give(Flg3, FLAG3, FLAG3);
	result = Flg2.wait(FLAG2, flgAll);            ASSERT_success(result);
	         flags = Flg1.give(FLAG1);            ASSERT(flags == FLAG1);
	result = Tsk3.join();                         ASSERT_success(result);
	         thisTask::stop();
}

static void proc1()
{
	unsigned flags;
	int result;
	                                              ASSERT(!Tsk2);
	         Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	result = Flg1.wait(FLAG1, flgAll);            ASSERT_success(result);
	         give(Flg2, FLAG2, 0);
	result = Flg1.wait(FLAG1, flgAll);            ASSERT_success(result);
	         flags = Flg0.give(FLAG0);            ASSERT(flags == FLAG0);
	result = Tsk2.join();                         ASSERT_success(result);
	         thisTask::stop();
}

static void proc0()
{
	int result;
	                                              ASSERT(!Tsk1);
	         Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	result = Flg0.wait(FLAG0, flgAll);            ASSERT_success(result);
	         give(Flg1, FLAG1, 0);
	result = Flg0.wait(FLAG0, flgAll);            ASSERT_success(result);
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
	         give(Flg0, FLAG0, 0);
	result = Tsk0.join();                         ASSERT_success(result);
}

extern "C"
void test_flag_3()
{
	TEST_Notify();
	TEST_Call();
}

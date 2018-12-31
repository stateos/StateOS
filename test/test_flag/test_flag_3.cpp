#include "test.h"

#define FLAG0 14U
#define FLAG1 13U
#define FLAG2 11U
#define FLAG3  7U

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
		flag = bitfield & (~bitfield + 1);
		bitfield -= flag;
		flags = Flg.give(flag);                  ASSERT(flags == check);
	}
}

static void proc3()
{
	unsigned flags;
	unsigned event;

	event = Flg3.wait(FLAG3, flgAll+flgProtect+flgIgnore); ASSERT_success(event);
	flags = Flg2.give(FLAG2);                    ASSERT(flags == FLAG2);
	        ThisTask::stop();
}

static void proc2()
{
	unsigned flags;
	unsigned event;
	                                             ASSERT(!Tsk3);
	        Tsk3.startFrom(proc3);               ASSERT(!!Tsk3);
	event = Flg2.wait(FLAG2, flgAll);            ASSERT_success(event);
	        give(Flg3, FLAG3, FLAG3);
	event = Flg2.wait(FLAG2, flgAll);            ASSERT_success(event);
	flags = Flg1.give(FLAG1);                    ASSERT(flags == FLAG1);
	event = Tsk3.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc1()
{
	unsigned flags;
	unsigned event;
	                                             ASSERT(!Tsk2);
	        Tsk2.startFrom(proc2);               ASSERT(!!Tsk2);
	event = Flg1.wait(FLAG1, flgAll);            ASSERT_success(event);
	        give(Flg2, FLAG2, 0);
	event = Flg1.wait(FLAG1, flgAll);            ASSERT_success(event);
	flags = Flg0.give(FLAG0);                    ASSERT(flags == FLAG0);
	event = Tsk2.join();                         ASSERT_success(event);
	        ThisTask::stop();
}

static void proc0()
{
	unsigned event;
	                                             ASSERT(!Tsk1);
	        Tsk1.startFrom(proc1);               ASSERT(!!Tsk1);
	event = Flg0.wait(FLAG0, flgAll);            ASSERT_success(event);
	        give(Flg1, FLAG1, 0);
	event = Flg0.wait(FLAG0, flgAll);            ASSERT_success(event);
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
	        give(Flg0, FLAG0, 0);
	event = Tsk0.join();                         ASSERT_success(event);
}

extern "C"
void test_flag_3()
{
	TEST_Notify();
	TEST_Call();
}

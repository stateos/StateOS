#include "test.h"

#define       LOOP 1
#define       SIZE 68

static cnt_t  summary = 0;
static fun_t *test[SIZE];
static int    count = 0;

void test_add(fun_t *fun)
{
	ASSERT(count < SIZE);
	test[count++] = fun;
}

void test_call(fun_t *fun)
{
	int i;
	cnt_t t = sys_time();
	for (i = 0; i < PASS; i++)
		fun();
	summary += t = sys_time() - t;
#ifdef DEBUG
//	printf(": %u\n", (unsigned) t);
#endif
}

static void test_init()
{
	TEST_Notify();
#ifdef DEBUG
//	printf(": %d / %d\n", count, SIZE);
#else
	LED_Init();
#endif
	srand(0);
}

static void test_fini()
{
	TEST_Notify();
#ifdef DEBUG
//	printf(": %u\n", (unsigned) summary);
#else
	LEDs = 15;
#endif
}

int main()
{
	int i;

	srand(0);

	test_init();
	
	TEST_AddUnit(test_alloc);
	TEST_AddUnit(test_spin_lock);
	TEST_AddUnit(test_once_flag);
	TEST_AddUnit(test_event);
	TEST_AddUnit(test_signal);
	TEST_AddUnit(test_flag);
	TEST_AddUnit(test_barrier);
	TEST_AddUnit(test_semaphore);
	TEST_AddUnit(test_mutex);
	TEST_AddUnit(test_fast_mutex);
	TEST_AddUnit(test_condition_variable);
	TEST_AddUnit(test_memory_pool);
	TEST_AddUnit(test_stream_buffer);
	TEST_AddUnit(test_message_queue);
	TEST_AddUnit(test_mailbox_queue);
	TEST_AddUnit(test_event_queue);
	TEST_AddUnit(test_job_queue);
	TEST_AddUnit(test_timer);
	TEST_AddUnit(test_task);

	size_t h = sys_heapSize();
	for (i = 0; i < count * LOOP * 2; i += 2)
	{
		printf("%3d%% ", (i + 1) * 50 / count / LOOP);
		test[i      % count]();
		ASSERT(h==sys_heapSize());
		printf("%3d%% ", (i + 2) * 50 / count / LOOP);
		test[rand() % count]();
		ASSERT(h==sys_heapSize());
	}

	test_fini();

	tsk_stop();
}

#include <stm32f4_discovery.h>
#include <os.h>

static_MUT(mut);

void consumer()
{
	mut_wait(mut);
	LED_Tick();
	mut_give(mut);
}

void producer()
{
	mut_wait(mut);
	tsk_delay(SEC);
	mut_give(mut);
}

static_TSK(cons, 0, consumer);
static_TSK(prod, 0, producer);

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_stop();
}

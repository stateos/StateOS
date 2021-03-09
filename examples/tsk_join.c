#include <stm32f4_discovery.h>
#include <os.h>

void consumer()
{
	static uint32_t time = 0;

	tsk_sleepUntil(time += SEC);
	tsk_stop();
}

OS_TSK(cons, 0, consumer);

void producer()
{
	tsk_start(cons);
	tsk_join(cons);

	LED_Tick();
}

OS_TSK(prod, 0, producer);

int main()
{
	LED_Init();

	tsk_start(prod);
	tsk_join(prod);
}

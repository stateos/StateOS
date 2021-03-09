#include <stm32f4_discovery.h>
#include <os.h>

OS_BAR(bar, 2);

void consumer()
{
	bar_wait(bar);
	LED_Tick();
}

void producer()
{
	tsk_delay(1000*MSEC);
	bar_wait(bar);
}

OS_TSK(cons, 0, consumer);
OS_TSK(prod, 0, producer);

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_stop();
}

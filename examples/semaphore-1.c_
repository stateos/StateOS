#include <stm32f4_discovery.h>
#include <os.h>

OS_SEM(sem, 0, semCounting);

void consumer()
{
	sem_wait(sem);
	LED_Tick();
}

void producer()
{
	tsk_delay(1000*MSEC);
	sem_give(sem);
}

OS_TSK(cons, 0, consumer, 256);
OS_TSK(prod, 0, producer, 256);

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_stop();
}

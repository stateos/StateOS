#include <stm32f4_discovery.h>
#include <os.h>

sem_t *sem;

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

int main()
{
	LED_Init();

	sem = sem_create(0, semCounting);
	wrk_create(0, consumer, 256, false, true);
	wrk_create(0, producer, 256, false, true);
	tsk_stop();
}

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
	tsk_delay(SEC);
	sem_give(sem);
}

int main()
{
	LED_Init();

	sem = sem_create(0, semCounting);
	tsk_create(0, consumer);
	tsk_create(0, producer);
	tsk_stop();
}

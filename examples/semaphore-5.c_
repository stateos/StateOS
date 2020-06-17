#include <stm32f4_discovery.h>
#include <os.h>

sem_t sem = SEM_INIT(0, semDirect);

void consumer()
{
	sem_wait(&sem);
	LED_Tick();
}

void producer()
{
	tsk_delay(SEC);
	sem_post(&sem);
}

int main()
{
	LED_Init();

	tsk_new(0, consumer);
	tsk_new(0, producer);
	tsk_stop();
}

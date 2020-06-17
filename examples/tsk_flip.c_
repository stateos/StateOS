#include <stm32f4_discovery.h>
#include <os.h>

void consumer(void);
void producer(void);

void consumer()
{
	LED_Tick();
	tsk_flip(producer);
}

void producer()
{
	static uint32_t time = 0;

	tsk_sleepUntil(time += SEC);
	tsk_flip(consumer);
}

int main()
{
	LED_Init();

	tsk_flip(producer);
}

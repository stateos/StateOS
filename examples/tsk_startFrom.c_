#include <stm32f4_discovery.h>
#include <os.h>

tsk_t tsk = TSK_INIT(0, 0);

void consumer()
{
	LED_Tick();
	tsk_stop();
}

void producer()
{
	uint32_t time = 0;

	for (;;)
	{
		tsk_sleepUntil(time += SEC);
		tsk_startFrom(&tsk, consumer);
	}
}

int main()
{
	LED_Init();

	tsk_flip(producer);
}

#include <stm32f4_discovery.h>
#include <os.h>

OS_EVT(evt);

void consumer()
{
	unsigned event;
	evt_wait(evt, &event);
	LEDs = event & 0x0F;
}

void producer()
{
	unsigned x = 1;

	for (;;)
	{
		tsk_delay(SEC);
		evt_give(evt, x);
		x = (x << 1) | (x >> 3);
	}
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

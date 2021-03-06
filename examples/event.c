#include <stm32f4_discovery.h>
#include <os.h>

OS_EVT(evt);

void consumer()
{
	unsigned x;

	evt_wait(evt, &x);
	LEDs = x & 0x0FUL;
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

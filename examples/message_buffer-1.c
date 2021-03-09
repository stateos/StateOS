#include <stm32f4_discovery.h>
#include <os.h>

OS_MSG(msg, 1, sizeof(unsigned));

OS_TSK_DEF(cons, 0)
{
	unsigned x;

	for (;;)
	{
		msg_wait(msg, &x, sizeof(x), NULL);
		LEDs = x & 0x0FUL;
	}
}

OS_TSK_DEF(prod, 0)
{
	unsigned x = 1;

	for (;;)
	{
		tsk_delay(SEC);
		msg_give(msg, &x, sizeof(x));
		x = (x << 1) | (x >> 3);
	}
}

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_sleep();
}

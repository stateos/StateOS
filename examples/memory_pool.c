#include <stm32f4_discovery.h>
#include <os.h>

OS_MEM(mem, 1, sizeof(unsigned));
OS_LST(lst);

void consumer()
{
	void *p;

	for (;;)
	{
		lst_wait(lst, &p);
		LEDs = *(unsigned *)p & 0x0F;
		mem_give(mem, p);
	}
}

void producer()
{
	void *p;
	unsigned x = 1;

	for (;;)
	{
		tsk_delay(SEC);
		mem_wait(mem, &p);
		*(unsigned *)p = x;
		lst_give(lst, p);
		x = (x << 1) | (x >> 3);
	}
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

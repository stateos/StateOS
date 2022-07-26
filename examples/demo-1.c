#include <stm32f4_discovery.h>
#include <os.h>

void proc()
{
	static 	 unsigned i = 0;
	volatile unsigned*led;

	switch (i++)
	{
		case  0: led = &LED[0]; tsk_sleepUntil(SEC/8*0); break;
		case  1: led = &LED[1]; tsk_sleepUntil(SEC/8*1); break;
		case  2: led = &LED[2]; tsk_sleepUntil(SEC/8*2); break;
		case  3: led = &LED[3]; tsk_sleepUntil(SEC/8*3); break;
		default: led = &GRN;    tsk_sleepUntil(SEC/8*4); break;
	}

	for (;;)
	{
		tsk_sleepNext(SEC/2);
		(*led)++;
	}
}

int main()
{
    LED_Init();
    GRN_Init();
	
	tsk_start((tsk_id)TSK_CREATE(0, proc));
	tsk_start((tsk_id)TSK_CREATE(0, proc));
	tsk_start((tsk_id)TSK_CREATE(0, proc));
	tsk_start((tsk_id)TSK_CREATE(0, proc));
	tsk_start((tsk_id)TSK_CREATE(0, proc));
	tsk_stop();
}

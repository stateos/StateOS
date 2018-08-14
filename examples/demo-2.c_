#include <stm32f4_discovery.h>
#include <os.h>

void proc(volatile unsigned *led, cnt_t timePoint)
{
	tsk_sleepUntil(timePoint);
	for (;;)
	{
		tsk_sleepNext(SEC/2);
		(*led)++;
	}
}

OS_TSK_START(t1, 0) { proc(&LED[0], SEC/8*0); }
OS_TSK_START(t2, 0) { proc(&LED[1], SEC/8*1); }
OS_TSK_START(t3, 0) { proc(&LED[2], SEC/8*2); }
OS_TSK_START(t4, 0) { proc(&LED[3], SEC/8*3); }
OS_TSK_START(t5, 0) { proc(&GRN,    SEC/8*4); }

int main()
{
	LED_Init();
	GRN_Init();

	tsk_stop();
}

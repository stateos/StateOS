#include <stm32f4_discovery.h>
#include <os.h>

OS_MTX(mtx, mtxErrorCheck);

OS_TSK_DEF(cons, 0)
{
	mtx_wait(mtx);
	LED_Tick();
	mtx_give(mtx);
}

OS_TSK_DEF(prod, 1)
{
	mtx_wait(mtx);
	tsk_delay(SEC);
	mtx_give(mtx);
}

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_stop();
}

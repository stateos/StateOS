#include <stm32f4_discovery.h>
#include <os.h>

OS_MTX(mtx, mtxNormal + mtxRobust);

OS_TSK_DEF(cons, 0)
{
	mtx_wait(mtx);
}

OS_TSK_DEF(prod, 0)
{
	switch(mtx_wait(mtx))
	{
	case OWNERDEAD:
		LED_Tick();
		tsk_start(cons);
		/* falls through */
	case E_SUCCESS:
		mtx_give(mtx);
	}
}

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);

	for (;;)
	{
		tsk_delay(SEC);
		tsk_kill(cons);
	}
}

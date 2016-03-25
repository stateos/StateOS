#include <stm32f4_discovery.h>
#include <os.h>

sig_t sig = SIG_INIT(sigClear);

void slave()
{
	sig_wait(&sig);

	LED_Tick();
}

void master()
{
	tsk_delay(SEC);

	sig_give(&sig);
}

tsk_t sla = TSK_INIT(0, slave);
tsk_t mas = TSK_INIT(0, master);

int main()
{
	LED_Config();

	tsk_start(&sla);
	tsk_start(&mas);
	tsk_stop();
}

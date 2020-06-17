#include <stm32f4_discovery.h>
#include <cmsis_os.h>

osSemaphoreId  sem;
osSemaphoreDef(sem);

void consumer(void const *arg)
{
	(void) arg;

	for (;;)
	{
		osSemaphoreWait(sem, osWaitForever);
		LED_Tick();
	}
}

void producer(void const *arg)
{
	(void) arg;

	for (;;)
	{
		osDelay(1000);
		osSemaphoreRelease(sem);
	}
}

osThreadDef(consumer, osPriorityNormal, 1, 1024);
osThreadDef(producer, osPriorityNormal, 1, 1024);

int main( void )
{
	LED_Init();
	osKernelInitialize();
	sem = osSemaphoreCreate(osSemaphore(sem), 1);
	osThreadCreate(osThread(consumer), NULL);
	osThreadCreate(osThread(producer), NULL);
	osKernelStart();
	osThreadTerminate(osThreadGetId());
}

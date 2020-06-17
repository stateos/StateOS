#include <stm32f4_discovery.h>
#include <os.h>

sem_t sem;
tsk_t cons; OS_STK(cons_stk);
tsk_t prod; OS_STK(prod_stk, 512);

void consumer()
{
	sem_wait(&sem);
	LED_Tick();
}

void producer()
{
	tsk_delay(SEC);
	sem_give(&sem);
}

int main()
{
	LED_Init();

	sem_init(&sem, 0, semBinary);
	tsk_init(&cons, 0, consumer, cons_stk, sizeof(cons_stk));
	tsk_init(&prod, 0, producer, prod_stk, sizeof(prod_stk));
	tsk_stop();
}

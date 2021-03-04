#include <stm32f4_discovery.h>
#include <os.h>
#include <stdlib.h>

// Dining philosophers problem: resource hierarchy solution (Dijkstra)

#define NUM 4 // num of philosophers (leds)
#define DLY 4 // num of seconds

mtx_t *forks[NUM];

cnt_t get_delay()
{
	return (cnt_t)((rand() % DLY) + 1) * SEC;
}

void philosopher()
{
	static
	int public_id = 0;
	int id = public_id++;
	mtx_t *fork1 = (id + 1 < NUM) ? forks[id]     : forks[0];
	mtx_t *fork2 = (id + 1 < NUM) ? forks[id + 1] : forks[id];
	cnt_t delay, i;

	for (;;)
	{
		LED[id] = 0;        // Philosopher is starving (led is off)
		mtx_wait(fork1);    // Philosopher is waiting for the first fork
		mtx_wait(fork2);    // Philosopher is waiting for the second fork
		delay = get_delay();
		LED[id] = 0;        // Philosopher is eating (led is blinking)
		for (i = 0; i < delay; i += SEC / 10) { LED[id]++; tsk_delay(SEC / 10); }
		mtx_give(fork2);    // Philosopher puts down the second fork
		mtx_give(fork1);    // Philosopher puts down the first fork
		LED[id] = 1;        // Philosopher is thinking (led is on)
		delay = get_delay();
		tsk_delay(delay);
	}
}

int main()
{
	int i;

	srand(0);
	LED_Init();

	for (i = 0; i < NUM; i++)
	{
		forks[i] = mtx_create(mtxDefault, 0);
		wrk_create(0, philosopher, 1024, false, true);
	}

	tsk_sleep();
}

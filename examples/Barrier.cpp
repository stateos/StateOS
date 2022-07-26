#include <stm32f4_discovery.h>
#include <os.h>

auto led = device::Led();
auto bar = stateos::Barrier(2);

void consumer()
{
	bar.wait();
	led.tick();
}

void producer()
{
	stateos::thisTask::delay(SEC);
	bar.wait();
}

auto cons = stateos::Task(0, consumer);
auto prod = stateos::Task(0, producer);

int main()
{
	cons.start();
	prod.start();
	stateos::thisTask::stop();
}

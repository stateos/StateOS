#include <stm32f4_discovery.h>
#include <os.h>

auto led = device::Led();
auto flg = os::Flag();

void consumer()
{
	flg.wait(3, flgAllNew);
	led.tick();
}

void producer()
{
	os::thisTask::delay(SEC/2);
	flg.give(1);
	os::thisTask::delay(SEC/2);
	flg.give(2);
}

auto cons = os::Task(consumer);
auto prod = os::Task(producer);

int main()
{
	cons.start();
	prod.start();
	os::thisTask::stop();
}

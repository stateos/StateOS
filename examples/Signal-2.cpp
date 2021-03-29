#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto sig = Signal();

void consumer()
{
	unsigned x;

	for (;;)
	{
		sig.wait(sigAll, &x);
		led = SIGSET(x);
	}
}

void producer()
{
	unsigned x = 0;

	for (;;)
	{
		thisTask::sleepFor(SEC);
		sig.give(x);
		x = (x + 1) % 4;
	}
}

int main()
{
	Task::Create(0, consumer);
	Task::Create(0, producer);

	thisTask::sleep();
}

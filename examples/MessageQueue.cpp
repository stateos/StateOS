#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto msg = MessageQueueTT<1, unsigned>();

void consumer()
{
	unsigned x;

	for (;;)
	{
		msg.wait(&x);
		led = x;
	}
}

void producer()
{
	unsigned x = 1;

	for (;;)
	{
		thisTask::delay(SEC);
		msg.give(&x);
		x = (x << 1) | (x >> 3);
	}
}

auto cons = Task(consumer);
auto prod = Task(producer);

int main()
{
	cons.start();
	prod.start();

	thisTask::stop();
}

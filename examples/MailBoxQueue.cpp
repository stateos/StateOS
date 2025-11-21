#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto box = MailBoxQueueTT<1, unsigned>();

void consumer()
{
	unsigned x;

	for (;;)
	{
		box.wait(&x);
		led = x;
	}
}

void producer()
{
	unsigned x = 1;

	for (;;)
	{
		thisTask::delay(SEC);
		box.give(&x);
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

#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

Led                            led;
ListTT<unsigned>::Ptr          lst;
MemoryPoolTT<1, unsigned>::Ptr mem;
TaskT<512>::Ptr                cons;
TaskT<768>::Ptr                prod;

void consumer()
{
	unsigned*p;

	lst->wait(&p);
	led = *p;
	mem->give(p);

	This::stop();
}

void producer()
{
	unsigned*p;
	unsigned x = 1;

	for (;;)
	{
		cons = TaskT<512>::Detached(0, consumer);
		This::sleepNext(SEC);

		mem->wait(&p);
		*p=x;
		lst->give(p);
		x = (x << 1) | (x >> 3);
	}
}

int main()
{
	lst = ListTT<unsigned>::Create();
	mem = MemoryPoolTT<1, unsigned>::Create();
	prod = TaskT<768>::Create(0, producer);

	This::stop();
}

#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

void consumer(FastMutex &mut, Led &led)
{
	auto lock = Lock<FastMutex>(mut);
	led.tick();
}

void producer(FastMutex &mut)
{
	auto lock = Lock<FastMutex>(mut);
	this_task::sleepFor(SEC);
}

int main()
{
	Led led;
	FastMutex mut;
	Task prod = Task::Start(2, [&mut]      { producer(mut); });
	Task cons = Task::Start(1, [&mut, &led]{ consumer(mut, led); });
	this_task::sleep();
}

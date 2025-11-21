#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

void consumer(Mutex &mtx, Led &led)
{
	auto lock = UniqueLock(mtx);
	led.tick();
}

void producer(Mutex &mtx)
{
	auto lock = UniqueLock(mtx);
	thisTask::sleepFor(SEC);
}

int main()
{
	Led led;
	Mutex mtx;
	Task prod = Task::Start(2, [&mtx]      { producer(mtx); });
	Task cons = Task::Start(1, [&mtx, &led]{ consumer(mtx, led); });
	thisTask::sleep();
}

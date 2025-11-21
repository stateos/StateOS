#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto mtx = Mutex();

void consumer()
{
	auto lock = LockGuard(mtx);
	led.tick();
}

void producer()
{
	auto lock = LockGuard(mtx);
	thisTask::sleepFor(SEC);
}

auto prod = Task::Start(2, producer);
auto cons = Task::Start(1, consumer);

int main()
{
	thisTask::sleep();
}

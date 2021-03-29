#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

int main()
{
	auto led = Led();
	auto sem = Semaphore::Binary();
	            Task::Create(0, [&]{ sem.wait(); led.tick(); });
	auto prod = Task::Create(0, [&]{ thisTask::sleepFor(SEC); sem.give(); });
	auto tsk = std::move(prod);
	tsk->join();
}

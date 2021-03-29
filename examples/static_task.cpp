#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;
using namespace std::chrono_literals;

int main()
{
	auto led = Led();
	auto sem = Semaphore::Binary();
	for (;;)
	{
		auto tsk0 = Task::Make (2, [&]{ led.tick(); sem.wait(); thisTask::exit(); });
		tsk0.start();
		auto tsk1 = Task::Start(1, [&]{ led.tick(); thisTask::suspend(); });
		auto tsk2 = Task::Start(0, [&]{ led.tick(); sem.post(); thisTask::exit(); });
		auto tsk3 = Task(0, [&]{ led.tick(); thisTask::exit(); });
		tsk1.reset();
		tsk3.start();
		led.tick();
		thisTask::sleepNext(1s);
	}
}

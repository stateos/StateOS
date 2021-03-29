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
		auto tsk0 = new Task(1, []{ thisTask::exit(); });
		tsk0->start();
		            Task::Create  (1, [&]{ led.tick(); thisTask::detach(); thisTask::exit(); });
		            Task::Detached(2, [&]{ led.tick(); sem.wait();         thisTask::exit(); });
		auto tsk1 = Task::Create  (1, [&]{ led.tick(); thisTask::suspend(); });
		auto tsk2 = Task::Create  (0, [&]{ led.tick(); sem.post();         thisTask::exit(); });
		auto tsk3 = Task::Create  (1, [&]{ led.tick();                     thisTask::exit(); });
		delete tsk0; // task must be stopped before deletion
		tsk1->destroy();
		tsk2->join();
		tsk3->detach();
		thisTask::sleepNext(1s);
	}
}

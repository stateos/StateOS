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
		auto tsk0 = new Task(1, []{ this_task::exit(); });
		tsk0->start();
		            Task::Create  (1, [&]{ led.tick(); this_task::detach(); this_task::exit(); });
		            Task::Detached(2, [&]{ led.tick(); sem.wait();         this_task::exit(); });
		auto tsk1 = Task::Create  (1, [&]{ led.tick(); this_task::suspend(); });
		auto tsk2 = Task::Create  (0, [&]{ led.tick(); sem.post();         this_task::exit(); });
		auto tsk3 = Task::Create  (1, [&]{ led.tick();                     this_task::exit(); });
		delete tsk0; // task must be stopped before deletion
		tsk1->destroy();
		tsk2->join();
		tsk3->detach();
		this_task::sleepNext(1s);
	}
}

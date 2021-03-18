#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace stateos;

auto led = Led();
auto sem = Semaphore::Binary();

struct myTask : public Task
{
	myTask(): Task{0, run} { start(); }

	virtual
	void main() = 0;

	private:
	static void run()
	{
		current<myTask>()->main();
	}
};

struct Consumer : public myTask
{
	virtual
	void main() override
	{
		sem.wait();
		led.tick();
	}
};
	
struct Producer : public myTask
{
	virtual
	void main() override
	{
		this_task::sleepFor(SEC);
		sem.give();
	}
};

auto cons = Consumer();
auto prod = Producer();

int main()
{
	this_task::sleep();
}

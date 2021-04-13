#include <stm32f4_discovery.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <chrono>
#include <cassert>
 
std::mutex m;
std::condition_variable cv;
 
bool ready = false;
std::string result;
 
void thread_func()
{
    thread_local std::string thread_local_data = "123";
    std::unique_lock<std::mutex> lk(m);
    result = thread_local_data;
    ready = true;
    std::notify_all_at_thread_exit(cv, std::move(lk));
}
 
int main()
{
	device::Led led;
    std::thread t(thread_func);
    t.detach();
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, []{ return ready; });
    assert(result == "123");
	led = 15;
}

#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <chrono>

#include "scheduler.hpp"
#include "nop_task.hpp"


// Allow an microseconds delay to be directly 'awaited' on.
auto operator co_await(std::chrono::microseconds delay)
{
   return awaitable_stdlib_timer{ delay };
}

// A simple task to schedule 
nop_task resuming_on_delay(std::chrono::microseconds period, const char *msg) {
    for (auto i=0; i<10; i++) {
        co_await  period;
        // At this point we should have waited perod ms * i
        std::cout << msg << " : " << i <<  "\n";
    }
}

int main() {
    std::cout << "main(): ENTRY\n" ; 
    // Run two concurrent loops. The first loop wil run concurrently to the second loop.
    auto t0 = resuming_on_delay(100ms, "100ms timer");
    // The second loop will finish later than the first loop.
    auto t1 = resuming_on_delay(200ms, "200ms timer");
    // As we are not in a task, at this point the loops have not completed, but have placed work in the schedule.
    // TODO - a future based solution is the proper way to do this.
    while (scheduler::update(true)) {}

    std::cout << "main(): EXIT\n" ;
}

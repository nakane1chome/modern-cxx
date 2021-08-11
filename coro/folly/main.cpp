#include <folly/experimental/coro/Task.h>
#include <folly/experimental/coro/BlockingWait.h>
#include <folly/experimental/coro/Collect.h>
#include <folly/futures/Future.h>
#include <folly/executors/GlobalExecutor.h>
#include <folly/init/Init.h>

#include <chrono>
#include <iostream>

using namespace std::literals::chrono_literals;

folly::coro::Task<int> resuming_on_delay(std::chrono::microseconds period, const char *msg) {
    std::chrono::microseconds collected_time{0};
    for (auto i=0; i<10; i++) {
        co_await folly::futures::sleep(period);
        collected_time += period;
        std::cout << msg << " : " << i <<  "\n";
    }
    co_return collected_time.count();
}


int main(int argc, char** argv) {
    std::cout << "main(): ENTRY\n" ; 
    folly::init(&argc, &argv);
    
    auto t0 = resuming_on_delay(100ms, "100ms timer");
    auto t1 = resuming_on_delay(200ms, "200ms timer");
    
    folly::coro::blockingWait(
        folly::coro::collectAll(std::move(t0), 
                                std::move(t1)
            ).scheduleOn(folly::getGlobalCPUExecutor().get()));

    std::cout << "main(): EXIT\n" ;

    return 0;
}


#include <coroutine>
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>

using namespace std::literals::chrono_literals;

/* A quick and dirty class to represent a coroutine that has been scheduled to run at a later time.

   This does NOT match any of the co-routine concepts.

 */
struct schedule_entry {

    using time_point = std::chrono::time_point<std::chrono::steady_clock>;

    schedule_entry(std::coroutine_handle<> h, std::chrono::microseconds delay)  
        : h(h)
        , expires(std::chrono::steady_clock::now() + delay) {}

    /* Overwrite an expired entry */
    void replace(std::coroutine_handle<> new_h, std::chrono::microseconds delay)  {
        done = false;
        h = new_h;
        expires = std::chrono::steady_clock::now() + delay;
    }

    std::coroutine_handle<> h;
    time_point expires; // Absolute time point
    bool done = false;
};


/* A quick and dirty class to act as a container for a set of scheduled co-routines.
   
   This does NOT match any of the co-routine concepts.

 */
struct scheduler {

    inline static std::vector<schedule_entry> _waiting{};

    /* Insert an entry to be scheduled to run after a given delay.
       Either replace an entry that has expired and been run,  or
       append a new entry at the end of the active list
    */
    static void insert(std::coroutine_handle<> h, std::chrono::microseconds delay) {
        for (auto &i : _waiting) {
            if (i.done) {
                // Re-use this slot.
                i.replace(h, delay);
                return;
            }
        }
        // No spare slots - append.
        _waiting.push_back({h, delay});
    }

    /* Visit all pending coutines and see if they are ready to be executed.
       If not then calcuate the time to the next schedule co-routine and wait that long.
       
       Only block if the caller requests.

       Return true if there are still pending co-routines.
    */
    static bool update(bool blocking) {
        std::chrono::time_point now {std::chrono::steady_clock::now()};
        std::chrono::time_point expires = now;
        bool have_expires = false;
        bool pending = false;
        for (auto &i : _waiting) {
            // The list can contain expired slots, ignore them.
            if (i.done) continue;
            // We have seen at least one pending co-routine.
            pending = true;
            if (now > i.expires) {
                // Flag it true and resume execution.
                i.done = true;
                
                i.h.resume(); // << TO PENDING CO-ROUTINE <<

                // Don't continue iteration here, let the caller descide what to do.
                // It's quite possible something else was scheduled in the above call.

                // Return true so it calls us until all entries have
                // been visited and seen to be done..

                return true;
            } else {
                // Keep track of the soonest scheduled co-routine.
                if  (!have_expires)  {
                    have_expires = true;
                    expires = i.expires;
                } else if (i.expires < expires) {
                    have_expires = true;
                    expires = i.expires;
                }
            }
        }
        if (pending && blocking && have_expires) {
            // No entry is active, but there are pending entries and the caller requested we block.
            auto diff = expires - now;
            std::this_thread::sleep_for(diff);
        }
        return pending;
    }
};

/* A class that implements the Awaitable concept .
   The template paramter is a singletone scheduler. (Yes singleons are bad.)
*/
template<class SCHEDULER> struct awaitable_timer {
    awaitable_timer(std::chrono::microseconds  delay) : _delay(delay) {}
    bool await_ready() {
        // Returning true will execute immediately - Only wait if there is a delay.
        return _delay.count() == 0; 
    }
    void await_suspend(std::coroutine_handle<> h) {
        // Insert into the schedule.
        SCHEDULER::insert(h, _delay);
        // Update the schedule and resume any pending entries - but do not block.
        SCHEDULER::update(false);
    }
    void await_resume() {
        // NOTE - At this point the member may have been clobered - dont' trust _delay..
    }
private:
    const std::chrono::microseconds _delay; // Relative delay
};

using awaitable_stdlib_timer = awaitable_timer<scheduler>;

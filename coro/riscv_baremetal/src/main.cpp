/*
   Baremetal main program with co-routines drivem by timer interrupt.
   SPDX-License-Identifier: Unlicense

   NOT TESTED. DO NOT USE.
   
*/


#include <cstdint>
#include <coroutine>
#include <chrono>

// RISC-V CSR definitions and access classes
// Download: wget https://raw.githubusercontent.com/five-embeddev/riscv-csr-access/master/include/riscv-csr.hpp
#include "riscv-csr.hpp"
#include "riscv-interrupts.hpp"
#include "timer.hpp"

#include "scheduler.hpp"
#include "nop_task.hpp"

// Machine mode interrupt service routine
static void irq_entry(void) __attribute__ ((interrupt ("machine")));

// Timer driver 
static driver::timer<> mtimer;

// Global to hold current timestamp
static volatile uint64_t timestamp{0};
static volatile uint32_t resume_count_100{0};
static volatile uint32_t resume_count_200{0};

struct mtimer_clock {

    using duration   = std::chrono::steady_clock::duration;
    using rep        = duration::rep;
    using period     = duration::period;
    using time_point = std::chrono::time_point<mtimer_clock>;
    static constexpr bool is_steady = true;

    static time_point now() noexcept {
        time_point epoch(duration::zero());
        auto duration_since_epoch =  mtimer.get_time<duration>();
        auto retval = epoch + duration_since_epoch;
        return retval;
    }
};


using awaitable_mtimer = awaitable_timer<scheduler<mtimer_clock>>;

// Allow an microseconds delay to be directly 'awaited' on.
auto operator co_await(std::chrono::microseconds delay)
{
   return awaitable_mtimer{ delay };
}

// A simple task to schedule 
nop_task resuming_on_delay(std::chrono::microseconds period, volatile uint32_t &resume_count) {
    for (auto i=0; i<10; i++) {
        co_await  period;
        resume_count=i+1;
    }
}


int main(void) {
    // Global interrupt disable
    riscv::csrs.mstatus.mie.clr();


    timestamp = mtimer.get_time<driver::timer<>::timer_ticks>().count();
    // Timer will fire immediately
     mtimer.set_time_cmp(mtimer_clock::duration::zero());
    // Setup the IRQ handler entry point
    riscv::csrs.mtvec.write( reinterpret_cast<std::uintptr_t>(irq_entry));


    // Test coro

    // Run two concurrent loops. The first loop wil run concurrently to the second loop.
    auto t0 = resuming_on_delay(100ms, resume_count_100);
    // The second loop will finish later than the first loop.
    auto t1 = resuming_on_delay(200ms, resume_count_200);
    // As we are not in a task, at this point the loops have not completed, but have placed work in the schedule.

    // Timer interrupt enable
    riscv::csrs.mie.mti.set();
    // Global interrupt enable
    riscv::csrs.mstatus.mie.set();


    // Busy loop
    do {
        // Get a delay to the next co-routine wakup

        auto [pending, delay] =  scheduler<mtimer_clock>::update();
        mtimer.set_time_cmp(delay);
        // Timer interrupt enable
        riscv::csrs.mie.mti.set();

        // Wait for the timer interrupt if 
        // TODO - we should put WFI in a critical section.
        if (pending && (delay !=  mtimer_clock::duration::zero()))  {
            // Timer exception, 
            __asm__ volatile ("wfi");  
        }

        
    } while (true);
    
    return 0;
}

#pragma GCC push_options
// Force the alignment for mtvec.BASE. A 'C' extension program could be aligned to to bytes.
#pragma GCC optimize ("align-functions=4")
static void irq_entry(void)  {
    auto this_cause = riscv::csrs.mcause.read();
    if (this_cause &  riscv::csr::mcause_data::interrupt::BIT_MASK) {
        this_cause &= 0xFF;
        // Known exceptions
        switch (this_cause) {
        case riscv::interrupts::mti :
            timestamp = mtimer.get_time<driver::timer<>::timer_ticks>().count();
            // Timer interrupt disable
            riscv::csrs.mie.mti.clr();

            break;
        }
    }
}
#pragma GCC pop_options


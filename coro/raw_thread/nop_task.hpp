#include <coroutine>

struct nop_task{

  struct promise_type {
    nop_task get_return_object() { 
        return {}; 
    }
    std::suspend_never initial_suspend() {
        return {}; 
    }
    std::suspend_never final_suspend() noexcept { 
        return {}; 
    }
    void return_void() {
    }
    void unhandled_exception() {
    }
  };
};

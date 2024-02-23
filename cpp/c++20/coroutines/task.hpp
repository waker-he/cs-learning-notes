#include <utility>
#include "coroutine.hpp"

class task {
public:
    struct promise_type;
    using CoroHdl = coroutine_handle<promise_type>;
    struct awaiter;

    // promise type
    struct promise_type {
        promise_type() noexcept = default;
        ~promise_type() = default;

        struct final_awaiter {
            bool await_ready() noexcept { return false; }
            coroutine_handle<> await_suspend(CoroHdl h) noexcept {
                return h.promise().continuation_;
            }
            void await_resume() noexcept {}
        };

        task get_return_object() noexcept { return CoroHdl::from_promise(*this); }
        suspend_always initial_suspend() noexcept { return {}; }
        void unhandled_exception() noexcept { std::unreachable(); }
        final_awaiter final_suspend() { return {}; }
        void return_value(int val) { result = val; }
    private:
        friend struct awaiter;
        coroutine_handle<> continuation_;
        int result;
    };

    // constructors and destructor
    task(coroutine_handle<promise_type> hdl) noexcept : hdl_{hdl} {}
    task(task&& other) noexcept : hdl_{std::exchange(other.hdl_, nullptr)} {}
    task& operator=(task&& other) noexcept {
        if (hdl_ == other.hdl_) return *this;
        if (hdl_) hdl_.destroy();
        hdl_ = std::exchange(other.hdl_, nullptr);
    }
    ~task() { if (hdl_) hdl_.destroy(); }

    // as awaitable
    struct awaiter {
        explicit awaiter(CoroHdl h) noexcept : hdl_{h} {}
        bool await_ready() noexcept { return false; }
        coroutine_handle<> await_suspend(CoroHdl h) {
            hdl_.promise().continuation_ = h;
            return hdl_;
        }
        int await_resume() { return hdl_.promise().result; }
    private:
        CoroHdl hdl_;
    };

    awaiter operator co_await() && noexcept {
        return awaiter{hdl_};
    }
private:
    CoroHdl hdl_;
};
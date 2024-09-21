// ****************************************************************************
// Compiler transform of following code:
// task f(int x);
// task g(int x) {
//     int fx = co_await f(x);
//     co_return fx*fx;
// }
// ****************************************************************************

#include "task.hpp"
#include "manual_lifetime.hpp"

// ****************************************************************************
// Definition of Concrete Coroutine State
// ****************************************************************************

void __g_resume(__coroutine_state*);
void __g_destroy(__coroutine_state*);

template <class Promise, class... Params>
Promise construct_promise([[maybe_unused]] Params&... params) {
    if constexpr (std::constructible_from<Promise, Params&...>)
        return Promise(params...);
    else
        return Promise();
}


using __g_promise_t = coroutine_traits<task, int>::promise_type;

struct __g_state : __coroutine_state_with_promise<__g_promise_t> {
    // constructor
    __g_state(int&& x_on_stack) : x(std::move(x_on_stack)) {
        // init the function pointers used by coroutine_handle::resume()/destroy()/done()
        resume = &__g_resume;
        destroy = &__g_destroy;

        // construct promise
        ::new (static_cast<void*>(std::addressof(promise)))
            __g_promise_t(construct_promise<__g_promise_t>(x));
    }

    // destructor
    ~__g_state() { std::destroy_at(std::addressof(promise)); }

    // argument copies
    int x;

    // information about the suspend-point
    int suspend_point = 0;

    // storage for local variables/temporaries
    //      fx does not span any suspension-point,
    //      so it will be stored on stack frame
    union {
        manual_lifetime<suspend_always> tmp1;
        struct {
            manual_lifetime<task> tmp2;
            manual_lifetime<task::awaiter> tmp3;
        } scope1;
        manual_lifetime<task::promise_type::final_awaiter> tmp4; 
    };

    suspend_always& construct_and_get_initial_awaiter() {
        tmp1.construct_from([] { return suspend_always{}; });
        return tmp1.get();
    }
};

// ****************************************************************************
// Definition of Resume
//  - implemented as state machine
//  - uses suspend point to decide where to jump to
// ****************************************************************************

void __g_resume(__coroutine_state* s) {
    auto* state = static_cast<__g_state*>(s);

    switch (state->suspend_point)
    {
    case 0: goto suspend_point_0;
    case 1: goto suspend_point_1;
    default: std::unreachable();
    }

suspend_point_0:
    {
        state->tmp1.get().await_resume();
        state->tmp1.destroy();

        // execute coroutine body:
        //  int fx = co_await f(x);
        state->scope1.tmp2.construct_from([state] { return f(state->x); });
        auto& awaiter = state->scope1.tmp3.construct_from([state] {
            return static_cast<task&&>(state->tmp2.get()).operator co_await();
        });

        if (!awaiter.await_ready()) {
            // suspend
            state->suspend_point = 1;

            // after suspension
            __exec_await_suspend(awaiter, state->promise);
            return;
            // if __exec_await_suspend (note this is inline) calls resume(),
            // since we return here immediately, the resume() is a tail call
        }
    }

suspend_point_1:
    {
        int fx = state->scope1.tmp3.get().await_resume();
        state->scope1.tmp3.destroy();
        state->scope1.tmp2.destroy();

        //  co_return fx * fx;
        state->promise.return_value(fx * fx);
        goto final_suspend;
    }

final_suspend:
    {
        auto& final_awaiter = state->tmp4.construct_from(
            [] { return __g_promise_t::final_awaiter{}; }
        );

        if (!final_awaiter.await_ready()) {
            // Suspend
            //  this suspend point is for destroy operation,
            //  it is undefined for resume operation
            state->suspend_point = 2;
            state->resume = nullptr;    // mark as done

            // after suspension
            __exec_await_suspend(final_awaiter, state->promise);
            return;
            // if __exec_await_suspend (note this is inline) calls resume(),
            // since we return here immediately, the resume() is a tail call
        }

        final_awaiter.await_resume();
    }

    // if final_suspend does not suspend (execution flows off end of coroutine)
    state->destroy(state);
}


// ****************************************************************************
// Definition of Destroy
//  - destory objects in coroutine state
//  - uses suspend point to decide where to jump to
//      - each code path destorys different temporaries
// ****************************************************************************

void __g_destroy(__coroutine_state* s) {
    auto* state = static_cast<__g_state*>(s);

    switch (state->suspend_point)
    {
    case 0:
        state->tmp1.destroy();
        break;
    case 1:
        state->scope1.tmp2.destroy();
        state->scope1.tmp3.destroy();
        break; 
    case 2:
        state->tmp4.destroy();
        break;
    default: std::unreachable();
    }

    // delete state
    std::destroy_at(state);
    __deallocate_state<__g_promise_t>(state, sizeof(__g_state));
}
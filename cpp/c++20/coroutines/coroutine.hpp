///////////////////////////////////////////////////////////////////////////////
// Simplified version of https://godbolt.org/z/xaj3Yxabn by Lewis Baker
// 
// For simplicity, exceptions are not considered
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <concepts>

// ****************************************************************************
// Coroutine State Definition
// coroutine state contains
// - promise object
// - parmaeters
// - information about the suspend-point
// - storage for any local variables/temporaries whose lifetimes
//   span a suspend point
// ****************************************************************************

struct __coroutine_state {
    static void noop(__coroutine_state*) noexcept {}
    static const __coroutine_state noop_coroutine_state;

    // affordances in type-erasure pattern
    void (*resume)(__coroutine_state*);
    void (*destroy)(__coroutine_state*);
};

inline const __coroutine_state __coroutine_state::noop_coroutine_state{
    &__coroutine_state::noop, &__coroutine_state::noop
};

template <class Promise>
struct __coroutine_state_with_promise : __coroutine_state {
    // has to define default ctor and dtor
    // because Promise might not be trivial type
    __coroutine_state_with_promise() noexcept {}
    ~__coroutine_state_with_promise() {}

    union {
        // reserve storage for promise but does not init it
        // init it after getting parameters in coroutine state,
        // since Promise might have ctor that take these params
        Promise promise;
    };
};

// ****************************************************************************
// Definition of coroutine_handle
// ****************************************************************************

template <class Promise = void>
struct coroutine_handle;

constexpr bool operator==(coroutine_handle<>, coroutine_handle<>) noexcept;
constexpr auto operator<=>(coroutine_handle<>, coroutine_handle<>) noexcept;

template <>
struct coroutine_handle<void> {
    // special member func
    constexpr coroutine_handle() noexcept : state(nullptr) {}
    constexpr coroutine_handle(std::nullptr_t) noexcept : state(nullptr) {}
    constexpr coroutine_handle(const coroutine_handle&) = default;
    constexpr coroutine_handle& operator=(std::nullptr_t) noexcept {
        state = nullptr;
        return *this;
    }
    constexpr coroutine_handle& operator=(const coroutine_handle&) = default;

    // export/import
    constexpr void* address() const noexcept { return state; }
    static constexpr coroutine_handle from_address(void* addr) {
        coroutine_handle h;
        h.state = static_cast<__coroutine_state*>(addr);
        return h;
    }

    // observers
    bool done() const { return state->resume == nullptr; }
    constexpr explicit operator bool() const noexcept { return state != nullptr; }

    // control
    void resume() const { state->resume(state); }
    void destroy() const { state->destroy(state); }

    // compare
    // this does not work with implicit conversion:
    //  constexpr auto operator<=>(const coroutine_handle&) const noexcept = default;
    // need to use outstanding compare function:
    friend constexpr bool operator==(coroutine_handle<> x,
                                     coroutine_handle<> y) noexcept {
        return x.state == y.state;
    }

    friend constexpr auto operator<=>(coroutine_handle<> x,
                                      coroutine_handle<> y) noexcept {
        return x.state <=> y.state;
    }
private:
    __coroutine_state* state;    
};

template <class Promise>
struct coroutine_handle {
    // special member func
    constexpr coroutine_handle() noexcept : state(nullptr) {}
    constexpr coroutine_handle(std::nullptr_t) noexcept : state(nullptr) {}
    constexpr coroutine_handle(const coroutine_handle&) = default;
    constexpr coroutine_handle& operator=(std::nullptr_t) noexcept {
        state = nullptr;
        return *this;
    }
    constexpr coroutine_handle& operator=(const coroutine_handle&) = default;

    // convert to coroutine_handle<>
    constexpr operator coroutine_handle<>() const noexcept {
        return coroutine_handle<>::from_address(address());
    }

    // export/import
    constexpr void* address() const noexcept { return state; }
    static constexpr coroutine_handle from_address(void* addr) {
        coroutine_handle h;
        h.state = static_cast<__state_t*>(addr);
        return h;
    }

    // observers
    bool done() const { return state->resume == nullptr; }
    constexpr explicit operator bool() const noexcept { return state != nullptr; }

    // control
    void resume() const { static_cast<coroutine_handle<>>(*this).resume(); }
    void destroy() const { static_cast<coroutine_handle<>>(*this).destroy(); }

    // promise access
    Promise& promise() const { return state->promise; }

    static coroutine_handle from_promise(Promise& p) {
        coroutine_handle h;
        h.state = reinterpret_cast<__state_t*>(
            reinterpret_cast<char *>(std::addressof(p)) - 
            offsetof(__state_t, promise)
        );
        return h;
    }
private:
    using __state_t = __coroutine_state_with_promise<Promise>;
    state_t* state;
};

struct noop_coroutine_promise {};

using noop_coroutine_handle = coroutine_handle<noop_coroutine_promise>;

// noop_coroutine_handle can only be constructed with this factory func
constexpr noop_coroutine_handle noop_coroutine() noexcept;

template <>
class coroutine_handle<noop_coroutine_promise> {
    constexpr coroutine_handle(const coroutine_handle&) noexcept = default;
    constexpr coroutine_handle& operator=(const coroutine_handle&) noexcept = default;

    constexpr operator coroutine_handle<>() const noexcept {
        return coroutine_handle<>::from_address(address());
    }

    // import/export
    constexpr void* address() const noexcept {
        return static_cast<void*>(const_cast<__coroutine_state *>(state));
    }

    // observer
    constexpr bool done() const noexcept { return false; }
    constexpr explicit operator bool() const noexcept { return true; }

    // control
    constexpr void resume() const noexcept {}
    constexpr void destroy() const noexcept {}

    // promise access
    noop_coroutine_promise& promise() const noexcept {
        static noop_coroutine_promise promise;
        return promise;
    }

private:
    constexpr coroutine_handle() noexcept : state{&__coroutine_state::noop_coroutine_state} {}
    friend constexpr noop_coroutine_handle noop_coroutine() noexcept {
        return {};
    }

    const __coroutine_state* state;
};


// ****************************************************************************
// Basic Awaiters
// ****************************************************************************

struct suspend_never {
    constexpr bool await_ready() const noexcept { return false; }
    constexpr void await_suspend(coroutine_handle<>) const noexcept {}
    constexpr void await_resume() const noexcept {}
};

struct suspend_always {
    constexpr bool await_ready() const noexcept { return true; }
    constexpr void await_suspend(coroutine_handle<>) const noexcept {}
    constexpr void await_resume() const noexcept {}
};


// ****************************************************************************
// Determine Promise Type
// ****************************************************************************

template <class Ret, class... Params>
struct coroutine_traits {
    using promise_type = typename std::remove_cvref_t<Ret>::promise_type;
};


// ****************************************************************************
// Execute await_suspend according to its return type
// ****************************************************************************

template <class Awaiter, class Promise>
inline void __exec_await_suspend(Awaiter& awaiter, Promise& promise) {
    using return_type = decltype(awaiter.await_suepend(declval<std::coroutine_handle<>>()));

    auto hdl = coroutine_handle<Promise>::from_promise(promise);
    if constexpr (std::same_as<return_type, void>)
        awaiter.await_suspend(hdl);
    else if constexpr (std::same_as<return_type, bool>)
        if (!awaiter.await_suspend(hdl))
            hdl.resume();
    else {
        static_assert(std::same_as<return_type, coroutine_handle<>);
        // for noop coroutine, here is noop
        awaiter.await_suspend(hdl).resume();
    }
}

// ****************************************************************************
// Ramp Function
// ****************************************************************************

// call to coroutine g will be converted to a call to __ramp_func:
//      auto t = g(2);
// equivalent to:
//      auto t = __ramp_func<__g_state, task, int>(2);
template <class State, class Ret, class... Params>
Ret __ramp_func(Params... args) {
    using Promise = typename coroutine_traits<Ret, Params...>::promise_type;

    // allocate and construct the coroutine state
    auto allocate_state = [] <class Promise> (std::size_t sz) {
        // if compiler can prove that the lifetime of coroutine state nested
        // in the caller, then allocate_state allocates on stack, for
        // simplicity, assume we always call `new` to allocate
        if constexpr ( requires { Promise::operator new(sz); }) 
            return Promise::operator new(sz);
        else
            return ::operator new(sz);
    };
    State* s = std::construct_at(
        static_cast<State*>(allocate_state.operator()<Promise>(sizeof(State))),
        std::move(args)...
        // copy/move the args into the coroutine state
    );

    // construct the return object from the promise
    Ret ret = s->promise.get_return_object();

    // initial await
    auto& initial_awaiter = s->construct_and_get_initial_awaiter();
    if (!initial_awaiter.await_ready())
        __exec_await_suspend(initial_awaiter, s->promise);
    else
        s->resume(s);

    return ret;
}

// symmetric to allocate_state
template <class Promise>
void __deallocate_state(void* ptr, std::size_t sz) {
    if constexpr ( requires { Promise::operator new(sz); }) 
        Promise::operator delete(ptr, sz);
    else
        return ::operator delete(ptr, sz);
}


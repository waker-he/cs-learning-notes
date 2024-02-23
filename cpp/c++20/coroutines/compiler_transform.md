# C++ Coroutine: Compiler Transform

```cpp
// Code to transform
task f(int x);
task g(int x) {
    int fx = co_await f(x);
    co_return fx*fx;
}
```
- [task.hpp](./task.hpp): definition of coroutine return type `task`, which is a coroutine that is executed when being `co_await`ed on
- [coroutine.hpp](./coroutine.hpp): definition of header `<coroutine>`
- [compiler_transform.cpp](./compiler_transform.cpp): compiler transform of coroutine `g`
    - [manual_lifetime.hpp](./manual_lifetime.hpp): manage temporaries' lifetime

## Coroutine State

- Contains:
    - promise object
    - parameters
    - information about the suspend-point
    - storage for any local variables/temporaries whose lifetimes span a suspend point
- `std::coroutine_handle<>` can have a pointer that point to any coroutine state, so the coroutine state type should be type-erased
    - name this type `__coroutine_state`
    - store two function pointers to `resume` and `destroy` and should not declare any data members above
    - when suspended at `final_suspend`, `resume` is set to `nullptr`, so we can check whether `resume == nullptr` to see if the coroutine is `done`
- `std::coroutine_handle<promise_type>` can have a pointer that point to any coroutine state with the same `promise_type`, so we should have another derived class
    - name this type `__coroutine_state_with_promise<promise_type>`
    - it should only declare promise object
- name the most derived coroutine state type `__<CORO_NAME>_state`
    - for coroutine `g`, we have `__g_state`

## Ramp Function

- exceptions thrown inside ramp function will propagate to caller, elsewhere it will transfer to `Promise::unhandle_exception()`
- steps:
    1. allocate and construct coroutine state
        1. copy/move args
        2. construct promise
        3. construct awaiter returned by `initial_suspend()` in storage
    2. construct return object from the promise in the state
    3. call `state->init_awaiter.await_ready()` 
        - if returns `false`, call `await_suspend`
        - else, call resume
    4. return `return_object`


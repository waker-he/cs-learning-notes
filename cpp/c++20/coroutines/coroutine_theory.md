## [Index](./coroutines.md)

## Contents

- [Coroutine Theory](#coroutine-theory)
    - [Activation Record](#activation-record)
    - [Call Operation (Subroutine & Coroutine)](#call-operation-subroutine--coroutine)
    - [Return Operation (Subroutine & Coroutine)](#return-operation-subroutine--coroutine)
    - [Suspend Operation (Coroutine Only)](#suspend-operation-coroutine-only)
    - [Resume Operation (Coroutine Only)](#resume-operation-coroutine-only)
    - [Destroy Operation (Coroutine Only)](#destroy-operation-coroutine-only)

# Coroutine Theory

- coroutine is function that can be suspended to transfer control to __caller/resumer and other coroutines__ and be later resumed
- by calling functions, subroutine can also suspend itself without destroying activation record, but only transfer control to __callee__
- coroutine is a generalization of subroutine, besides __Call__ and __Return__ operations, there are three extra operations: __Suspend__, __Resume__ and __Destroy__

## Activation Record

- block of memory that holds the current state of a particular invocation of a function

### Subroutine AR

- with normal functions (subroutine), all activation records have __strictly nested lifetimes__
    - this allows the use of a highly efficient memory allocation data-structure called "__stack__" for allocating and deallocating the activation records
        - to allocate and deallocate, simply moving the frame pointer (usually a register designated for it) around
    - so activation record of subroutine is also called "__stack frame__", which includes
        - parameters and local variables
        - return address
        - address of activation record of caller

### Coroutine AR

- lifetime of coroutine activation record might not be nested, allocated using heap instead of stack
    - if compiler can prove that it is indeed nested, it is still allowed to allocate on stack
- comprised of two parts
    - __coroutine frame__ (typically on heap): parts that need to be preserved acrosss coroutine suspensions
        - parameters
        - address of resumption-point (kept during suspension)
        - local variables that span across suspension-points
    - __stack frame__: parts that only need to be kept around during executing
        - local variables that does not span any suspension-points
            - so there would be no local objects alive on stack frame at suspension-point, because objects live beyond suspension-point won't be on stack frame
        - return address (to caller/resumer)
        - address of activation record of caller/resumer

## Call Operation (Subroutine & Coroutine)

The Call operation creates an activation record, suspends execution of the calling function and transfers execution to the start of the function being called.
1. save values of registers in current activation record (specifically in coroutine frame if caller is coroutine)
2. construct objects of passed parameter into new stack frame
3. __coroutine only__: allocate __coroutine frame__ and move/copy the parameters from __stack frame__ to __the coroutine frame__
    - parameters left on stack frame are destroyed on first suspension/return
4. push __return address__ and __current frame pointer__ on stack
5. move frame pointer to base of `(isSubroutine ? new stack frame : coroutine frame)`
6. jump to the address of start of the function

## Return Operation (Subroutine & Coroutine)

The Return operation marks the completion of the function and it transfers execution back to caller/resumer
1. construct returned object in:
    - __subroutine__: in place (or copy/move construct it) in activation record of caller
    - __coroutine__: customized by the coroutine
2. destroy local objects on current stack frame
3. pop stack frame and transfer control back to caller/resumer

## Suspend Operation (Coroutine Only)

- The Suspend operation of a coroutine allows the coroutine to suspend execution in the middle of the function and transfer execution back to the caller/resumer of the coroutine.
- keywords `co_await` and `co_yield` will request a suspend operation, if `await_ready()` returns `false`, then start to suspend:
    1. save values of registers in coroutine frame
    2. save resumption-point in coroutine frame
- after suspension, execute some additional logic in `await_suspend()`, it is completely customizable
    - can choose to transfer to another coroutine by changing the return type of `await_suspend`
    - if so, stack frame is reused by the called/resumed coroutine and not popped
    - else, pop stack frame and transfer control back to caller/resumer

## Resume Operation (Coroutine Only)

The Resume operation can be performed on a coroutine that is currently in the ‘suspended’ state.

1. if not resumed by `await_suspend`, allocate a new stack frame
    - save registers, return address and frame pointer
2. set frame pointer to address of coroutine frame
3. transfer execution to __resumption-point__ stored in coroutine frame


## Destroy Operation (Coroutine Only)

The Destroy operation destroys the coroutine frame without resuming execution of the coroutine.

1. allocate a new stack frame
    - save registers, return address and frame pointer
    - set frame pointer to address of coroutine frame
2. transfer execution to a code-path that calls the destructors of all local variables in-scope at the suspend-point then freeing the memory used by the coroutine frame.

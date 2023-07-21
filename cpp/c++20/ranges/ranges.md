- [Ranges and Views](#ranges-and-views)
- [Benefits](#benefits)
- [Caveats](#caveats)
- [Spans](#spans)

# Ranges and Views

- a `range` object is a seqeunce of elements defined by a begin point and end point
    ```cpp
    template <class T>
    concept range = requires(T& t) {
        ranges::begin(t);
        ranges::end(t);
    };
    ```
    - begin point: begin iterator
    - end point
        - sentinel
            - end iterator for traditional STL
            - do not need to be the same type as begin iterator
        - count
- a `view` object is a lightweight `range` that is cheap to move and copy (if copyable)
    ```cpp
    template <class T>
    concept view = ranges::range<T> && std::movable<T> && ranges::enable_view<T>;
    ```
    - usually does not own the elements in the range
        - exception: `owning_view` (move-only), `single_view`
    - `std::views::all(rg)` convert a range to a view
        - it returns `std::views::all_t<decltype(rg)>`, which equals:
            - `decltype(rg)` if `rg` is already a view
            - `std::ranges::ref_view` of `rg` if `rg` is an lvalue
            - `std::ranges::owning_view` of `rg` if `rg` is rvalue
    - all `viewType`s in `std::ranges` inherit from `view_interface<viewType>`
- `borrowed range` is a range that a function can take it by value and return a iterator obtained from it without danger of dangling
    ```cpp
    template <class R>
    concept borrowed_range =
        ranges::range<R> &&
        (std::is_lvalue_reference_v<R> ||
         ranges::enable_borrowed_range<std::remove_cvref_t<R>>);
    ```
    - check [cppreference](https://en.cppreference.com/w/cpp/ranges/borrowed_range) to see what `viewType`s have `enable_borrowed_range<viewType> = true`
- `viewable range` is a range that can be safely converted to a view with range adaptor `std::views::all`
    - non-`view`
        - an lvalue non-`view` range is always a `viewable_range`, we simply takes `ref_view` of it
        - an rvalue non-`view` range is converted as `owning_view`
    - `view`: `std::constructible_from<std::remove_cvref_t<T>, T>`
        - an lvalue `view` must be copyable
        - an rvalue `view` is always a `viewable_range` since `view` is `movable`

# Benefits

- can pass range as a single argument instead of passing two arguments of iterators for algorithms
    ```cpp
    std::sort(vec.begin(), vec.end());
    std::ranges::sort(vec);
    ```
- implementation uses [concepts feature](../../cppcon/template/template.md#concepts-since-c20), which can give more understandble error messages
- uses __sentinel__ to define end of range
    - does not need to be same type as begin iterator
        - avoid duplicate traversal to find end iterator (c-str)
    - allows unbounded range, which can also avoid bound checking and less assembly code
- pipeline of __range adapters__, convenient and more readable for processing ranges
    - __range adaptor object__: [customization point object](../../README.md#argument-dependent-lookup-adl) that takes a `viewable range` and produces a `view`
        - view is adapted according to other passed-in arguments
        - overloaded operator `|` that takes the output range on the other side as first argument
        - all in namespace `std::views` (alias for `std::ranges::views`)
            - avoid collision with `std::ranges` algorithms like `transform`
            - all view types are in namespace `std::ranges`
    ```cpp
    auto first4 = std::views::take(coll, 4);
    auto view2to4 = coll | std::views::take(5) | std::views::drop(2);
    ```
    - caveat: hard to define custom adapter that opt-in using pipe syntax

# Caveats

## Caching

- one of semantic requirements of concept `std::ranges::range` is that `ranges::begin()` and `ranges::end()` should operate on amortized constant time
- caching of begin iterator and end iterator (if same type as begin iterator) may be needed to satisfy this requirement
- consequences
    - result in inconsistencies if underlying range changes
        - `std` view types that cache `begin()`:
            - `filter_view`
            - `drop_while_view`
            - `drop_view` (unless random-access range and sized range)
            - ...
        - do not use a view with caching after `begin()` has been called and the underlying range has been modified
    - not `const`-iterable: a `const` view that needs caching is not iterable
        - any views that need to change states when iterating are not `const`-iterable

## `const`-ness propagation

- `const` non-owning views do not propagate `const`-ness to its data like containers do
- view can be thought as a pointer to a range
    - a `const` pointer still allows us to modify its pointed-to data
    - `std::cbegin(v)` implemented as `std::as_const(v).begin()`, which does not work
- solutions
    - using `std::as_const` to wrap the container when creating view from it
    - since C++23:
        - new range adaptor `std::views::as_const` (only works for views that do not cache)
        - `std::ranges::cbegin` (works for all views)

## dangling iterator returned from `ranges` algorithm
- for non `borrowed range`, return `ranges::dangling` object, an empty struct
    - compiler error for usage
- iterator can still dangle if the underlying range of `borrowed range` is destructed before the `borrowed range`
    - `std::string_view`, `std::span`, `std::ranges::subrange`, ...


# Spans

```cpp
inline constexpr std::size_t dynamic_extent = -1; // max size_t
template <class T, std::size_t Extent = std::dynamic_extent>
class span;

vector<int> vec = {1,2,3};
std::span sp1{vec}; // dynamic extent, size can vary

array<int, 3> arr = {1,2,3};
std::span sp2{vec}; // static extent, size fixed
```
- refer to [cppreference `std::span`](https://en.cppreference.com/w/cpp/container/span) for operations
- generalization of string views, in `std` namespace instead of `std::ranges`
- requires contiguous storage of elements
    - major difference to `subrange`
    - can be created using `std::views::counted`
    - do not require iterator support for the type referring to
        - just need to provide a `data()` member
- cheap and fast `std::ranges::view`
    - `sizeof(span<T>) == sizeof(T*) + sizeof(std::size_t)`
    - `sizeof(span<T, 3>) == sizeof(T*)`
- careful when referrring to dynamically growing container
    - might need to reinitialize
    ```cpp
    auto oldCapa = vec.capacity();
    vec.push_back(1);
    if (oldCapa != vec.capacity()) {
        sp = std::span{vec};
    }
    ```

# Ranges and Views

- a __range__ object is a seqeunce of elements defined by a start point and an end point
    - start point: begin iterator
    - end point
        - sentinel
            - end iterator for traditional STL
            - do not need to be the same type as begin iterator
        - count
- a __view__ object is a lightweight __range__ that is cheap to copy/move
    - usually does not own the elements in the range
        - exception: `std::ranges::owning_view` (move-only)

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
- pipeline of range adapters, convenient and more readable for processing ranges
    - range adaptor object: function object that takes a range and produces a view
        - view is adapted according to other passed-in arguments
        - overloaded operator `|` that takes the output range on the other side as first argument
    ```cpp
    auto first4 = std::views::take(coll, 4);
    auto view2to4 = coll | std::views::take(5) | std::views::drop(2);
    ```

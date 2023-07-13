# Content

- [Chapter 1: Comparisons and Operator `<=>`](#chapter-1-comparisons-and-operator)

# Chapter 1: Comparisons and Operator `<=>`

- defining `operator==` enables use for all equality operators
    - compiler rewirtes `a != b` to `!(a == b)`
    - change the order when implicit conversion applies
- defining `operator<=>` enables uses for all relational operators
    - called "three-way comparison operator", "spaceship operator"
    - compiler rewrites `a < b` to `a <=> b < 0`
    - change the order when implicit conversion applies
    - defined as `default` to perform member-wise comparison
        - also implicitly generated `default` version of `operator==`
      ```cpp
      struct S {
        // enable use of all comparison operaotors
        [[nodiscard]] auto operator<=> (const S& rhs) const = default;
      };
      ```
- for `default` versions
    - `noexcept` if comparing the members `noexcept`
    - `constexpr` if possible

## Return Types of `operator<=>`

- returns __comparison category types__ instead of `bool`
- `std::strong_ordering` for the types that one of (`==`, `<`, `>`) must applies for any two values
    - contains static member constants `less`, `equal`/`equivalent`, `greater`
- `std::weak_ordering` for the types that __equivalent__ values might not be __equal__ (eg. case-insensitive strings)
    - contains static member constants `less`, `equivalent`, `greater`
- `std::partial_ordering` for the types that contains comparable value (e.g. `NaN` in floating-point type, any comparison yields `false`)
    - contains static member constants `less`, `equivalent`, `greater`, `unordered`
- notes
    - static member constants have the same type as the class
        - e.g., `std::strong_ordering::less` is of type `std::strong_ordering`
    - comparison category types are not integers, they contain an integral type (`signed char` in [gcc](https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/libsupc%2B%2B/compare)) data member
        - can only be compared with `0` (or `nullptr`/`NULL`)
    - stronger ordering types can be implicitly converted to weaker ordering types
        ```cpp
        using namespace std;
        static_assert(
            is_same_v<
                common_comparison_category_t<strong_ordering,
                                             weak_ordering>,
                weak_ordering
            >);
        ```

# Chapter 2: Placeholder Types for Function Parameters

- function template syntax can be abbreviated
    ```cpp
    void foo(auto t);
    ```
- in the following code, `foo1` is equivalent to `foo2`
    ```cpp
    template <class T>
    void foo1(auto t1, T t2);

    template <class T2, class T1>
    void foo2(T1 t1, T2 t2);
    ```
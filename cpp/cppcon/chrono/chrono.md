# `<chrono>`

- in namespace `std::chrono`
- three main components
    - [duration](#duration)
    - [time point](#time-point)
    - [clock](#clock)

# duration

```cpp
template <class Rep, class Period = std::ratio<1>>
class duration;
```

- [cppreference link](https://en.cppreference.com/w/cpp/chrono/duration)
- `Rep` or `duration::rep`: an arithmetic type representing the number of ticks
- `Period` or `duration::period`: the tick period
    - `std::ratio<Numerator, Denominator>`
        - `Denominator = 1` by default
        - `std::ratio<1>` represents a __second__ period
        - `num` and `den` member objects (`constexpr static std::intmax_t`)
- have helper alias types from `std::chrono::nanoseconds` to `std::chrono::years`
- durations are implicitly convertible to each other
    - need to be lossless
    - non lossless still requires explicit conversion
        - `duration_cast`: discard floating point part
        - `floor`, `ceil`, `round`
    ```cpp
    using namespace std::literals::chrono_literals;
    using frame = std::chrono::duration<int, ratio<1, 60>>;
    frame frs = 4min - 4s;
    mins = std::chrono::duration_cast<std::chrono::minutes>(frs);
    ```
- non implicitly convertible between arithmetic types
    ```cpp
    std::chrono::seconds s = 2; // ERROR
    std::chrono::seconds s{2};  // OK

    int i = s;          // ERROR
    int i = s.count();  // OK
    ```

# time point

```cpp
template<
    class Clock,
    class Duration = typename Clock::duration
> class time_point;
```

- arithmetic operations are like pointers
    - `{time_point - time_point} -> duration`
        - this operation only valid for `time_point` with same `clock`
    - `{time_point +/- duration} -> time_point`
- `time_since_epoch` returns the `time_point` as `duration` since the start of its clock

# clock

a clock is a bundle of a `duration`, a `time_point` and a `static` function to get the current time

```cpp
class some_clock {
    using duration = std::chrono::microseconds;
    using time_point = std::chrono::time_point<some_clock>;

    static constexpr bool is_steady = false;

    static time_point now() noexcept;
};
```

- `std::chrono::system_clock` gives you the time of the day and the date
- `std::chrono::steady_clock` is just for timing

```cpp
auto t1 = std::chrono::steady_clock::now();
f();
auto t2 = std::chrono::steady_clock::now();
std::cout << std::chrono::microseconds{t1 - t2}.count() << "us\n";
```

# References

- [CppCon 2016: Howard Hinnant “A ＜chrono＞ Tutorial"](https://www.youtube.com/watch?v=P32hvk8b13M)
- [cppreference chrono](https://en.cppreference.com/w/cpp/chrono)
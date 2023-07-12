template <class T, T v>
struct integral_constant {
    constexpr static T value = v;
    constexpr operator T() const noexcept { return value; }
    constexpr T operator ()() const noexcept { return value; }
};

template <bool b>
using bool_constant = integral_constant<bool, b>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

template <class T, class U>
struct is_same : false_type {};

template <class T>
struct is_same<T, T> : true_type {};

template <class T, class U>
inline constexpr bool is_same_v = is_same<T, U>::value;

template <class T, class... Ts>
struct is_one_of {
    constexpr static bool value = (... || is_same_v<T, Ts>);
};

template <class T, class... Ts>
inline constexpr bool is_one_of_v = is_one_of<T, Ts...>::value;

int main() {
    static_assert(!is_one_of_v<int, float, double>);
    static_assert(is_one_of_v<int, float, double, int>);
    static_assert(is_one_of_v<int, int>);
    static_assert(!is_one_of_v<int>);
}
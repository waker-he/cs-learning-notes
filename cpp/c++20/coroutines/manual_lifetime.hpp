#include <concepts>
#include <type_traits>
#include <memory>

/*
Seprating the steps for beginning lifetime
- Obtain storage with proper size and alignment for type T
- Initializing the object
*/
template <class T>
class manual_lifetime {
public:
    manual_lifetime() noexcept = default;
    ~manual_lifetime() = default;

    // Not copyable/movable
    manual_lifetime(const manual_lifetime&) = delete;
    manual_lifetime(manual_lifetime&&) = delete;
    manual_lifetime& operator=(const manual_lifetime&) = delete;
    manual_lifetime& operator=(manual_lifetime&&) = delete;

    template <class Factory>
        requires
            std::invocable<Factory&> &&
            std::same_as<std::invoke_result_t<Factory&>, T>
    T& construct_from(Factory factory) noexcept(std::is_nothrow_invocable_v<Factory&>) {
        // note std::construct_at() needs passing argument instead of factory
        // so we direclty use placement new here
        return *::new (static_cast<void*>(&storage)) T(factory());
    }

    void destroy() noexcept(std::is_nothrow_destructible_v<T>) {
        std::destroy_at(reinterpret_cast<T*>(&storage));
    }

    T& get() & noexcept {
        return *std::launder(reinterpret_cast<T*>(&storage));
    }
private:
    alignas(T) char storage[sizeof(T)];
};
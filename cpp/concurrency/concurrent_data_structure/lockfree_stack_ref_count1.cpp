#include <atomic>
#include <optional>
#include <concepts>
#include <memory>

template <typename T>
  requires std::is_nothrow_move_constructible_v<T>
class lockfree_stack {
private:
    struct node {
        T data;
        std::shared_ptr<node> next; // every node is managed by shared_ptr
    };
    std::atomic<std::shared_ptr<node>> head;
public:
    void push(T const& data) {
        std::shared_ptr<node> new_node = new node{data, head.load(std::memory_order_relaxed)};
        while (!head.compare_exchange_weak(new_node->next, new_node,
                std::memory_order_release, std::memory_order_relaxed));
    }

    std::optional<T> pop() noexcept {
        std::shared_ptr<node> old_head = head.load(std::memory_order_relaxed);
        while (old_head && !head.compare_exchange_weak(old_head, old_head->next,
                std::memory_order_acquire, std::memory_order_relaxed));
        std::optional<T> item;
        if (old_head) item.emplace(std::move(old_head->data));
        return item;
    }
};
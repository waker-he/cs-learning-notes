#include <atomic>
#include <optional>
#include <concepts>

/*
- exception safety
    - another option to handle exception safety is to return pointer in pop,
    in this case we have to make the heap allocation. If we put this allocation
    in pop then the popped item will be lost when bad_alloc is thrown. Instead,
    we makes node.data to be pointer and do the heap allocation in push
    - pass in a reference does not work since if the move assign throws, the data
    will still be lost
- not wait-free: note that CAS operations in push and pop are in while loop and it
can loop forever if compare_exchange_weak keeps failing
*/
template <typename T>
  requires std::is_nothrow_move_constructible_v<T>
class lockfree_stack {
private:
    struct node {
        T data;
        node* next;
    };
    std::atomic<node*> head;
public:
    void push(T const& data) {
        node* new_node = new node{data, head.load(std::memory_order_relaxed)};
        while (!head.compare_exchange_weak(new_node->next, new_node,
                std::memory_order_release, std::memory_order_relaxed));
    }

    std::optional<T> pop() {
        node* old_head = head.load(std::memory_order_relaxed);
        // old_head is dereferenced here, if other thread comes in,
        // pop this node and delete the node when returning, then we get UB
        while (old_head && !head.compare_exchange_weak(old_head, old_head->next,
                std::memory_order_acquire, std::memory_order_relaxed));
        std::optional<T> item;
        if (old_head) item.emplace(std::move(old_head->data));
        return item;

        // leaking memory pointed by old_head
    }
};
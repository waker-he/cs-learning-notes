#include <atomic>
#include <optional>
#include <concepts>

template <typename T>
  requires std::is_nothrow_move_constructible_v<T>
class lockfree_stack {
private:
    struct node;
    struct counted_node_ptr {
        // a new node in the list must be referenced by head or previous node
        // value of external_count does not matter if ptr == nullptr
        int external_count = 1; 
        node* ptr = nullptr;    // when stack is empty
        counted_node_ptr(node* p) : ptr(p) {}
    };
    
    struct node {
        T data;
        counted_node_ptr next;
        std::atomic<int> internal_count = 0;

        node(T const& data_, counted_node_ptr const& next_)
            : data{data_}, next{next_} {}
    };

    std::atomic<counted_node_ptr> head;
public:
    void push(T const& data) {
        counted_node_ptr new_node{new node{data, head.load()}};
        while (!head.compare_exchange_weak(new_node.ptr->next, new_node,
                                           std::memory_order_release,
                                           std::memory_order_relaxed));
    }

    std::optional<T> pop() {
        std::optional<T> res;
        counted_node_ptr old_head = head.load(std::memory_order_relaxed);

        while (true) {
            // similar idea to hazard pointer, before dereference old_head.ptr,
            // we need to make sure the node is not deleted, and we achieve this by
            // incrementing external count
            counted_node_ptr new_counter;
            do {
                new_counter = old_head;
                ++new_counter.external_count;

                // compare_exchange_strong can fail in two cases:
                //  - external count is incremented by other threads
                //  - the head loaded in old_head is already popped by other thread
            } while (!head.compare_exchange_strong(old_head, new_counter,
                                                   std::memory_order_acquire,
                                                   std::memory_order_relaxed));

            // update old_head.external_count so that it matches head for later
            // compare_exchange operation
            old_head.external_count = new_counter.external_count;

            node* const ptr = old_head.ptr;
            if (!ptr) return res;   // stack is empty

            // dereference ptr here, must have std::memory_order_acquire before
            if (head.compare_exchange_strong(old_head, ptr->next, std::memory_order_relaxed)) {
                res.emplace(std::move(ptr->data));
                int const count_increase = old_head.external_count - 2;

                // release the change made to the node (move from data) so that if it is other
                // thread that will delete it, the other thread can acquire and see the change correclty
                if (ptr->internal_count.fetch_add(count_increase, std::memory_order_release) == -count_increase) {
                    delete ptr;
                }
                break;
            }

            // the external count is incremented by other threads or the head is
            // popped by other thread, we need to reload head

            // before reloading, check whether current thread is the last reference
            // to the node
            if (ptr->internal_count.fetch_sub(1, std::memory_order_relaxed) == 1) {
                // if need to delete ptr, we need to acquire the change made by the
                // thread that pops this node
                std::atomic_thread_fence(std::memory_order_acquire);
                delete ptr;
            }
        }

        return res;
    }
};
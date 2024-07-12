#include "manual_lifetime.hpp"
#include <optional>
#include <type_traits>

template <typename T>
    requires std::is_nothrow_move_constructible_v<T>
class waitfree_spsc_queue {
    struct node {
        manual_lifetime<T> data;
        node* next = nullptr;
    };

    node* head;
    std::atomic<node*> tail;    // to get synchronizes-with relation
public:
    waitfree_spsc_queue() : head(new node{}), tail(head) {}
    waitfree_spsc_queue(waitfree_spsc_queue const&) = delete;
    waitfree_spsc_queue& operator=(waitfree_spsc_queue const&) = delete;
    ~waitfree_spsc_queue() {
        while (pop());
        delete head;
    }

    void push(T data) {
        node* new_tail = new node{};
        node* old_tail = tail.load(std::memory_order_relaxed);
        old_tail->data.construct(std::move(data));
        old_tail->next = new_tail;
        tail.store(new_tail, std::memory_order_release);
    }

    std::optional<T> pop() {
        std::optional<T> res;
        if (head == tail.load(std::memory_order_acquire)) return res;
        node* const old_head = head;
        head = head->next;
        res.emplace(std::move(old_head->data.get()));
        old_head->data.destroy();
        delete old_head;
        return res;
    }
};
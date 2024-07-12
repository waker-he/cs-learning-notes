#include <mutex>
#include <optional>
#include <memory>
#include "manual_lifetime.hpp"

/*
- push appends at tail and pop will consume the front
- dummy node is needed otherwise we need both locks for push or pop:
    - if queue is empty
        - we need to set head in push and set tail in pop
    - else if queue has one item:
        - push and pop accesses the same `next` pointer
*/

template <typename T>
class threadsafe_queue {
private:
    struct node {
        manual_lifetime<T> data;    // to enable dummy node, alternative: pointer
        node* next = nullptr;
    };

    node* head;
    node* tail;     // tail will always point to a dummy node
    std::mutex head_mutex;
    std::mutex tail_mutex;
    std::condition_variable data_cond;

    /*
    called in pop operation, needed so that:
    - no data race for tail between push and pop
    - push synchronizes-with pop 
        - so that pop can see the correct data published by push
    */
    node* get_tail() {
        std::scoped_lock tail_lock(tail_mutex);
        return tail;
    }

    node* pop_head() {
        std::scoped_lock head_lock(head_mutex);
        /*
        note that the lock of head_mutex must be before get_tail(), otherwise:
        after get_tail() and before locking head_mutex:
         1. some threads pop all items
         2. some threads push some items
            - now the old_tail is not tail anymore
            - head == old_tail will be true even if the queue is not empty
         3. some threads pop all items again
            - now the old_tail is not even part of the queue
            - head == old_tail will be false even though the queue is empty
                - popping the item will break the invariant that head != nullptr
        */
        if (head == get_tail()) return nullptr;

        node* old_head = head;
        head = head->next;
        return old_head;
    }

    node* wait_pop_head() {
        std::unique_lock head_lock(head_mutex);
        data_cond.wait(head_lock, [&]{return head != get_tail();});
        node* old_head = head;
        head = head->next;
        return old_head;
    }
public:
    threadsafe_queue() : head(new node{}), tail(head) {}
    threadsafe_queue(threadsafe_queue const&) = delete;
    threadsafe_queue& operator=(threadsafe_queue const&) = delete
    ~threadsafe_queue() {
        while (head != tail) {
            auto next = head->next;
            head->data.destroy();
            delete head;
            head = next;
        }

        // last node is a dummy node
        delete head;
    }

    void push(T new_data) {
        node* p(new node{});
        {
            std::scoped_lock tail_lock(tail_mutex);
            tail->data.emplace(std::move(new_data));
            tail->next = p;
            tail = p;
        }

        // unlock before notify so that the thread waking up can grab
        // the lock immediately
        data_cond.notify_one();
    }

    std::optional<T> try_pop() {
        std::unique_ptr<node> old_head{pop_head()};
        // perform the return and node delete outside the lock
        if (!old_head) return {};
        std::optional<T> item(std::move(old_head->data.get()));
        old_head->data.destroy();
        return item;
    }

    T wait_and_pop() {
        std::unique_ptr<node> old_head{wait_pop_head()};
        T item(std::move(old_head->data.get()));
        old_head->data.destroy();
        return item;
    }
};
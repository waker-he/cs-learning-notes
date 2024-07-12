#include <memory>
#include <atomic>

template <typename T>
class lockfree_queue {
    struct node {
        // to perform CAS operation on data, we must use a ptr
        std::atomic<T*> data{nullptr};   
        // next pointer made as atomic so that other thread can help
        // set next of tail in push
        std::atomic<std::shared_ptr<node>> next{nullptr};
    };

    std::atomic<std::shared_ptr<node>> head;
    std::atomic<std::shared_ptr<node>> tail;
public:
    lockfree_queue() : head(std::make_shared<node>()), tail(head.load()) {}
    lockfree_queue(lockfree_queue const&) = delete;
    lockfree_queue& operator=(lockfree_queue const&) = delete;
    ~lockfree_queue() { while (pop()); }

    void push(T data) {
        auto new_data{std::make_unique<T>(std::move(data))};
        auto new_tail{std::make_shared<node>()};

        while (true) {
            auto old_tail = tail.load();
            T* old_data = nullptr;
            if (old_tail->data.compare_exchange_strong(old_data, new_data.get())) {
                // Successfully emplace the data and declare that the current tail
                // belongs to current thread, next operations are:
                //   1. set the next of current tail to be new tail
                //   2. set tail to be new tail
                // If we do these two operations on current thread, then before it
                // finishes these two operations, other threads can theoretically
                // infinitely reload the tail and retry the CAS operation on data,
                // we are effectively forming a spinlock with tail->data here, so
                // it won't be lock-free.
                //
                // Solution: since the new tail is just a dummy node, any other
                // thread is able to help complete these two operations

                // 1. set the next of current tail to be new tail
                std::shared_ptr<node> old_next;
                if (!old_tail->next.compare_exchange_strong(old_next, new_tail)) {
                    // other thread helps set the old_tail->next
                    new_tail = old_next;
                }

                // 2. set tail to be new_tail
                // If it fails, then it means other thread has already done it for us,
                // nothing to handle here.
                tail.compare_exchange_strong(old_tail, new_tail);
                new_data.release();
                break;
            }
            else {
                // try to help the other thread which successfully emplace the data
                std::shared_ptr<node> old_next;
                if (old_tail->next.compare_exchange_strong(old_next, new_tail)) {
                    // successfuly help set the next of current tail to the new tail
                    // If it fails, the old_next is set to the dummy node that will be
                    // set to new tail.
                    // If it succeeds (the control flow go in this if branch), then we
                    // give the dummy node to other thread
                    old_next = new_tail;
                    // need to make a new dummy node
                    new_tail = std::make_shared<node>();
                }
                tail.compare_exchange_strong(old_tail, old_next);
            }
        }
    }

    std::unique_ptr<T> pop() noexcept {
        std::unique_ptr<T> res;
        std::shared_ptr<node> old_head = head.load();
        while (old_head != tail.load()) {
            if (head.compare_exchange_strong(old_head, old_head->next.load())) {
                res.reset(old_head->data.load());
                break;
            }
            // original head is already popped by other thread, after reload
            // we need to compare to tail again to guarantee we won't pop
            // dummy node

            // note that with the following code, we cannot determine whether
            // the compare/exchange executes/succeeds when we get out of the loop: 
            /* 
            while (old_head != tail.load() &&
                !head.compare_exchange_strong(old_head, old_head->next));

            // some other thread push

            if (old_head != tail.load()) {
                // it is possible that we didn't pop old_head
            }
            */
        }
        return res;
    }
};
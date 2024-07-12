#include <atomic>
#include <optional>
#include <concepts>
#include <array>
#include <thread>
#include <algorithm>

std::atomic<void*>& get_hazard_pointer_for_current_thread();
bool outstanding_hazard_pointers_for(void* p);

// ****************************************************************************
// lockfree_stack using hazard pointers for memory reclamation
// ****************************************************************************

template <typename T>
  requires std::is_nothrow_move_constructible_v<T>
class lockfree_stack {
    struct node {
        T data;
        node* next;
    };
    std::atomic<node*> head;

    static std::atomic<node*> nodes_to_reclaim;
    static void add_to_reclaim_list(node* new_head) {
        new_head->next = nodes_to_reclaim.load();
        while (!nodes_to_reclaim.compare_exchange_weak(new_head->next, new_head));
    }

    static void delete_node_with_no_hazards() {
        node* curr = nodes_to_reclaim.exchange(nullptr);
        while (curr) {
            node* const next = curr->next;
            if (outstanding_hazard_pointers_for(curr)) {
                add_to_reclaim_list(curr);
            } else {
                delete curr;
            }
            curr = next;
        }
    }
public:
    void push(T const& data) {
        node* new_node = new node{data, head.load()};
        while (!head.compare_exchange_weak(new_node->next, new_node,
                std::memory_order_release, std::memory_order_relaxed));
    }

    std::optional<T> pop() {
        node* old_head = head.load();
        std::atomic<void*>& hp = get_hazard_pointer_for_current_thread();
        do {
            // set hazard pointer for old_head
            node* temp;
            do {
                temp = old_head;
                hp.store(old_head);
                old_head = head.load();
                // if old_head is already popped, it can be deleted before the hazard
                // pointer is set, we need to reload the head and reset hazard pointer
            } while (old_head != temp);

            // old_head can be popped by other thread after we set hazard pointer,
            // but we are safe to dereference it as other threads will not delete it
            // as we already set the hazard pointer

        } while (old_head && !head.compare_exchange_strong(old_head, old_head->next));
        // redo the body of the while loop can be expensive, so we use
        // compare_exchange_strong here

        // now we extract the node, other thread that loads the same head will need to
        // reload and won't even try to delete this node as the current thread is the
        // only thread that might put this node inside the nodes_to_reclaim list
        //
        // so it is safe to clear the hazard pointer
        hp.store(nullptr);

        std::optional<T> res;
        if (old_head) {
            res.emplace(std::move(old_head->data));
            if (outstanding_hazard_pointers_for(old_head)) {
                add_to_reclaim_list(old_head);
            } else {
                delete old_head;
            }
        }
        delete_nodes_with_no_hazards();
        return res;
    }
};

// ****************************************************************************
// Simple implementation of get_hazard_pointer_for_current_thread and
// outstanding_hazard_pointers_for
// ****************************************************************************

struct hazard_pointer {
    std::atomic<std::thread::id> id;
    std::atomic<void*> ptr;
};

constexpr std::size_t max_hazard_pointers = 100;
std::array<hazard_pointer, max_hazard_pointers> hazard_pointers{};

class hp_owner {
    hazard_pointer* hp;
public:
    hp_owner(hp_owner const&) = delete;
    hp_owner& operator=(hp_owner const&) = delete;
    hp_owner() : hp(nullptr) {
        for (auto& spot : hazard_pointers) {
            std::thread::id old_id;
            if (spot.id.compare_exchange_strong(old_id, std::this_thread::get_id())) {
                hp = &spot;
                break;
            }
        }

        if (!hp) {
            throw std::runtime_error("No spot available for hazard pointer!");
        }
    }
    ~hp_owner() {
        hp->ptr.store(nullptr);
        hp->id.store(std::thread::id());
    }
    std::atomic<void*>& get_pointer() { return hp->ptr; }
};

std::atomic<void*>& get_hazard_pointer_for_current_thread() {
    thread_local static hp_owner hazard;
    return hazard.get_pointer();
}

bool outstanding_hazard_pointers_for(void* p) {
    return std::ranges::any_of(
        hazard_pointers,
        [p](std::atomic<void*>& ptr) { return p == ptr.load(); },
        &hazard_pointer::ptr
    );
}

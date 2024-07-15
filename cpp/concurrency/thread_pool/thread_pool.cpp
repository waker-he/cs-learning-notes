#include <vector>
#include <thread>
#include <stop_token>
#include <future>
#include <optional>


class alignas(std::hardware_destructive_interference_size) work_stealing_queue {
    using TaskType = std::function<void()>;
    std::deque<TaskType> q;
    // use lock-based for simplification, lock-free might be better in real use case
    mutable std::mutex mut; 
public:
    work_stealing_queue() {}
    work_stealing_queue(work_stealing_queue const&) = delete;
    work_stealing_queue& operator=(work_stealing_queue const&) = delete;

    void push(TaskType&& task) {
        std::scoped_lock lock(mut);
        q.push_front(std::move(task));
    }

    // pop at front
    std::optional<TaskType> try_pop() {
        std::optional<TaskType> res;
        std::scoped_lock lock(mut);
        if (q.empty()) return res;
        res.emplace(std::move(q.front()));
        q.pop_front();
        return res;
    }

    // steal at back
    std::optional<TaskType> try_steal() {
        std::optional<TaskType> res;
        std::scoped_lock lock(mut);
        if (q.empty()) return res;
        res.emplace(std::move(q.back()));
        q.pop_back();
        return res;
    }
};

class ThreadPool {
    using TaskType = std::function<void()>;
    std::vector<std::jthread> threads;
    thread_safe_queue<TaskType> tasks;

    // to reduce contention on the global work queue
    std::vector<work_stealing_queue> localQueues;
    static thread_local unsigned int index;

    // use work stealing to address problem of uneven distribution
    std::optional<TaskType> tryStealTask() {
        std::optional<TaskType> res;
        for (unsigned i = 0, n = localQueues.size() - 1; i < n; ++i) {
            // each thread starts stealing from different thread to avoid contention
            if (res = localQueues[(index + i + 1) % n].try_steal()) break;
        }
        return res;
    }

    void worker(std::stop_token st, unsigned int myIndex) {
        index = myIndex;
        while (!st.stop_requested()) {
            runPendingTask();
        }
    }
public:
    ThreadPool(unsigned int numThreads = std::jthread::hardware_concurrency())
        : localQueues(numThreads)
    {
        for (unsgined int i = 0; i < numThreads; ++i) {
            threads.emplace_back([this, i](std::stop_token st) {
                this->worker(st, i);
            });
        }
        index = numThreads;
    }

    ThreadPool(ThreadPool const&) = delete;
    ThreadPool& operator=(ThreadPool const&) = delete;

    ~ThreadPool() {
        for (auto& thread : threads)
            thread.request_stop();
    }

    // return future and stop_source so that user can wait for the task to
    // complete or requesting stop for the task
    template <std::invocable<std::stop_token> F>
    auto submit(F&& f) -> std::pair<std::future<std::invoke_result_t<F, std::stop_token>>,
                                    std::stop_source> {
        using Res = std::invoke_result_t<F, std::stop_token>;
        std::stop_source ssrc;
        std::packaged_task<Res()> task([f = std::move(f), st = ssrc.get_token()] {
            return f(st);
        });
        auto futRes = task.get_future();

        // push task into work queue
        if (index == threads.size()) {
            // currently on master thread, push to global queue
            tasks.push(std::move(task));
        }
        else {
            // push to local queue
            localQueues[index].push(std::move(task));
        }
        return make_pair(futRes, ssrc);
    }

    // when tasks need to wait for other tasks, they can call this method
    void runPendingTask() {
        std::optional<TaskType> task;
        if (task = localQueues[index].try_pop() ||
            task = tasks.try_pop() ||
            task = tryStealTask())
        {
            task();
        }
        else {
            std::this_thread::yield();
        }
    }
};
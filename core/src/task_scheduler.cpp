#include "openperf/task_scheduler.hpp"

namespace openperf {

TaskScheduler::TaskScheduler(std::size_t workerCount) {
    workers_.reserve(std::max(std::size_t{1}, workerCount));
}

TaskScheduler::~TaskScheduler() {
    stop();
}

void TaskScheduler::start() {
    running_ = true;
    for (std::size_t i = 0; i < workers_.capacity(); ++i) {
        workers_.emplace_back(&TaskScheduler::workerLoop, this);
    }
}

void TaskScheduler::stop() {
    running_ = false;
    cv_.notify_all();
    for (auto& w : workers_)
        if (w.joinable())
            w.join();
}

void TaskScheduler::enqueue(Task task) {
    {
        std::lock_guard<std::mutex> lock{mutex_};
        queue_.emplace(std::move(task));
        queueDepth_.store(queue_.size(), std::memory_order_relaxed);
    }
    cv_.notify_one();
}

std::size_t TaskScheduler::getQueueDepth() const {
    return queueDepth_.load(std::memory_order_relaxed);
}

void TaskScheduler::workerLoop() {
    while (true) {
        Task task;

        {
            std::unique_lock<std::mutex> lock{mutex_};
            cv_.wait(lock, [&]{ return !running_ || !queue_.empty(); });

            if (!running_ && queue_.empty()) return;

            task = std::move(queue_.front());
            queue_.pop();
            queueDepth_.store(queue_.size(), std::memory_order_relaxed);
        }

        if (task) task();
    }
}

}
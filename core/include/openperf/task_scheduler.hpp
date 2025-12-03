#pragma once

#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace openperf {

using Task = std::function<void()>;

class TaskScheduler {
public:
    explicit TaskScheduler(std::size_t workerCount = std::thread::hardware_concurrency());
    ~TaskScheduler();

    void start();
    void stop();
    void enqueue(Task task);
    
    // Get current queue depth (thread-safe)
    std::size_t getQueueDepth() const;

private:
    void workerLoop();

    std::vector<std::thread> workers_;
    std::queue<Task> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_{false};
    std::atomic<std::size_t> queueDepth_{0};
};

}
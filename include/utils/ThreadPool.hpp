#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();

    // Submits a void function (like a lambda) to the queue
    void submit(std::function<void()> task);

private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_queue_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_stop;
};
#include "utils/ThreadPool.hpp"
#include <stdexcept>

ThreadPool::ThreadPool(size_t num_threads) : m_stop(false) {
    for (size_t i = 0; i < num_threads; ++i) {
        m_workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->m_queue_mutex);
                    // Wait until a task is available or we are stopping
                    this->m_cv.wait(lock, [this] {
                        return this->m_stop || !this->m_tasks.empty();
                    });
                    
                    if (this->m_stop && this->m_tasks.empty()) {
                        return; // Exit thread
                    }
                    
                    task = std::move(this->m_tasks.front());
                    this->m_tasks.pop();
                }
                task(); // Execute the task
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_stop = true;
    }
    m_cv.notify_all(); // Wake up all threads
    for (std::thread &worker : m_workers) {
        worker.join(); // Wait for them to finish
    }
}

void ThreadPool::submit(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        if (m_stop) {
            throw std::runtime_error("submit on stopped ThreadPool");
        }
        m_tasks.push(std::move(task));
    }
    m_cv.notify_one(); // Wake up one worker
}
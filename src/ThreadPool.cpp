#include "ThreadPool.h"

void ThreadPool::SignalError() {
    error.store(true);
}

ThreadPool::ThreadPool(size_t threads) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this]() {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(mutex);
                    cv.wait(lock, [&] {
                        return stop || !tasks.empty();
                    });

                    if (stop && tasks.empty())
                        return;

                    task = std::move(tasks.front());
                    tasks.pop();
                    activeTasks++;
                }

                task();

                {
                    std::lock_guard<std::mutex> lock(mutex);
                    activeTasks--;
                    if (tasks.empty() && activeTasks == 0) {
                        finishedCv.notify_all();
                    }
                }
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        stop = true;
    }
    cv.notify_all();
    for (auto& t : workers) {
        t.join();
    }
}

void ThreadPool::Enqueue(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        tasks.push(std::move(task));
    }
    cv.notify_one();
}

void ThreadPool::Wait() {
    std::unique_lock<std::mutex> lock(mutex);
    finishedCv.wait(lock, [&] {
        return tasks.empty() && activeTasks == 0;
    });
}

bool ThreadPool::HasError() const {
    return error.load();
}

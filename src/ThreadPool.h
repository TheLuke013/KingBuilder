#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t threads);
    ~ThreadPool();

    void Enqueue(std::function<void()> task);
    void Wait();
    bool HasError() const;
    void SignalError();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex mutex;
    std::condition_variable cv;
    std::condition_variable finishedCv;

    std::atomic<bool> stop{false};
    std::atomic<bool> error{false};
    std::atomic<size_t> activeTasks{0};
};

#endif
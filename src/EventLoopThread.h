#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <condition_variable>
#include <functional>
#include "Thread.h"
#include "noncopyable.h"

class EventLoop;
 
class EventLoopThread:noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    explicit EventLoopThread(const ThreadInitCallback&cb, const std::string& name);
    EventLoop* startLoop();
    ~EventLoopThread() noexcept;
private:
    EventLoop* loop_;
    std::atomic<bool> exciting_;
    Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    ThreadInitCallback callback_;
    void ThreadFunc();
};
 
#include "EventLoopThread.h"
#include "EventLoop.h"

EventLoopThread::EventLoopThread(const EventLoopThread::ThreadInitCallback &cb, const std::string &name) :loop_{nullptr},
exciting_(false), thread_(std::bind(&EventLoopThread::ThreadFunc, this), name), callback_(cb){

}

void EventLoopThread::ThreadFunc() {
    EventLoop event_loop;
    if(callback_)
        callback_(&event_loop);
    {
        std::scoped_lock lock(mutex_);
        loop_ = &event_loop;
        cond_.notify_one();
    }
    event_loop.loop();
    std::scoped_lock lock(mutex_);
    loop_ = nullptr;
}

EventLoop *EventLoopThread::startLoop() {
    thread_.start();
    EventLoop* event_loop = nullptr;
    std::unique_lock<std::mutex> lock(mutex_);
    while(!loop_)
        cond_.wait(lock);
    event_loop = loop_;
    return event_loop;
}

EventLoopThread::~EventLoopThread() noexcept {
    exciting_ = true;
    if(loop_){
        loop_ ->quit();
        thread_.join();
    }
}

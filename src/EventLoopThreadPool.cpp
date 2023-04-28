//
// Created by wuyifei0305 on 23-4-18.
//
#include <memory>
#include <fmt/core.h>
#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *base_loop, const std::string &name) :base_loop(base_loop), name_(name),
started_(false), num_threads(0), next_(0){

}

void EventLoopThreadPool::start(const EventLoopThreadPool::ThreadInitCallback &cb) {
    started_ = true;
    for(int i=0;i!=num_threads;i++){
        std::string this_thread_name = fmt::format("{}_{}", name_, i);
        std::unique_ptr<EventLoopThread> t = std::make_unique<EventLoopThread>(cb, this_thread_name);
        threads_.emplace_back(t);
        loops_.emplace_back(t->startLoop());
    }
    if(num_threads == 0 && cb)
        cb(base_loop);

}

EventLoop* EventLoopThreadPool::get_next_loop() {
    EventLoop* eventLoop = base_loop;
    if(!loops_.empty()){
        eventLoop = loops_[next_];
        next_++;
        if(next_>=loops_.size())
            next_ = 0;
    }
    return eventLoop;
}


std::vector<EventLoop*> EventLoopThreadPool::get_all_loops() const{
    if(loops_.empty())
        return std::vector<EventLoop*>(1, base_loop);
    else
        return loops_;
}
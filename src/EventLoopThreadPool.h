//
// Created by wuyifei0305 on 23-4-18.
//

#pragma once

#include <string>
#include <functional>

class EventLoop;

class EventLoopThreadPool {
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    explicit EventLoopThreadPool(EventLoop* base_loop, const std::string& name);
    ~EventLoopThreadPool();
};




//
// Created by wuyifei0305 on 23-4-18.
//

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "noncopyable.h"


class EventLoop;
class EventLoopThread;

class EventLoopThreadPool:noncopyable {
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    explicit EventLoopThreadPool(EventLoop* base_loop, const std::string& name);
    inline void set_num_threads(int num){
        num_threads = num;
    }
    void start(const ThreadInitCallback& cb);
    inline bool isStarted() const{
        return started_;
    }
    inline std::string get_name() const{
        return name_;
    }
    std::vector<EventLoop*> get_all_loops() const;
    EventLoop* get_next_loop();
    ~EventLoopThreadPool() {}
private:
    EventLoop* base_loop;
    std::string name_;
    bool started_;
    int num_threads;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;

};




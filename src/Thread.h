//
// Created by wuyifei0305 on 23-4-4.
//

#pragma once

#include "noncopyable.h"
#include <atomic>
#include <thread>
#include <memory>
#include <string>
#include <functional>
#include <unistd.h>
#include <spdlog/spdlog.h>

class Thread:noncopyable{
public:
    using ThreadFunc = std::function<void()>;
    explicit Thread(ThreadFunc threadFunc, const std::string& name): func(threadFunc), thread_name(name),
    started_(false), joined_(false), tid_(0){
        setDefaultName();
    }
    void start();
    void join();
    inline bool isStarted(){
        return started_;
    }
    inline pid_t tid(){
        return tid_;
    }
    inline std::string ThreadName(){
        return thread_name;
    }
    static int32_t NumCreated(){
        return num_created;
    }
    ~Thread() noexcept;
private:
    bool started_;
    bool joined_;
    std::shared_ptr<std::thread> thread_;
    pid_t tid_;
    ThreadFunc func;
    std::string thread_name;
    static std::atomic<int32_t> num_created;
    void setDefaultName();
};




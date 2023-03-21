//
// Created by wuyifei0305 on 23-3-12.
//

#pragma once

#include<thread>
#include<mutex>
#include<atomic>
#include<list>
#include<vector>
#include<memory>
#include<functional>
#include "Timestamp.h"
#include"CurrentThread.h"
#include "noncopyable.h"

class Channel;
class Poller;

class EventLoop:noncopyable {
public:
    using Functor = std::function<void()>;
    using ChannelList = std::list<Channel*>;
    [[maybe_unused]] [[nodiscard]] static int createEventfd();
    explicit EventLoop();
    void loop();
    void quit();
    [[nodicard]] inline Timestamp pollReturnTime() const {
        return poll_return_time;
    }
    void runInLoop(Functor cb); // 在当前loop中执行cb
    void queueInLoop(Functor cb); // 把cb放入队列中，唤醒loop所在的线程，执行cb
    void wakeup();
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);
    [[nodicard]] inline bool isInLoopThread() const{
       return thread_id == CurrentThread::get_tid();
    }
    ~EventLoop();
private:
    const int loop_id;
    std::atomic<bool> looping_;
    std::atomic<bool> quit_;
    std::atomic<bool> callingPendingFunctors_;
    std::vector<Functor> pendingFunctors_; // 存储loop需要执行的所有的回调操作
    const pid_t thread_id;
    Timestamp poll_return_time; // poller返回发生事件的channels的时间点
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<Channel> wake_up_channel;
    ChannelList activeChannels_;
    Channel* current_active_channel;
    std::mutex mutex_;
    int wake_up_fd; // 主要作用，当mainLoop获取一个新用户的channel，通过轮询算法选择一个subloop，通过该成员唤醒subloop处理channel
    void handle_read();
    void do_pending_functors();
};


//




//
// Created by wuyifei0305 on 23-3-12.
//
#include<functional>
#include<mutex>
#include<cstring>
#include<cerrno>
#include<cstdlib>
#include<unistd.h>
#include<sys/eventfd.h>
#include<spdlog/spdlog.h>
#include "Poller.h"
#include "Channel.h"
#include "Timestamp.h"
#include "EventLoop.h"

__thread EventLoop *t_loopInThisThread{nullptr}; // 防止一个线程创建多个EventLoop   thread_local
constexpr int kPollTimeMs{10000};

int EventLoop::createEventfd() {
    int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(fd<0){
        spdlog::critical("event fd error: {}", ::strerror(errno));
        exit(1);
    }
    return fd;
}

EventLoop::EventLoop() : thread_id(CurrentThread::get_tid()),looping_(false), quit_(false), callingPendingFunctors_(false), poller_(Poller::getDefaultPoller(this)),
                         wake_up_fd(EventLoop::createEventfd()), loop_id(random_engine(eng)),
current_active_channel(nullptr){
    
    wake_up_channel = std::make_unique<Channel>(this, wake_up_fd);
    spdlog::debug("Event Loop created {0} in thead {1}.", loop_id, thread_id);
    if(t_loopInThisThread){
        spdlog::critical("Another Evevntloop {0} in this thread {1}.", t_loopInThisThread->loop_id, thread_id);
        exit(1);
    }else{
        t_loopInThisThread = this;
    }
    wake_up_channel->setReadCallback(std::bind(&EventLoop::handle_read, this));
    wake_up_channel->enableReading();
}

void EventLoop::handle_read() const {
    uint64_t one{1};
    ssize_t n = ::read(wake_up_fd, &one, sizeof(one));
    if(n!=sizeof(one)){
        spdlog::error("Eventloop read {0} bytes instead of {1}.", n, sizeof(one));
    }
}

void EventLoop::wakeup() {
    uint64_t one{1};
    ssize_t n = ::write(wake_up_fd, &one, sizeof(one));
    if(n!=sizeof(one)){
        spdlog::error("Eventloop write {0} bytes instead of {1}.", n, sizeof(one));
    }
}

void EventLoop::do_pending_functors() // 执行回调
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        std::scoped_lock lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (const Functor &functor : functors)
        functor(); // 执行当前loop需要执行的回调操作


    callingPendingFunctors_ = false;
}


void EventLoop::loop() {
    looping_ = true;
    quit_ = false;
    spdlog::info("Eventloop {} start looping.", loop_id);
    while(!quit_){
        activeChannels_.clear();
        poll_return_time = poller_->poll(kPollTimeMs, &activeChannels_);
    }
    for(auto channel:activeChannels_)
        channel->handleEvent(poll_return_time);
    do_pending_functors();
    spdlog::info("Eventloop {} finish.", loop_id);
    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;
    if(!isInLoopThread()){
        wakeup();
    }
}

void EventLoop::runInLoop(EventLoop::Functor cb) {
    if(isInLoopThread())
        cb();
    else
        queueInLoop(cb);
}

void EventLoop::queueInLoop(EventLoop::Functor cb) {
    std::scoped_lock lock(mutex_);
    pendingFunctors_.emplace_back(cb);
    if(!isInLoopThread() || callingPendingFunctors_)
        wakeup();
}

EventLoop::~EventLoop() noexcept {
    wake_up_channel->disableAll();
    wake_up_channel->remove();
    ::close(wake_up_fd);
    t_loopInThisThread = nullptr;
}
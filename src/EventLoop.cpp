//
// Created by wuyifei0305 on 23-3-12.
//

#include<cstring>
#include<cerrno>
#include<cstdlib>
#include<unistd.h>
#include<sys/eventfd.h>
#include<spdlog/spdlog.h>
#include "Poller.h"
#include "Channel.h"
#include "EventLoop.h"

__thread EventLoop *t_loopInThisThread{nullptr}; // 防止一个线程创建多个EventLoop   thread_local
const int kPollTimeMs{10000};

int EventLoop::createEventfd() {
    int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(fd<0){
        spdlog::critical("event fd error: {}", ::strerror(errno));
        exit(1);
    }
    return fd;
}

EventLoop::EventLoop() : thread_id(CurrentThread::get_tid()),looping_(false), quit_(false), callingPendingFunctors_(false),
wake_up_fd(EventLoop::createEventfd()), wake_up_channel(new Channel(this, wake_up_fd)),
poller_(Poller::getDefaultPoller(this)), current_active_channel(nullptr), loop_id{rand()}{
    spdlog::debug("Event Loop created {0} in thead {1}.", loop_id, thread_id);
}
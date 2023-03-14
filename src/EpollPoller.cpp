//
// Created by wuyifei0305 on 23-3-14.
//
#include "Poller.h"
#include "EpollPoller.h"
#include<errno.h>
#include<unistd.h>
#include<cstring>
#include<spdlog/spdlog.h>

constexpr int kNew{-1};
constexpr int kAdded{1};
constexpr int kDeleted{2};

EpollPoller::EpollPoller(EventLoop *loop) : Poller(loop), epoll_fd(::epoll_create(EPOLL_CLOEXEC)), events_(kInitEventListSize){
    if(epoll_fd<0){
        spdlog::critical("epoll_create error: {} ", ::strerror(errno));
        exit(1);
    }
}

EpollPoller::~EpollPoller() noexcept {
    ::close(epoll_fd);
}
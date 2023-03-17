//
// Created by wuyifei0305 on 23-3-14.
//
#include "Poller.h"
#include "Channel.h"
#include "EpollPoller.h"
#include "Timestamp.h"
#include<errno.h>
#include<unistd.h>
#include<cstring>
#include<sys/epoll.h>
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

Timestamp EpollPoller::poll(int TimeoutMs, Poller::ChannelList *activeChannels) {
    spdlog::info("fd total count: {}", activeChannels->size());
    int numEvents = ::epoll_wait(epoll_fd, &*events_.begin(), static_cast<int>(events_.size()), TimeoutMs);
    int saveErrno = errno;
    Timestamp Now{Timestamp::nowa()};
    if(numEvents>0){
        spdlog::info("{} events happen.", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if(numEvents >= events_.size())
            events_.resize(events_.size()*2);
    }
    else if(numEvents==0)
        spdlog::warn("Timeout!");
    else
        spdlog::error(::strerror(saveErrno));
    return Now;
}

void EpollPoller::updateChannel(Channel *channel) {
    const int index{channel->get_index()};
    spdlog::info("New fd: {0}, events: {1}, index: {2}", channel->get_fd(), channel->get_events(), channel->get_index());
    if(index == kNew || index==kDeleted){
        if(index == kNew){
            int fd = channel->get_fd();
            channelMap_.insert({fd, channel});
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else{
        int fd = channel->get_fd();
        if(channel->isNoneEvents()){
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else
            update(EPOLL_CTL_MOD, channel);

    } // channel已经被注册过
}

void EpollPoller::removeChannel(Channel *channel) {
    int fd = channel->get_fd();
    int index = channel->get_index();
    spdlog::info("Remove fd: {0}, events: {1}, index: {2}", channel->get_fd(), channel->get_events(), channel->get_index());
    channelMap_.erase(fd);
    if(index == kAdded)
        update(EPOLL_CTL_DEL, channel);
    channel->set_index(kNew);
}

void EpollPoller::update(int operation, Channel *channel) {
    epoll_event epollEvent;
    memset(&epollEvent, 0, sizeof(epollEvent));
    int fd = channel->get_fd();
    epollEvent.events = channel->get_events();
    epollEvent.data.fd = fd;
    epollEvent.data.ptr = channel;

    int epoll_res = epoll_ctl(epoll_fd, operation, fd, &epollEvent);
    if(epoll_res<0){
        if(operation==EPOLL_CTL_DEL)
            spdlog::error("epoll_ctl delete error: {}", ::strerror(errno));
        else
            spdlog::critical("epoll_ctl add/modify error: {}", ::strerror(errno));
    }

}

void EpollPoller::fillActiveChannels(int numEvents, Poller::ChannelList *activeChannels) const {
    for(int i=0;i!=numEvents;i++){
        Channel* channel = static_cast<Channel*>(events_.at(i).data.ptr);
        channel->setREvents(events_.at(i).events);
        activeChannels->emplace_back(channel);
    }
}
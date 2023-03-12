//
// Created by wuyifei0305 on 23-3-12.
//
#include "EventLoop.h"
#include "Timestamp.h"
#include "Channel.h"
#include<fmt/core.h>
#include<spdlog/spdlog.h>
#include<string>
 
Channel::Channel(EventLoop *loop, int fd): loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1), tied_(false){

}

Channel::~Channel() noexcept {

}

void Channel::handleEvent(const Timestamp& receiveTime) {
    if(tied_){
        std::shared_ptr<void> guard = tie_.lock();
        if(guard) {
            handleEventWithGuard(receiveTime);
        }
    } else
        handleEventWithGuard(receiveTime);
}

void Channel::handleEventWithGuard(const Timestamp &receiveTime) {
    // std::string info_re = fmt::format("Channel handle revents: {}", revents_);
    spdlog::info("Channel handle revents: {}", revents_);
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)){
        if(closeEventCallback_)
            closeEventCallback_();
    }
    if (revents_ & EPOLLERR){
        if(errorEventCallback_)
            errorEventCallback_();
    }
    if (revents_ & (EPOLLIN | EPOLLPRI)){
        if(readEventCallback_)
            readEventCallback_(receiveTime);
    }
    if (revents_ & EPOLLOUT){
        if(writeEventCallback_)
            writeEventCallback_();
    }

}

void Channel::tie(const std::shared_ptr<void> & obj) {
    tie_ = obj;
    tied_ = true;
}

// 当改变channel所表示的文件描述符fd的event事件改变后， 该函数负责更改poller里对应的事件epoll_ctl
// Eventloop => ChannelList, Poller
void Channel::update() {

}

// 通知Eventloop删除当前Channel
void Channel::remove() {

}


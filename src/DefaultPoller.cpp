//
// Created by wuyifei0305 on 23-3-14.
//

#include "Poller.h"
#include "EpollPoller.h"

Poller* Poller::getDefaultPoller(EventLoop *loop) {
    if(::getenv("MUDUO_USE_POLL"))
        return nullptr;
    else
        return new EpollPoller(loop);
}
//
// Created by wuyifei0305 on 23-3-14.
//

#pragma once

#include<sys/epoll.h>
#include<vector>
#include "EventLoop.h"
#include "Timestamp.h"
#include "Poller.h"

class Channel;


class EpollPoller:public Poller {
public:
    using EventList = std::vector<epoll_event>;
    explicit EpollPoller(EventLoop* loop);
    [[nodiscard]] Timestamp poll(int TimeoutMs, ChannelList *activeChannels) override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;
    ~EpollPoller() noexcept override;
private:
    static const int kInitEventListSize{16};
    int epoll_fd;
    EventList events_;
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    void update(int operation, Channel* channel);
};


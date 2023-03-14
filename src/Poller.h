//
// Created by wuyifei0305 on 23-3-14.
//

#pragma once

#include "Timestamp.h"
#include "noncopyable.h"
#include<list>
#include<unordered_map>

class Channel;
class EventLoop;

class Poller:noncopyable {
public:
    using ChannelList = std::list<Channel*>;
    using ChannelMap = std::unordered_map<int, Channel*>; // key -> socket_fd; value -> Channel
    explicit Poller(EventLoop* loop): ownerLoop_(loop){}
    virtual Timestamp poll(int timeoutMs, ChannelList *activeChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;
    bool hasChannel(Channel* channel) const;
    [[nodiscard]] static Poller* getDefaultPoller(EventLoop* loop);
    virtual ~Poller();
protected:
    ChannelMap channelMap_;
private:
    EventLoop* ownerLoop_; //　定义Ｐoller所属于的事件循环
};



// 多路事件分发器的核心IO复用模块
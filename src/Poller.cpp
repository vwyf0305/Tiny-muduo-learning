//
// Created by wuyifei0305 on 23-3-14.
//

#include "Poller.h"
#include "Channel.h"

bool Poller::hasChannel(Channel *channel) const {
    auto iter = channelMap_.find(channel->get_fd());
    return (iter!=channelMap_.end() && iter->second==channel);
}
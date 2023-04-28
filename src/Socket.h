//
// Created by wuyifei0305 on 23-4-28.
//

#pragma once

#include "InetAddress.h"
#include "noncopyable.h"

class Socket :noncopyable{
public:
    explicit Socket(int fd): socket_fd(fd){}
    inline int get_fd() const{
        return socket_fd;
    }
    ~Socket();

private:
    const int socket_fd;
};




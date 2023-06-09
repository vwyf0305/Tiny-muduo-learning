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
    void bind_address(const InetAddress& address);
    void socket_listen();
    int socket_accept(InetAddress* peeraddr);
    void shutdown_write();
    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);
    ~Socket();

private:
    const int socket_fd;
};




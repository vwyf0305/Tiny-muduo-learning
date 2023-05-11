//
// Created by wuyifei0305 on 23-4-28.
//

#include "Socket.h"
#include <unistd.h>
#include <cerrno>
#include <spdlog/spdlog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <netinet/tcp.h>
#include <sys/socket.h>

Socket::~Socket(){
    ::close(socket_fd);
}

void Socket::bind_address(const InetAddress &address) {
    int bind_result = ::bind(socket_fd, (sockaddr*)(address.getAddr()), sizeof(sockaddr_in));
    if(bind_result==-1)
    {
        spdlog::critical("{} .bind() socket fd {} failed.", strerror(errno),socket_fd);
        exit(1);
    }
    else
        spdlog::info("{} binds {} success!", address.toIpPort(), socket_fd);
}

void Socket::socket_listen() {
    int listen_result = ::listen(socket_fd, 1024);
    if(listen_result!=0) {
        spdlog::critical("{} .{} listen failed. ",  strerror(errno), socket_fd);
        exit(1);
    }
}

int Socket::socket_accept(InetAddress *peeraddr) {
    sockaddr_in accept_adr;
    socklen_t len = sizeof(accept_adr);
    bzero(&accept_adr, sizeof(accept_adr));
    int connect_fd = ::accept4(socket_fd, (sockaddr*)(&accept_adr), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(connect_fd>=0)
        peeraddr->set_sock_addr(accept_adr);
    return connect_fd;
}

void Socket::shutdown_write() {
    int shutdown_result = ::shutdown(socket_fd, SHUT_WR);
    if(shutdown_result<0)
        spdlog::error("Shutdown error!");
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof optval);
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
}

void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof optval);
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof optval);
}




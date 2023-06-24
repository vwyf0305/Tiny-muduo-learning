//
// Created by wuyifei0305 on 23-4-28.
//

#include "Acceptor.h"
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstring>
#include <unistd.h>
#include <cerrno>

static int create_nonblocking()
{
    int socket_fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (socket_fd < 0)
    {
        spdlog::critical("%s:%s:%d listen socket create err:%s \n", __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        exit(1);
    }
    else
        spdlog::info("Socket success.");
    return socket_fd;
}

void Acceptor::handle_read(Timestamp timestamp){
    fmt::print("Time: {}\n", timestamp.toString());
    InetAddress peer_addr;
    int connect_fd = accept_socket.socket_accept(&peer_addr);
    if(connect_fd>=0){
        if(new_connection_callbcak)
            new_connection_callbcak(connect_fd, peer_addr);
        else
            ::close(connect_fd);
    }
    else{
        spdlog::error("%s:%s:%d accept err:%s \n", __FILE__, __FUNCTION__, __LINE__, strerror(errno));
        if (errno == EMFILE)
            spdlog::error("%s:%s:%d socket_fd reached limit! \n", __FILE__, __FUNCTION__, __LINE__);

    }
}

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listen_addr, bool reuseport) :base_loop(loop), accept_socket(create_nonblocking()),
                                                                                     accept_channel(loop, accept_socket.get_fd()), is_listenling(false){
    auto time = Timestamp::nowa();
    if(reuseport){
        accept_socket.setReuseAddr(true);
        accept_socket.setReusePort(true);
    }
    accept_socket.bind_address(listen_addr);
    accept_channel.setReadCallback(std::bind(&Acceptor::handle_read, this, time));
}

void Acceptor::listen() {
    is_listenling = true;
    accept_socket.socket_listen();
    accept_channel.enableReading();
}

Acceptor::~Acceptor() noexcept {
    accept_channel.disableAll();
    accept_channel.remove();
}

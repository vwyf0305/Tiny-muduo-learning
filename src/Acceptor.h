//
// Created by wuyifei0305 on 23-4-28.
//

#pragma once


#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "noncopyable.h"
#include "InetAddress.h"
#include <functional>


class Acceptor:noncopyable {
public:
    using NewConnectionCallback = std::function<void(int socket_fd, const InetAddress&)>;
    explicit Acceptor(EventLoop* loop, const InetAddress& listen_addr, bool reuseport);
    inline void set_newconnection_callback(const NewConnectionCallback& cb){
        new_connection_callbcak = cb;
    }
    inline bool IsListenning() const{
        return is_listenling;
    }
    void listen();
    ~Acceptor() noexcept;
private:
    NewConnectionCallback new_connection_callbcak;
    EventLoop* base_loop;
    Socket accept_socket;
    Channel accept_channel;
    bool is_listenling;
    void handle_read();
};




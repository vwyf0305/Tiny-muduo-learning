//
// Created by wuyifei0305 on 23-6-17.
//
#include "Timestamp.h"
#include "TcpServer.h"
#include <spdlog/spdlog.h>
#include <string>
#include <functional>

#pragma once

class EchoServer {
public:
    explicit EchoServer(EventLoop* loop, const InetAddress& address, const std::string name);
    void start();
private:
    EventLoop* loop_;
    TcpServer tcp_server;
    void onConnection(const TcpConnectionPtr &conn);
    void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time);
};




//
// Created by wuyifei0305 on 23-6-17.
//
#include "Callbacks.h"
#include "TcpConnection.h"
#include "EchoServer.h"

EchoServer::EchoServer(EventLoop *loop, const InetAddress &address, const std::string name) : loop_(loop), tcp_server(loop, address, name){
    tcp_server.setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
    tcp_server.setMessageCallback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    tcp_server.set_threadnum(3);
}

void EchoServer::start() {
    tcp_server.start();
}

void EchoServer::onConnection(const TcpConnectionPtr &conn) {
    if(conn->is_connectd())
        spdlog::info("Connection UP : {}", conn->peer_address().toIpPort());
    else
        spdlog::info("Connection DOWN : {}", conn->peer_address().toIpPort());


}

void EchoServer::onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time) {
    std::string msg = buf->retrieve_all_as_string();
    conn->send(msg);
    conn->shutdown(); // 写端   EPOLLHUP =》 closeCallback_
}
//
// Created by wuyifei0305 on 23-3-12.
//
#include "TcpConnection.h"
#include "TcpServer.h"
#include <fmt/core.h>
#include <sys/socket.h>
#include <spdlog/spdlog.h>

static EventLoop* CheckLoopNotNull(EventLoop* loop){
    if(!loop){
        spdlog::critical("Main loop is NULL!\n");
        exit(1);
    }
    return loop;
}

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listen_addr, const std::string& name_args,
                     TcpServer::Option option): loop_(CheckLoopNotNull(loop)), name_(name_args),  ip_port(listen_addr.toIpPort()), acceptor_(new Acceptor(loop, listen_addr, option == Option::kReusePort)),
                     thread_pool(new EventLoopThreadPool(loop, name_args)), next_connection_id(1), started_(0), connectionCallback_(), messageCallback_(){
    acceptor_->set_newconnection_callback(std::bind(&TcpServer::new_connection, this, std::placeholders::_1, std::placeholders::_2));

}

void TcpServer::set_threadnum(int thread_num) {
    thread_pool->set_num_threads(thread_num);
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {
    spdlog::info("TcpServer::removeConnectionInLoop [{}] - connection {}\n",name_, conn->get_name());
    connections_.erase(conn->get_name());
    EventLoop *io_loop = conn->get_loop();
    io_loop->queueInLoop(std::bind(&TcpConnection::connect_destroyed, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::new_connection(int socket_fd, const InetAddress &peer_addr) {
    EventLoop* io_loop = thread_pool->get_next_loop(); // 轮询算法，选择一个subLoop，来管理channel
    std::string conn_name = fmt::format("{}-{}", ip_port, next_connection_id);
    conn_name = name_+conn_name;
    next_connection_id++;
    spdlog::info("TcpServer::newConnection [{}] - new connection [{}] from {} \n", name_, conn_name, peer_addr.toIpPort());
    sockaddr_in local_socket;
    ::bzero(&local_socket, sizeof(local_socket));
    socklen_t addrlen = sizeof(local_socket);
    if (::getsockname(socket_fd, (sockaddr*)(&local_socket), &addrlen) < 0)
        spdlog::error("sockets::getLocalAddr");
    InetAddress local_addr{local_socket};
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(io_loop, conn_name, socket_fd, local_addr, peer_addr);
    connections_.insert({conn_name, conn});
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    io_loop->runInLoop(std::bind(&TcpConnection::connect_established, conn));
}

void TcpServer::start() {
    if(started_++ == 0){
        thread_pool->start(threadInitCallback_);
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
    }

}

TcpServer::~TcpServer() noexcept {
    for(auto& item:connections_){
        TcpConnectionPtr  conn{item.second};
        item.second.reset();
        conn->get_loop()->runInLoop(std::bind(&TcpConnection::connect_destroyed, conn));
    }
}
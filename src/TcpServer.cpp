//
// Created by wuyifei0305 on 23-3-12.
//

#include "TcpServer.h"
#include <spdlog/spdlog.h>

static EventLoop* CheckLoopNotNull(EventLoop* loop){
    if(!loop){
        spdlog::critical("Main loop is NULL!\n");
        exit(1);
    }
    return loop;
}

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listen_addr, const std::string name_args,
                     TcpServer::Option option): loop_(CheckLoopNotNull(loop)), name_(name_args),  ip_port(listen_addr.toIpPort()), acceptor_(new Acceptor(loop, listen_addr, option == Option::kReusePort)),
                     thread_pool(new EventLoopThreadPool(loop, name_args)), next_connection_id(1), started_(0), connectionCallback_(), messageCallback_(){
    acceptor_->set_newconnection_callback(std::bind(&TcpServer::new_connection, this, std::placeholders::_1, std::placeholders::_2));

}

void TcpServer::set_threadnum(int thread_num) {
    thread_pool->set_num_threads(thread_num);
}

void TcpServer::new_connection(int socket_fd, const InetAddress &peer_addr) {

}

void TcpServer::start() {
    if(started_++ == 0){
        thread_pool->start(threadInitCallback_);
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
    }

}

TcpServer::~TcpServer() noexcept {

}
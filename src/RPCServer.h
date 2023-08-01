//
// Created by wuyifei0305 on 23-8-1.
//

#pragma once

#include <string>
#include <concepts>
#include <unordered_set>
#include <type_traits>
#include <pystring/pystring.h>
#include "Timestamp.h"
#include "TcpServer.h"
#include "RPC/ParseMsg.h"



class RPCServer {
public:
    explicit RPCServer(EventLoop* loop, const InetAddress& listen_addr);
    void start(int thread_count=0);
    ~RPCServer() noexcept;
private:
    void onConnectionCallback(const TcpConnectionPtr& conn);
    void onMessageCallback(const TcpConnectionPtr& conn,Buffer* buf,Timestamp timestamp);
    TcpServer tcp_server;
    void* rpc_context;
    std::unordered_set<std::string> id_set;
    [[nodicard]] int sum(int a, int b){
        return a+b;
    }
    [[nodicard]] std::string uppercase(const std::string& msg){
        return pystring::upper(msg);
    }
};



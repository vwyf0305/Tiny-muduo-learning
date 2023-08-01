//S
// Created by wuyifei0305 on 23-8-1.
//

#include "RPCServer.h"
#include "TcpConnection.h"
#include "RPC/ParseMsg.h"
#include <functional>
#include <spdlog/spdlog.h>


RPCServer::RPCServer(EventLoop *loop, const InetAddress &listen_addr) : tcp_server(loop, listen_addr, "JSON RPC starts"), rpc_context(nullptr){
    tcp_server.setConnectionCallback(std::bind(&RPCServer::onConnectionCallback, this, std::placeholders::_1));
    tcp_server.setMessageCallback(std::bind(&RPCServer::onMessageCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}


void RPCServer::start(int thread_count) {
    tcp_server.set_threadnum(thread_count);
    tcp_server.start();
}



void RPCServer::onConnectionCallback(const TcpConnectionPtr &conn) {
    if(conn->is_connectd()) {
        spdlog::info("Connection UP : {}", conn->peer_address().toIpPort());
        ParseMsg<int>* parse_msg = new ParseMsg<int>(std::bind(&RPCServer::sum, this, std::placeholders::_1, std::placeholders::_2), std::bind(&RPCServer::uppercase, this, std::placeholders::_1));
        rpc_context = static_cast<void*>(parse_msg);
    }
    else
        spdlog::error("Connection DOWN : {}", conn->peer_address().toIpPort());
}

void RPCServer::onMessageCallback(const TcpConnectionPtr &conn, Buffer *buf, Timestamp timestamp) {
    std::string msg{buf->retrieve_all_as_string()};
    spdlog::info("Receive {} from {}", msg, conn->peer_address().toIpPort());
    ParseMsg<int>* parse_msg = static_cast<ParseMsg<int>*>(rpc_context);
    if(parse_msg->get_request_msg(msg)){
        std::string reply_msg{parse_msg->get_reply_msg()};
        std::string msg_id{parse_msg->get_id()};
        if(id_set.find(msg_id)==id_set.end()){
            conn->send(reply_msg);
            conn->shutdown();
            id_set.insert(msg);
        }

    }
}


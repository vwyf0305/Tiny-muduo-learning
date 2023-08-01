//
// Created by wuyifei0305 on 23-3-12.
//

#pragma once

#include <atomic>
#include <string>
#include <memory>
#include <string_view>
#include <functional>
#include <unordered_map>
#include "Callbacks.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"

class TcpServer {
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;
    enum class Option
    {
        kNoReusePort,
        kReusePort,
    };
    explicit TcpServer(EventLoop *loop, const InetAddress& listen_addr, const std::string& name_args, Option option=Option::kNoReusePort);
    void start();
    inline void setThreadInitcallback(const ThreadInitCallback &cb){
        threadInitCallback_ = cb;
    }
    inline void setConnectionCallback(const ConnectionCallback &cb){
        connectionCallback_ = cb;
    }
    inline void setMessageCallback(const MessageCallback &cb){
        messageCallback_ = cb;
    }
    inline void setWriteCompleteCallback(const WriteCompleteCallback &cb){
        writeCompleteCallback_ = cb;
    }
    void set_threadnum(int thread_num);
    ~TcpServer() noexcept;
private:
    int next_connection_id;
    EventLoop *loop_;
    ConnectionMap connections_;
    ConnectionCallback connectionCallback_; // callback when new connection comes
    MessageCallback messageCallback_; // callback when new message comes
    WriteCompleteCallback writeCompleteCallback_; // callback when message sends completely
    ThreadInitCallback threadInitCallback_; // thead init callback
    std::atomic<int> started_;
    std::string_view ip_port;
    std::string  name_;
    std::unique_ptr<Acceptor> acceptor_; // runs in mainloop and monitors new connections
    std::shared_ptr<EventLoopThreadPool> thread_pool;
    void new_connection(int socket_fd, const InetAddress& peer_addr);
    void removeConnection(const TcpConnectionPtr &conn);
    void removeConnectionInLoop(const TcpConnectionPtr &conn);
};




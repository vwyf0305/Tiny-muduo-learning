//
// Created by wuyifei0305 on 23-5-20.
//

#pragma once

#include <atomic>
#include <string>
#include <memory>
#include <string_view>
#include <unordered_map>
#include "Buffer.h"
#include "Socket.h"
#include "Channel.h"
#include "Timestamp.h"
#include "Callbacks.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "noncopyable.h"


class TcpConnection :noncopyable, public std::enable_shared_from_this<TcpConnection>{
public:
    enum class StateE{
        kDisconnected,
        kConnecting,
        kConnected,
        kDisconnecting,
    };
    TcpConnection(EventLoop *loop,
                  const std::string &name,
                  int socket_fd,
                  const InetAddress& local_addr,
                  const InetAddress& peer_addr);
    ~TcpConnection();

    EventLoop* get_loop() const { return loop_; }
    const std::string get_name() const { return name_; }
    const InetAddress& local_address() const { return local_addr; }
    const InetAddress& peer_address() const { return peer_addr; }
    bool is_connectd() { return state_ == StateE::kConnected;}
    // 发送数据
    void send(const std::string &buf);
    // 关闭连接
    void shutdown();
    inline void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }

    inline void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }

    inline void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }

    inline void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, std::size_t highWaterMark)
    { highWaterMarkCallback_ = cb; high_water_mark = highWaterMark; }

    inline void setCloseCallback(const CloseCallback& cb)
    { closeCallback_ = cb; }

    // 连接建立
    void connect_established();
    // 连接销毁
    void connect_destroyed();
private:
    bool reading_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    EventLoop* loop_; // monitor subloop
    Buffer input_buffer;  // 接收数据的缓冲区
    Buffer output_buffer; // 发送数据的缓冲区
    std::string name_;
    std::atomic<StateE> state_;
    const std::unordered_map<StateE, std::string> state_map{{StateE::kConnected, "Connected"}, {StateE::kDisconnecting, "Disconnecting"}, {StateE::kConnecting, "Connecting"}, {StateE::kDisconnected, "Disconnected"}};
    ConnectionCallback connectionCallback_; // 有新连接时的回调
    MessageCallback messageCallback_; // 有读写消息时的回调
    WriteCompleteCallback writeCompleteCallback_; // 消息发送完成以后的回调
    HighWaterMarkCallback highWaterMarkCallback_;
    CloseCallback closeCallback_;
    std::size_t high_water_mark;
    const InetAddress local_addr;
    const InetAddress peer_addr;
    void handle_read(Timestamp receive_time);
    void handle_write();
    void handle_close();
    void handle_error();
    void send_in_loop(const void* message, std::size_t len);
    void shutdown_in_loop();
    inline void set_state(StateE stateE){
        state_ = stateE;
    };
};



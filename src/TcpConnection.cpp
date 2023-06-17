//
// Created by wuyifei0305 on 23-5-20.
//

#include "TcpConnection.h"
#include <cerrno>
#include <cstring>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <spdlog/spdlog.h>

static EventLoop* CheckLoopNotNull(EventLoop* loop){
    if(!loop){
        spdlog::critical("TcpConnection loop is NULL!\n");
        exit(1);
    }
    return loop;
}




TcpConnection::TcpConnection(EventLoop *loop, const std::string &name, int socket_fd, const InetAddress &local_addr,
                             const InetAddress &peer_addr) :loop_(CheckLoopNotNull(loop)), name_(name), state_(StateE::kConnecting), reading_(true),
                             local_addr(local_addr), peer_addr(peer_addr), highWaterMark_(64*1024*1024),
                             socket_(new Socket(socket_fd)), channel_(new Channel(loop, socket_fd)){
    channel_->setReadCallback(std::bind(&TcpConnection::handle_read, this, std::placeholders::_1));
    channel_->setWriteCallback(std::bind(&TcpConnection::handle_write, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handle_close, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handle_error, this));
    spdlog::info("TcpConnection::ctor[{}] at fd={}\n", name_, socket_fd);
    socket_->setKeepAlive(true);
}

void TcpConnection::handle_read(Timestamp receive_time) {
    int save_errno{0};
    ssize_t n = input_buffer.read_fd(channel_->get_fd(), &save_errno);
    if(n>0)
        messageCallback_(shared_from_this(), &input_buffer, receive_time);
    else if(n==0)
        handle_close();
    else{
        errno = save_errno;
        spdlog::error("TcpConnection Error!!\n");
        handle_error();
    }

}

void TcpConnection::handle_write() {
    if(channel_->isWriting()) {
        int save_errno{0};
        ssize_t n = output_buffer.write_fd(channel_->get_fd(), &save_errno);
        if (n > 0) {
            output_buffer.retrieve(static_cast<std::size_t>(n));
            if (output_buffer.readable_bytes() == 0) {
                channel_->disableWriting();
                if (writeCompleteCallback_)
                    loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
                if (state_ == StateE::kDisconnecting)
                    shutdown_in_loop();
            }
        } else
            spdlog::error("TcpConnection::handle_write() error\n");
    }
    else
        spdlog::error("TcpConnection fd={} is down, no more writing \n", channel_->get_fd());
}


void TcpConnection::handle_close() {
    spdlog::info("TcpConnection::handleClose fd={}, state={}\n", channel_->get_fd(), state_map[state_]);
    set_state(StateE::kDisconnectd);
    channel_->disableAll();
    TcpConnectionPtr connection_ptr{shared_from_this()};
    if(connectionCallback_)
        connectionCallback_(connection_ptr);
    if(closeCallback_)
        closeCallback_(connection_ptr);
}

void TcpConnection::handle_error() {
    int optval;
    socklen_t optlen = sizeof(optval);
    int err = 0;
    if (::getsockopt(channel_->fd(), SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
        err = errno;
    else
        err = optval;
    spdlog::error("TcpConnection::handleError name:{} - SO_ERROR:{} \n", name_, err);
}

void TcpConnection::send_in_loop(const void *message, std::size_t len) {
    ssize_t nwrote = 0;
    std::size_t remaining = len;
    bool fault_error = false;
    if(state_ == StateE::kDisconnected || state_ == StateE::kDisconnecting){
        spdlog::error("Disconnected, give up writing!\n");
        return;
    }
    // 表示channel_第一次开始写数据，而且缓冲区没有待发送数据
    if(!channel_->isWriting() && output_buffer.readable_bytes()==0){
        nwrote = ::write(channel_->get_fd(), message, len);
        if(nwrote>=0){
            remaining = len-nwrote;
            if(remaining==0 && writeCompleteCallback_)
                loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
        }
        else{
            nwrote = 0;
            if (errno != EWOULDBLOCK){
                spdlog::error("TcpConnection::sendInLoop");
                if (errno == EPIPE || errno == ECONNRESET)
                    faultError = true;

            }
        }
    }
    // 说明当前这一次write，并没有把数据全部发送出去，剩余的数据需要保存到缓冲区当中，然后给channel
    // 注册epollout事件，poller发现tcp的发送缓冲区有空间，会通知相应的sock-channel，调用writeCallback_回调方法
    // 也就是调用TcpConnection::handleWrite方法，把发送缓冲区中的数据全部发送完成
    if(!fault_error && remaining>0){
        std::size_t buffer_readalbebytes = output_buffer.readable_bytes();
        if(buffer_readalbebytes+remaining >= high_water_mark && buffer_readalbebytes < high_water_mark && highWaterMarkCallback_)
            loop_->queueInLoop(std::bind(highWaterMarkCallback_, shared_from_this(), buffer_readalbebytes+remaining));
    }
    if(!channel_->isWriting())
        channel_->enableWriting();
}

void TcpConnection::send(const std::string &buf) {
    if(state_ == StateE::kConnectd){
        if(loop_->isInLoopThread())
            send_in_loop(buf.c_str(), buf.size());
        else
            loop_->runInLoop(std::bind(&TcpConnection::send_in_loop, this, buf.c_str(), buf.size()));
    }
}

void TcpConnection::shutdown_in_loop() {
    if(!channel_->isWriting())
        socket_->shutdown_write();
}

void TcpConnection::shutdown() {
    if(state_ == StateE::kConnectd){
        set_state(StateE::kDisconnecting);
        loop_->runInLoop(std::bind(&TcpConnection::shutdown_in_loop, this));
    }
}

void TcpConnection::connect_established() {
    set_state(StateE::kConnectd);
    channel_->tie(shared_from_this());
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

void TcpConnection::connect_destroyed() {
    if(state_ == StateE::kConnectd){
        set_state(StateE::kDisconnectd);
        channel_->disableAll();
        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}

TcpConnection::~TcpConnection() {
    spdlog::info("TcpConnection::dtor[{}] at fd={}, state={}\n", name_, channel_->get_fd(), state_map[state_]);
}

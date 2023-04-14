//
// Created by wuyifei0305 on 23-3-12.
//

#pragma once


#include "noncopyable.h"
#include<functional>
#include<memory>
#include<sys/epoll.h>

class EventLoop;
class Timestamp;

class Channel:noncopyable {
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;
    explicit Channel(EventLoop* loop, int fd);
    void handleEvent(const Timestamp& receiveTime);
    inline void setReadCallback(ReadEventCallback&& cb){
        readEventCallback_ = std::move(cb);
    }
    inline void setWriteCallback(EventCallback&& cb){
        writeEventCallback_ = std::move(cb);
    }
    inline void setCloseCallback(EventCallback&& cb){
        closeEventCallback_ = std::move(cb);
    }
    inline void setErrorCallback(EventCallback&& cb){
        errorEventCallback_ = std::move(cb);
    }
    void tie(const std::shared_ptr<void>& obj);
    [[nodiscard]] inline int get_fd() const{
        return fd_;
    }
    [[nodiscard]] inline int get_events() const{
        return events_;
    }
    void setREvents(int revents){
        revents_ = revents;
    }
    [[nodiscard]] inline bool isNoneEvents() const{
        return events_ == kNoneEvent;
    }
    [[nodiscard]] inline bool isWriting() const{
        return events_ & kWriteEvent;
    }
    [[nodiscard]] inline bool isReading() const{
        return events_ & kReadEvent;
    }
    inline void enableReading(){
        events_ |= kReadEvent;
        update();
    }
    inline void disableReading(){
        events_ &= ~kReadEvent;
        update();
    }
    inline void enableWriting(){
        events_ |= kWriteEvent;
        update();
    }
    inline void disableWriting(){
        events_ &= ~kWriteEvent;
        update();
    }
    inline void disableAll(){
        events_ = kNoneEvent;
        update();
    }
    [[nodiscard]] inline int get_index() const{
        return index_;
    }
    inline void set_index(int index){
        index_ = index;
    }
    [[nodiscard]] inline EventLoop* ownerloop() const{
        return loop_;
    }
    void remove();
    ~Channel() noexcept override = default;
private:
    static const int kNoneEvent{0};
    static const int kReadEvent{EPOLLIN | EPOLLPRI};
    static const int kWriteEvent{EPOLLOUT};
    EventLoop* loop_;
    const int fd_;
    int events_; // 注册fd感兴趣的事件
    int revents_; // Poller返回的具体发生的事件
    int index_;
    bool tied_;
    std::weak_ptr<void> tie_;
    ReadEventCallback readEventCallback_;
    EventCallback writeEventCallback_;
    EventCallback closeEventCallback_;
    EventCallback errorEventCallback_;
    void handleEventWithGuard(const Timestamp& receiveTime);
    void update();
};


// 通道，封装了socket文件描述符和对应的事件(EPOLLIN, EPOLLOUT...)，还绑定了poller返回的具体事件

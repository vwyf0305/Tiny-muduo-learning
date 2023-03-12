//
// Created by wuyifei0305 on 23-3-11.
//

#pragma once

class noncopyable{
public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
protected:
    explicit noncopyable() = default;
    virtual ~noncopyable() = default;
};

/*
 继承该类后无法拷贝构造或者拷贝赋值
 */
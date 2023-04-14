//
// Created by wuyifei0305 on 23-4-4.
//
#include <fmt/core.h>
#include <semaphore.h>
#include "Thread.h"
#include "CurrentThread.h"

std::atomic<int32_t> Thread::num_created = 0;

void Thread::setDefaultName() {
    num_created++;
    int num = num_created;
    if(thread_name.empty())
        thread_name = fmt::format("Thread{}", num);
}

void Thread::start() {
    started_ = true;
    sem_t sem;
    sem_init(&sem, false, 0);
    thread_ = std::shared_ptr<std::thread>(new std::thread([&]{
        tid_ = CurrentThread::get_tid();
        sem_post(&sem);
        func();
    }));
    sem_wait(&sem);

}

void Thread::join() {
    joined_ = true;
    thread_->join();
}

Thread::~Thread() noexcept {
    if(started_ && !joined_){
        thread_->detach();
    }
}
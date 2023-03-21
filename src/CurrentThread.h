//
// Created by wuyifei0305 on 23-3-18.
//

#pragma once

#include<thread>
#include<sys/syscall.h>
#include<pthread.h>
#include<unistd.h>

namespace CurrentThread{
    extern __thread pid_t t_cachedTid;
    void cacheTid();
    inline pid_t get_tid(){
        if(__builtin_expect(t_cachedTid==0, 0))
            cacheTid();

        return t_cachedTid;
    }


}


//
// Created by wuyifei0305 on 23-3-18.
//

#include "CurrentThread.h"

namespace CurrentThread
{
    __thread pid_t t_cachedTid{0};

    void cacheTid()
    {
        if (t_cachedTid == 0)
        {
            // 通过linux系统调用，获取当前线程的tid值
            t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
        }
    }
}
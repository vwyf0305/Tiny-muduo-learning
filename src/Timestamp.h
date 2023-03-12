//
// Created by wuyifei0305 on 23-3-12.
//

#pragma once

#include<ctime>
#include<chrono>
#include<string>
#include<fmt/core.h>
#include<memory>
class Timestamp{
public:
    explicit Timestamp() : MicroSecondsSinceEpoch_(0){

    }
    explicit Timestamp(int64_t time): MicroSecondsSinceEpoch_(time){

    }
    inline static Timestamp nowa(){
        return Timestamp(time(nullptr));
    }
    [[nodiscard]] inline std::string toString() const{
        tm *tm_time = localtime(&MicroSecondsSinceEpoch_);
        std::string res = fmt::format("{0}/{1}/{2}, {3}:{4}:{5}",
                                      tm_time->tm_year+1900,
                                      tm_time->tm_mon+1,
                                      tm_time->tm_mday,
                                      tm_time->tm_hour,
                                      tm_time->tm_min,
                                      tm_time->tm_sec);
        // delete tm_time;
        return res;
    }
private:
    int64_t MicroSecondsSinceEpoch_;
};
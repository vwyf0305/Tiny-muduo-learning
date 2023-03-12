//
// Created by wuyifei0305 on 23-3-11.
//
#include<chrono>
#include<fmt/core.h>
#include "Logger.h"

Logger* Logger::get_instance() {
     if(instance_ == nullptr)
         instance_ = new Logger();
     return instance_;
}

void Logger::set_level(LogLevel level) {
    level_ = level;
}

void Logger::print_log(const std::string &msg) {
     fmt::print("LogLevel: {} \n", level_map[level_]);
     fmt::print("Current Time:{0} Message:{1}\n", std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count(), msg);
}

std::string Logger::timeToString(std::chrono::system_clock::time_point &t) {
    std::time_t time = std::chrono::system_clock::to_time_t(t);
    std::string time_str = std::ctime(&time);
    time_str.resize(time_str.size() - 1);
    return time_str;
}

Logger* Logger::instance_ = nullptr;
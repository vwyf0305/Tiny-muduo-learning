//
// Created by wuyifei0305 on 23-3-11.
//

#pragma once

#include<chrono>
#include<string>
#include<unordered_map>
#include "noncopyable.h"

enum LogLevel{
    INFO,
    ERROR,
    FATAL,
    DEBUG
};

class Logger:noncopyable {
public:
    static Logger* get_instance(); // 获取日志唯一的实例对象
    void set_level(LogLevel level);
    void print_log(const std::string& msg);
    static Logger* instance_;
    static std::string timeToString(std::chrono::system_clock::time_point &t);
private:
    LogLevel level_;
    std::unordered_map<LogLevel, std::string> level_map{{INFO, "[INFO]"}, {ERROR,"[ERROR"}, {FATAL, "[FATAL]"}, {DEBUG, "[DEBUG]"}};
};






//
// Created by wuyifei0305 on 23-7-31.
//

#pragma once

#include <spdlog/spdlog.h>
#include <type_traits>
#include <concepts>
#include <string>
#include <algorithm>
#include <functional>
#include <fmt/core.h>
#include <json/json.h>


template<typename T>
concept IsNumber = std::is_arithmetic<T>::value;

template<IsNumber T>
class ParseMsg {
public:
    explicit ParseMsg(const std::function<T(T ,T)>& sum, const std::function<std::string(const std::string&)>& uppercase);
    bool get_request_msg(const std::string& msg);
    std::string get_reply_msg();
    T str_to_type(const std::string& num);
    [[nodiscard]] inline std::string get_id() const{
        return id_;
    }
private:
    std::string id_;
    std::string request_msg;
    std::string reply_msg;
    unsigned int request_msg_size{0};
    unsigned int reply_msg_size{0};
    unsigned int left_brace_count_;
    unsigned int right_brace_count_;
    unsigned int parse_idx;
    std::function<T(T ,T)> sum_;
    std::function<std::string(std::string&)> uppercase_;
};

template<IsNumber T>
ParseMsg<T>::ParseMsg(const std::function<T(T, T)>& sum, const std::function<std::string(const std::string&)>& uppercase) :id_(0),sum_(sum), uppercase_(uppercase), left_brace_count_(0),
                                                                                                      right_brace_count_(0), parse_idx(0), request_msg(""){

}

template<IsNumber T>
bool ParseMsg<T>::get_request_msg(const std::string &msg) {
    spdlog::info(msg);
    request_msg_size = static_cast<unsigned int>(msg.size());
//    if(parse_idx!=request_msg_size){
//        spdlog::critical("parse_idx!=request_msg_size");
//        exit(1);
//    }
    request_msg += msg;
    for(int i = parse_idx;i!=request_msg_size;i++){
        const char tmp{request_msg.at(i)};
        if(tmp == '{')
            left_brace_count_++;
        if(tmp == '}')
            right_brace_count_++;
    }
    parse_idx = request_msg_size;
    if(left_brace_count_==right_brace_count_)
        return true;
    else
        return false;
}

template<IsNumber T>
std::string ParseMsg<T>::get_reply_msg() {
    if(!reply_msg.empty())
        return reply_msg;
    Json::Reader reader;
    Json::Value request_value;
    reader.parse(request_msg, request_value);
    id_ = request_value["id"].asString();
    std::string function_name{request_value["function"].asString()};
    Json::Value reply_value;
    if(function_name == "sum"){
        std::string a_str = request_value["value"]["a"].asString();
        std::string b_str =  request_value["value"]["b"].asString();
        T a = str_to_type(a_str);
        T b = str_to_type(b_str);
        reply_value["result"] = sum_(a,b);
        reply_msg = reply_value.toStyledString();
    }
    else if(function_name == "uppercase"){
        std::string src_str = request_value["value"]["str"].asString();
        reply_value["result"] = uppercase_(src_str);
        reply_msg = reply_value.toStyledString();
    }
    spdlog::info(reply_msg);
    return reply_msg;
}

template<IsNumber T>
T ParseMsg<T>::str_to_type(const std::string &num) {
    double double_num = std::stod(num);
    T type_num = static_cast<T>(double_num);
    return type_num;
}





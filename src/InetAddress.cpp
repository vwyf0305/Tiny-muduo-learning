//
// Created by wuyifei0305 on 23-3-11.
//
#include<spdlog/spdlog.h>
#include<fmt/printf.h>
#include "InetAddress.h"

InetAddress::InetAddress(uint16_t port, std::string ip) {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

std::string InetAddress::toIp() const {
    char buf[24] = {0};
    auto ptr = ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    if(ptr){
        std::string res{buf};
        return res;
    }
    else{
        spdlog::error("IP addres fails to convert to string.");
        return "";
    }
}

std::string InetAddress::toIpPort() const {
    char buf[24] = {0};
    auto ptr = ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    if(ptr){
        std::size_t end = strlen(buf);
        uint16_t port = ntohs(addr_.sin_port);
        //fmt::sprintf(buf+end);
        std::string res = fmt::format("IP : {0}, Port : {1}", buf, port);
        return res;
    }
    else{
        spdlog::error("IP or port failes to convert");
        return "";
    }

}


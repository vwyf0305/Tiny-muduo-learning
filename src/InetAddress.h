//
// Created by wuyifei0305 on 23-3-11.
//

#pragma once

#include<spdlog/spdlog.h>
#include<arpa/inet.h>
#include<string>
#include<string.h>

class InetAddress {
public:
    explicit InetAddress(uint16_t port, std::string ip="127.0.0.1");
    explicit InetAddress(sockaddr_in addr): addr_(addr) {

    }
    explicit InetAddress(){
        bzero(&addr_, sizeof(addr_));
        spdlog::warn("You need to enter at least the port number.");
    }
    std::string toIp() const;


    std::string toIpPort() const;


    inline uint16_t toPort() const{
        return ntohs(addr_.sin_port);
    }
    inline const sockaddr_in* getAddr() const{
        return &addr_;
    }
    inline void set_sock_addr(const sockaddr_in &addr) {
        addr_ = addr;
    }
private:
    sockaddr_in addr_;
};




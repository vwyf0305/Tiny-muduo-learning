//
// Created by wuyifei0305 on 23-5-20.
//

#pragma once

#include <memory>
#include <functional>

class Buffer;
class TcpConnection;
class Timestamp;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void (const TcpConnectionPtr&)>;
using CloseCallback = std::function<void (const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void (const TcpConnectionPtr&)>;
using MessageCallback = std::function<void (const TcpConnectionPtr&,
                                            Buffer*,
                                            Timestamp)>;
using HighWaterMarkCallback = std::function<void (const TcpConnectionPtr&, std::size_t)>;

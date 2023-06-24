#include "EventLoop.h"
#include "InetAddress.h"
#include "EchoServer.h"
#include <spdlog/spdlog.h>
#include <memory>




int main(int argc, char* argv[])
{
     EventLoop loop;
     spdlog::info("EventLoop loop created.");
     InetAddress addr{8000};
     std::shared_ptr<EchoServer> echo_server = std::make_shared<EchoServer>(&loop, addr, "Hello World");
     echo_server->start();
     loop.loop();
     return 0;
}

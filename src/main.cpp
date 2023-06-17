#include "EventLoop.h"
#include "InetAddress.h"
#include "EchoServer.h"





int main(int argc, char* argv[])
{
     EventLoop loop;
     InetAddress addr{8000};
     EchoServer echoServer{&loop, addr, "Hello World"};
     echoServer.start();
     loop.loop();
     return 0;
}

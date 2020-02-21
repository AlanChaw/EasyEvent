#include "net/TcpServer.h"
#include "net/EventLoop.h"
#include "base/CapsuledAddr.h"
#include "base/Socket.h"
#include <stdio.h>

using namespace EasyEvent;

void onConnection(const TcpConnectionPtr& conn)
{
  if (conn->connected())
  {
    printf("onConnection(): new connection [%s] from %s\n",
           conn->getName().c_str(),
           conn->getPeerAddress().toString().c_str());
  }
  else
  {
    printf("onConnection(): connection [%s] is down\n",
           conn->getName().c_str());
  }
}

void onMessage(const TcpConnectionPtr& conn,
               Buffer* buf,
               muduo::Timestamp receiveTime)
{
  printf("onMessage(): received %zd bytes from connection [%s] at %s\n",
         buf->readableBytes(),
         conn->getName().c_str(),
         receiveTime.toFormattedString().c_str());

  conn->send(buf->retrieveAsString());
}

int main()
{
  printf("main(): pid = %d\n", getpid());

  CapsuledAddr listenAddr(9981);
  EventLoop loop;

  TcpServer server(&loop, listenAddr);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.start();

  loop.startLoop();
}

#include "net/TcpServer.h"
#include "net/EventLoop.h"
#include "base/CapsuledAddr.h"
#include <stdio.h>

std::string message1;
std::string message2;

using namespace EasyEvent;

void onConnection(const TcpConnectionPtr& conn)
{
  if (conn->connected())
  {
    printf("onConnection(): new connection [%s] from %s\n",
           conn->getName().c_str(),
           conn->getPeerAddress().toString().c_str());

    conn->send(message1);
    conn->send(message2);
    conn->shutdown();
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

  buf->retrieveAll();
}

int main(int argc, char* argv[])
{
  printf("main(): pid = %d\n", getpid());

  int len1 = 100;
  int len2 = 200;

  if (argc > 2)
  {
    len1 = atoi(argv[1]);
    len2 = atoi(argv[2]);
  }

  message1.resize(len1);
  message2.resize(len2);
  std::fill(message1.begin(), message1.end(), 'A');
  std::fill(message2.begin(), message2.end(), 'B');

  CapsuledAddr listenAddr(9981);
  EventLoop loop;
  printf("loop created\n");
  TcpServer server(&loop, listenAddr);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);

  server.setThreadNum(5);
  printf("before start\n");
  server.start();

  loop.startLoop();
}

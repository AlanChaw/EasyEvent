#include "net/TcpServer.h"
#include "net/EventLoop.h"
#include "net/TcpConnection.h"
#include "base/Buffer.h"
#include <stdio.h>

using namespace EasyEvent;

void onConnection(const TcpConnectionPtr& conn){
    if(conn->connected()){
        printf("new connection [%s] from %s \n", 
        conn->getName().c_str(), conn->getPeerAddress().toString().c_str());
    }else{
        printf("connection [%s] is down \n", conn->getName().c_str());
    }
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, muduo::Timestamp receiveTime){
  printf("onMessage(): received %zd bytes from connection [%s] at %s\n",
         buf->readableBytes(),
         conn->getName().c_str(),
         receiveTime.toFormattedString().c_str());

  printf("onMessage(): [%s]\n", buf->retrieveAsString().c_str());
}

int main(){
    printf("main: pid = %d\n", getpid());

    CapsuledAddr listenAddr(2000);
    EventLoop loop;

    TcpServer server(&loop, listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    server.start();
    loop.startLoop();

    return 0;
}
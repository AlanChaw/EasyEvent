#include "net/Acceptor.h"
#include "net/EventLoop.h"
#include <unistd.h>

using namespace EasyEvent;

void newConnCB1(int sockfd, const CapsuledAddr& peerAddr){
    printf("port 2000 received new connection from : %s \n", peerAddr.toString().c_str());
    std::string info = "Hello world from port 2000\n";
    ::write(sockfd, info.c_str(), sizeof info);
    Socket::close(sockfd);
}

void newConnCB2(int sockfd, const CapsuledAddr& peerAddr){
    printf("port 2001 received new connection from : %s \n", peerAddr.toString().c_str());
    std::string info = "Hello world from port 2001\n";
    ::write(sockfd, info.c_str(), sizeof info);
    Socket::close(sockfd);
}

int main(){
    EventLoop loop;

    CapsuledAddr listenAddr1(2000);
    Acceptor acceptor1(&loop, listenAddr1);
    acceptor1.setNewConnectionCallback(newConnCB1);
    acceptor1.listen();
    printf("port 2000 start listen... \n");

    CapsuledAddr listenAddr2(2001);
    Acceptor acceptor2(&loop, listenAddr2);
    acceptor2.setNewConnectionCallback(newConnCB2);
    acceptor2.listen();
    printf("port 2001 start listen... \n");

    loop.startLoop();
    
    return 0;
}

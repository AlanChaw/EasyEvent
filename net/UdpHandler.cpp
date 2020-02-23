
#include "UdpHandler.h"
#include "EventLoop.h"
#include "base/CapsuledAddr.h"
#include "string.h"
#include "UdpServer.h"

#include <assert.h>

#define BUFFLEN 65536

using namespace EasyEvent;

UdpHandler::UdpHandler(EventLoop* loop, const CapsuledAddr& listenAddr)
    : _loop(loop),
      _acceptSocket(Socket::createUdpBlocking()),
      _acceptChannel(loop, _acceptSocket.getFd()),
      _listenning(false)
{
    _acceptSocket.setReuseAddr(true);   // 允许地址端口复用
    _acceptSocket.bindAddress(listenAddr);
    printf("listening socket fd: %d\n", _acceptSocket.getFd());

    _acceptChannel.setReadCallback(     // 交由 Channel 侦听当前 Socket，有连接接入时回调
        std::bind(&UdpHandler::handleNewConnection, this)
    );
}

void UdpHandler::listen(){
    assert(_loop->isInLoopThread());
    _listenning = true;
    _acceptChannel.enableReading();     // 将 Channel 注入到 EventLoop
}

void UdpHandler::handleReceiveMessage(UdpServer* server){
    // 开始接收消息
    // FIX ME!! 这里目前是阻塞模式，直接读取。以后将应用层缓冲更改后可实现非阻塞读取
    printf("handling receive msg \n");

    CapsuledAddr peerAddr(0);
    char msg[BUFFLEN];
    socklen_t len;
    sockaddr_in client_addr;
    len = sizeof(client_addr);
    bzero(&client_addr, len);
    ssize_t count;
    // 阻塞 IO
    count = recvfrom(_acceptSocket.getFd(), msg, BUFFLEN, 0, Socket::cast_to_sockaddr(&client_addr), &len);
    assert(count >= 0);

    peerAddr.setSocketAddr(client_addr);

    if(_msgRecvCB){
        _msgRecvCB(server, msg, BUFFLEN, peerAddr);
    }
}

void UdpHandler::handleSendMessage(char* msg, int len, CapsuledAddr& peerAddr){
    // printf("server:%s\n",buf);  //打印自己发送的信息给
    sockaddr_in peer = peerAddr.getSocketAddr();
    sendto(_acceptSocket.getFd(), msg, len, 0, Socket::cast_to_sockaddr(&peer), sizeof peer);  //发送信息给client，注意使用了clent_addr结构体指针

    if(_msgSentCB){
        _msgSentCB();
    }
}

/********************** private function **************************/

void UdpHandler::handleNewConnection(){
    assert(_loop->isInLoopThread());
    CapsuledAddr peerAddr(0);

    printf("acceptor receive new connection\n");
    // 这里没考虑 fd 用尽的情况
    // muduo的处理方法是，准备一个坑位用来填坑，如果已满则拒绝连接
    // printf("socket fd: %d received new connection\n", _acceptSocket.getFd());
    // int connfd = _acceptSocket.accept(&peerAddr);
    // printf("connfd: %d \n", connfd);
    if(_newConnCB)
        _newConnCB();       // 调用 TcpServer 传入的 callback
}

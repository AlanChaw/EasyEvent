
#include "Acceptor.h"
#include "EventLoop.h"
#include "base/CapsuledAddr.h"

#include <assert.h>

using namespace EasyEvent;

Acceptor::Acceptor(EventLoop* loop, const CapsuledAddr& listenAddr)
    : _loop(loop),
      _acceptSocket(Socket::createNonblockingOrDie()),
      _acceptChannel(loop, _acceptSocket.getFd()),
      _listenning(false)
{
    _acceptSocket.setReuseAddr(true);   // 允许地址端口复用
    _acceptSocket.bindAddress(listenAddr);
    _acceptChannel.setReadCallback(     // 交由 Channel 侦听当前 Socket，有连接接入时回调
        std::bind(&Acceptor::handleNewConnection, this)
    );
}

void Acceptor::listen(){
    assert(_loop->isInLoopThread());
    _listenning = true;
    _acceptSocket.listen();
    _acceptChannel.enableReading();     // 将 Channel 注入到 EventLoop
}


/********************** private function **************************/

void Acceptor::handleNewConnection(){
    assert(_loop->isInLoopThread());
    CapsuledAddr peerAddr(0);

    // 这里没考虑 fd 用尽的情况
    // muduo的处理方法是，准备一个坑位用来填坑，如果已满则拒绝连接
    printf("socket fd: %d \n", _acceptSocket.getFd());
    int connfd = _acceptSocket.accept(&peerAddr);
    printf("connfd: %d \n", connfd);

    if(connfd >= 0){
        if(_newConnCB){
            _newConnCB(connfd, peerAddr);       // 如果用户传入了 onConnectionCB，则调用之
        }else{
            FileDescriptor::close(connfd);      // 关闭新连接，而不是 socket
        }
    }
    // TODO
}


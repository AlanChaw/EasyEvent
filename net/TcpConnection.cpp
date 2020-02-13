#include "TcpConnection.h"

#include "EventLoop.h"
#include "Channel.h"
#include "base/Socket.h"

#include <assert.h>
// #include <errno.h>
#include <unistd.h>

using namespace EasyEvent;

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& name,
                             int connfd,
                             const CapsuledAddr& localAddr,
                             const CapsuledAddr& peerAddr)
    : _loop(loop),
      _name(name),
      _hasConnected(false),
      _conn(new FileDescriptor(connfd)),
      _channel(new Channel(loop, connfd)),
      _localAddr(localAddr),
      _peerAddr(peerAddr)    
{
    _channel->setReadCallback(
        std::bind(&TcpConnection::handleRead, this)
    );
}

TcpConnection::~TcpConnection(){

}

void TcpConnection::connectEstablished(){
    assert(_loop->isInLoopThread());
    assert(!_hasConnected);
    _hasConnected = true;
    _channel->enableReading();      // 将 Channel 注册到 EventLoop，开始侦听
}


/************************** private functions **************************/

void TcpConnection::handleRead(){
    // 这里将来用 buffer 来处理
    char buf[65536];
    ssize_t n = ::read(_channel->getFd(), buf, sizeof buf);
    _msgCB(shared_from_this(), buf, n);
}

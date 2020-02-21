#include "TcpConnection.h"

#include "EventLoop.h"
#include "Channel.h"
#include "base/Socket.h"

#include <assert.h>
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
        std::bind(&TcpConnection::handleRead, this, std::placeholders::_1)
    );
    _channel->setWriteCallback(
        std::bind(&TcpConnection::handleWrite, this)
    );
    _channel->setCloseCallback(
        std::bind(&TcpConnection::handleClose, this)
    );
    _channel->setErrorCallback(
        std::bind(&TcpConnection::handleError, this)
    );
}

TcpConnection::~TcpConnection(){

}

void TcpConnection::connectEstablished(){
    assert(_loop->isInLoopThread());
    assert(!_hasConnected);
    _hasConnected = true;
    _channel->enableReading();      // 将 Channel 注册到 EventLoop，开始侦听

    _connCB(shared_from_this());    // 连接建立时，回调用户传入的 onConnectionCB
}

void TcpConnection::connectDestroyed(){
    assert(_loop->isInLoopThread());
    assert(_hasConnected);
    _hasConnected = false;

    _channel->disableAll();
    _connCB(shared_from_this());    // connCB 既处理连接建立，又处理连接断开

    _loop->removeChannel(_channel.get());
}


/************************** private functions **************************/

void TcpConnection::handleRead(muduo::Timestamp receiveTime){
    int savedErrno = 0;
    ssize_t n = _inputBuffer.readFd(_channel->getFd(), &savedErrno);
    if(n > 0){
        _msgCB(shared_from_this(), &_inputBuffer, receiveTime);     // n > 0 ，有新消息
    }else if(n == 0){
        handleClose();                          // n == 0，读到 0，表示断开(自行规定)
    }else{
        errno = savedErrno;
        handleError();                          // n < 0，出现错误
    }
    
}

// 处理信息发送
void TcpConnection::handleWrite(){

}

// 处理连接断开
void TcpConnection::handleClose(){
    assert(_loop->isInLoopThread());
    assert(_hasConnected);
    _channel->disableAll();
    _closeCB(shared_from_this());   // 保证关闭期间该 TcpConnection 不会被析构
}

//
void TcpConnection::handleError(){
    // 获取的实际上是 connfd 上的 error
    int err = Socket::getSocketErr(_channel->getFd());
    printf("TcpConnection [%s] handle error [%d]", _name.c_str(), err);
    abort();
}


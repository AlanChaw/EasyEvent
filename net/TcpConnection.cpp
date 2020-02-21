#include "TcpConnection.h"

#include "EventLoop.h"
#include "Channel.h"
#include "base/Socket.h"

#include <assert.h>
#include <unistd.h>
#include <netinet/tcp.h>


using namespace EasyEvent;

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& name,
                             int connfd,
                             const CapsuledAddr& localAddr,
                             const CapsuledAddr& peerAddr)
    : _loop(loop),
      _name(name),
      _state(kConnecting),
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
    // LOG OUTPUT
}

void TcpConnection::send(const std::string& message){
    if( _state == kConnected){
        if(_loop->isInLoopThread()){
            sendInLoop(message);
        }else{
            _loop->runInLoop(
                std::bind(&TcpConnection::sendInLoop, this, std::move(message))
            );
        }
    }
}

void TcpConnection::shutdown(){
    if(_state == kConnected){
        setState(kDisconnecting);
        _loop->runInLoop(std::bind(&TcpConnection::shutDownInLoop, this));
    }
}

void TcpConnection::setTcpNoDelay(bool on){
    int optval = on ? 1 : 0;
    ::setsockopt(_conn->getFd(), IPPROTO_TCP, TCP_NODELAY,
               &optval, sizeof optval);
}

void TcpConnection::connectEstablished(){
    assert(_loop->isInLoopThread());
    assert(_state == kConnecting);
    setState(kConnected);
    _channel->enableReading();      // 将 Channel 注册到 EventLoop，开始侦听

    _connCB(shared_from_this());    // 连接建立时，回调用户传入的 onConnectionCB
}

void TcpConnection::connectDestroyed(){
    assert(_loop->isInLoopThread());
    assert(_state == kConnected || _state == kDisconnecting);
    setState(kDisconnected);

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

// 处理信息发送的回调
void TcpConnection::handleWrite(){
    assert(_loop->isInLoopThread());
    if(_channel->isWriting()){
        size_t n = _outputBuffer.writeFd(_channel->getFd());

        if(n > 0){
            _outputBuffer.retrieve(n);
            if(_outputBuffer.readableBytes() == 0){     // level trigger,读完要记得关掉
                _channel->disableWriting();
                if(_writeCompleteCB){
                    _loop->queueInLoop(std::bind(_writeCompleteCB, shared_from_this()));
                }
                if(_state == kDisconnecting){
                    shutDownInLoop();
                }
            }else{
                printf("write more data ...");
            }
        }else{
            // LOG ERROR
            abort();
        }
    }else{
        printf("connection is down, no more writing");
    }
}

// 处理连接断开回调
void TcpConnection::handleClose(){
    assert(_loop->isInLoopThread());
    assert(_state == kConnected || _state == kDisconnecting);
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


void TcpConnection::sendInLoop(const std::string& message){
    assert(_loop->isInLoopThread());
    ssize_t nwrote = 0;

    // 没有在 output buffer中排队的数据，尝试直接写 socket
    if(!_channel->isWriting() && _outputBuffer.readableBytes() == 0){
        nwrote = ::write(_channel->getFd(), message.data(), message.size());
        if(nwrote >= 0){
            if(static_cast<size_t>(nwrote) < message.size()){
            // LOG: output more data
            }else if(_writeCompleteCB){
                _loop->queueInLoop(std::bind(_writeCompleteCB, shared_from_this()));
            }
        }else{
            nwrote = 0;
            if(errno != EWOULDBLOCK){
                // LOG: error send in loop
                abort();
            }
        }
    }

    assert(nwrote >= 0);
    if(static_cast<size_t>(nwrote) < message.size()){        // 还有一部分数据没写完
        _outputBuffer.append(message.data()+nwrote, message.size()-nwrote);
        if(!_channel->isWriting()){
            _channel->enableWriting();
        }
    }

}


void TcpConnection::shutDownInLoop(){
    assert(_loop->isInLoopThread());
    if(!_channel->isWriting()){
        assert(::shutdown(_conn->getFd(), SHUT_WR) >= 0);
    }
}

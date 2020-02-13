#include "TcpConnection.h"

#include "EventLoop.h"
#include "Channel.h"
#include "base/Socket.h"

#include <errno.h>
#include <stdio.h>

using namespace EasyEvent;

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& name,
                             int sockfd,
                             const CapsuledAddr& localAddr,
                             const CapsuledAddr& peerAddr)
    : _loop(loop),
    _name(name),
    _socket(new Socket(sockfd)),
    _channel(new Channel(loop, sockfd)),
    _localAddr(localAddr),
    _peerAddr(peerAddr)    
{
    _channel->setReadCallback(
        std::bind(&TcpConnection::handleRead, this)
    );
}


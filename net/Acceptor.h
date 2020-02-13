#pragma once

#include <functional>

#include "base/noncopyable.h"
#include "base/Socket.h"
#include "Channel.h"

namespace EasyEvent{

class EventLoop;
class CapsuledAddress;

/*
  TCP 连接的接收器
   1. 创建 socket (非阻塞) 并绑定到指定 addr
   2. 创建 管理 socket（socket fd）的 Channel 对象，观察该 listen socket 的 read 事件
   3. 用于 accept() (非阻塞) TCP 连接，并通过回调通知使用者
   4. 内部类，供 TcpServer 使用，生命周期由 TcpServer 控制
*/
class Acceptor : private noncopyable{
public:
    typedef std::function<void (int connfd, const CapsuledAddr&)> ConnCB;

    Acceptor(EventLoop* loop, const CapsuledAddr& listenAddr);

    // 为新连接绑定 readable 时的回调函数
    void setNewConnectionCallback(const ConnCB& cb) { _newConnCB = cb; }

    bool isListenning() const { return _listenning; }
    void listen();
    
private:
    void handleRead();

    EventLoop* _loop;
    Socket _acceptSocket;
    Channel _acceptChannel;
    ConnCB _newConnCB;
    bool _listenning;
};

}


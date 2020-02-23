#pragma once

#include <functional>

#include "base/noncopyable.h"
#include "base/Socket.h"
#include "Channel.h"
#include "base/CapsuledAddr.h"
#include "UdpCallbacks.h"

namespace EasyEvent{

class EventLoop;

/*
  UDP 接收器，供 UdpServer 使用，暂时仅支持阻塞模式的 udp 数据报处理
  
*/
class UdpHandler : private noncopyable{
public:
    // typedef std::function<void ()> ConnCB;
    // typedef std::function<void (char* msg, int BUFFLEN, CapsuledAddr& peerAddr)> MessageReceivedCallback;
    // typedef std::function<void ()> MessageSentCallback;
    
    explicit UdpHandler(EventLoop* loop, const CapsuledAddr& listenAddr);
    // ~Acceptor();

    void setNewConnectionCallback(const ConnectionCallback& cb) { _newConnCB = cb; }
    void setMessageReceivedCallback(const MessageReceivedCallback& cb) { _msgRecvCB = cb; }
    void setMessageSentCallback(const MessageSentCallback& cb) { _msgSentCB = cb; }

    const Socket& getAcceptSocket() const { return _acceptSocket; }

    bool isListenning() const { return _listenning; }
    void listen();

    void handleReceiveMessage(UdpServer* server);    // 收发消息，由 UdpServer 直接调用
    void handleSendMessage(char* msg, int len, CapsuledAddr& peerAddr);
    
private:
    void handleNewConnection();     // 接收新连接，由 Channel 负责调用

    EventLoop* _loop;
    Socket _acceptSocket;
    Channel _acceptChannel;
    ConnectionCallback _newConnCB;
    MessageReceivedCallback _msgRecvCB;
    MessageSentCallback _msgSentCB;
    bool _listenning;
};

}


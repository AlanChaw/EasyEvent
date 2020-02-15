#pragma once
#include "TcpConnection.h"

#include "base/noncopyable.h"
#include "Acceptor.h"

#include <memory>
#include <unordered_map>


namespace EasyEvent{

class EventLoop;

// TcpServer 用于管理 TcpConnections，直接供用户使用。
// 但其不控制 TcpConnection 的生命周期，只是持有其 shared_ptr
class TcpServer : private noncopyable{
public:

    TcpServer(EventLoop* loop, const CapsuledAddr& listenAddr);
    ~TcpServer();

    // 是幂等函数，所以不用考虑线程安全问题
    void start();

    void setConnectionCallback(const TcpConnection::ConnectionCallback& cb){
        _connCB = cb;
    }
    void setMessageCallback(const TcpConnection::MessageCallback& cb){
        _msgCB = cb;
    }

private:
    void newConnection(int connfd, const CapsuledAddr& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);

private:
    typedef std::unordered_map<std::string, TcpConnectionPtr> ConnMap;

    EventLoop* _loop;
    const std::string _name;
    std::unique_ptr<Acceptor> _acceptor;
    TcpConnection::ConnectionCallback _connCB;
    TcpConnection::MessageCallback _msgCB;
    bool _started;
    int _nextConnId;
    ConnMap _connMap;
};


}

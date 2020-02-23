#pragma once
#include "TcpConnection.h"

#include "base/noncopyable.h"

#include <memory>
#include <unordered_map>


namespace EasyEvent{

class EventLoopThreadPool;
class EventLoop;
class Acceptor;

// TcpServer 用于管理 TcpConnections，直接供用户使用。
// 但其不控制 TcpConnection 的生命周期，只是持有其 shared_ptr
class TcpServer : private noncopyable{
public:

    TcpServer(EventLoop* loop, const CapsuledAddr& listenAddr);
    ~TcpServer();

    // 是幂等函数，所以不用考虑线程安全问题
    void start();

    // 0 表示所有IO都在 TcpServer 线程进行
    // >0 表示 TcpServer 只处理新连接，并将 IO 交给大小为 num 的线程池来处理
    void setThreadNum(int numThreads);

    // 非线程安全
    void setConnectionCallback(const TcpConnection::ConnectionCallback& cb){
        _connCB = cb;
    }

    // 非线程安全
    void setMessageCallback(const TcpConnection::MessageCallback& cb){
        _msgCB = cb;
    }

    void setWriteCompleteCallback(const TcpConnection::WriteCompleteCallback& cb){
        _writeCompleteCb = cb; 
    }

private:
    void newConnection(int connfd, const CapsuledAddr& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);
private:
    typedef std::unordered_map<std::string, TcpConnectionPtr> ConnMap;

    EventLoop* _loop;
    const std::string _name;
    std::unique_ptr<Acceptor> _acceptor;
    std::unique_ptr<EventLoopThreadPool> _threadPool;
    TcpConnection::ConnectionCallback _connCB;
    TcpConnection::MessageCallback _msgCB;
    TcpConnection::WriteCompleteCallback _writeCompleteCb;
    bool _started;
    int _nextConnId;
    ConnMap _connMap;
};


}

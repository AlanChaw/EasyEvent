#pragma once

#include "base/CapsuledAddr.h"
#include "base/noncopyable.h"
#include "base/FileDescriptor.h"
#include "base/Timestamp.h"
#include "base/Buffer.h"

#include <memory>

namespace EasyEvent{

class EventLoop;
class Channel;
class Socket;
class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
/* 
  TcpConnection 相当于对每个 socket 连接做了封装
  1. 唯一一个用 shared_ptr 持有的类
  2. 为了防止串话，程序不能只记住 connfd，而应该持有 TcpConnection 对象
  3. TcpConnection 用来封装 socket 返回的 connfd，这样保证处理请求期间 connfd不会被关闭
  4. TcpConnection 表示的是 “一次连接”，一旦连接断开，这个对象就没用了
*/
class TcpConnection : private noncopyable, 
                      public std::enable_shared_from_this<TcpConnection>{

public:
    
    // typedef std::function<void()> TimerCallback;
    typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
    typedef std::function<void (const TcpConnectionPtr&,
                                Buffer* buf,
                                muduo::Timestamp)> MessageCallback;
    typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
    typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;


    TcpConnection(EventLoop* loop,
                  const std::string& name,
                  int connfd,
                  const CapsuledAddr& localAddr,
                  const CapsuledAddr& peerAddr);
    ~TcpConnection();
            
    EventLoop* getLoop() const { return _loop; }
    const std::string& getName() const { return _name;}
    const CapsuledAddr& getLocalAddress() { return _localAddr; }
    const CapsuledAddr& getPeerAddress() { return _peerAddr; }
    bool connected() const { return _state == kConnected; }

    // thread safe，可以跨线程调用
    void send(const std::string& message);
    void shutdown();

    void setTcpNoDelay(bool on);

    void setConnectionCallback(const ConnectionCallback& cb){
        _connCB = cb;
    }
    void setMessageCallback(const MessageCallback& cb){
        _msgCB = cb;
    }
    // 内部使用，用于通知 TcpServer 析构该 TcpConnection
    void setCloseCallback(const CloseCallback& cb){
        _closeCB = cb;
    }

    // 用于给 TcpServer 调用
    void connectEstablished();
    void connectDestroyed();

private:
    enum StateE {kConnecting, kConnected, kDisconnecting, kDisconnected, };
    void setState(StateE s) { _state = s; }
    void handleRead(muduo::Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const std::string& message);
    void shutDownInLoop();

private:
    EventLoop* _loop;
    std::string _name;
    StateE _state;
    // bool _hasConnected;


    std::unique_ptr<FileDescriptor> _conn;
    std::unique_ptr<Channel> _channel;
    CapsuledAddr _localAddr;
    CapsuledAddr _peerAddr;
    ConnectionCallback _connCB;
    MessageCallback _msgCB;
    WriteCompleteCallback _writeCompleteCB;
    CloseCallback _closeCB;
    Buffer _inputBuffer;
    Buffer _outputBuffer;
};

}
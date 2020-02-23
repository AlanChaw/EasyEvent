#pragma once

#include "base/noncopyable.h"
#include "base/CapsuledAddr.h"
#include "base/Timestamp.h"

#include "UdpHandler.h"
#include "UdpCallbacks.h"

#include <memory>
#include <unordered_map>


namespace EasyEvent{

class EventLoopThreadPool;
class EventLoop;
// class UdpServer;

// typedef std::shared_ptr<UdpServer> UdpServerPtr;

class UdpServer : private noncopyable, 
                  public std::enable_shared_from_this<UdpServer>
{
public:
    // typedef std::function<void ()> ConnectionCallback;
    // typedef std::function<void (char* msg, int BUFFLEN, CapsuledAddr& peerAddr)> MessageReceivedCallback;
    // typedef std::function<void ()> MessageSentCallback;
    
    UdpServer(EventLoop* loop, const CapsuledAddr& listenAddr);
    ~UdpServer();

    void start();

    void sendMessage(char* msg, int len, CapsuledAddr& peerAddr);

    void setThreadNum(int numThreads);

    void setConnectionCallback(const ConnectionCallback& cb){
        _connCB = cb;
    }

    void setMessageCallback(const MessageReceivedCallback& cb);

    void setWriteCompleteCallback(const MessageSentCallback& cb);

private:
    void newConnection();
    // void receiveMessage();
    // void sendMessage();
    // void removeConnection(const TcpConnectionPtr& conn);
    // void removeConnectionInLoop(const TcpConnectionPtr& conn);

private:
    // typedef std::unordered_map<std::string, TcpConnectionPtr> ConnMap;
    EventLoop* _loop;
    const std::string _name;
    std::unique_ptr<UdpHandler> _handler;
    std::unique_ptr<EventLoopThreadPool> _threadPool;
    ConnectionCallback _connCB;
    MessageReceivedCallback _msgCB;
    MessageSentCallback _writeCompleteCb;
    bool _started;
    int _nextConnId;
    // ConnMap _connMap;
};


}

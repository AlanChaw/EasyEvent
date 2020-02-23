#include "UdpServer.h"

#include "EventLoopThreadPool.h"
#include "EventLoop.h"

#include <functional>

using namespace EasyEvent;
using namespace std::placeholders;

UdpServer::UdpServer(EventLoop* loop, const CapsuledAddr& listenAddr)
    : _loop(loop),
      _name(listenAddr.toString()),
      _handler(new UdpHandler(loop, listenAddr)),
      _threadPool(new EventLoopThreadPool(_loop)),
      _started(false),
      _nextConnId(1)
{
    // 有新连接接入时，Acceptor 会获取到 connfd 和 peerAddr，并传入 newConnection
    // newConnection 相当于用户

    _handler->setNewConnectionCallback(
        std::bind(&UdpServer::newConnection, this)
    );
}

UdpServer::~UdpServer()
{
}

void UdpServer::sendMessage(char* msg, int len, CapsuledAddr& peerAddr){
    EventLoop* ioLoop = _threadPool->getNextLoop();
    ioLoop->runInLoop(std::bind(&UdpHandler::handleSendMessage, _handler.get(), msg, len, peerAddr));
}

void UdpServer::setMessageCallback(const MessageReceivedCallback& cb){
    _handler->setMessageReceivedCallback(cb);
}

void UdpServer::setWriteCompleteCallback(const MessageSentCallback& cb){
    _handler->setMessageSentCallback(cb);
}

void UdpServer::setThreadNum(int numThreads){
    assert(numThreads >= 0);
    _threadPool->setThreadNum(numThreads);
}

// 线程安全
void UdpServer::start(){
    if(!_started){
        _started = true;
    }
    
    if(!_handler->isListenning()){
        _loop->runInLoop(
            std::bind(&UdpHandler::listen, _handler.get())
        );
    }
}

/************************ private functions *************************/
void UdpServer::newConnection(){
    assert(_loop->isInLoopThread());

    EventLoop* ioLoop = _threadPool->getNextLoop();
    if(_connCB){
        ioLoop->runInLoop(_connCB);
    }
    printf("before shared from this\n");
    ioLoop->runInLoop(std::bind(&UdpHandler::handleReceiveMessage, _handler.get(), this));
}

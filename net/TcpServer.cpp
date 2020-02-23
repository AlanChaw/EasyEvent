#include "TcpServer.h"

#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "Acceptor.h"


#include <functional>
// #include <boost/bind.hpp>

using namespace EasyEvent;
using namespace std::placeholders;

TcpServer::TcpServer(EventLoop* loop, const CapsuledAddr& listenAddr)
    : _loop(loop),
      _name(listenAddr.toString()),
      _acceptor(new Acceptor(loop, listenAddr)),
      _threadPool(new EventLoopThreadPool(_loop)),
      _started(false),
      _nextConnId(1)
{
    // 有新连接接入时，Acceptor 会获取到 connfd 和 peerAddr，并传入 newConnection
    // newConnection 相当于用户

    _acceptor->setNewConnectionCallback(
        std::bind(&TcpServer::newConnection, this, _1, _2));    
}

TcpServer::~TcpServer()
{
}


// 0 表示所有IO都在 TcpServer 线程进行
// >0 表示 TcpServer 只处理新连接，并将 IO 交给大小为 num 的线程池来处理
void TcpServer::setThreadNum(int numThreads){
    assert(numThreads >= 0);
    _threadPool->setThreadNum(numThreads);
}

// 线程安全
void TcpServer::start(){
    if(!_started){
        _started = true;
    }
    if(!_acceptor->isListenning()){
        _loop->runInLoop(
            std::bind(&Acceptor::listen, _acceptor.get())
        );
    }
}


/************************ private functions *************************/
void TcpServer::newConnection(int connfd, const CapsuledAddr& peerAddr){
    assert(_loop->isInLoopThread());
    char buf[32];
    snprintf(buf, sizeof buf, "#%d", _nextConnId++);
    std::string connName = _name + buf;

    // !!!!!! 这里实际上是通过新连接的 fd 获取到的本地地址，而不是通过 socket fd
    // !!!!!! 在 muduo 中这个部分的实现有严重的歧义。虽然传入 socket fd 也能拿到同样的本地地址
    CapsuledAddr localAddr(Socket::getLocalAddr(connfd));  // 0.0.0.0 : listen port

    EventLoop* ioLoop = _threadPool->getNextLoop();

    // printf("new connection socket fd: %d \n", _acceptor->getAcceptSocket().getFd());
    // printf("new connection connfd: %d \n", connfd);
    // shared ptr 间接持有
    TcpConnectionPtr conn(
      std::make_shared<TcpConnection>(ioLoop, connName, connfd, localAddr, peerAddr)
    );

    // printf("localAddr: %s \n", localAddr.toString().c_str());
    // printf("peerAddr: %s \n", peerAddr.toString().c_str());

    _connMap[connName] = conn;

    // 把用户传入的回调函数原封传入新的 TcpConnection
    conn->setConnectionCallback(_connCB);
    conn->setMessageCallback(_msgCB);
    conn->setWriteCompleteCallback(_writeCompleteCb);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, _1));

    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn){
    _loop->runInLoop(bind(&TcpServer::removeConnectionInLoop, this, conn));
    // assert(_loop->isInLoopThread());
    // size_t n = _connMap.erase(conn->getName());
    // assert(n == 1);  (void)n;
    // _loop->queueInLoop(
    //     std::bind(&TcpConnection::connectDestroyed, conn)
    // );
    // // 删除后再告诉一声
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn){
    assert(_loop->isInLoopThread());
    size_t n = _connMap.erase(conn->getName());
    assert(n == 1); (void)n;
    EventLoop* ioLoop = conn->getLoop();

    ioLoop->queueInLoop(
        std::bind(&TcpConnection::connectDestroyed, conn)
    );
}

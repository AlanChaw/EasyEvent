#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"

#include <functional>

using namespace EasyEvent;

void testBind(int connfd, const CapsuledAddr& peerAddr);

TcpServer::TcpServer(EventLoop* loop, const CapsuledAddr& listenAddr)
    : _loop(loop),
      _name(listenAddr.toString()),
      _acceptor(new Acceptor(_loop, listenAddr)),
      _started(false),
      _nextConnId(1)
{
    auto func = std::bind(newConnection, this, std::placeholders::_1, std::placeholders::_2);
    _acceptor->setNewConnectionCallback(func);
}


/************************ private functions *************************/
void TcpServer::newConnection(int connfd, const CapsuledAddr& peerAddr){

}

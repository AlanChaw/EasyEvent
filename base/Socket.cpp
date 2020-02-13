
#include "Socket.h"

#include <sys/socket.h>     // socket() bind() listen() accept()
#include <netinet/in.h>     // struct sockaddr_in
#include <netinet/tcp.h>    // TCP 基本操作
#include <string.h>         // bzero()
#include <assert.h>         
#include <unistd.h>         // close(fd)
#include <stdio.h>
#include <fcntl.h>          // 文件描述符相关宏定义


using namespace EasyEvent;

Socket::Socket(int socket_fd)
    : FileDescriptor(socket_fd) {}


const sockaddr* cast_to_sockaddr(const sockaddr_in* addr);
sockaddr* cast_to_sockaddr(sockaddr_in* addr);

void Socket::bindAddress(const CapsuledAddr& addr){
    // socket::bind(), 不是 C++11 std::bind()，这里是将“地址：端口”信息绑定到 fd
    int returnVal = ::bind(getFd(), cast_to_sockaddr(&(addr.getSocketAddr())), sizeof addr);
    assert(returnVal >= 0);
}

void Socket::listen(){
    /*
     将对应的 fd 设置为被动侦听（默认是主动）
     也就是当对应的 socket 产生 IO 事件时，对应的 fd 变为可读
     SOMAXCONN 默认 128，最大侦听排队数量，实际相当于同一时间最多允许同一个连接上请求的最大数量
               注意不是同时支持的最大连接数，一个 sockfd 对应一个连接,
               而 sockfd 数量由系统所支持的最多 fd 数量决定
    */
    int returnVal = ::listen(getFd(), SOMAXCONN);
    assert(returnVal >= 0);
}

int Socket::accept(CapsuledAddr* peerAddr){
    sockaddr_in addr;
    bzero(&addr, sizeof addr);

    /*
      直接使用 accept4() 系统调用。
      如果accpet成功，那么其返回值是由内核自动生成的一个全新的文件描述符，代表与返回客户的TCP连接
      这里的 accept() 已经是非阻塞的，会立即返回结果
    */
    // int connfd = accept(_socket_fd, &addr);
    socklen_t addrLen = sizeof addr;
    int connfd = ::accept4(getFd(), cast_to_sockaddr(&addr), &addrLen, 
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(connfd >= 0){
        peerAddr->setSocketAddr(addr);
    }
    return connfd;
}

void Socket::setReuseAddr(bool on){
    int optval = on ? 1 : 0;
    ::setsockopt(getFd(), SOL_SOCKET, SO_REUSEADDR,      // 端口复用
                &optval, sizeof optval);
}


/********************** helper functions **********************/

// namespace{
const sockaddr* cast_to_sockaddr(const sockaddr_in* addr){
    sockaddr_in* temp = const_cast<sockaddr_in*>(addr);
    return reinterpret_cast<sockaddr*>(temp);
}

sockaddr* cast_to_sockaddr(sockaddr_in* addr){
    return reinterpret_cast<sockaddr*>(addr);
}

void setNonBlockAndCloseOnExec(int sockfd){
    // non-block，非阻塞，使用时会立即返回
    int flags = ::fcntl(sockfd, F_GETFL, 0);        //取得文件描述符状态 flag
    flags |= O_NONBLOCK;
    int returnVal = ::fcntl(sockfd, F_SETFL, flags);      // 将 flag 写回
    assert(returnVal >= 0);

    // close-on-exec, 子进程 fork() 并 exec 时会自动 close父进程的fd
    flags = ::fcntl(sockfd, F_GETFD, 0);            // 取得close-on-exec flag
    flags |= FD_CLOEXEC;
    returnVal = ::fcntl(sockfd, F_SETFD, flags);          // 将 flag 写回
    assert(returnVal >= 0);
}

// }


/********************** static functions **********************/
int Socket::createNonblockingOrDie(){
    /*
        socket函数对应于普通文件的打开操作
        socket()创建一个socket描述符（socket descriptor），唯一标识一个socket。
        这个socket描述字跟文件描述字一样.
    */
    int sockfd = ::socket(AF_INET,                      // IPV4
        SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,     // 流式传输、非阻塞、close-on-exec
        IPPROTO_TCP);                                   // TCP
    
    assert(sockfd >= 0);
    return sockfd;
}

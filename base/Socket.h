#pragma once

#include <arpa/inet.h>

#include "CapsuledAddr.h"
#include "noncopyable.h"
#include "FileDescriptor.h"

namespace EasyEvent{

// 系统 socket fd 的包装类，目前仅支持 TCP
// 继承自 FileDescriptor，RAII类，析构时会自动关闭对应的文件
class Socket : public FileDescriptor, private noncopyable{

public:
    explicit Socket(int socket_fd);
    // ~Socket();

    // 绑定该 socket 侦听的地址
    void bindAddress(const CapsuledAddr& localAddr);
    void listen();

    // 返回指向对应 socket 的 fd
    // 失败返回-1
    int accept(CapsuledAddr* peerAddr);
    
    // 是否允许重用
    void setReuseAddr(bool on);

    static int createNonblockingOrDie();
    static int createUdpBlocking();
    static sockaddr_in getLocalAddr(int sockfd);
    static int getSocketErr(int sockfd);
    static const sockaddr* cast_to_sockaddr(const sockaddr_in* addr);
    static sockaddr* cast_to_sockaddr(sockaddr_in* addr);
};


}
#pragma once

#include <arpa/inet.h>

#include "CapsuledAddr.h"
#include "noncopyable.h"

namespace EasyEvent{

// 系统 socket fd 的包装类，目前仅支持 TCP
// RAII Class，对象析构时自动关闭对应 fd
class Socket : private noncopyable{

public:
    explicit Socket(int socket_fd);
    ~Socket();

    int getFd() const { return _socket_fd; }

    // 绑定该 socket 侦听的地址
    void bindAddress(const CapsuledAddr& localAddr);
    void listen();

    // 返回指向对应 socket 的 fd
    // 失败返回-1
    int accept(CapsuledAddr* peerAddr);
    
    // 是否允许重用
    void setReuseAddr(bool on);

    static int createNonblockingOrDie();
    static void close(int fd);

private:
    const int _socket_fd;
};

}
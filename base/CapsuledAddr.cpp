#include "CapsuledAddr.h"

#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace EasyEvent;

/*
    IPV4 地址格式: xxx.xxx.xxx.xxx : yyyyy
    ip 地址是 32 位无符号整数
    端口号是 16 位无符号整数，范围 0-65535

    int inet_aton(const char *cp, struct in_addr *inp); 将 ip 地址转化为二进制
    char *inet_ntoa(struct in_addr in);                 将 二进制地址转化为 ip 地址

    int inet_pton(int family, const char *strptr, void *addrptr);     //将点分十进制的ip地址转化为用于网络传输的数值格式
    返回值：若成功则为1，若输入不是有效的表达式则为0，若出错则为-1

    const char * inet_ntop(int family, const void *addrptr, char *strptr, size_t len);     //将数值格式转化为点分十进制的ip地址格式
    返回值：若成功则为指向结构的指针，若出错则为NULL

    //     struct sockaddr_in {
    //     sa_family_t    sin_family; // address family: AF_INET 
    //     in_port_t      sin_port;   // port in network byte order 
    //     struct in_addr sin_addr;   // internet address 
    //      };
*/

// 只提供端口号
CapsuledAddr::CapsuledAddr(uint16_t port){
    _addr.sin_family = AF_INET;     // ipv4
    _addr.sin_port = hostToNetwork16(port);
    _addr.sin_addr.s_addr = hostToNetwork32(INADDR_ANY);  // uint32_t
}

// 同时提供地址和端口号
CapsuledAddr::CapsuledAddr(const std::string& ip, uint16_t port){
    _addr.sin_family = AF_INET;
    _addr.sin_port = hostToNetwork16(port);
    assert(inet_pton(AF_INET, ip.c_str(), &(_addr.sin_addr)) > 0);
}

// 从基本类型转换构造
CapsuledAddr::CapsuledAddr(const sockaddr_in& addr)
    : _addr(addr)
    {}

std::string CapsuledAddr::toString() const{
    // xxx.xxx.xxx.xxx : yyyyy
    char buf[32];
    char host[INET_ADDRSTRLEN] = "INVALID";
    ::inet_ntop(AF_INET, &(_addr.sin_addr), host, sizeof host);
    uint16_t port = networkToHost16(_addr.sin_port);
    snprintf(buf, sizeof buf, "%s:%u", host, port);
    return buf;
}

// void sockets::toHostPort(char* buf, size_t size,
//                          const struct sockaddr_in& addr)
// {
//   char host[INET_ADDRSTRLEN] = "INVALID";
//   ::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof host);
//   uint16_t port = sockets::networkToHost16(addr.sin_port);
//   snprintf(buf, size, "%s:%u", host, port);
// }

// void sockets::HostPortToAddr(const char* ip, uint16_t port,
//                            struct sockaddr_in* addr)
// {
//   addr->sin_family = AF_INET;
//   addr->sin_port = hostToNetwork16(port);
//   if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
//   {
//     LOG_SYSERR << "sockets::fromHostPort";
//   }
// }

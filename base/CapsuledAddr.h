#pragma once

#include <string>
#include <netinet/in.h>

namespace EasyEvent{

// 对网络地址 struct sockaddr_in 的封装
class CapsuledAddr{
public:
    explicit CapsuledAddr(uint16_t port);
    CapsuledAddr(const std::string& ip, uint16_t port);
    CapsuledAddr(const sockaddr_in& addr);

    const sockaddr_in& getSocketAddr() const { return _addr; }
    void setSocketAddr(const sockaddr_in& addr) { _addr = addr; }

    std::string toString() const;

    // 地址转换
    static inline uint64_t hostToNetwork64(uint64_t host64) { return htobe64(host64);}
    static inline uint32_t hostToNetwork32(uint32_t host32) { return htonl(host32);}
    static inline uint16_t hostToNetwork16(uint16_t host16) { return htons(host16);}
    static inline uint64_t networkToHost64(uint64_t net64) { return be64toh(net64);}
    static inline uint32_t networkToHost32(uint32_t net32) { return ntohl(net32);}
    static inline uint16_t networkToHost16(uint16_t net16) { return ntohs(net16);}

private:
    sockaddr_in _addr;          // ipv4 地址结构
};


}

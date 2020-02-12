#include <iostream>
#include <netinet/in.h>
#include "base/CapsuledAddr.h"


int main(){
    EasyEvent::CapsuledAddr addr1(2000);
    EasyEvent::CapsuledAddr addr2("192.168.0.1", 2001);
    
    sockaddr_in structedAddr;
    structedAddr.sin_family = AF_INET;
    in_addr netAddr;
    netAddr.s_addr = 1234567;
    uint16_t netPort = EasyEvent::CapsuledAddr::hostToNetwork16(12345);
    structedAddr.sin_addr = netAddr;
    structedAddr.sin_port = netPort;
    EasyEvent::CapsuledAddr addr3(structedAddr);

    std::cout << "addr1: " << addr1.toString() << std::endl;
    std::cout << "addr2: " << addr2.toString() << std::endl;
    std::cout << "addr3: " << addr3.toString() << std::endl;
    
    return 0;
}

